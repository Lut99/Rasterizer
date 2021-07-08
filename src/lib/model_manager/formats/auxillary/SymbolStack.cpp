/* SYMBOL STACK.cpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:50
 * Last edited:
 *   08/07/2021, 13:43:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines a stack of symbols, which are used to parse as a shift-reduce
 *   parser.
**/

#include "SymbolStack.hpp"

using namespace std;
using namespace ArgumentParser::Parser;


/***** SYMBOLSTACK CONST_ITERATOR *****/

/* Private constructor that initializes the const_iterator with the out_of_bounds flag. */
template <class T>
SymbolStack<T>::const_iterator::const_iterator(const SymbolStack<T>& stack, size_t i, bool out_of_bounds) :
    stack(stack),
    i(i),
    out_of_bounds(out_of_bounds)
{}

/* Constructor for the const_iterator class, which takes a SymbolStack object and the starting position from the top of the stack (i.e., stored i = stack.size() - 1 - given i). */
template <class T>
SymbolStack<T>::const_iterator::const_iterator(const SymbolStack<T>& stack, size_t i) :
    stack(stack),
    i(stack.size() - 1 - i),
    out_of_bounds(stack.size() == 0 || i >= stack.size())
{}



/* Increments the iterator by one. */
template <class T>
SymbolStack<T>::const_iterator& SymbolStack<T>::const_iterator::operator++() {
    // If already out of bounds, do nothing
    if (this->out_of_bounds) { return *this; }

    // Otherwise, check if we will go out of bounds
    if (this->i == 0) {
        this->out_of_bounds = true;
        return *this;
    }

    // If that's not the case, just normally subtract one
    --this->i;
    return *this;
}

/* Increments the iterator by one, after returning the value. */
template <class T>
SymbolStack<T>::const_iterator SymbolStack<T>::const_iterator::operator++(int) {
    const_iterator before = *this;
    ++*this;
    return before;
}

/* Increments the iterator by N values. */
template <class T>
SymbolStack<T>::const_iterator SymbolStack<T>::const_iterator::operator+(size_t N) {
    // If already out of bounds, do nothing
    if (this->out_of_bounds) { return const_iterator(this->stack, this->i, true); }

    // Otherwise, check if we will go out of bounds
    if (this->i < N) {
        return const_iterator(this->stack, this->i, true);
    }

    // If that's not the case, just normally subtract one
    return const_iterator(this->stack, i - N, false);
}

/* Increments the iterator by N values. */
template <class T>
SymbolStack<T>::const_iterator& SymbolStack<T>::const_iterator::operator+=(size_t N) {
    // If already out of bounds, do nothing
    if (this->out_of_bounds) { return *this; }

    // Otherwise, check if we will go out of bounds
    if (this->i < N) {
        this->out_of_bounds = true;
        return *this;
    }

    // If that's not the case, just normally subtract one
    this->i -= N;
    return *this;
}

/* Allows random access in the iterator. */
template <class T>
SymbolStack<T>::const_iterator SymbolStack<T>::const_iterator::operator[](size_t i) {
    // Simply create a new iterator and let the public constructor deal with it
    return const_iterator(this->stack, i);
}



/* Derefencres the iterator. */
template <class T>
T* SymbolStack<T>::const_iterator::operator*() const {
    // If this iterator points to something out-of-bounds, then return the empty symbol
    if (this->out_of_bounds) {
        return this->empty_symbol;
    }

    // Otherwise, return an actualy symbol
    return this->stack[this->i];
}





/***** SYMBOLSTACK CLASS *****/

/* Constructor for the SymbolStack, which takes the empty symbol to return when no symbols are available and optionally takes an initial size. Note that the empty symbol will be deallocated when the SymbolStack dies. */
template<class T>
SymbolStack<T>::SymbolStack(T* empty_symbol, size_t init_size) :
    length(0),
    max_length(init_size),
    empty_symbol(empty_symbol)
{
    // Create a list of max_size length
    this->symbols = new T*[this->max_length];
}

/* Copy constructor for the SymbolStack class. */
template <class T>
SymbolStack<T>::SymbolStack(const SymbolStack<T>& other) :
    length(other.length),
    max_length(other.max_length),
    empty_symbol(new T(other.empty_symbol))
{
    // Create a new list and copy all pointers
    this->symbols = new T*[other.max_length];
    for (size_t i = 0; i < other.length; i++) {
        this->symbols[i] = other.symbols[i]->copy();
    }
}

/* Move constructor for the SymbolStack class. */
template <class T>
SymbolStack<T>::SymbolStack(SymbolStack<T>&& other) :
    symbols(other.symbols),
    length(other.length),
    max_length(other.length),
    empty_symbol(other.empty_symbol)
{
    // Set the other to nullptr
    other.symbols = nullptr;
    other.empty_symbol = nullptr;
}

/* Destructor for the SymbolStack class. */
template <class T>
SymbolStack<T>::~SymbolStack() {
    // Simply delete the internal list if it isn't stolen
    if (this->symbols != nullptr) {
        // Don't forget to free all pointers themselves
        for (size_t i = 0; i < this->size(); i++) {
            delete this->symbols[i];
        }
        delete[] this->symbols;
    }
    // Also delete the empty symbol
    if (this->empty_symbol != nullptr) {
        delete this->empty_symbol;
    }
}



/* Used to double the internal capacity. */
template <class T>
void SymbolStack<T>::resize() {
    // Declare a new vector
    size_t new_max_length = this->max_length * 2;
    T** new_symbols = new T*[new_max_length];
    
    // Copy the data
    memcpy(new_symbols, symbols, sizeof(T*) * this->length);

    // Free the old one
    delete[] this->symbols;

    // Overwrite the data in the struct
    this->max_length = new_max_length;
    this->symbols = new_symbols;
}



/* Adds a new symbol on the stack. */
template <class T>
void SymbolStack<T>::add_terminal(T* symbol) {
    // Create a new Terminal that's a copy of the given one
    T* term = new T(token);

    // Make sure there is enough space left
    if (this->length >= this->max_length) { this->resize(); }

    // Add the new terminal symbol
    this->symbols[this->length++] = term;
}

/* Replaces the first N symbols (deallocating them) with the new symbol. */
template <class T>
void SymbolStack<T>::replace(size_t N, T* symbol) {
    this->remove(N);

    // Insert the new symbol at that place
    this->symbols[this->length++] = symbol;

    // Done!
}

/* Removes the N first symbols from the stack (deallocating them). */
template <class T>
void SymbolStack<T>::remove(size_t N) {
    // Loop & deallocate
    for (size_t i = 0; i < N; i++) {
        delete this->symbols[this->length - 1 - i];
    }
    // Decrease the length
    this->length -= N;
}



/* Returns the i'th symbol from the stack, but this one performs out-of-bounds checking and returns the empty Symbol (i.e., terminal wrapping an empty token) if it's out-of-bounds. */
template <class T>
T* SymbolStack<T>::peek(size_t i) const {
    if (i >= this->length) {
        // Simply return the empty token
        return this->empty_symbol;
    }

    // Otherwise, return the correct pointer
    return this->symbols[i];
}



/* Move assignment operator for the SymbolStack class. */
template <class T>
SymbolStack<T>& SymbolStack<T>::operator=(SymbolStack<T>&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the SymbolStack class. */
template <class T>
void ArgumentParser::Parser::swap(SymbolStack<T>& ss1, SymbolStack<T>& ss2) {
    using std::swap;

    swap(ss1.symbols, ss2.symbols);
    swap(ss1.length, ss2.length);
    swap(ss1.max_length, ss2.max_length);
    swap(ss1.empty_symbol, ss2.empty_symbol);
}
