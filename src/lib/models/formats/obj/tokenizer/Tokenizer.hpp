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
#include <cstdio>
#include "tools/Array.hpp"

#include "Terminal.hpp"

namespace Rasterizer::Models::Obj {
    /* The Tokenizer for .obj model files. */
    class Tokenizer {
    private:
        /* File handle to the open file. */
        FILE* file;
        /* Name of the file we opened. */
        std::string path;

        /* The line number we're currently at. */
        size_t line;
        /* The column number we're currently at. */
        size_t col;
        /* The last stream position of a sentence start. */
        long last_sentence_start;
        /* Buffer for temporary tokens. */
        Tools::Array<Terminal*> terminal_buffer;
    
    public:
        /* Constructor for the Tokenizer class, which takes the path to the file to tokenizer. */
        Tokenizer(const std::string& path);
        /* Copy constructor for the Tokenizer class, which is deleted as it makes no sense to copy a stream. */
        Tokenizer(const Tokenizer& other) = delete;
        /* Move constructor for the Tokenizer class. */
        Tokenizer(Tokenizer&& other);
        /* Destructor for the Tokenizer class. */
        ~Tokenizer();
        
        /* Returns the next Token from the stream. If no more tokens are available, returns an EOF token. Note that, due to polymorphism, the token is allocated on the heap and has to be deallocated manually. */
        Terminal* get();
        /* Puts a token back on the internal list of tokens, so it can be returned next get call. Note that the Tokenizer will deallocate these if it gets deallocated. */
        void unget(Terminal* term);
        /* Returns whether or not the Tokenizer is done parsing. */
        inline bool eof() const { return feof(this->file); }

        /* Returns the current amount of bytes read. */
        inline size_t bytes() const { return static_cast<size_t>(ftell(this->file)); }
        /* Returns the total amount of bytes read. */
        size_t size();

        /* Copy assignment operator for the Tokenizer class, which is deleted as it makes no sense to copy a stream. */
        Tokenizer& operator=(const Tokenizer& other) = delete;
        /* Move assignment operator for the Tokenizer class. */
        inline Tokenizer& operator=(Tokenizer&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Tokenizer class. */
        friend void swap(Tokenizer& t1, Tokenizer& t2);

    };

    /* Swap operator for the Tokenizer class. */
    void swap(Tokenizer& t1, Tokenizer& t2);

}

#endif
