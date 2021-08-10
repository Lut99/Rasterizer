/* TOKENIZER TOOLS.hpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 21:36:19
 * Last edited:
 *   07/08/2021, 21:42:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains common macros and functions for Tokenizers.
**/

#ifndef MODELS_TOKENIZER_TOOLS_HPP
#define MODELS_TOKENIZER_TOOLS_HPP

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <istream>
#include <string>
#include "tools/Typenames.hpp"

/* If defined, enables extra debug prints tracing the tokenizer's steps. */
// #define EXTRA_DEBUG

/* Allows tokens to be converted to a string. */
#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

#ifdef EXTRA_DEBUG
/* Helper macro that prints what the token was we got for this label. */
#define DEBUG_LABEL(LABEL, C) \
    printf("[tokenizer] Label " STR(LABEL) ": '%s'\n", readable_char((C)));
/* Helper macro that prints the path we took. */
#define DEBUG_PATH(C, NEXT) \
    printf("[tokenizer] --> Found %s, %s\n", (C), (NEXT));
#else
/* Helper macro that prints what the token was we got for this label. */
#define DEBUG_LABEL(LABEL, C);
/* Helper macro that prints the path we took. */
#define DEBUG_PATH(C, NEXT);
#endif

#ifdef _WIN32
/* Helper macro for fetching characters of the input stream. Uses Windows-style error handling */
#define GET_CHAR(C, FILE, COL, I) \
    (FILE)->get((C)); \
    if ((FILE)->eof()) { \
        (C) = EOF; \
    } else if ((FILE)->fail()) { \
        char buffer[BUFSIZ]; \
        strerror_s(buffer, errno); \
        std::string err = buffer; \
        DLOG(CppDebugger::Severity::fatal, "Something went wrong while reading from the stream: " + err); \
    } \
    ++(COL); \
    ++(I);

#else
/* Helper macro for fetching characters of the input stream. Uses Unix-style error handling */
#define GET_CHAR(C, FILE, COL, I) \
    (FILE)->get((C)); \
    if ((FILE)->eof()) { \
        (C) = EOF; \
    } else if ((FILE)->fail()) { \
        std::string err = strerror(errno); \
        DLOG(CppDebugger::Severity::fatal, "Something went wrong while reading from the stream: " + err); \
    } \
    ++(COL); \
    ++(I);

#endif
    
/* Helper macro for ungetting the last fetched character. */
#define UNGET_CHAR(FILE, COL) \
    (FILE)->seekg(-1, ios_base::cur); \
    --(COL);

/* Parses an unsigned integer. */
#define PARSE_UINT(VALUE, RAW_VALUE, DEBUG_INFO) \
    try { \
        unsigned long lvalue = std::stoul((RAW_VALUE)); \
        if (lvalue > std::numeric_limits<uint32_t>::max()) { throw std::out_of_range("Manual overflow"); } \
        (VALUE) = (uint32_t) lvalue; \
    } catch (std::invalid_argument& e) { \
        (DEBUG_INFO).print_error(cerr, "Illegal character parsing an unsigned integer: " + std::string(e.what())); \
        DRETURN nullptr; \
    } catch (std::out_of_range&) { \
        (DEBUG_INFO).print_error(cerr, "Value is out-of-range for a 32-bit unsigned integer (maximum: " + std::to_string(std::numeric_limits<uint32_t>::max()) + ")."); \
        DRETURN nullptr; \
    }

/* Helper macro for determining if the given character is a whitespce. */
#define IS_WHITESPACE(C) \
    ((C) == ' ' || (C) == '\n' || (C) == '\t' || (C) == '\r')



namespace Rasterizer::Models {
    // /* Function that, given a file stream and the start of this line, parses an entire line. */
    // std::string get_line(FILE* file, long sentence_start);
    /* Function that, given a file stream and the start of this line, parses an entire line. */
    std::string get_line(std::istream* is, std::streampos sentence_start);

    /* Given a char, returns a readable string representation of it. */
    const char* readable_char(char c);

    /* Returns a string representing the given number, padded with enough spaces to be at least N character long. */
    template <class T>
    std::string pad_spaces(T value, size_t N) {
        DENTER("Models::pad_zeros<" + Tools::type_sname<T>() + ">");
        
        // Convert to string
        std::string result = std::to_string(value);
        while (result.size() < N) {
            result = ' ' + result;
        }

        // DOne
        DRETURN result;
    }

    /* Splits a given string in two strings on the first slash it finds. */
    void split_string(const std::string& to_split, std::string& part1, std::string& part2);
    /* Splits a given string in three strings on the first and second slash it finds. */
    void split_string(const std::string& to_split, std::string& part1, std::string& part2, std::string& part3);

}

#endif
