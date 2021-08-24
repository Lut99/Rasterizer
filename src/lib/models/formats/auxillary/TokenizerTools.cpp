/* TOKENIZER TOOLS.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 21:39:42
 * Last edited:
 *   07/08/2021, 21:44:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains common macros and functions for Tokenizers.
**/

#include <sstream>

#include "TokenizerTools.hpp"

using namespace std;
using namespace Rasterizer;


// /***** LIBRARY FUNCTIONS *****/
// /* Function that, given a file stream and the start of this line, parses an entire line. */
// std::string Models::get_line(FILE* file, long sentence_start) {
//     

//     // Backup the current cursor and go to the start of the line
//     long old_cursor = ftell(file);
//     // Go to the start of the line
//     fseek(file, sentence_start, SEEK_SET);

//     // Loop to assemble the line
//     char c;
//     int col = 0;
//     int i = 0;
//     std::stringstream sstr;
//     while (true) {
//         // Get the character
//         GET_CHAR(c, file, col, i);

//         // If it's a newline, stop
//         if (c == '\n' || c == EOF) {
//             fseek(file, old_cursor, SEEK_SET);
//             return sstr.str();
//         }

//         // Otherwise, store and re-try
//         sstr << c;
//     }

//     // We should never get here
//     return "";
// }

/* Function that, given a file stream and the start of this line, parses an entire line. */
std::string Models::get_line(std::istream* is, std::streampos sentence_start) {
    

    // // Backup the current cursor and go to the start of the line
    // std::streampos old_cursor = is->tellg();
    // // Go to the start of the line
    // is->seekg(sentence_start);

    // // Loop to assemble the line
    // char c;
    // int col = 0;
    // int i = 0;
    // std::stringstream sstr;
    // while (true) {
    //     // Get the character
    //     GET_CHAR(c, is, col, i);

    //     // If it's a newline, stop
    //     if (c == '\n' || c == EOF) {
    //         is->seekg(old_cursor);
    //         is->clear();
    //         return sstr.str();
    //     }

    //     // Otherwise, store and re-try
    //     sstr << c;
    // }

    // We should never get here
    return "";
}



/* Given a char, returns a readable string representation of it. */
const char* Models::readable_char(char c) {
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
        case '0': return "0";
        case '1': return "1";
        case '2': return "2";
        case '3': return "3";
        case '4': return "4";
        case '5': return "5";
        case '6': return "6";
        case '7': return "7";
        case '8': return "8";
        case '9': return "9";
        case ' ': return " ";
        case '~': return "~";
        case '`': return "`";
        case '!': return "!";
        case '@': return "@";
        case '#': return "#";
        case '$': return "$";
        case '%': return "%";
        case '^': return "^";
        case '&': return "&";
        case '*': return "*";
        case '(': return "(";
        case ')': return ")";
        case '-': return "-";
        case '_': return "_";
        case '=': return "=";
        case '+': return "+";
        case '{': return "{";
        case '}': return "}";
        case '[': return "[";
        case ']': return "]";
        case ';': return ";";
        case ':': return ":";
        case '\'': return "'";
        case '"': return "\"";
        case '\\': return "\\";
        case '|': return "|";
        case ',': return ",";
        case '.': return ".";
        case '<': return "<";
        case '>': return ">";
        case '/': return "/";
        case '?': return "?";
        case '\n': return "newline";
        case '\r': return "carriage return";
        case '\t': return "tab";
        case '\0': return "null";
        default: return "special char";
    }
}



/* Splits a given string in two strings on the first slash it finds. */
void Models::split_string(const std::string& to_split, std::string& part1, std::string& part2) {
    // Loop to find the slash, noting everything in the stringstream
    std::stringstream sstr;
    size_t i = 0;
    for ( ; i < to_split.size(); i++) {
        if (to_split[i] == '/') { ++i; break; }
        sstr << to_split[i];
    }
    part1 = sstr.str();

    // Do the same for the second half
    sstr.str("");
    for ( ; i < to_split.size(); i++) {
        sstr << to_split[i];
    }
    part2 = sstr.str();

    // Done
    return;
}

/* Splits a given string in three strings on the first and second slash it finds. */
void Models::split_string(const std::string& to_split, std::string& part1, std::string& part2, std::string& part3) {
    // Loop to find the slash, noting everything in the stringstream
    std::stringstream sstr;
    size_t i = 0;
    for ( ; i < to_split.size(); i++) {
        if (to_split[i] == '/') { ++i; break; }
        sstr << to_split[i];
    }
    part1 = sstr.str();

    // Do the same for the second half
    sstr.str("");
    for ( ; i < to_split.size(); i++) {
        if (to_split[i] == '/') { ++i; break; }
        sstr << to_split[i];
    }
    part2 = sstr.str();

    // And for the third
    sstr.str("");
    for ( ; i < to_split.size(); i++) {
        sstr << to_split[i];
    }
    part3 = sstr.str();

    // Done
    return;
}

