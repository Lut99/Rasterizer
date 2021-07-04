/* TOKENIZER.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:28:46
 * Last edited:
 *   04/07/2021, 16:28:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Tokenizer class for the .obj format. It's a very simple
 *   one.
**/

#ifndef MODELS_OBJ_TOKENIZER_HPP
#define MODELS_OBJ_TOKENIZER_HPP

#include <string>
#include <fstream>

#include "tools/Array.hpp"
#include "Terminal.hpp"

namespace Rasterizer::Models::Obj {
    /* The Tokenizer for .obj model files. */
    class Tokenizer {
    private:
        /* File handle to the open file. */
        std::ifstream file;
        /* Name of the file we opened. */
        std::string path;

        /* The line number we're currently at. */
        size_t line;
        /* The column number we're currently at. */
        size_t col;
        /* Buffer for temporary tokens. */
        Tools::Array<Terminal*> terminal_buffer;
    
    public:
        /* Constructor for the Tokenizer class, which takes the path to the file to tokenizer. */
        Tokenizer(const std::string& path);
        
        /* Returns the next Token from the stream. If no more tokens are available, returns an EOF token. Note that, due to polymorphism, the token is allocated on the heap and has to be deallocated manually. */
        Terminal* get();
        /* Puts a token back on the internal list of tokens, so it can be returned next get call. Note that the Tokenizer will deallocate these if it gets deallocated. */
        void unget(Terminal* term);

    };
}

#endif
