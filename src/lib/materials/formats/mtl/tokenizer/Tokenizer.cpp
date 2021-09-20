/* TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:40:45
 * Last edited:
 *   07/08/2021, 23:12:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Tokenizer class, which allows a parser to read a file in
 *   a more abstract way.
**/


/***** MACROS *****/
/* Uncomment to enable very detailed debug prints. */
// #define EXTRA_DEBUG





/***** INCLUDES *****/
#include <iostream>
#include <fstream>
#include <sstream>
#include "../../../../auxillary/TokenizerTools.hpp"
#include "ValueTerminal.hpp"
#include "Tokenizer.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;
using namespace Makma3D::Materials::Mtl;





/***** TOKENIZER CLASS *****/
/* Constructor for the Tokenizer class, which takes the path to the file to tokenizer. */
Tokenizer::Tokenizer(const std::string& path) :
    path(path),
    line(1),
    col(0),
    last_sentence_start(0)
{
    

    // First, open a handle
    std::ifstream* is = new std::ifstream(this->path, std::ios::ate);
    if (!is->is_open()) {
        #ifdef _WIN32
        char buffer[BUFSIZ];
        strerror_s(buffer, errno);
        std::string err = buffer;
        #else
        std::string err = strerror(errno);
        #endif
        logger.fatalc(Tokenizer::channel, "Could not open file handle: ", err);
    }
    this->file = static_cast<std::istream*>(is);

    // Store the current state as the size of the file
    this->file_size = static_cast<size_t>(is->tellg());

    // Reset the file to the start
    is->seekg(0);
    is->clear();
}

/* Move constructor for the Tokenizer class. */
Tokenizer::Tokenizer(Tokenizer&& other) :
    file(other.file),
    path(other.path),
    file_size(other.file_size),

    line(other.line),
    col(other.col),
    last_sentence_start(other.last_sentence_start),
    terminal_buffer(other.terminal_buffer)
{
    // Clear the other's list of terminal buffers to not deallocate them
    other.file = nullptr;
    other.terminal_buffer.clear();
}

/* Destructor for the Tokenizer class. */
Tokenizer::~Tokenizer() {
    for (uint32_t i = 0; i < this->terminal_buffer.size(); i++) {
        delete this->terminal_buffer[i];
    }
    if (this->file != nullptr) {
        delete this->file;
    }
}



/* Returns the next Token from the stream. If no more tokens are available, returns an EOF token. Note that, due to polymorphism, the token is allocated on the heap and has to be deallocated manually. */
Terminal* Tokenizer::get() {
    // If there's something in the buffer, pop that off first
    if (this->terminal_buffer.size() > 0) {
        Terminal* result = this->terminal_buffer.last();
        this->terminal_buffer.pop_back();
        return result;
    }

    // Prepare some variables
    char c;
    int i = 0;
    size_t line_start, col_start;
    Terminal* to_return;
    std::stringstream sstr;



start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
            // cout << ' ' << pad_spaces(this->line, 4) << " | " << Auxillary::get_line(file, this->last_sentence_start) << endl;
            this->col = 0;
            ++this->line;
        }

        // Try again
        goto start;

    } else if (c == EOF) {
        // End-of-file; return that we reached it
        DEBUG_PATH("EOF", "done");
        return new Terminal(TerminalType::eof, Auxillary::DebugInfo(this->path, this->line, this->col, { "" }));

    } else {
        // Unexpected token
        Auxillary::DebugInfo debug(this->path, this->line, this->col, { Auxillary::get_line(file, this->last_sentence_start) });
        debug.print_error(cerr, std::string("Unexpected character '") + Auxillary::readable_char(c) + "'");
        return nullptr;

    }
}



n_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
    DEBUG_LABEL(newmtl, c);

    // Switch on its value
    if (IS_WHITESPACE(c)) {
        // Make sure that the next one is a whitespace
        DEBUG_PATH("whitespace", "going name_start");
        UNGET_CHAR(this->file, this->col);
        to_return = new Terminal(TerminalType::newmtl, Auxillary::DebugInfo(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(this->file, this->last_sentence_start) }));
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
    DEBUG_LABEL(Kd, c);

    // Switch on its value
    if (IS_WHITESPACE(c)) {
        // Make sure that the next one is a whitespace
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        return new Terminal(TerminalType::Kd, Auxillary::DebugInfo(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(this->file, this->last_sentence_start) }));

    } else {
        // Unknown token instead!
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;     

    }
}



float_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
        Auxillary::DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        float value;
        try {
            value = std::stof(sstr.str());
        } catch (std::invalid_argument& e) {
            // printf("'\n");
            debug_info.print_error(cerr, "Illegal character parsing a decimal: " + std::string(e.what()));
            return nullptr;
        } catch (std::out_of_range&) {
            // printf("'\n");
            debug_info.print_error(cerr, "Value is out-of-range for a decimal (maximum: " + std::to_string(std::numeric_limits<float>::max()) + ").");
            return nullptr;
        }

        // Otherwise, we have a valid value
        return (Terminal*) new ValueTerminal<float>(TerminalType::decimal, value, debug_info);

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

float_dot: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
        Auxillary::DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        float value;
        try {
            value = std::stof(sstr.str());
        } catch (std::invalid_argument& e) {
            debug_info.print_error(cerr, "Illegal character parsing a decimal number: " + std::string(e.what()));
            // printf("'\n");
            return nullptr;
        } catch (std::out_of_range&) {
            debug_info.print_error(cerr, "Value is out-of-range for a 32-bit floating-point (maximum: " + std::to_string(std::numeric_limits<float>::max()) + ").");
            // printf("'\n");
            return nullptr;
        }

        // Otherwise, we have a valid value
        return (Terminal*) new ValueTerminal<float>(TerminalType::decimal, value, debug_info);

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

float_e_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
        Auxillary::DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Encountered scientific 'E' but without power.");
        return nullptr;

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}

float_e: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
        Auxillary::DebugInfo debug_info(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(file, this->last_sentence_start) });

        // Try to parse the string as a uint32_t value
        float value;
        try {
            value = std::stof(sstr.str());
        } catch (std::invalid_argument& e) {
            debug_info.print_error(cerr, "Illegal character parsing a decimal number: " + std::string(e.what()));
            // printf("'\n");
            return nullptr;
        } catch (std::out_of_range&) {
            debug_info.print_error(cerr, "Value is out-of-range for a 32-bit floating-point (maximum: " + std::to_string(std::numeric_limits<float>::max()) + ").");
            // printf("'\n");
            return nullptr;
        }

        // Otherwise, we have a valid value
        return (Terminal*) new ValueTerminal<float>(TerminalType::decimal, value, debug_info);

    } else {
        // Parse as unknown token
        DEBUG_PATH("other", "parsing as unknown token");
        UNGET_CHAR(this->file, this->col);
        goto unknown_token;

    }
}



name_start: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
    DEBUG_LABEL(name_start, c);

    // Switch on the character's value
    if (c == '\n') {
        // It's over before we begun; return only the to_return
        DEBUG_PATH("newline", "done");
        UNGET_CHAR(this->file, this->col);
        return to_return;

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
        Auxillary::DebugInfo debug_info(this->path, this->line, this->col - 1, { Auxillary::get_line(this->file, this->last_sentence_start) });
        debug_info.print_error(cerr, "Illegal name character '" + std::string(Auxillary::readable_char(c)) + "'.");
        delete to_return;
        return nullptr;

    }

}

name_contd: {
    // Possibly store the sentence start
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
    DEBUG_LABEL(name_contd, c);

    // Switch on the character's value
    if (IS_WHITESPACE(c)) {
        // Reached the endof the filename
        DEBUG_PATH("whitespace", "done");
        UNGET_CHAR(this->file, this->col);
        this->terminal_buffer.push_back((Terminal*) new ValueTerminal<std::string>(TerminalType::name, sstr.str(), Auxillary::DebugInfo(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(this->file, this->last_sentence_start) })));
        return to_return;

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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
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
    if (this->col == 0) { this->last_sentence_start = this->file->tellg(); }

    // Get a character from the stream
    GET_CHAR(c, this->file, this->col, i);
    DEBUG_LABEL(unknown_token, c);

    // Switch on the character's value
    if (IS_WHITESPACE(c)) {
        // We found the end of the token; unget it, then throw an error
        UNGET_CHAR(this->file, this->col);
        Auxillary::DebugInfo debug(this->path, line_start, col_start, this->line, this->col, { Auxillary::get_line(file, this->last_sentence_start) });
        debug.print_error(cerr, "Unknown token '" + sstr.str() + "'.");
        return nullptr;
    } else {
        // Keep consuming
        DEBUG_PATH("other", "retrying");
        sstr << c;
        goto unknown_token;
    }
}



    // We should never get here
    logger.fatalc(channel, "Hole in jump logic encountered; reached point we should never reach");
    return nullptr;
}

/* Puts a token back on the internal list of tokens, so it can be returned next get call. Note that the Tokenizer will deallocate these if it gets deallocated. */
void Tokenizer::unget(Terminal* term) {
    

    // Put the token back on the list of tokens
    this->terminal_buffer.push_back(term);

    // Done
    return;
}


        
/* Swap operator for the Tokenizer class. */
void Mtl::swap(Tokenizer& t1, Tokenizer& t2) {
    using std::swap;

    swap(t1.file, t2.file);
    swap(t1.path, t2.path);
    swap(t1.file_size, t2.file_size);
    
    swap(t1.line, t2.line);
    swap(t1.col, t2.col);
    swap(t1.last_sentence_start, t2.last_sentence_start);
    swap(t1.terminal_buffer, t2.terminal_buffer);
}
