/* DEBUG INFO.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 17:30:01
 * Last edited:
 *   07/08/2021, 15:08:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DebugInfo class, which keeps track of a terminal's origin
 *   in a file to use in (error) messages.
**/

#include <cmath>
#include <cstring>
#include <cerrno>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <sstream>
#include "tools/CppDebugger.hpp"

#include "DebugInfo.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Returns whether or not the associated terminal supports ANSI color codes. */
static bool terminal_supports_colours() {
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

/* Returns a string representing the given number, padded with enough spaces to be at least N character long. */
template <class T>
static std::string pad_spaces(T value, size_t N) {
    DENTER("pad_zeros");
    
    // Convert to string
    std::string result = std::to_string(value);
    while (result.size() < N) {
        result = ' ' + result;
    }

    // DOne
    DRETURN result;
}





/***** DEBUGINFO CLASS *****/
/* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column and a file handle to read the relevant lines from. Although the given file object isn't constant, it should be reinstated back to its original state once the constructor is done. */
DebugInfo::DebugInfo(const std::string& filename, size_t line, size_t col, FILE* file) :
    DebugInfo(filename, line, col, line, col, file)
{}

/* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column and lines to show during debugging. The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
DebugInfo::DebugInfo(const std::string& filename, size_t line, size_t col, const Tools::Array<std::string>& raw_lines) :
    DebugInfo(filename, line, col, line, col, raw_lines)
{}

/* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column, an end line, an end column and a file handle to read the relevant lines from. */
DebugInfo::DebugInfo(const std::string& filename, size_t line1, size_t col1, size_t line2, size_t col2, FILE* file) :
    filename(filename),

    line_start(line1),
    col_start(col1),
    line_end(line2),
    col_end(col2),

    raw_lines((uint32_t) (this->line_end - this->line_start + 1))
{
    DENTER("Tools::DebugInfo::DebugInfo");

    // First, reset the file to standard position
    long old_cursor = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Next, go through the characters until we find the line(s) we're looking for
    char c = '\0';
    size_t line = 1;
    std::stringstream sstr;
    while (c != EOF) {
        // Get the next character
        c = fgetc(file);
        if (c == EOF && ferror(file)) {
            #ifdef _WIN32
            char buffer[BUFSIZ];
            strerror_s(buffer, errno);
            std::string err = buffer;
            #else
            std::string err = strerror(errno);
            #endif
            DLOG(fatal, "Something went wrong while reading from the stream: " + err);
        }

        // If it's a newline, move to the next line
        if (c == '\n') {
            // If this was a relevant line, then store the linebuffer and clear it
            if (line >= this->line_start && line <= this->line_end) {
                this->raw_lines.push_back(sstr.str());
                sstr.str("");
            }

            // Increment the counters
            ++line;

            // If we're now past the target line, we can stop
            if (line > this->line_end) {
                // Reinstate the old cursor
                fseek(file, old_cursor, SEEK_SET);

                // Done
                DRETURN;
            }
        }

        // If we're at the range, then keep track of the line
        if (line >= this->line_start && line <= this->line_end) {
            sstr << c;
        }
    }

    // We reached EOF before we read all lines
    DLOG(warning, "Given lines (from " + std::to_string(this->line_start) + " to " + std::to_string(this->line_end) + ") don't exist in the given file (" + this->filename + ").");
    this->raw_lines = Tools::Array<std::string>("", (uint32_t) (this->line_end - this->line_start + 1));
    DLEAVE;
}

/* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column, an end line, an end column and lines to show during debugging. The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
DebugInfo::DebugInfo(const std::string& filename, size_t line1, size_t col1, size_t line2, size_t col2, const Tools::Array<std::string>& raw_lines) :
    filename(filename),

    line_start(line1),
    col_start(col1),
    line_end(line2),
    col_end(col2),

    raw_lines(raw_lines)
{}



/* Adds another DebugInfo, and returns a new one that spans from the start of this one to the end of the other one. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
DebugInfo DebugInfo::operator+(const DebugInfo& other) const {
    DENTER("Models::DebugInfo::operator+");

    // If our line start is after the other's line end or there're the same but the columns are swapped, then swap the operation
    if (this->line_start > other.line_end || (this->line_start == this->line_end && other.line_start == other.line_end && this->line_start == other.line_start && this->col_start > other.col_end)) {
        DRETURN other + *this;
    }

    // Start with our raw lines
    Tools::Array<std::string> new_raw_lines = this->raw_lines;
    // Do some padding if needed
    if (this->line_end > other.line_start - 1) {
        // There's overlap, so remove the last i lines
        new_raw_lines.resize(new_raw_lines.size() - (uint32_t) (this->line_end - other.line_start + 1));
    } else if (this->line_end < other.line_start - 1) {
        // There's a gap in between the info's, so add empty lines
        for (size_t i = this->line_end; i < other.line_start; i++) {
            new_raw_lines.push_back("");
        }
    }
    // Append the other's raw lines
    new_raw_lines += other.raw_lines;

    // Done, create the new DebugInfo
    DRETURN DebugInfo(this->filename, this->line_start, this->col_start, other.line_end, other.col_end, new_raw_lines);
}

/* Adds another DebugInfo to this one s.t. we end at the given DebugInfo's end line and end column. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
DebugInfo& DebugInfo::operator+=(const DebugInfo& other) {
    DENTER("Models::DebugInfo::operator+=");

    // If our line start is after the other's line end or there're the same but the columns are swapped, then error since we cannot swap +=
    if (this->line_start > other.line_end || (this->line_start == this->line_end && other.line_start == other.line_end && this->line_start == other.line_start && this->col_start > other.col_end)) {
        DLOG(fatal, "Cannot add two DebugInfo's where the second is before the first");
    }

    // Start with our raw lines
    Tools::Array<std::string> new_raw_lines = this->raw_lines;
    // Do some padding if needed
    if (this->line_end > other.line_start - 1) {
        // There's overlap, so remove the last i lines
        new_raw_lines.resize(new_raw_lines.size() - (uint32_t) (this->line_end - other.line_start + 1));
    } else if (this->line_end < other.line_start - 1) {
        // There's a gap in between the info's, so add empty lines
        for (size_t i = this->line_end; i < other.line_start; i++) {
            new_raw_lines.push_back("");
        }
    }
    // Append the other's raw lines
    new_raw_lines += other.raw_lines;

    // Update our internal info
    this->line_end  = other.line_end;
    this->col_end   = other.col_end;
    this->raw_lines = new_raw_lines;

    // Done
    DRETURN *this;
}



/* Private helper function that does most of the printing. */
void DebugInfo::_print(std::ostream& os, const std::string& message, const std::string& accent_colour) {
    DENTER("Tools::DebugInfo::_print");

    // First, print the error message properly
    bool supports_ansi = terminal_supports_colours();
    if (supports_ansi) {
        os << endl << "\033[1m" << this->filename << ":" << this->line_start << ":" << this->col_start << ": \033[0m" << message << endl;
    } else {
        os << endl << this->filename << ":" << this->line_start << ":" << this->col_start << ": " << message << endl;
    }

    // Print the raw_lines, with the correct characters in colour
    bool accent_mode = false;
    uint32_t number_width = (uint32_t) ceil(log10(this->line_end + 1));
    for (size_t y = this->line_start; y <= this->line_end; y++) {
        // First, print the line number
        os << ' ' << pad_spaces(y, number_width) << " | ";
        // Get the line
        std::string line = this->get_line((uint32_t) (y - this->line_start));

        // Loop to print it
        for (size_t x = 0; x < line.size(); x++) {
            // If we're at the start pos, print the start colour
            if (supports_ansi && !accent_mode && y == this->line_start && x == this->col_start - 1) {
                os << accent_colour;
                accent_mode = true;
            }

            // Print the character
            os << line[x];

            // If we're at the end pos, print the end colour
            if (supports_ansi && accent_mode && y == this->line_end && x == this->col_end - 1) {
                os << "\033[0m";
                accent_mode = false;
            }
        }

        // When done, print a newline
        os << endl;
    }

    // If we're still in accent mode, always print closing character
    if (supports_ansi && accent_mode) {
        os << "\033[0m";
    }

    // We're done
    os << endl;
    DRETURN;
}

/* Pretty prints a given note to the CLI using the internal debug information about the symbol. Usually used with another DebugInfo to refer some secondary symbol in an error message. */
void DebugInfo::print_note(std::ostream& os, const std::string& note) {
    DENTER("Tools::DebugInfo::print_note");

    if (terminal_supports_colours()) {
        this->_print(os, std::string(DebugInfo::note_accent) + "note: \033[0m" + note, DebugInfo::note_accent);
    } else {
        this->_print(os, "note: " + note, "");
    }

    DRETURN;
}

/* Pretty prints a given warning message to the CLI using the internal debug information about the symbol. */
void DebugInfo::print_warning(std::ostream& os, const std::string& warning_message) {
    DENTER("Tools::DebugInfo::print_warning");

    if (terminal_supports_colours()) {
        this->_print(os, std::string(DebugInfo::warning_accent) + "warning: \033[0m" + warning_message, DebugInfo::warning_accent);
    } else {
        this->_print(os, "warning: " + warning_message, "");
    }

    DRETURN;
}

/* Pretty prints a given error message to the CLI using the internal debug information about the symbol. */
void DebugInfo::print_error(std::ostream& os, const std::string& error_message) {
    DENTER("Tools::DebugInfo::print_error");

    if (terminal_supports_colours()) {
        this->_print(os, std::string(DebugInfo::error_accent) + "error: \033[0m" + error_message, DebugInfo::error_accent);
    } else {
        this->_print(os, "error: " + error_message, "");
    }

    DRETURN;
}
