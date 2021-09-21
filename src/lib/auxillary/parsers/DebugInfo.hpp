/* DEBUG INFO.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 17:29:59
 * Last edited:
 *   07/08/2021, 15:07:40
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DebugInfo class, which keeps track of a symbol's origin
 *   in a file to use in (error) messages.
**/

#ifndef AUXILLARY_DEBUG_INFO_HPP
#define AUXILLARY_DEBUG_INFO_HPP

#include <cstddef>
#include <string>
#include <ostream>
#include <cstdio>
#include "tools/Array.hpp"

#include "FileCoord.hpp"

namespace Makma3D::Auxillary {
    /* The DebugInfo class, which keeps track of relevant origin information of the symbol we're looking at. */
    class DebugInfo {
    public:
        /* Channel name for the DebugInfo class. */
        static constexpr const char* channel = "DebugInfo";
        /* Accent colour for notes. */
        static constexpr const char* note_accent = "\033[36;1m";
        /* Accent colour for warning messages. */
        static constexpr const char* warning_accent = "\033[33m";
        /* Accent colour for error messages. */
        static constexpr const char* error_accent = "\033[31;1m";

    private:
        /* The filename of the file. */
        std::string filename;

        /* The start line number of the symbol. */
        size_t line_start;
        /* The start column number of the symbol. */
        size_t col_start;
        /* The end line number of the symbol. Usually the same. */
        size_t line_end;
        /* The end column number of the symbol. */
        size_t col_end;

        /* The actualy line itself to show to the user. */
        Tools::Array<std::string> raw_lines;


        /* Private helper function that does most of the printing. */
        void _print(std::ostream& os, const std::string& message, const std::string& accent_colour) const;

    public:
        /* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column and a file handle to read the relevant lines from. Although the given file object isn't constant, it should be reinstated back to its original state once the constructor is done. */
        DebugInfo(const std::string& filename, size_t line, size_t col, FILE* file);
        /* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column and lines to show during debugging. The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
        DebugInfo(const std::string& filename, size_t line, size_t col, const Tools::Array<std::string>& raw_lines);
        /* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column, an end line, an end column and a file handle to read the relevant lines from. Although the given file object isn't constant, it should be reinstated back to its original state once the constructor is done. */
        DebugInfo(const std::string& filename, size_t line1, size_t col1, size_t line2, size_t col2, FILE* file);
        /* Constructor for the DebugInfo class, which takes the filename of the token's file, a start line, a start column, an end line, an end column and lines to show during debugging. The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
        DebugInfo(const std::string& filename, size_t line1, size_t col1, size_t line2, size_t col2, const Tools::Array<std::string>& raw_lines);

        /* Compares another DebugInfo with this one. */
        inline bool operator==(const DebugInfo& other) const { return this->line_start == other.line_start && this->col_start == other.col_start && \
                                                                this->line_end == other.line_end && this->col_end == other.col_end; }
        /* Compares another DebugInfo with this one by inequality. */
        inline bool operator!=(const DebugInfo& other) const { return !(*this == other); }
        /* Adds another DebugInfo, and returns a new one that spans from the start of this one to the end of the other one. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
        DebugInfo operator+(const DebugInfo& other) const;
        /* Adds another DebugInfo to this one s.t. we end at the given DebugInfo's end line and end column. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
        DebugInfo& operator+=(const DebugInfo& other);

        /* Pretty prints a given note to the given stream using the internal debug information about the symbol. Usually used with another DebugInfo to refer some secondary symbol in an error message. */
        void print_note(std::ostream& os, const std::string& note) const;
        /* Pretty prints a given warning message to the given stream using the internal debug information about the symbol. */
        void print_warning(std::ostream& os, const std::string& warning_message) const;
        /* Pretty prints a given error message to the given stream using the internal debug information about the symbol. */
        void print_error(std::ostream& os, const std::string& error_message) const;

        /* Returns the filename of the file. */
        inline const std::string& path() const { return this->filename; }
        /* Returns the file coordinates of the start of the symbol. */
        inline FileCoord start() const { return FileCoord({ this->line_start, this->col_start }); }
        /* Returns the file coordinates of the end of the symbol. */
        inline FileCoord end() const { return FileCoord({ this->line_end, this->col_end }); }
        /* Returns the desired line from the list of lines. Negative offsets can be given, which are then interpreted as index counting back from the start: -1 means the last character, -2 the second to last, etc. */
        inline const std::string& get_line(int offset) const {  return this->raw_lines[ \
                                                                    offset >= 0 ? \
                                                                    offset : \
                                                                    this->raw_lines.size() + offset \
                                                                ]; }

    };

}

#endif
