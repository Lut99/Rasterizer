/* LOGGER.hpp
 *   by Lut99
 *
 * Created:
 *   25/07/2021, 14:11:20
 * Last edited:
 *   20/09/2021, 14:04:18
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Logger class, which logs different things based on the
 *   severity level.
**/

#ifndef TOOLS_LOGGER_HPP
#define TOOLS_LOGGER_HPP

#include <ostream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <exception>

#include "StreamOperators.hpp"

namespace Tools {
    /* Namespace encapsulating the verbosity enum. */
    namespace VerbosityValues {
        /* The values of the Verbosity enum. */
        enum values {
            /* No verbosity; ignore all messages except for the most fatal of errors. */
            none = 0,
            /* A little verbosity: shows some major plotpoints, such as tokenizing and parsing, but leave most out. */
            important = 1,
            /* Some more verbosity: shows most of the steps the parser goes through, but leaves out some more debug information such as parameter values. */
            details = 2,
            /* Shows everything, including debug information of parameters and junk. */
            debug = 3
        };
    }
    /* The Verbosity enum, which is directly mapped to verbosity integers. */
    using Verbosity = VerbosityValues::values;



    /* The Logger class, which is used to dynamically log stuff. */
    class Logger {
    public:
        /* Static string that is used in case an empty sting is needed. */
        inline static const std::string empty_string = "";

        /* Exception that is thrown on fatal() calls. */
        class Fatal: public std::exception {
        public:
            /* The message passed to the fatal error message. */
            std::string message;

            /* Constructor for the Fatal exception, which takes a message describing what happened. */
            Fatal(const std::string& message);

            /* Overload of the what() function, which is used to pretty-print the message. */
            inline const char* what() const noexcept { return this->message.c_str(); }
        };

    private:
        /* The output stream to log non-error messages to. */
        std::ostream* stdos;
        /* The output stream to log error messages to. */
        std::ostream* erros;

        /* The verbosity level of logging. */
        Verbosity verbosity;
        /* Start time of the logger. */
        std::chrono::system_clock::time_point start_time;

        /* Map of thread IDs to readable names. */
        std::unordered_map<std::thread::id, std::string> thread_names;
        /* Mutex to synchronize Logger access. */
        std::mutex lock;


        #if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(NDEBUG)
        /* Private static helper function that prints the current stack on unix systems. */
        static void print_stacktrace();
        #endif
        /* Private static helper function that pads a float to always have three decimals. */
        static std::string pad_float(float value);
        /* Private helper function that returns the number of seconds since the start of the Logger. */
        inline std::string get_time_passed() const { return pad_float((float) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - this->start_time).count() / 1000.0f); }
        /* Internal helper function that populates a given stringstream with all given types, converted to strings. */
        template <class T>
        static void _add_args(std::ostream* os, const T& arg) {
            *os << arg;
        }
        /* Internal helper function that populates a given stringstream with all given types, converted to strings. */
        template <class T, class... Ts>
        static void _add_args(std::ostream* os, const T& arg, const Ts&... rest) {
            *os << arg;
            Logger::_add_args(os, rest...);
        }
  
    public:
        /* Constructor for the Logger class, which takes an output stream to write its non-messages to, an output stream to write error messages to and a verbosity level. */
        Logger(std::ostream& stdos, std::ostream& erros, Verbosity verbosity);
        /* Copy constructor for the Logger class. */
        Logger(const Logger& other);
        /* Move constructor for the Logger class. */
        Logger(Logger&& other);
        /* Destructor for the Logger class. */
        ~Logger();

        /* Links the current thread ID to the given name. */
        inline void set_thread_name(const std::string& name) { return this->set_thread_name(std::this_thread::get_id(), name); }
        /* Links the given thread ID to the given name. */
        void set_thread_name(const std::thread::id& tid, const std::string& name);
        /* Removes the name mapping for the current thread. */
        inline void unset_thread_name() { return this->unset_thread_name(std::this_thread::get_id()); }
        /* Removes the name mapping for the given thread ID. */
        void unset_thread_name(const std::thread::id& tid);

        /* Sets the output stream for this Logger. */
        void set_output_stream(std::ostream& os);
        /* Returns the output stream for this Logger. */
        inline std::ostream& get_output_stream() const { return *this->stdos; }
        /* Sets the error stream for this Logger. */
        void set_error_stream(std::ostream& os);
        /* Returns the error stream for this Logger. */
        inline std::ostream& get_error_stream() const { return *this->erros; }
        /* Sets the internal verbosity to the given value. */
        void set_verbosity(Verbosity new_value);
        /* Returns the internal verbosity. */
        inline Verbosity get_verbosity() const { return this->verbosity; }
        /* Returns the start time of the Logger. */
        inline std::chrono::system_clock::time_point get_start_time() const { return this->start_time; }

        /* Writes a debug message to the internal standard output stream. Assumes a verbosity of "debug" and no channel. */
        template <class... Ts>
        inline void debug(const Ts&... message) { this->debugc("", message...); }
        /* Writes a debug message to the internal standard output stream. Assumes a verbosity of "debug", and never prints a channel. */
        template <class... Ts>
        void debugc(const std::string& channel, const Ts&... message) {
            using namespace date;

            // Check if we should print
            if (this->verbosity < Verbosity::debug) { return; }

            // Try to see if this thread has a canonical name
            std::string tname = "";
            std::unordered_map<std::thread::id, std::string>::iterator iter = this->thread_names.find(std::this_thread::get_id());
            if (iter != this->thread_names.end()) {
                tname = (*iter).second + '/';
            }

            // Otherwise, start constructing the stringstream
            {
                // Get the lock first
                std::unique_lock<std::mutex>(this->lock);

                // Write to the stream now that we have synchronized access
                *this->stdos << '[' << this->get_time_passed() << ']';
                *this->stdos << '[' << tname << "DEBUG]";
                if (!channel.empty()) { *this->stdos << '[' << channel << ']'; }
                *this->stdos << ' ';
                this->_add_args(this->stdos, message...);
                *this->stdos << std::endl;
            }

            // Done
            return;
        }
        /* Writes a message to the internal standard output stream. The given verbosity determines if the message is printed or not. The arguments are appended (in order) and without spaces in between. */
        template <class... Ts>
        inline void log(Verbosity verbosity, const Ts&... message) { this->logc(verbosity, "", message...); }
        /* Writes a message to the internal standard output stream. The given verbosity determines if the message is printed or not, and the channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. */
        template<class... Ts>
        void logc(Verbosity verbosity, const std::string& channel, Ts... message) {
            using namespace date;

            // Check if we should print
            if (this->verbosity < verbosity) { return; }

            // Try to see if this thread has a canonical name
            std::string tname = "";
            std::unordered_map<std::thread::id, std::string>::iterator iter = this->thread_names.find(std::this_thread::get_id());
            if (iter != this->thread_names.end()) {
                tname = (*iter).second + '/';
            }

            // Otherwise, start constructing the stringstream
            {
                // Get the lock first
                std::unique_lock<std::mutex>(this->lock);

                // Write to the stream now that we have synchronized access
                *this->stdos << '[' << this->get_time_passed() << ']';
                *this->stdos << '[' << tname << "INFO]";
                if (!channel.empty()) { *this->stdos << '[' << channel << ']'; }
                *this->stdos << ' ';
                this->_add_args(this->stdos, message...);
                *this->stdos << std::endl;
            }

            // Done
            return;
        }
        /* Writes a warning message to the internal error output stream. The arguments are appended (in order) and without spaces in between. Since it's a warning, its verbosity is fixed to 1 (important). */
        template <class... Ts>
        inline void warning(const Ts&... message) { this->warningc("", message...); }
        /* Writes a warning message to the internal error output stream. The channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. Since it's a warning, its verbosity is fixed to 1 (important). */
        template<class... Ts>
        void warningc(const std::string& channel, Ts... message) {
            using namespace date;

            // Check if we should print
            if (this->verbosity < Verbosity::important) { return; }

            // Try to see if this thread has a canonical name
            std::string tname = "";
            std::unordered_map<std::thread::id, std::string>::iterator iter = this->thread_names.find(std::this_thread::get_id());
            if (iter != this->thread_names.end()) {
                tname = (*iter).second + '/';
            }

            // Otherwise, start constructing the stringstream
            {
                // Get the lock first
                std::unique_lock<std::mutex>(this->lock);

                // Write to the stream now that we have synchronized access
                *this->erros << '[' << this->get_time_passed() << ']';
                *this->erros << '[' << tname << "WARNING]";
                if (!channel.empty()) { *this->erros << '[' << channel << ']'; }
                *this->erros << ' ';
                this->_add_args(this->erros, message...);
                *this->erros << std::endl;
            }

            // Done
            return;
        }
        /* Writes an error message to the internal error output stream. The arguments are appended (in order) and without spaces in between. Since it's an error, its verbosity is fixed to 0 (always shown). */
        template <class... Ts>
        inline void error(const Ts&... message) { this->errorc("", message...); }
        /* Writes an error message to the internal error output stream. The channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. Since it's an error, its verbosity is fixed to 0 (always shown). */
        template<class... Ts>
        void errorc(const std::string& channel, Ts... message) {
            using namespace date;

            // Try to see if this thread has a canonical name
            std::string tname = "";
            std::unordered_map<std::thread::id, std::string>::iterator iter = this->thread_names.find(std::this_thread::get_id());
            if (iter != this->thread_names.end()) {
                tname = (*iter).second + '/';
            }

            // Otherwise, start constructing the stringstream
            {
                // Get the lock first
                std::unique_lock<std::mutex>(this->lock);

                // Write to the stream now that we have synchronized access
                *this->erros << '[' << this->get_time_passed() << ']';
                *this->erros << '[' << tname << "ERROR]";
                if (!channel.empty()) { *this->erros << '[' << channel << ']'; }
                *this->erros << ' ';
                this->_add_args(this->erros, message...);
                *this->erros << std::endl;

                // Next, print the stacktrace
                #if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(NDEBUG)
                this->print_stacktrace();
                #endif
            }

            // Done
            return;
        }
        /* Writes an error message to the internal error output stream. The arguments are appended (in order) and without spaces in between. Since it's a fatal error, its verbosity is fixed to 0 (always shown). */
        template <class... Ts>
        [[ noreturn ]] inline void fatal(const Ts&... message) { this->fatalc("", message...); }
        /* Writes an error message to the internal error output stream. The channel is used to group certain messages together. The arguments are appended (in order) and without spaces in between. Since it's a fatal error, its verbosity is fixed to 0 (always shown). */
        template<class... Ts>
        [[ noreturn ]] void fatalc(const std::string& channel, Ts... message) {
            using namespace date;

            // We first construct the message separately
            std::stringstream sstr;
            this->_add_args((std::ostream*) &sstr, message...);

            // Try to see if this thread has a canonical name
            std::string tname = "";
            std::unordered_map<std::thread::id, std::string>::iterator iter = this->thread_names.find(std::this_thread::get_id());
            if (iter != this->thread_names.end()) {
                tname = (*iter).second + '/';
            }

            // Always start constructing the stringstream
            {
                // Get the lock first
                std::unique_lock<std::mutex>(this->lock);

                // Write to the stream now that we have synchronized access
                *this->erros << '[' << this->get_time_passed() << ']';
                *this->erros << '[' << tname << "FATAL]";
                if (!channel.empty()) { *this->erros << '[' << channel << ']'; }
                *this->erros << ' ' << sstr.str() << std::endl;

                // Next, print the stacktrace
                #if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(NDEBUG)
                this->print_stacktrace();
                #endif
            }

            // Instead of returning, hit 'em with the exception
            throw Logger::Fatal(sstr.str());
        }

        /* Prints the "Initializing..." message for the given channel and with the given verbosity. */
        inline void init_start(Verbosity verbosity, const std::string& channel) { return this->logc(verbosity, channel, "Initializing..."); }
        /* Prints the "Init success." message for the given channel and with the given verbosity. */
        inline void init_success(Verbosity verbosity, const std::string& channel) { return this->logc(verbosity, channel, "Initialized."); }
        /* Prints the "Copying..." message for the given channel. Always has a verbosity of 'debug'. */
        inline void copy_start(const std::string& channel) { return this->logc(Verbosity::debug, channel, "Copying..."); }
        /* Prints the "Copy success." message for the given channel. Always has a verbosity of 'debug'. */
        inline void copy_success(const std::string& channel) { return this->logc(Verbosity::debug, channel, "Copied."); }
        /* Prints the "Moving..." message for the given channel. Always has a verbosity of 'debug'. */
        inline void move_start(const std::string& channel) { return this->logc(Verbosity::debug, channel, "Moving..."); }
        /* Prints the "Move success." message for the given channel. Always has a verbosity of 'debug'. */
        inline void move_success(const std::string& channel) { return this->logc(Verbosity::debug, channel, "Moved."); }
        /* Prints the "Cleaning..." message for the given channel and with the given verbosity. */
        inline void clean_start(Verbosity verbosity, const std::string& channel) { return this->logc(verbosity, channel, "Cleaning..."); }
        /* Prints the "Cleaned." message for the given channel and with the given verbosity. */
        inline void clean_success(Verbosity verbosity, const std::string& channel) { return this->logc(verbosity, channel, "Cleaned."); }

        /* Copy assignment operator for the Logger class. */
        inline Logger& operator=(const Logger& other) { return *this = Logger(other); }
        /* Move assignment operator for the Logger class. */
        inline Logger& operator=(Logger&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Logger class. */
        friend void swap(Logger& l1, Logger& l2);

    };
    
    /* Swap operator for the Logger class. */
    void swap(Logger& l1, Logger& l2);



    /* Instance of the logger that is globally available. */
    extern Logger logger;

}

#endif
