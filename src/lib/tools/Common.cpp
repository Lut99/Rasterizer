/* COMMON.cpp
 *   by Lut99
 *
 * Created:
 *   02/05/2021, 17:12:29
 * Last edited:
 *   8/1/2021, 5:35:38 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains common helper functions used throughout the project.
**/

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>
#endif
#include <cmath>
#include <sstream>
#include <string>
#include <stdexcept>

#include "Common.hpp"

using namespace std;
using namespace Tools;


/***** LIBRARY FUNCTIONS *****/
/* Function that returns the path of the folder of the executable. */
std::string Tools::get_executable_path() {
    #ifdef _WIN32
    /* For Windows machiens */

    // First, get the executable name
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    
    // Strip the executable from it to only get the directory
    for (int i = MAX_PATH - 1; i >= 0; i--) {
        if (path[i] == '\\' || path[i] == '/') {
            path[i] = '\0';
            break;
        }
    }

    // Prepare the converting of unicode to ascii and then return the converted value
    return path;

    #else
    /* For Linux machiens */

    // Get the executable path
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    // Strip the executable from it to only get the directory
    for (ssize_t i = count - 1; i >= 0; i--) {
        if (result[i] == '/') {
            result[i] = '\0';
            break;
        }
    }

    // Done, return
    return result;

    #endif
}



/* Appends the given string at the end of the given output stringstream as if both were paths (i.e., making sure there's only one slash in between them). */
std::stringstream& Tools::merge_paths(std::stringstream& left, const std::string& right) {
    // If right is empty, we can stop right away
    if (right.empty()) { return left; }

    // Get the last character in the stream
    char c = '\0';
    if (left.tellp() > 0) { left.seekg(-1, ios::end); left >> c; }

    // Check if there's already a slash at left's end or right's start
    if ((c != '/' && c != '\\') &&
        (right[0] != '/' && right[1] != '\\'))
    {
        // Add the slash first
        left << '/';
    }

    // Add right, we're done
    left << right;
    return left;
}



/* Given a char, returns a readable string representation of it. */
const char* Tools::readable_char(char c) {
    switch(c) {
        case 'a': return "a";
        case 'b': return "b";
        case 'c': return "c";
        case 'd': return "d";
        case 'e': return "e";
        case 'f': return "f";
        case 'g': return "g";
        case 'h': return "h";
        case 'i': return "i";
        case 'j': return "j";
        case 'k': return "k";
        case 'l': return "l";
        case 'm': return "m";
        case 'n': return "n";
        case 'o': return "o";
        case 'p': return "p";
        case 'q': return "q";
        case 'r': return "r";
        case 's': return "s";
        case 't': return "t";
        case 'u': return "u";
        case 'v': return "v";
        case 'w': return "w";
        case 'x': return "x";
        case 'y': return "y";
        case 'z': return "z";
        case 'A': return "A";
        case 'B': return "B";
        case 'C': return "C";
        case 'D': return "D";
        case 'E': return "E";
        case 'F': return "F";
        case 'G': return "G";
        case 'H': return "H";
        case 'I': return "I";
        case 'J': return "J";
        case 'K': return "K";
        case 'L': return "L";
        case 'M': return "M";
        case 'N': return "N";
        case 'O': return "O";
        case 'P': return "P";
        case 'Q': return "Q";
        case 'R': return "R";
        case 'S': return "S";
        case 'T': return "T";
        case 'U': return "U";
        case 'V': return "V";
        case 'W': return "W";
        case 'X': return "X";
        case 'Y': return "Y";
        case 'Z': return "Z";
        case ' ': return " ";
        case '.': return ".";
        case ',': return ",";
        case ';': return ";";
        case ':': return ":";
        case '(': return "(";
        case ')': return ")";
        case '{': return "{";
        case '}': return "}";
        case '<': return "<";
        case '>': return ">";
        case '+': return "+";
        case '-': return "-";
        case '=': return "=";
        case '_': return "_";
        case '/': return "/";
        case '!': return "!";
        case '@': return "@";
        case '#': return "#";
        case '$': return "$";
        case '%': return "%";
        case '^': return "^";
        case '&': return "&";
        case '*': return "*";
        case '~': return "~";
        case '`': return "`";
        case '|': return "|";
        case '\\': return "\\";
        case '\'': return "'";
        case '"': return "\"";
        case '\n': return "newline";
        case '\r': return "carriage return";
        case '\t': return "tab";
        case '\0': return "null";
        default: return "special char";
    }
}



/* Splits a given string on the given character, returning an Array of strings. If the character isn't present, returns a split of one (the string itself). */
Array<std::string> Tools::split_string(const std::string& to_split, char splitter) {
    // Search and collect the list
    std::stringstream sstr;
    Array<std::string> to_return(1);
    size_t i = 0;
    while (i < to_split.size()) {
        // Collect the string up to this point
        sstr.str("");
        for ( ; i < to_split.size(); i++) {
            if (to_split[i] == splitter) { break; }
            sstr << to_split[i];
        }
        to_return.push_back(sstr.str());

        // Increment i to skip over the splitter
        ++i;
    }

    // Done
    return to_return;
}



/* Function that returns a string more compactly describing the given number of bytes. */
std::string Tools::bytes_to_string(size_t n_bytes) {
    // Select the proper scale to write the bytes to
    std::stringstream sstr;
    if (n_bytes < 1024) {
        // Write as normal bytes
        sstr << n_bytes << " bytes";
    } else if (n_bytes < 1024 * 1024) {
        // Write as kilobytes
        sstr << ((double) n_bytes / 1024.0) << " KiB";
    } else if (n_bytes < 1024 * 1024 * 1024) {
        // Write as megabytes
        sstr << ((double) n_bytes / (1024.0 * 1024.0)) << " MiB";
    } else {
        // Write as gigabytes
        sstr << ((double) n_bytes / (1024.0 * 1024.0 * 1024.0)) << " GiB";
    }
    
    // Done, return
    return sstr.str();
}

/* Function that parses a bytes string with format and returns the raw number of bytes meant by it. */
size_t Tools::string_to_bytes(const std::string& text) {
    // First, remove spaces from the text
    size_t i = 0;
    while (text[i] == ' ' && i < text.size()) { ++i; }

    // Consume as many numbers we can first, ignoring the spaces
    std::stringstream numbers;
    std::stringstream format;
    bool is_float = false;
    bool parse_format = false;
    for (; i < text.size(); i++) {
        char c = text[i];
        if (!parse_format && c >= '0' && c <= '9') {
            numbers << c;
        } else if (!parse_format && !is_float && c == '.') {
            // Float, still continue
            numbers << c;
            is_float = true;
        } else if (!parse_format && is_float && c == '.') {
            throw std::invalid_argument("More than one period in number.");
        } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            // Parse as format from now on
            format << c;
            parse_format = true;
        } else if (c == ' ' || c == '\t' || c == '\n') {
            // Always skip, but disable number mode
            parse_format = true;
        } else {
            // Always unknown character
            throw std::invalid_argument("Illegal character '" + std::string(readable_char(c)) + "'.");
        }
    }

    // First, try to parse the numbers
    std::string snumbers = numbers.str();
    if (snumbers.size() == 0) { throw std::invalid_argument("No number given."); }
    float inumbers = std::stof(snumbers);

    // Next, try to parse the format
    std::string sformat = format.str();
    if (sformat.size() == 0) { throw std::invalid_argument("No format given."); }
    else if (sformat == "b") {
        // Bits, divide by 8
        inumbers /= 8.0f;
    } else if (sformat == "B") {
        // Bytes, no change
    } else if (sformat == "Kb") {
        // Kilobits, divide by 8 but times 1000
        inumbers *= 1000.0f / 8.0f;
    } else if (sformat == "KB") {
        // Kilobytes, times 1000
        inumbers *= 1000.0f;
    } else if (sformat == "Kib") {
        // Kibibits, divide by 8 but times 1024
        inumbers *= 1024.0f / 8.0f;
    } else if (sformat == "KiB") {
        // Kibibytes, times 1024
        inumbers *= 1024.0f;
    } else if (sformat == "Mb") {
        // Kilobits, divide by 8 but times 1000000
        inumbers *= 1000.0f * 1000.0f / 8.0f;
    } else if (sformat == "MB") {
        // Kilobytes, times 1000000
        inumbers *= 1000.0f * 1000.0f;
    } else if (sformat == "Mib") {
        // Kibibits, divide by 8 but times 1024 * 1024
        inumbers *= 1024.0f * 1024.0f / 8.0f;
    } else if (sformat == "MiB") {
        // Kibibytes, times 1024 * 1024
        inumbers *= 1024.0f * 1024.0f;
    } else if (sformat == "Gb") {
        // Kilobits, divide by 8 but times 1000000000
        inumbers *= 1000.0f * 1000.0f * 1000.0f / 8.0f;
    } else if (sformat == "GB") {
        // Kilobytes, times 1000000000
        inumbers *= 1000.0f * 1000.0f * 1000.0f;
    } else if (sformat == "Gib") {
        // Kibibits, divide by 8 but times 1024 * 1024 * 1024
        inumbers *= 1024.0f * 1024.0f * 1024.0f / 8.0f;
    } else if (sformat == "GiB") {
        // Kibibytes, times 1024 * 1024 * 1024
        inumbers *= 1024.0f * 1024.0f * 1024.0f;
    } else if (sformat == "Tb") {
        // Kilobits, divide by 8 but times 1000000000000
        inumbers *= 1000.0f * 1000.0f * 1000.0f * 1000.0f / 8.0f;
    } else if (sformat == "TB") {
        // Kilobytes, times 1000000000000
        inumbers *= 1000.0f * 1000.0f * 1000.0f * 1000.0f;
    } else if (sformat == "Tib") {
        // Kibibits, divide by 8 but times 1024 * 1024 * 1024 * 1024
        inumbers *= 1024.0f * 1024.0f * 1024.0f * 1024.0f / 8.0f;
    } else if (sformat == "TiB") {
        // Kibibytes, times 1024 * 1024 * 1024 * 1024
        inumbers *= 1024.0f * 1024.0f * 1024.0f * 1024.0f;
    } else {
        throw std::invalid_argument("Unsupported format '" + sformat + "'.");
    }

    // Try to cast to size_t
    if (round(inumbers) > (float) std::numeric_limits<size_t>::max()) {
        throw std::out_of_range("'" + std::to_string(inumbers) + "' is too large for a 64-bit unsigned integer (max " + std::to_string(std::numeric_limits<size_t>::max()) + ").");
    }
    
    // Otherwise, return the casted version!
    return (size_t) round(inumbers);
}
