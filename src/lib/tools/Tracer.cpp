/* TRACER.cpp
 *   by Lut99
 *
 * Created:
 *   17/08/2021, 16:45:49
 * Last edited:
 *   17/08/2021, 16:45:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a debug tracer for debugging purposes. Is only enabled when
 *   NDEBUG and NTRACE are not defined. If TRACE_LOG is defined, then the
 *   class prints changes to the callstack to stdout.
**/

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <io.h>
#include <string>
#include <sstream>
#include <iostream>

#include "Tracer.hpp"

using namespace std;
using namespace Tools;

#if !(defined(NDEBUG) && defined(NTRACE))


/***** GLOBALS *****/
/* Global instance of the Tracer class, used for debugging. */
Tracer Tools::tracer;





/***** HELPER FUNCTIONS *****/
/* Returns whether or not the associated terminal supports ANSI color codes. */
static bool terminal_supports_colours() {
    // First, check if the stderr is a terminal at all
    if (!isatty(fileno(stderr))) { return false; }

    #ifdef _WIN32
    // For Windows, we check
    DWORD modes;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &modes);
    return modes & ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    #else
    // Let's assume the rest does
    return true;
    #endif
}

/* Returns the terminal width. */
static size_t terminal_width() {
    #ifdef _WIN32
    /* Code from: https://stackoverflow.com/a/23370070/5270125. */
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;

    #else
    /* Code from: https://stackoverflow.com/a/23369919/5270125. */
    struct winsize size;
    ioctl(fileno(stderr), TIOCGWINSZ, &size);
    return size.width;

    #endif
}

/* Prints a given string over multiple lines, pasting n spaces in front of each one and linewrapping on the target width. Optionally, a starting x can be specified. */
static void print_linewrapped(std::ostream& os, size_t& x, size_t width, const std::string& message) {
    // Get the string to be pasted in front of every new line
    std::string prefix = std::string(Tracer::prefix_width, ' ');

    // Loop to print each character
    bool ignoring = false;
    for (size_t i = 0; i < message.size(); i++) {
        // If we're seeing a '\033', ignore until an 'm' is reached
        if (!ignoring && message[i] == '\033') { ignoring = true; }
        else if (ignoring && message[i] == 'm') { ignoring = false; }

        // Otherwise, check if we should print a newline (only when we're not printing color codes)
        if (!ignoring && ++x >= width) {
            os << std::endl << prefix;
            x = 0;
        }

        // Print the character itself
        os << message[i];
    }
}





/***** TRACER CLASS *****/
/* Default constructor for the Tracer class. */
Tracer::Tracer() :
    sred(terminal_supports_colours() ? "\033[31;1m" : ""),
    sbold(terminal_supports_colours() ? "\033[1m" : ""),
    sreset(terminal_supports_colours() ? "\033[0m" : "")
{}



/* Registers the current thread in the tracer by giving it a name. */
void Tracer::register_thread(const char* thread_name) {
    // Get the lock
    std::unique_lock<std::mutex> local_lock(this->lock);

    // Simply add the mapping
    this->thread_names[this_thread::get_id()] = thread_name;
}

/* Unregisters the current thread. */
void Tracer::unregister_thread() {
    // Get the lock
    std::unique_lock<std::mutex> local_lock(this->lock);

    // Only remove if it exists
    std::unordered_map<thread::id, const char*>::iterator iter = this->thread_names.find(this_thread::get_id());
    if (iter != this->thread_names.end()) {
        this->thread_names.erase(iter);
    }
}



/* Enter the function with the given name that is called from the given file at the given line number. Use std::numeric_limits<size_t>::max() to indicate it wasn't called. */
void Tracer::push_frame(const char* func_name, const char* file_name, size_t line) {
    // Insert the stack if it doesn't exist
    std::unordered_map<thread::id, std::vector<Frame>>::iterator iter = this->stack.find(this_thread::get_id());
    if (this->stack.find(this_thread::get_id()) == this->stack.end()) {
        iter = this->stack.insert(make_pair(this_thread::get_id(), std::vector<Frame>())).first;
        (*iter).second.reserve(Tracer::initial_stack_size);
    }

    // Double the frame size while needed
    while ((*iter).second.size() >= (*iter).second.capacity()) {
        (*iter).second.reserve((*iter).second.capacity() * 2);
    }

    // Insert the frame
    (*iter).second.push_back({
        func_name,
        file_name, line
    });
}

/* Pop the last function off the stack again. */
void Tracer::pop_frame() {
    // Try to find this thread's stack
    std::unordered_map<thread::id, std::vector<Frame>>::iterator iter = this->stack.find(this_thread::get_id());
    if (this->stack.find(this_thread::get_id()) != this->stack.end() && (*iter).second.size() > 0) {
        // Remove the back
        (*iter).second.pop_back();
    }
}



/* Prints the given text as fatal error to the given stream, printing all traces. */
void Tracer::throw_error(const char* message) const {
    // Define the terminal width
    size_t termw = isatty(fileno(stderr)) ? terminal_width() : std::numeric_limits<size_t>::max();

    // Print the main message first
    size_t x = 0;
    size_t width = termw - Tracer::prefix_width;
    cerr << '[' << this->sred << "ERROR" << this->sreset << "] ";
    print_linewrapped(cerr, x, width, message);
    cerr << this->sreset << std::endl;

    // Try to find the stacktrace for this thread
    std::unordered_map<thread::id, std::vector<Frame>>::const_iterator iter = this->stack.find(this_thread::get_id());
    if (iter != this->stack.end() && (*iter).second.size() > 0) {
        // We found it, so print it
        std::stringstream sstr;
        std::string prefix = std::string(Tracer::prefix_width, ' ');
        cerr << prefix << this->sbold << "Stacktrace:" << this->sreset << std::endl;
        for (size_t i = 0; i < (*iter).second.size(); i++) {
            // Get the frame
            const Frame& f = (*iter).second[i];

            // Construct the string to print
            sstr << prefix << (i == 0 ? "in" : "from") << " function '" << this->sbold << f.name << this->sreset << '\'';
            if (f.line != std::numeric_limits<size_t>::max()) {
                sstr << ", called from " << this->sbold << f.file << ':' << f.line << this->sreset;
            } else {
                sstr << " defined in " << this->sbold << f.file << this->sreset;
            }

            // Print the string to print
            x = 0;
            print_linewrapped(cerr, x, width, sstr.str());

            // Reset the stringstream
            sstr.str("");
        }
        sstr << prefix << "from thread" << this->sbold << this_thread::get_id() << this->sreset;
        std::unordered_map<thread::id, const char*>::const_iterator name_iter = this->thread_names.find(this_thread::get_id());
        if (name_iter != this->thread_names.end()) {
            sstr << '(' << this->sbold << ')';
        }
        x = 0;
        print_linewrapped(cerr, x, width, sstr.str());
        cerr << std::endl;
    }

    // // Return by throwing
    // throw Tracer::Fatal(message);

}

#endif
