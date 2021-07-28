/* ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 17:11:02
 * Last edited:
 *   27/07/2021, 17:11:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <stdexcept>

#include "Array.hpp"

using namespace std;
using namespace Tools;


/***** ARRAYSTORAGE CLASS *****/
/* Default constructor for the ArrayStorage class, which initializes itself to 0. */
template <class T>
_intern::ArrayStorage<T>::ArrayStorage() :
    elements(nullptr),
    length(0),
    max_length(0)
{}

/* Copy constructor for the ArrayStorage class. */
template <class T>
_intern::ArrayStorage<T>::ArrayStorage(const _intern::ArrayStorage<T>& other) :
    length(other.length),
    max_length(other.max_length)
{
    // Allocate a new array for ourselves
    this->elements = (T*) malloc(this->length * sizeof(T));
    if (this->elements == nullptr) { throw std::bad_alloc(); }

    // Copy everything over
    for (array_size_t i = 0; i < other.length; i++) {
        new(this->elements + i) T(other.elements[i]);
    }
}

/* Move constructor for the ArrayStorage class. */
template <class T>
_intern::ArrayStorage<T>::ArrayStorage(_intern::ArrayStorage<T>&& other) :
    elements(other.elements),
    length(other.length),
    max_length(other.max_length)
{
    other.elements = nullptr;
}

/* Destructor for the ArrayStorage class. */
template <class T>
_intern::ArrayStorage<T>::~ArrayStorage() {
    if (this->elements != nullptr) {
        if (std::is_destructible<T>::value) {
            for (array_size_t i = 0; i < this->length; i++) {
                this->elements[i].~T();
            }
        }
        free(this->elements);
    }
}



/* Swap operator for the ArrayStorage class. */
template <class T>
void _intern::swap(_intern::ArrayStorage<T>& as1, _intern::ArrayStorage<T>& as2) {
    using std::swap;

    swap(as1.elements, as2.elements);
    swap(as1.length, as2.length);
    swap(as1.max_length, as2.max_length);
}





/***** ARRAY CLASS *****/
/* Default constructor for the Array class, which initializes it to not having any elements. */
template <class T, bool D, bool C, bool M>
Array<T, D, C, M>::Array() {}

/* Constructor for the Array class, which takes an initial amount to set its capacity to. Each element will thus be uninitialized. */
template <class T, bool D, bool C, bool M>
Array<T, D, C, M>::Array(array_size_t initial_size) {
    // Allocate memory for the internal storage class
    this->storage.elements = (T*) malloc(initial_size * sizeof(T));
    this->storage.max_length = initial_size;
}

/* Constructor for the Array class, which takes a single element and repeats that the given amount of times. Makes use of the element's copy constructor. */
template <class T, bool D, bool C, bool M>
template <typename U, typename>
Array<T, D, C, M>::Array(const T& elem, array_size_t n_repeats) :
    Array(n_repeats)
{
    // Make enough copies
    for (array_size_t i = 0; i < n_repeats; i++) {
        new(this->storage.elements + this->storage.length++) T(elem);
    }
}

/* Constructor for the Array class, which takes an initializer_list to initialize the Array with. Makes use of the element's copy constructor. */
template <class T, bool D, bool C, bool M>
template <typename U, typename>
Array<T, D, C, M>::Array(const std::initializer_list<T>& list) :
    Array(static_cast<array_size_t>(list.size()))
{
    // Copy all the elements over
    for (const T& elem : list) {
        new(this->storage.elements + this->storage.length++) T(elem);
    }
}

/* Constructor for the Array class, which takes a raw C-style vector to copy elements from and its size. Note that the Array's element type must have a copy custructor defined. */
template <class T, bool D, bool C, bool M>
template <typename U, typename>
Array<T, D, C, M>::Array(T* list, array_size_t list_size) :
    Array(list_size)
{
    // Copy all the elements over
    for (array_size_t i = 0; i < list_size; i++) {
        new(this->storage.elements + this->storage.length++) T(list[i]);
    }
}

/* Constructor for the Array class, which takes a C++-style vector. Note that the Array's element type must have a copy custructor defined. */
template <class T, bool D, bool C, bool M>
template <typename U, typename>
Array<T, D, C, M>::Array(const std::vector<T>& list) :
    Array(list.data(), static_cast<array_size_t>(list.size()))
{}



/* Adds a whole array worth of new elements to the array, copying them. Note that this requires the elements to be copy constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::operator+=(const Array<T>& elems) -> std::enable_if_t<C, U> {
    // Make sure the Array has enough size
    if (this->storage.length + elems.storage.length > this->storage.max_length) {
        this->reserve(this->storage.length + elems.storage.length);
    }

    // Add the new elements to the end of the array
    for (array_size_t i = 0; i < elems.storage.length; i++) {
        new(this->storage.elements + this->storage.length++) T(elems.storage.elements[i]);
    }

    // D0ne
    return *this;
}

/* Adds a whole array worth of new elements to the array, leaving the original array in an unused state (moving it). Note that this still requires the elements to have some form of a move constructor defined. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::operator+=(Array<T>&& elems) -> std::enable_if_t<M, U> {
    // Make sure the Array has enough size
    if (this->storage.length + elems.storage.length > this->storage.max_length) {
        this->reserve(this->storage.length + elems.storage.length);
    }

    // Add the new elements to the end of the array
    for (array_size_t i = 0; i < elems.storage.length; i++) {
        new(this->storage.elements + this->storage.length++) T(std::move(elems.storage.elements[i]));
    }

    // Already deallocate the other's list to prevent the other deallocating them
    free(elems.storage.elements);
    elems.storage.elements = nullptr;
    
    // D0ne
    return *this;
}



/* Adds a new element of type T to the array, copying it. Note that this requires the element to be copy constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::push_back(const T& elem) -> std::enable_if_t<C, U> {
    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        this->reserve(this->storage.length + 1);
    }

    // Add the element to the end of the array
    new(this->storage.elements + this->storage.length++) T(elem);
}

/* Adds a new element of type T to the array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::push_back(T&& elem) -> std::enable_if_t<M, U> {
    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        this->reserve(this->storage.length + 1);
    }

    // Add the element to the end of the array
    new(this->storage.elements + this->storage.length++) T(std::move(elem));
}

/* Removes the last element from the array. */
template <class T, bool D, bool C, bool M>
void Array<T, D, C, M>::pop_back() {
    // Check if there are any elements
    if (this->storage.length == 0) { return; }

    // Delete the last element if we need to
    if (std::is_destructible<T>::value) {
        this->elements[this->length - 1].~T();
    }

    // Decrement the length
    --this->length;
}



/* Erases an element with the given index from the array. Does nothing if the index is out-of-bounds. Because the elements need to be moved forward, the element is required to be move constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::erase(array_size_t index) -> std::enable_if_t<M, U> {
    // Check if the index is within bounds
    if (index >= this->storage.length) { return; }

    // Otherwise, delete the element if needed
    if (std::is_destructible<T>::value) {
        this->elements[index].~T();
    }

    // Move the other elements one back
    for (array_size_t i = index; i < this->storage.length - 1; i++) {
        new(this->storage.elements + i) T(std::move(this->storage.elements[i + 1]));
    }

    // Decrease the length
    --this->storage.length;
}

/* Erases multiple elements in the given (inclusive) range from the array. Does nothing if the any index is out-of-bounds or if the start_index is larger than the stop_index. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::erase(array_size_t start_index, array_size_t stop_index) -> std::enable_if_t<M, U> {
    // Check if in bounds
    if (start_index >= this->storage.length || stop_index >= this->storage.length || start_index > stop_index) { return; }

    // Otherwise, delete the elements if needed
    if (std::is_destructible<T>::value) {
        for (array_size_t i = start_index; i <= stop_index; i++) {
            this->storage.elements[i].~T();
        }
    }

    // Move all elements following it back
    for (array_size_t i = stop_index + 1; i < this->length; i++) {
        new(this->storage.elements + (i - (stop_index - start_index) - 1)) T(std::move(this->storage.elements[i]));
    }

    // Decrease the length
    this->length -= 1 + stop_index - start_index;
}

/* Erases everything from the array, but leaves the internally located array intact. */
template <class T, bool D, bool C, bool M>
void Array<T, D, C, M>::clear() {
    // Delete everything in the Array if the type wants it to
    if (std::is_destructible<T>::value) {
        for (array_size_t i = 0; i < this->length; i++) {
            this->elements[i].~T();
        }
    }

    // Set the new length
    this->length = 0;
}

/* Erases everything from the array, even removing the internal allocated array. */
template <class T, bool D, bool C, bool M>
void Array<T, D, C, M>::reset() {
    // Delete everything in the Array if the type wants it to
    if (std::is_destructible<T>::value) {
        for (array_size_t i = 0; i < this->length; i++) {
            this->elements[i].~T();
        }
    }
    free(this->elements);

    // Set the new values
    this->elements = nullptr;
    this->length = 0;
    this->max_length = 0;
}



/* Re-allocates the internal array to the given size. Any leftover elements will be left unitialized, and elements that won't fit will be deallocated. Requires a move constructor to be able to actually move them. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::reserve(array_size_t new_size) -> std::enable_if_t<M, U> {
    // Do some special cases for the new_size
    if (new_size == 0) {
        // Simply call reset
        this->reset();
        return;
    } else if (new_size == this->storage.length) {
        // Do nothing
        return;
    }

    // Start by allocating space for a new array
    T* new_elements = (T*) malloc(new_size = sizeof(T));
    if (new_elements == nullptr) { throw std::bad_alloc(); }

    // Copy the elements over using their move constructor
    array_size_t n_to_copy = std::min(new_size, this->storage.length);
    for (array_size_t i = 0; i < n_to_copy; i++) {
        new(new_elements + i) T(std::move(this->storage.elements[i]));
    }

    // Delete the elements that are too many
    if (std::is_destructible<T>::value) {
        for (array_size_t i = n_to_copy; i < this->storage.length; i++) {
            this->storage.elements[i].~T();
        }
    }
    free(this->storage.elements);

    // Set the new properties of the elements array
    this->storage.elements = new_elements;
    this->storage.length = n_to_copy;
    this->storage.max_length = new_size;
}

/* Resizes the array to the given size. Any leftover elements will be initialized with their default constructor (and thus requires the type to have one), and elements that won't fit will be deallocated. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::resize(array_size_t new_size) -> std::enable_if_t<std::conjunction<std::integral_constant<bool, D>, std::integral_constant<bool, M>>::value, U> {
    // Simply reserve the space
    this->reserve(new_size);
    printf("\n\nlmao\n\n");

    // Populate the other elements with default constructors
    for (array_size_t i = this->storage.length; i < this->storage.max_length; i++) {
        new(this->storage.elements + this->storage.length++) T();
    }
}



/* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
template <class T, bool D, bool C, bool M>
T& Array<T, D, C, M>::at(array_size_t index) {
    if (index >= this->storage.length) { throw std::out_of_range("Index " + std::to_string(index) + " is out-of-bounds for Array with size " + std::to_string(this->storage.length)); }
    return this->storage.elements[index];
}

/* Returns a muteable pointer to the internal data struct. Use this to fill the array using C-libraries, but beware that the array needs to have enough space reserved. Also note that object put here will still be deallocated by the Array using ~T(). The optional new_size parameter is used to update the size() value of the array, so it knows what is initialized and what is not. Leave it at numeric_limits<array_size_t>::max() to leave the array size unchanged. */
template <class T, bool D, bool C, bool M>
T* Array<T, D, C, M>::wdata(array_size_t new_size) {
    // Update the size if it's not the max
    if (new_size != std::numeric_limits<array_size_t>::max()) { this->length = new_size; }
    // Return the pointer
    return this->elements;
}



/* Swap operator for the Array class. */
template <class T>
void Tools::swap(Array<T>& a1, Array<T>& a2) {
    using std::swap;

    swap(a1.storage, a2.storage);
}
