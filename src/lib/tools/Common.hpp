/* COMMON.hpp
 *   by Lut99
 *
 * Created:
 *   02/05/2021, 17:12:22
 * Last edited:
 *   8/1/2021, 5:20:20 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains common helper functions used throughout the project.
**/

#ifndef TOOLS_COMMON_HPP
#define TOOLS_COMMON_HPP

#include <string>
#include "Array.hpp"

namespace Tools {
    /* Function that returns the path of the folder of the executable. */
    std::string get_executable_path();

    /* Given a char, returns a readable string representation of it. */
    const char* readable_char(char c);

    /* Splits a given string on the given character, returning an Array of strings. If the character isn't present, returns a split of one (the string itself). */
    Array<std::string> split_string(const std::string& to_split, char splitter);

    /* Function that returns a string more compactly describing the given number of bytes. */
    std::string bytes_to_string(size_t n_bytes);
    /* Function that parses a bytes string with format and returns the raw number of bytes meant by it. */
    size_t string_to_bytes(const std::string& text);

}

#endif
