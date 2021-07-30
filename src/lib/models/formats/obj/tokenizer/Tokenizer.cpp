/* TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:28:49
 * Last edited:
 *   04/07/2021, 16:28:49
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
#include "tools/CppDebugger.hpp"

#include "ValueTerminal.hpp"
#include "Tokenizer.hpp"

using namespace std;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Obj;
using namespace CppDebugger::SeverityValues;


/***** MACROS *****/
/* Helper macro for fetching characters of the input stream. Uses Unix-style error handling */
#define GET_CHAR(C, FILE, COL) \
    (C) = fgetc((FILE)); \
    if ((C) == EOF && ferror((FILE))) { \
        std::string err = strerror(errno); \
        DLOG(fatal, "Something went wrong while reading from the stream: " + err); \
    } \
    ++(COL);

/* Helper macro for fetching characters of the input stream. Uses Windows-style error handling */
#define GET_CHAR_W(C, FILE, COL) \
    (C) = fgetc((FILE)); \
    if ((C) == EOF && ferror((FILE))) { \
        char buffer[BUFSIZ]; \
        strerror_s(buffer, errno); \
        std::string err = buffer; \
        DLOG(fatal, "Something went wrong while reading from the stream: " + err); \
    } \
    ++(COL);

/* Helper macro for determining if the given character is a whitespce. */
#define IS_WHITESPACE(C) \
    ((C) == ' ' || (C) == '\n' || (C) == '\t')





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
    std::stringstream sstr;
    while (true) {
        // Get the character
        #ifdef _WIN32
        GET_CHAR_W(c, file, col);
        #else
        GET_CHAR(c, file, col);
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
        this->terminal_buffer.resize(this->terminal_buffer.size() - 1);
        DRETURN result;
    }

    // Prepare some variables
    char c;
    size_t line_start, col_start;
    std::stringstream sstr;
    // printf("\nNew try: '");



start:
    {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col);
        #else
        GET_CHAR(c, this->file, this->col);
        #endif
        // printf("start: %c\n", c);

        // Switch on its value
        if (c == 'v') {
            // It's a vertex start symbol; simple token
            // printf("--> Found 'vertex' token, done\n");
            // printf("'\n");
            DRETURN new Terminal(TerminalType::vertex, DebugInfo(this->path, this->line, this->col, { get_line(file, this->last_sentence_start) }));

        } else if (c == 'f') {
            // It's a face start symbol; simple token
            // printf("--> Found 'face' token, done\n");
            // printf("'\n");
            DRETURN new Terminal(TerminalType::face, DebugInfo(this->path, this->line, this->col, { get_line(file, this->last_sentence_start) }));
        
        } else if (c == 'm') {
            // Might be mtllib or something
            sstr << c;
            line_start = this->line;
            col_start = this->col;
            goto m_start;

        } else if (c >= '0' && c <= '9') {
            // It's a digit, but we do not yet known which
            // printf("--> Found digit, going digit_start\n");
            sstr << c;
            line_start = this->line;
            col_start = this->col;
            goto digit_start;

        } else if (c == '-' || c == '.') {
            // It's a floating-point that's negative for sure
            // printf("--> Found minus/dot, going float_start\n");
            sstr << c;
            line_start = this->line;
            col_start = this->col;
            goto float_start;
        
        } else if (c == '#') {
            // It's a comment; ignore until newline
            // printf("--> Found comment, going comment_start\n");
            goto comment_start;

        } else if (IS_WHITESPACE(c)) {
            // If it's a newline, update the line counters
            // printf("--> Found whitespace, retrying\n");
            if (c == '\n') {
                this->col = 0;
                ++this->line;
            }

            // Try again
            goto start;

        } else if (c == EOF) {
            // End-of-file; return that we reached it
            // printf("--> Found EOF, done\n");
            // printf("'\n");
            DRETURN new Terminal(TerminalType::eof, DebugInfo(this->path, this->line, this->col, { get_line(file, this->last_sentence_start) }));

        } else {
            // Unexpected token
            DebugInfo debug(this->path, this->line, this->col, { get_line(file, this->last_sentence_start) });
            // printf("'\n");
            debug.print_error(cerr, (std::string("Unexpected character '") += c) + "'");
            DRETURN nullptr;

        }
    }



m_start:
    {
        std::string to_parse = "mtllib";
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col);
        #else
        GET_CHAR(c, this->file, this->col);
        #endif
        // printf("m_start: %c\n", c);

        // Switch on its value
        if (c == 't') {
            // On our way to mtllib, it seems

        }
    }



digit_start:
    {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col);
        #else
        GET_CHAR(c, this->file, this->col);
        #endif
        // printf("digit_start: %c\n", c);

        // Switch on its value
        if (c >= '0' && c <= '9') {
            // Simply add the digit and continue
            // printf("--> Found digit, continuing\n");
            sstr << c;
            goto digit_start;
        
        } else if (c == '.') {
            // Surprise! It's a floating-point! Go to floating-point parsing
            // printf("--> Found dot, going float_dot\n");
            sstr << c;
            goto float_dot;

        } else {
            // Unget the token
            // printf("--> Found other token, done\n");
            fseek(this->file, -1, SEEK_CUR);
            --this->col;

            // Create the debug info
            DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });

            // Try to parse the string as a uint32_t value
            uint32_t value;
            try {
                unsigned long lvalue = std::stoul(sstr.str());
                if (lvalue > std::numeric_limits<uint32_t>::max()) { throw std::out_of_range("Manual overflow"); }
                value = (uint32_t) lvalue;
            } catch (std::invalid_argument& e) {
                // printf("'\n");
                debug_info.print_error(cerr, "Illegal character parsing an unsigned integer: " + std::string(e.what()));
                DRETURN nullptr;
            } catch (std::out_of_range&) {
                // printf("'\n");
                debug_info.print_error(cerr, "Value is out-of-range for a 32-bit unsigned integer (maximum: " + std::to_string(std::numeric_limits<uint32_t>::max()) + ").");
                DRETURN nullptr;
            }

            // Otherwise, we have a valid value
            // printf("'\n");
            DRETURN (Terminal*) new ValueTerminal<uint32_t>(TerminalType::uint, value, debug_info);

        }
    }



float_start:
    {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col);
        #else
        GET_CHAR(c, this->file, this->col);
        #endif
        // printf("float_start: %c\n", c);

        // Switch on its value
        if (c >= '0' && c <= '9') {
            // Keep parsing as normal number
            // printf("--> Found digit, continuing\n");
            sstr << c;
            goto float_start;

        } else if (c == '.') {
            // It's a floating-point for real
            // printf("--> Found dot, going float_dot\n");
            sstr << c;
            goto float_dot;

        } else {
            // We haven't seen a dot yet!
            DebugInfo debug(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });
            // printf("'\n");
            debug.print_error(cerr, "Encountered decimal value without a dot to mark it as such.");
            DRETURN nullptr;

        }
    }

float_dot:
    {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col);
        #else
        GET_CHAR(c, this->file, this->col);
        #endif
        // printf("float_dot: %c\n", c);

        // Switch on its value
        if (c >= '0' && c <= '9') {
            // Keep parsing as normal number
            // printf("--> Found digit, continuing\n");
            sstr << c;
            goto float_dot;

        } else {
            // Unget the token
            // printf("--> Found other token, done\n");
            fseek(this->file, -1, SEEK_CUR);
            --this->col;

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
            // printf("'\n");
            DRETURN (Terminal*) new ValueTerminal<float>(TerminalType::decimal, value, debug_info);
        }
    }



comment_start:
    {
        // Possibly store the sentence start
        if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c, this->file, this->col);
        #else
        GET_CHAR(c, this->file, this->col);
        #endif
        // printf("comment_start: %c\n", c);

        // Switch on its value
        if (c == '\n') {
            // printf("--> Found newline, done\n");
            // We're done; reset to start
            fseek(this->file, -1, SEEK_CUR);
            --this->col;
            goto start;
        } else {
            // printf("--> Found other token, retrying\n");
            // Continue popping
            goto comment_start;
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
