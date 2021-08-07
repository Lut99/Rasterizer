/* TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:40:45
 * Last edited:
 *   07/08/2021, 22:08:56
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Tokenizer class, which allows a parser to read a file in
 *   a more abstract way.
**/

#include <sstream>
#include "tools/CppDebugger.hpp"

#include "../../auxillary/TokenizerTools.hpp"
#include "ValueTerminal.hpp"
#include "Tokenizer.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Mtl;
using namespace CppDebugger::SeverityValues;


/***** TOKENIZER CLASS *****/
/* Constructor for the Tokenizer class, which takes the path to the file to tokenizer. */
Tokenizer::Tokenizer(const std::string& path) :
    path(path),
    line(1),
    col(0),
    last_sentence_start(0)
{
    DENTER("Models::Mtl::Tokenizer::Tokenizer");

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
    DENTER("Models::Mtl::Tokenizer::~Tokenizer");

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
    DENTER("Models::Mtl::Tokenizer::get");

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
    if (c == 'n') {
        // newmtl, probably
        DEBUG_PATH("n", "going n_start");
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto n_start;
    
    } else if (c == 'K') {
        // It's probably Kd
        sstr << c;
        line_start = this->line;
        col_start = this->col;
        goto K_start;

    } else if ((c >= '0' && c <= '9') || c == '-' || c == '.') {
        // It's a digit, but we do not yet known which
        DEBUG_PATH("digit/dash/dot", "going float_start");
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



n_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(n_start, c);

    // Switch on its value
    if (c == 'e') {
        // Keep going
        DEBUG_PATH("e", "going ne");
        sstr << c;
        goto ne;

    } else {
        // Unknown token!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}

ne: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(ne, c);

    // Switch on its value
    if (c == 'w') {
        // Keep going
        DEBUG_PATH("w", "going _new");
        sstr << c;
        goto _new;

    } else {
        // Unknown token!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}

_new: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(_new, c);

    // Switch on its value
    if (c == 'm') {
        // Keep going
        DEBUG_PATH("m", "going newm");
        sstr << c;
        goto newm;

    } else {
        // Unknown token!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}

newm: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(newm, c);

    // Switch on its value
    if (c == 't') {
        // Keep going
        DEBUG_PATH("t", "going newmt");
        sstr << c;
        goto newmt;

    } else {
        // Unknown token!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}

newmt: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(newmt, c);

    // Switch on its value
    if (c == 'l') {
        // Make sure that the next one is a whitespace
        DEBUG_PATH("l", "checking for whitespace");
        sstr << c;
        goto newmtl;

    } else {
        // Unknown token!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}

newmtl: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(newmtl, c);

    // Switch on its value
    if (IS_WHITESPACE(c)) {
        // Make sure that the next one is a whitespace
        DEBUG_PATH("whitespace", "going name_start");
        UNGET_CHAR(this->file, this->col);
        to_return = new Terminal(TerminalType::newmtl, DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));
        goto name_start;

    } else {
        // Unknown token instead!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}



K_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(K_start, c);

    // Switch on its value
    if (c == 'd') {
        // Keep going
        DEBUG_PATH("d", "checking for whitespace");
        sstr << c;
        goto Kd;

    } else {
        // Unknown token!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}

Kd: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = ftell(this->file); }

    // Get a character from the stream
    #ifdef _WIN32
    GET_CHAR_W(c, this->file, this->col, i);
    #else
    GET_CHAR(c, this->file, this->col, i);
    #endif
    DEBUG_LABEL(Kd, c);

    // Switch on its value
    if (IS_WHITESPACE(c)) {
        // Make sure that the next one is a whitespace
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        DRETURN new Terminal(TerminalType::Kd, DebugInfo(this->path, line_start, col_start, this->line, this->col, { get_line(this->file, this->last_sentence_start) }));

    } else {
        // Unknown token instead!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

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
    
    } else if (c == 'E' || c == 'e') {
        // It's definitely a float now, but then with scientific notation
        DEBUG_PATH("E", "going float_e");
        sstr << c;
        goto float_e_start;

    } else if (IS_WHITESPACE(c)) {
        // We haven't seen a dot yet! So parse as int
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);

        // Create the debug info
        DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        float value;
        try {
            value = std::stof(sstr.str());
        } catch (std::invalid_argument& e) {
            // printf("'\n");
            debug_info.print_error(cerr, "Illegal character parsing a decimal: " + std::string(e.what()));
            DRETURN nullptr;
        } catch (std::out_of_range&) {
            // printf("'\n");
            debug_info.print_error(cerr, "Value is out-of-range for a decimal (maximum: " + std::to_string(std::numeric_limits<float>::max()) + ").");
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
    DENTER("Models::Mtl::Tokenizer::unget");

    // Put the token back on the list of tokens
    this->terminal_buffer.push_back(term);

    // Done
    DRETURN;
}


        
/* Swap operator for the Tokenizer class. */
void Mtl::swap(Tokenizer& t1, Tokenizer& t2) {
    using std::swap;

    swap(t1.file, t2.file);
    swap(t1.path, t2.path);
    
    swap(t1.line, t2.line);
    swap(t1.col, t2.col);
    swap(t1.last_sentence_start, t2.last_sentence_start);
    swap(t1.terminal_buffer, t2.terminal_buffer);
}

