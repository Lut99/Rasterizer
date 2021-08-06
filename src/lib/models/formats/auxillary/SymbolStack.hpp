/* SYMBOL STACK.hpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:42
 * Last edited:
 *   06/08/2021, 12:39:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines a stack of symbols, which are used to parse as a shift-reduce
 *   parser.
**/

#ifndef SYMBOLSTACK_HPP
#define SYMBOLSTACK_HPP

#include <cstdlib>
#include <ostream>

namespace Rasterizer::Models {
    /* Stack used to store, access and manage symbols. Is based off the given Symbol class. */
    template <class T>
    class SymbolStack {
    private:
        /* Pointer to the list of Symbols. */
        T** symbols;
        /* Number of symbols currently on the stack. */
        size_t length;
        /* Maximum number of symbols before we have to reallocate. */
        size_t max_length;

        /* The empty symbol, returned when we have no more tokens left. */
        T* empty_symbol;

        /* Used to double the internal capacity. */
        void resize();

    public:
        /* Constant iterator over the SymbolStack class. */
        class const_iterator {
        private:
            /* Stack object we iterate over. */
            const SymbolStack<T>& stack;
            /* Current position in the stack. */
            size_t i;
            /* Marks if we went out-of-bounds. */
            bool out_of_bounds;

            /* Private constructor that initializes the const_iterator with the out_of_bounds flag. */
            const_iterator(const SymbolStack<T>& stack, size_t i, bool out_of_bounds);

        public:
            /* Constructor for the const_iterator class, which takes a SymbolStack object and the starting position from the top of the stack (i.e., stored i = stack.size() - 1 - given i). */
            const_iterator(const SymbolStack<T>& stack, size_t i = 0);

            /* Returns true if both iterators point to the same symbolstack and are at the same location. */
            inline bool operator==(const const_iterator& other) { return &this->stack == &other.stack && this->i == other.i && this->out_of_bounds == other.out_of_bounds; }
            /* Returns true if both iterators point to a different symbolstack or are at a different location. */
            inline bool operator!=(const const_iterator& other) { return &this->stack != &other.stack || this->i != other.i || this->out_of_bounds != other.out_of_bounds; }

            /* Increments the iterator by one. */
            const_iterator& operator++();
            /* Increments the iterator by one, after returning the value. */
            const_iterator operator++(int);
            /* Increments the iterator by N values. */
            const_iterator operator+(size_t N);
            /* Increments the iterator by N values. */
            const_iterator& operator+=(size_t N);
            /* Allows random access in the iterator. */
            const_iterator operator[](size_t i);

            /* Derefencres the iterator. */
            T* operator*() const;

        };



        /* Constructor for the SymbolStack, which takes the empty symbol to return when no symbols are available and optionally takes an initial size. Note that the empty symbol will be deallocated when the SymbolStack dies. */
        SymbolStack(T* empty_symbol, size_t init_size = 64);
        /* Copy constructor for the SymbolStack class. */
        SymbolStack(const SymbolStack<T>& other);
        /* Move constructor for the SymbolStack class. */
        SymbolStack(SymbolStack<T>&& other);
        /* Destructor for the SymbolStack class. */
        ~SymbolStack();

        /* Adds a new symbol on the stack. */
        void add_terminal(T* symbol);
        /* Replaces the first N symbols (deallocating them) with the new symbol. */
        void replace(size_t N, T* symbol);
        /* Removes the N first symbols from the stack (deallocating them). */
        void remove(size_t N);

        /* Returns the i'th symbol from the top of the stack. Note that it doesn't perform any form of memory-safe checking, so use size() to be sure you don't go out of bounds. */
        inline T* operator[](size_t i) const { return this->symbols[i]; }
        /* Returns the i'th symbol from the stack, but this one performs out-of-bounds checking and returns the empty Symbol (i.e., terminal wrapping an empty token) if it's out-of-bounds. */
        T* peek(size_t i) const;

        /* Get the number of elements current on the stack. */
        inline size_t size() const { return this->length; }
        /* Get the maximum number of elements we can accept before we need to resize. */
        inline size_t capacity() const { return this->max_length; }

        /* Returns an iterator pointing to the start of the SymbolStack (i.e., the top). */
        const_iterator begin() const { return const_iterator(*this, 0); }
        /* Returns an iterator pointing past the end of the SymbolStack (i.e., the bottom). */
        const_iterator end() const { return const_iterator(*this, this->length); }

        /* Copy assignment oeprator for the SymbolStack class. */
        inline SymbolStack<T>& operator=(const SymbolStack<T>& other) { return *this = SymbolStack<T>(other); }
        /* Move assignment operator for the SymbolStack class. */
        SymbolStack<T>& operator=(SymbolStack<T>&& other);
        /* Swap operator for the SymbolStack class. */
        friend void swap(SymbolStack<T>& ss1, SymbolStack<T>& ss2) {
            using std::swap;

            swap(ss1.symbols, ss2.symbols);
            swap(ss1.length, ss2.length);
            swap(ss1.max_length, ss2.max_length);
            swap(ss1.empty_symbol, ss2.empty_symbol);
        }

    };

}

#endif
