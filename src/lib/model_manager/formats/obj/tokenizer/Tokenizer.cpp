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

#include <cstring>
#include <cerrno>
#include "tools/CppDebugger.hpp"

#include "Tokenizer.hpp"

using namespace std;
using namespace Rasterizer::Models::Obj;
using namespace CppDebugger::SeverityValues;


/***** MACROS *****/
/* Helper macro for fetching characters of the input stream. Uses Unix-style error handling */
#define GET_CHAR(C) \
    std::istream& _state = this->file.get((C)); \
    if (!_state) { \
        if (this->file.eof()) { \
            c = EOF; \
        } else { \
            std::string err = strerror(errno); \
            DLOG(fatal, "Something went wrong while reading from the stream: " + err); \
        } \
    } \
    ++this->col;

/* Helper macro for fetching characters of the input stream. Uses Windows-style error handling */
#define GET_CHAR_W(C) \
    std::istream& _state = this->file.get((C)); \
    if (!_state) { \
        if (this->file.eof()) { \
            c = EOF; \
        } else { \
            char buffer[BUFSIZ]; \
            strerror_s(buffer, errno); \
            std::string err = buffer; \
            DLOG(fatal, "Something went wrong while reading from the stream: " + err); \
        } \
    } \
    ++this->col;

/* Helper macro for determining if the given character is a whitespce. */
#define IS_WHITESPACE(C) \
    ((C) == ' ' || (C) == '\n' || (C) == '\t')





/***** TOKENIZER CLASS *****/
/* Constructor for the Tokenizer class, which takes the path to the file to tokenizer. */
Tokenizer::Tokenizer(const std::string& path) :
    path(path),
    line(1),
    col(0)
{
    DENTER("Models::Obj::Tokenizer::Tokenizer");

    // First, open a handle
    this->file = std::ifstream(this->path);
    if (!this->file.is_open()) {
        // Get the error number
        std::string err;
        #ifdef _WIN32
        char buffer[BUFSIZ];
        strerror_s(buffer, errno);
        err = buffer;
        #else
        // Simply use strerrno
        err = strerror(errno);
        #endif

        // Show the error message
        DLOG(fatal, "Could not open file handle: " + err)
    }

    // Done
    DLEAVE;
}



/* Returns the next Token from the stream. If no more tokens are available, returns an EOF token. Note that, due to polymorphism, the token is allocated on the heap and has to be deallocated manually. */
Terminal* Tokenizer::get() {
    DENTER("Models::Obj::Tokenizer::get");

    /* The character that we write to. */
    char c;

start:
    {
        // Get a character from the stream
        #ifdef _WIN32
        GET_CHAR_W(c);
        #else
        GET_CHAR(c);
        #endif

        // Switch on its value
        if (c == 'v') {
            // It's a vertex start symbol; simple token
            DRETURN new Terminal(TerminalType::vertex, DebugInfo(this->file, this->line, this->col));

        } else if (c == 'f') {
            // It's a face start symbol; simple token
            DRETURN new Terminal(TerminalType::face, DebugInfo(this->file, this->line, this->col));

        } else if (c >= '0' || c <= '9') {
            // It's a digit, but we do not yet known which


        } else if (c == '-') {
            // It's a floating-point that's negative for sure


        } else if (IS_WHITESPACE(c)) {
            // If it's a newline, update the line counters
            if (c == '\n') {
                this->col = 0;
                ++this->line;
            }

            // Try again
            goto start;

        } else if (c == EOF) {
            // End-of-file; return that we reached it
            DRETURN new Terminal(TerminalType::eof, DebugInfo(this->file, this->line, this->col));

        }

    }

    // Done!
    DRETURN nullptr;
}

/* Puts a token back on the internal list of tokens, so it can be returned next get call. Note that the Tokenizer will deallocate these if it gets deallocated. */
void Tokenizer::unget(Terminal* term) {
    DENTER("Models::Obj::Tokenizer::unget");

    

    DRETURN;
}
