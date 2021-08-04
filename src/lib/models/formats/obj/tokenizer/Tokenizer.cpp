/* TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:28:49
 * Last edited:
 *   04/08/2021, 18:41:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Tokenizer class for the .obj format. It's a very simple
 *   one.
**/

#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <tuple>
#include "tools/CppDebugger.hpp"

#include "ValueTerminal.hpp"
#include "Tokenizer.hpp"

using namespace std;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Obj;
using namespace CppDebugger::SeverityValues;


/***** MACROS *****/
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

/* Helper macro for fetching characters of the input stream. Uses Unix-style error handling */
#define GET_CHAR(C, FILE, COL, I) \
    (C) = fgetc((FILE)); \
    if ((C) == EOF && ferror((FILE))) { \
        std::string err = strerror(errno); \
        DLOG(fatal, "Something went wrong while reading from the stream: " + err); \
    } \
    ++(COL); \
    ++(I);

/* Helper macro for fetching characters of the input stream. Uses Windows-style error handling */
#define GET_CHAR_W(C, FILE, COL, I) \
    (C) = fgetc((FILE)); \
    if ((C) == EOF && ferror((FILE))) { \
        char buffer[BUFSIZ]; \
        strerror_s(buffer, errno); \
        std::string err = buffer; \
        DLOG(fatal, "Something went wrong while reading from the stream: " + err); \
    } \
    ++(COL); \
    ++(I);
    
/* Helper macro for ungetting the last fetched character. */
#define UNGET_CHAR(FILE, COL) \
    fseek((FILE), -1, SEEK_CUR); \
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





/***** HELPER FUNCTIONS *****/
/* Function that, given a file stream and the start of this line, parses an entire line. */
static std::string get_line(FILE* file, long sentence_start) {
    DENTER("get_line");

    // Backup the current cursor and go to the start of the line
    long old_cursor = ftell(file);
    // Go to the start of the line
    fseek(file, sentence_start, SEEK_SET);

    // Loop to assemble the line
    char c;
    int col = 0;
    int i = 0;
    std::stringstream sstr;
    while (true) {
        // Get the character
        #ifdef _WIN32
        GET_CHAR_W(c, file, col, i);
        #else
        GET_CHAR(c, file, col, i);
        #endif

        // If it's a newline, stop
        if (c == '\n' || c == EOF) {
            fseek(file, old_cursor, SEEK_SET);
            DRETURN sstr.str();
        }

        // Otherwise, store and re-try
        sstr << c;
    }

    // We should never get here
    DRETURN "";
}

/* Given a char, returns a readable string representation of it. */
static const char* readable_char(char c) {
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
        case '\n': return "newline";
        case '\r': return "carriage return";
        case '\t': return "tab";
        case '\0': return "null";
        default: return "special char";
    }
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

/* Splits a given string in two strings on the first slash it finds. */
static void split_string(const std::string& to_split, std::string& part1, std::string& part2) {
    DENTER("split_string(2)");

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
    DRETURN;
}

/* Splits a given string in three strings on the first and second slash it finds. */
static void split_string(const std::string& to_split, std::string& part1, std::string& part2, std::string& part3) {
    DENTER("split_string(3)");

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
    DRETURN;
}





/***** TOKENIZER CLASS *****/
/* Constructor for the Tokenizer class, which takes the path to the file to tokenizer. */
Tokenizer::Tokenizer(const std::string& path) :
    path(path),
    line(1),
    col(0),
    last_sentence_start(0)
{
    DENTER("Models::Obj::Tokenizer::Tokenizer");

    // First, open a handle
    #ifdef _WIN32
    int res = fopen_s(&this->file, this->path.c_str(), "r");
    if (res != 0) {
        // Get the error number
        char buffer[BUFSIZ];
        strerror_s(buffer, errno);
        std::string err = buffer;

        // Show the error message
        DLOG(fatal, "Could not open file handle: " + err)
    }

    #else
    this->file = fopen(this->path.c_str(), "r");
    if (this->file == NULL) {
        // Get the error number
        std::string err = strerror(errno);

        // Show the error message
        DLOG(fatal, "Could not open file handle: " + err)
    }
    #endif

    // Done
    DLEAVE;
}

/* Move constructor for the Tokenizer class. */
Tokenizer::Tokenizer(Tokenizer&& other) :
    file(other.file),
    path(other.path),

    line(other.line),
    col(other.col),
    last_sentence_start(other.last_sentence_start),
    terminal_buffer(other.terminal_buffer)
{
    // Clear the other's list of terminal buffers to not deallocate them
    other.file = NULL;
    other.terminal_buffer.clear();
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    DENTER("Obj::Tokenizer::~Tokenizer");

    for (uint32_t i = 0; i < this->terminal_buffer.size(); i++) {
        delete this->terminal_buffer[i];
    }
    if (this->file != NULL) {
        fclose(this->file);
    }

    DLEAVE;
}



/* Returns the next Token from the stream. If no more tokens are available, returns an EOF token. Note that, due to polymorphism, the token is allocated on the heap and has to be deallocated manually. */
Terminal* Tokenizer::get() {
    DENTER("Models::Obj::Tokenizer::get");

    // If there's something in the buffer, pop that off first
    if (this->terminal_buffer.size() > 0) {
        Terminal* result = this->terminal_buffer.last();
        this->terminal_buffer.pop_back();
        DRETURN result;
    }

    // Prepare some variables
    char c;
    int i = 0;
    size_t line_start, col_start;
    Terminal* to_return;
    std::stringstream sstr;



start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(start, c);

    // Switch on its value
    if (c == 'v') {
        // It's a vertex start symbol; simple token
        DEBUG_PATH("vertex", "checking for whitespace");
        line_start = this->line;
        col_start = this->col;
        goto vertex;

    } else if (c == 'f') {
        // It's a face start symbol; simple token
        DEBUG_PATH("face", "checking for whitespace");
        line_start = this->line;
        col_start = this->col;
        goto face;
    
    } else if (c == 'g') {
        // It's a group start symbol; simple token, but harder parsing afterwards
        DEBUG_PATH("group", "going g_end");
        line_start = this->line;
        col_start = this->col;
        goto g_end;

    } else if (c == 'm') {
        // Might be mtllib or something
        DEBUG_PATH("mtllib", "going m_start");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto m_start;
    
    } else if (c == 'u') {
        // Could be usemtl
        DEBUG_PATH("usemtl", "going u_start");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto u_start;

    } else if (c >= '0' && c <= '9') {
        // It's a digit, but we do not yet known which
        DEBUG_PATH("digit", "going digit_start");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto digit_start;

    } else if (c == '-' || c == '.') {
        // It's a floating-point that's negative for sure
        DEBUG_PATH("minus/dot", "going float_start");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto float_start;
    
    } else if (c == '#') {
        // It's a comment; ignore until newline
        DEBUG_PATH("comment", "going comment_start");
        goto comment_start;

    } else if (IS_WHITESPACE(c)) {
        // If it's a newline, update the line counters
        DEBUG_PATH("whitespace", "retrying");
        if (c == '\n') {
            // cout << ' ' << pad_spaces(this->line, 4) << " | " << get_line(file, this->last_sentence_start) << endl;
            this->col = 0;
            ++this->line;
        }

        // Try again
        goto start;

    } else if (c == EOF) {
        // End-of-file; return that we reached it
        DEBUG_PATH("EOF", "done");
        DRETURN new Terminal(TerminalType::eof, DebugInfo(this->path, this->line, this->col, { get_line(file, this->last_sentence_start) }));

    } else {
        // Unexpected token
        DebugInfo debug(this->path, this->line, this->col, { get_line(file, this->last_sentence_start) });
        debug.print_error(cerr, std::string("Unexpected character '") + readable_char(c) + "'");
        DRETURN nullptr;

    }
}



vertex: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(vertex, c);

    // If not whitespace, parse as unknown keyword
    if (IS_WHITESPACE(c)) {
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        DRETURN new Terminal(TerminalType::vertex, DebugInfo(this->path, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));

    } else if (c == 'n') {
        DEBUG_PATH("n", "going vertex_normal");
        goto vertex_normal;

    } else if (c == 't') {
        DEBUG_PATH("t", "going vertex_texture");
        goto vertex_texture;

    } else {
        // Parse as unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        sstr << 'v';
        goto unknown_token;
    }
}

vertex_normal: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(vertex_normal, c);

    // If not whitespace, parse as unknown keyword
    if (IS_WHITESPACE(c)) {
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        DRETURN new Terminal(TerminalType::normal, DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));

    } else {
        // Parse as unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        sstr << "vn";
        goto unknown_token;
    }
}

vertex_texture: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(vertex_texture, c);

    // If not whitespace, parse as unknown keyword
    if (IS_WHITESPACE(c)) {
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        DRETURN new Terminal(TerminalType::texture, DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));

    } else {
        // Parse as unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        sstr << "vt";
        goto unknown_token;
    }
}



face: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(face, c);

    // If not whitespace, parse as unknown keyword
    if (IS_WHITESPACE(c)) {
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        DRETURN new Terminal(TerminalType::face, DebugInfo(this->path, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));
    } else {
        // Parse as unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        sstr << 'f';
        goto unknown_token;
    }
}



g_end: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(g_end, c);

    // If not whitespace, parse as unknown keyword
    if (IS_WHITESPACE(c)) {
        // It's the start of a groupname; we can parse the group token already
        DEBUG_PATH("letter/underscore", "going name_start");
        UNGET_CHAR(this->file, this->col);
        to_return = new Terminal(TerminalType::group, DebugInfo(this->path, line_start, col_start, { get_line(this->file, this->last_sentence_start) }));
        goto name_start;

    } else {
        // Parse as some other, unknown token
        DEBUG_PATH("other", "pasring as unknown token");
        UNGET_CHAR(this->file, this->col);
        sstr << 'g';
        goto unknown_token;

    }
}



m_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(m_start, c);

    // Switch depending on the number of times we've been here
    if (c == 't') {
        // Continue
        DEBUG_PATH("t", "going mt");
        sstr << c;
        goto mt;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

mt: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(mt, c);

    // Switch depending on the number of times we've been here
    if (c == 'l') {
        // Continue
        DEBUG_PATH("l", "going mtl");
        sstr << c;
        goto mtl;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

mtl: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(mtl, c);

    // Switch depending on the number of times we've been here
    if (c == 'l') {
        // Continue
        DEBUG_PATH("l", "going mtll");
        sstr << c;
        goto mtll;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

mtll: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(mtll, c);

    // Switch depending on the number of times we've been here
    if (c == 'i') {
        // Continue
        DEBUG_PATH("i", "going mtli");
        sstr << c;
        goto mtlli;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

mtlli: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(mtlli, c);

    // Switch depending on the number of times we've been here
    if (c == 'b') {
        // Done; check if the next is a whitespace as well
        DEBUG_PATH("b", "checking for whitespace");
        sstr << c;
        goto mtllib;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}
    
mtllib: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(mtllib, c);

    // Switch depending on the number of times we've been here
    if (IS_WHITESPACE(c)) {
        // We know for sure it's an mtllib token. Create it, then try to parse the filename (essentially entering a special tokenizer mode)
        DEBUG_PATH("whitespace", "going filename_start");
        UNGET_CHAR(this->file, this->col);
        to_return = new Terminal(TerminalType::mtllib, DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));
        goto filename_start;

    } else {
        // There is more keyword than we bargained for; continue
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}



u_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(u_start, c);

    // Switch depending on the character value
    if (c == 's') {
        // Continue
        DEBUG_PATH("s", "going us");
        sstr << c;
        goto us;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

us: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(us, c);

    // Switch depending on the character value
    if (c == 'e') {
        // Continue
        DEBUG_PATH("e", "going use");
        sstr << c;
        goto use;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

use: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(use, c);

    // Switch depending on the character value
    if (c == 'm') {
        // Continue
        DEBUG_PATH("m", "going usem");
        sstr << c;
        goto usem;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

usem: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(usem, c);

    // Switch depending on the character value
    if (c == 't') {
        // Continue
        DEBUG_PATH("t", "going usemt");
        sstr << c;
        goto usemt;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

usemt: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(usemt, c);

    // Switch depending on the character value
    if (c == 'l') {
        // Continue
        DEBUG_PATH("l", "checking whitespace");
        sstr << c;
        goto usemtl;
    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}

usemtl: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(usemtl, c);

    // Switch depending on the number of times we've been here
    if (IS_WHITESPACE(c)) {
        // We know for sure it's an mtllib token. Create it, then try to parse the filename (essentially entering a special tokenizer mode)
        DEBUG_PATH("whitespace", "going name_start");
        UNGET_CHAR(this->file, this->col);
        to_return = new Terminal(TerminalType::usemtl, DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));
        goto name_start;

    } else {
        // Unget the token to allow for whitespace to exist already, then consume the entire thing as an unknown keyword
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;
    }
}



digit_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(digit_start, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Simply add the digit and continue
        DEBUG_PATH("digit", "continuing");
        sstr << c;
        goto digit_start;
    
    } else if (c == '.') {
        // Surprise! It's a floating-point! Go to floating-point parsing
        DEBUG_PATH("dot", "going float_dot");
        sstr << c;
        goto float_dot;
    
    } else if (c == '/') {
        // We're looking at a combined texture instead
        DEBUG_PATH("slash", "going v_vt_start");
        sstr << c;
        goto v_vt_start;

    } else if (IS_WHITESPACE(c)) {
        // Unget the token
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        uint32_t value;
        PARSE_UINT(value, sstr.str(), debug_info);

        // Otherwise, we have a valid value
        DRETURN (Terminal*) new ValueTerminal<uint32_t>(TerminalType::uint, value, debug_info);

    } else {
        // Parse as unknown
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

v_vt_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(v_vt_start, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Simply add the digit and continue
        DEBUG_PATH("digit", "going v_vt");
        sstr << c;
        goto v_vt;
    
    } else if (c == '/') {
        // We're for sure looking at a v_vn now
        DEBUG_PATH("slash", "going v_vn_start");
        sstr << c;
        goto v_vn;

    } else if (IS_WHITESPACE(c)) {
        // Missing the second texture coordinate
        UNGET_CHAR(this->file, this->col);
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Missing texture coordinate in vertex/texture pair.");
        DRETURN nullptr;

    } else {
        // Parse as unknown
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

v_vt: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(v_vt, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Simply add the digit and continue
        DEBUG_PATH("digit", "continuing");
        sstr << c;
        goto v_vt;
    
    } else if (c == '/') {
        // It's another slash, thus a triplet!
        DEBUG_PATH("slash", "going v_vt_vn_start");
        sstr << c;
        goto v_vt_vn_start;

    } else if (IS_WHITESPACE(c)) {
        // Alright, we got a succesful pair!
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        Tools::Array<std::string> lines = { get_line(this->file, this->last_sentence_start) };
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, lines);

        // Try to parse the string as a uint32_t value
        std::string vertex, texture;
        split_string(sstr.str(), vertex, texture);
        printf("[v_vt] Split into: %s and %s\n", vertex.c_str(), texture.c_str());
        uint32_t ivertex, itexture;
        PARSE_UINT(ivertex, vertex, DebugInfo(this->path, line_start, col_start, this->line, col_start + vertex.size() - 1, lines));
        PARSE_UINT(itexture, texture, DebugInfo(this->path, line_start, col_start + vertex.size() + 1, this->line, this->col, lines));

        // Otherwise, we have a valid value
        DRETURN (Terminal*) new ValueTerminal<std::tuple<uint32_t, uint32_t>>(TerminalType::v_vt, { ivertex, itexture }, debug_info);

    } else {
        // Parse as unknown
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

v_vn: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(v_vn, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Simply add the digit and continue
        DEBUG_PATH("digit", "continuing");
        sstr << c;
        goto v_vn;
    
    } else if (c == '/') {
        // We cannot have another slash
        DEBUG_PATH("slash", "going too_many_slashes");
        sstr << c;
        goto too_many_slashes;

    } else if (IS_WHITESPACE(c)) {
        // Alright, we got a succesful pair!
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        Tools::Array<std::string> lines = { get_line(this->file, this->last_sentence_start) };
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, lines);

        // Try to parse the string as a uint32_t value
        std::string vertex, dummy, normal;
        split_string(sstr.str(), vertex, dummy, normal);
        printf("[v_vn] Split into: %s and %s\n", vertex.c_str(), normal.c_str());
        uint32_t ivertex, inormal;
        PARSE_UINT(ivertex, vertex, DebugInfo(this->path, line_start, col_start, this->line, col_start + vertex.size() - 1, lines));
        PARSE_UINT(inormal, normal, DebugInfo(this->path, line_start, col_start + vertex.size() + 1, this->line, this->col, lines));

        // Otherwise, we have a valid value
        DRETURN (Terminal*) new ValueTerminal<std::tuple<uint32_t, uint32_t>>(TerminalType::v_vn, { ivertex, inormal }, debug_info);

    } else {
        // Parse as unknown
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

v_vt_vn_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(v_vt_vn_start, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Simply add the digit and continue
        DEBUG_PATH("digit", "going v_vt_vn");
        sstr << c;
        goto v_vt_vn;
    
    } else if (c == '/') {
        // Too many slashes!
        DEBUG_PATH("slash", "going too_many_slashes");
        sstr << c;
        goto too_many_slashes;

    } else if (IS_WHITESPACE(c)) {
        // Missing the third texture coordinate
        UNGET_CHAR(this->file, this->col);
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Missing normal coordinate in vertex/texture/normal pair.");
        DRETURN nullptr;

    } else {
        // Parse as unknown
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }

}

v_vt_vn: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(v_vt_vn, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Simply add the digit and continue
        DEBUG_PATH("digit", "continuing");
        sstr << c;
        goto v_vt_vn;
    
    } else if (c == '/') {
        // We cannot have another slash
        DEBUG_PATH("slash", "going too_many_slashes");
        sstr << c;
        goto too_many_slashes;

    } else if (IS_WHITESPACE(c)) {
        // Alright, we got a succesful pair!
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        Tools::Array<std::string> lines = { get_line(this->file, this->last_sentence_start) };
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, lines);

        // Try to parse the string as a uint32_t value
        std::string vertex, texture, normal;
        split_string(sstr.str(), vertex, texture, normal);
        printf("[v_vt_vn] Split into: %s, %s and %s\n", vertex.c_str(), texture.c_str(), normal.c_str());
        uint32_t ivertex, itexture, inormal;
        PARSE_UINT(ivertex, vertex, DebugInfo(this->path, line_start, col_start, this->line, col_start + vertex.size() - 1, lines));
        PARSE_UINT(itexture, texture, DebugInfo(this->path, line_start, col_start + vertex.size() + 1, this->line, col_start + vertex.size() + texture.size(), lines));
        PARSE_UINT(inormal, normal, DebugInfo(this->path, line_start, col_start + vertex.size() + texture.size() + 2, this->line, this->col, lines));

        // Otherwise, we have a valid value
        DRETURN (Terminal*) new ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>(TerminalType::v_vt_vn, { ivertex, itexture, inormal }, debug_info);

    } else {
        // Parse as unknown
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

too_many_slashes: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(too_many_slashes, c);

    // Switch on its value
    if (IS_WHITESPACE(c)) {
        // We can now error properly
        UNGET_CHAR(this->file, this->col);
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Too many coordinates for face coordinate; only supports [v], [v/vt], [v//vn] or [v/vt/vn].");
        DRETURN nullptr;

    } else {
        // Continue parsing
        DEBUG_PATH("other", "continuing");
        goto too_many_slashes;

    }
}



float_start: {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col, i);
        #else
        GET_CHAR(c, this->file, this->col, i);
        #endif
        DEBUG_LABEL(float_start, c);

        // Switch on its value
        if (c >= '0' && c <= '9') {
            // Keep parsing as normal number
            DEBUG_PATH("digit", "continuing");
            sstr << c;
            goto float_start;

        } else if (c == '.') {
            // It's a floating-point for real
            DEBUG_PATH("dot", "going float_dot");
            sstr << c;
            goto float_dot;

        } else if (IS_WHITESPACE(c)) {
            // We haven't seen a dot yet! So parse as int
            DEBUG_PATH("whitespace", "done (but returning as sint)");
            UNGET_CHAR(this->file, this->col);

            // Create the debug info
            DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });

            // Try to parse the string as a uint32_t value
            int32_t value;
            try {
                value = std::stoi(sstr.str());
            } catch (std::invalid_argument& e) {
                // printf("'\n");
                debug_info.print_error(cerr, "Illegal character parsing an unsigned integer: " + std::string(e.what()));
                DRETURN nullptr;
            } catch (std::out_of_range&) {
                // printf("'\n");
                debug_info.print_error(cerr, "Value is out-of-range for a 32-bit integer (maximum: " + std::to_string(std::numeric_limits<uint32_t>::max()) + ").");
                DRETURN nullptr;
            }

            // Otherwise, we have a valid value
            DRETURN (Terminal*) new ValueTerminal<int32_t>(TerminalType::sint, value, debug_info);

        } else {
            // Parse as unknown token
            DEBUG_PATH("other", "parsing as unknown token");
            UNGET_CHAR(this->file, this->col);
            goto unknown_token;

        }
    }

float_dot: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(float_dot, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Keep parsing as normal number
        DEBUG_PATH("digit", "continuing");
        sstr << c;
        goto float_dot;
    
    } else if (c == 'E' || c == 'e') {
        // It's definitely a float now, but then with scientific notation
        DEBUG_PATH("E", "going float_e");
        sstr << c;
        goto float_e_start;

    } else if (IS_WHITESPACE(c)) {
        // Unget the token
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        float value;
        try {
            value = std::stof(sstr.str());
        } catch (std::invalid_argument& e) {
            debug_info.print_error(cerr, "Illegal character parsing a decimal number: " + std::string(e.what()));
            // printf("'\n");
            DRETURN nullptr;
        } catch (std::out_of_range&) {
            debug_info.print_error(cerr, "Value is out-of-range for a 32-bit floating-point (maximum: " + std::to_string(std::numeric_limits<float>::max()) + ").");
            // printf("'\n");
            DRETURN nullptr;
        }

        // Otherwise, we have a valid value
        DRETURN (Terminal*) new ValueTerminal<float>(TerminalType::decimal, value, debug_info);

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

float_e_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(float_e_start, c);

    // Switch on its value
    if ((c >= '0' && c <= '9') || c == '-') {
        // Keep parsing as number
        DEBUG_PATH("digit/minus", "going float_e");
        sstr << c;
        goto float_e;

    } else if (IS_WHITESPACE(c)) {
        // Found float with E but without value
        UNGET_CHAR(this->file, this->col);
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Encountered scientific 'E' but without power.");
        DRETURN nullptr;

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

float_e: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(float_e, c);

    // Switch on its value
    if (c >= '0' && c <= '9') {
        // Keep parsing as number
        DEBUG_PATH("digit", "going float_e");
        sstr << c;
        goto float_e;

    } else if (IS_WHITESPACE(c)) {
        // Unget the token
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        float value;
        try {
            value = std::stof(sstr.str());
        } catch (std::invalid_argument& e) {
            debug_info.print_error(cerr, "Illegal character parsing a decimal number: " + std::string(e.what()));
            // printf("'\n");
            DRETURN nullptr;
        } catch (std::out_of_range&) {
            debug_info.print_error(cerr, "Value is out-of-range for a 32-bit floating-point (maximum: " + std::to_string(std::numeric_limits<float>::max()) + ").");
            // printf("'\n");
            DRETURN nullptr;
        }

        // Otherwise, we have a valid value
        DRETURN (Terminal*) new ValueTerminal<float>(TerminalType::decimal, value, debug_info);

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}



filename_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(filename_start, c);

    // Switch on the character
    if (c == '\n') {
        // It's over before we begun; return only the to_return
        UNGET_CHAR(this->file, this->col);
        DRETURN to_return;

    } else if (IS_WHITESPACE(c)) {
        // Keep retrying until we find something non-whitespacey
        DEBUG_PATH("whitespace", "retrying");
        goto filename_start;

    } else {
        // Keep parsing as filename
        DEBUG_PATH("other", "going filename_contd");
        sstr.str("");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto filename_contd;

    }
}

filename_contd: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(filename_contd, c);

    // Switch on the character
    if (c == '\n') {
        // That's the filename, apparently; store the filename token on the unget stack
        DEBUG_PATH("newline", "done");
        UNGET_CHAR(this->file, this->col);
        ++this->line;
        this->terminal_buffer.push_back((Terminal*) new ValueTerminal<std::string>(TerminalType::filename, sstr.str(), DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) })));
        DRETURN to_return;

    } else {
        // Keep parsing as filename
        DEBUG_PATH("other", "continuing");
        sstr << c;
        goto filename_contd;

    }
}



name_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(name_start, c);

    // Switch on the character's value
    if (c == '\n') {
        // It's over before we begun; return only the to_return
        UNGET_CHAR(this->file, this->col);
        DRETURN to_return;

    } else if (IS_WHITESPACE(c)) {
        // Wait until we find a non-whitespace
        DEBUG_PATH("whitespace", "retrying");
        goto name_start;

    } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        // Looks like the start of the name; keep going
        DEBUG_PATH("letter/underscore", "going name_contd");
        sstr.str("");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto name_contd;

    } else {
        // Not a name token!
        DebugInfo debug_info(this->path, this->line, this->col - 1, { get_line(this->file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Illegal name character '" + std::string(readable_char(c)) + "'.");
        delete to_return;
        DRETURN nullptr;

    }

}

name_contd: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(name_contd, c);

    // Switch on the character's value
    if (IS_WHITESPACE(c)) {
        // Reached the endof the filename
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        if (c == '\n') { ++this->line; }
        this->terminal_buffer.push_back((Terminal*) new ValueTerminal<std::string>(TerminalType::name, sstr.str(), DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) })));
        DRETURN to_return;

    } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        // Keep parsing as name
        DEBUG_PATH("letter/underscore", "continuing");
        sstr << c;
        goto name_contd;

    } else {
        // There is more keyword than we bargained for; continue
        DEBUG_PATH("other", "parsing as unknown token");
        delete to_return;
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}



comment_start: {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col, i);
        #else
        GET_CHAR(c, this->file, this->col, i);
        #endif
        DEBUG_LABEL(comment_start, c);

        // Switch on its value
        if (c == '\n') {
            // We're done; reset to start
            DEBUG_PATH("newline", "going to start");
            UNGET_CHAR(this->file, this->col);
            goto start;
        } else {
            // Continue popping
            DEBUG_PATH("other", "retrying");
            goto comment_start;
        }
    }

unknown_token: {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col, i);
        #else
        GET_CHAR(c, this->file, this->col, i);
        #endif
        DEBUG_LABEL(unknown_token, c);

        // Switch on the character's value
        if (IS_WHITESPACE(c)) {
            // We found the end of the token; unget it, then throw an error
            UNGET_CHAR(this->file, this->col);
            DebugInfo debug(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });
            debug.print_error(cerr, "Unknown token '" + sstr.str() + "'.");
            DRETURN nullptr;
        } else {
            // Keep consuming
            DEBUG_PATH("other", "retrying");
            sstr << c;
            goto unknown_token;
        }
     }



    // We should never get here
    DLOG(fatal, "Hole in jump logic encountered; reached point we should never reach");
    DRETURN nullptr;
}

/* Puts a token back on the internal list of tokens, so it can be returned next get call. Note that the Tokenizer will deallocate these if it gets deallocated. */
void Tokenizer::unget(Terminal* term) {
    DENTER("Models::Obj::Tokenizer::unget");

    // Put the token back on the list of tokens
    this->terminal_buffer.push_back(term);

    // Done
    DRETURN;
}


        
/* Swap operator for the Tokenizer class. */
void Obj::swap(Tokenizer& t1, Tokenizer& t2) {
    using std::swap;

    swap(t1.file, t2.file);
    swap(t1.path, t2.path);
    
    swap(t1.line, t2.line);
    swap(t1.col, t2.col);
    swap(t1.last_sentence_start, t2.last_sentence_start);
    swap(t1.terminal_buffer, t2.terminal_buffer);
}
