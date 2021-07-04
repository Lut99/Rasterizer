/* DEBUG INFO.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 17:29:59
 * Last edited:
 *   04/07/2021, 17:29:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DebugInfo class, which keeps track of a symbol's origin
 *   in a file to use in (error) messages.
**/

#ifndef MODELS_DEBUG_INFO_HPP
#define MODELS_DEBUG_INFO_HPP

#include <cstddef>
#include <string>
#include <fstream>
#include "tools/Array.hpp"

#include "FileCoord.hpp"

namespace Rasterizer::Models {
    /* The DebugInfo class, which keeps track of relevant origin information of the symbol we're looking at. */
    class DebugInfo {
    private:
        /* The start line number of the symbol. */
        size_t line_start;
        /* The start column number of the symbol. */
        size_t col_start;
        /* The end line number of the symbol. Usually the same. */
        size_t line_end;
        /* The end column number of the symbol. */
        size_t col_end;
        /* The number of characters spanned by the symbol (includes newlines and the like). */
        size_t length;

        /* The actualy line itself to show to the user. */
        Tools::Array<std::string> raw_lines;

    public:
        /* Constructor for the DebugInfo class, which takes a file handle to extract the line from, a start line, a start column and the length of the symbol (in characters). */
        DebugInfo(std::ifstream& file, size_t line, size_t col, size_t length);
        /* Constructor for the DebugInfo class, which takes a file handle to extract the line from, a start line, a start column, an end line and an end column. */
        DebugInfo(std::ifstream& file, size_t line1, size_t col1, size_t line2, size_t col2);
        /* Constructor for the DebugInfo class, which takes lines to show during debugging, a start line, a start column and the length of the symbol (in characters). The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
        DebugInfo(const Tools::Array<std::string>& raw_lines, size_t line, size_t col, size_t length);
        /* Constructor for the DebugInfo class, which takes lines to show during debugging, a start line, a start column, an end line and an end column. The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
        DebugInfo(const Tools::Array<std::string>& raw_lines, size_t line1, size_t col1, size_t line2, size_t col2);

        /* Compares another DebugInfo with this one. */
        inline bool operator==(const DebugInfo& other) { return this->line_start == other.line_start && this->col_start == other.col_start && \
                                                                this->line_end == other.line_end && this->col_end == other.col_end; }
        /* Compares another DebugInfo with this one by inequality. */
        inline bool operator!=(const DebugInfo& other) { return !(*this == other); }
        /* Adds another DebugInfo, and returns a new one that spans from the start of this one to the end of the other one. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
        DebugInfo operator+(const DebugInfo& other);
        /* Adds another DebugInfo to this one s.t. we end at the given DebugInfo's end line and end column. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
        DebugInfo& operator+=(const DebugInfo& other);

        /* Returns the file coordinates of the start of the symbol. */
        inline FileCoord start() const { return FileCoord({ this->line_start, this->col_start }); }
        /* Returns the file coordinates of the end of the symbol. */
        inline FileCoord end() const { return FileCoord({ this->line_end, this->col_end }); }
        /* Returns the desired line from the list of lines. Negative offsets can be given, which are then interpreted as index counting back from the start: -1 means the last character, -2 the second to last, etc. */
        inline const std::string& get_line(int offset) const {  return this->raw_lines[ \
                                                                    offset >= 0 ? \
                                                                    offset : \
                                                                    this->raw_lines.size() + offset
                                                                ]; }

    };

}

#endif
