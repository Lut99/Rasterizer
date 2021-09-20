/* ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 17:11:02
 * Last edited:
 *   9/20/2021, 8:38:11 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <cstring>
#include <stdexcept>
#include <algorithm>

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
    // printf("Copying from array with size %u\n", this->length);
    this->elements = (T*) malloc(this->length * sizeof(T));
    if (this->elements == nullptr) { throw std::bad_alloc(); }

    // Copy everything over
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(this->elements, other.elements, this->length * sizeof(T));
    } else {
        for (array_size_t i = 0; i < other.length; i++) {
            new(this->elements + i) T(other.elements[i]);
        }
    }
}

/* Move constructor for the ArrayStorage class. */
template <class T>
_intern::ArrayStorage<T>::ArrayStorage(_intern::ArrayStorage<T>&& other) :
    elements(other.elements),
    length(other.length),
    max_length(other.max_length)
{
    // printf("Moving from array with size %u\n", this->length);
    other.elements = nullptr;
    other.length = 0;
    other.max_length = 0;
}

/* Destructor for the ArrayStorage class. */
template <class T>
_intern::ArrayStorage<T>::~ArrayStorage() {
    if (this->elements != nullptr) {
        if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
            for (array_size_t i = 0; i < this->length; i++) {
                this->elements[i].~T();
            }
        }
        free(this->elements);
    }
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
    if (this->storage.elements == nullptr) { throw std::bad_alloc(); }
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
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(this->storage.elements, list.begin(), list.size() * sizeof(T));
        this->storage.length = static_cast<array_size_t>(list.size());
    } else {
        for (const T& elem : list) {
            new(this->storage.elements + this->storage.length++) T(elem);
        }
    }
}

/* Constructor for the Array class, which takes a raw C-style vector to copy elements from and its size. Note that the Array's element type must have a copy custructor defined. */
template <class T, bool D, bool C, bool M>
template <typename U, typename>
Array<T, D, C, M>::Array(const T* list, array_size_t list_size) :
    Array(list_size)
{
    // Copy all the elements over
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(this->storage.elements, list, list_size * sizeof(T));
        this->storage.length = list_size;
    } else {
        for (array_size_t i = 0; i < list_size; i++) {
            new(this->storage.elements + this->storage.length++) T(list[i]);
        }
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
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(this->storage.elements + this->storage.length, elems.storage.elements, elems.storage.length * sizeof(T));
        this->storage.length += elems.storage.length;
    } else {
        for (array_size_t i = 0; i < elems.storage.length; i++) {
            new(this->storage.elements + this->storage.length++) T(elems.storage.elements[i]);
        }
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
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + this->storage.length, elems.storage.elements, elems.storage.length * sizeof(T));
        this->storage.length += elems.storage.length;
    } else {
        for (array_size_t i = 0; i < elems.storage.length; i++) {
            new(this->storage.elements + this->storage.length++) T(std::move(elems.storage.elements[i]));
        }
    }

    // Already deallocate the other's list to prevent the other deallocating them
    free(elems.storage.elements);
    elems.storage.elements = nullptr;
    
    // D0ne
    return *this;
}



/* Adds a new element of type T to the front of the array, pushing the rest back. The element is initialized with with its default constructor. Needs a default constructor to be present, but also to be move assignable in some way to be moved around in the array. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::push_front() -> std::enable_if_t<D && M, U> {
    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Move all elements one place back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + 1, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        for (uint32_t i = this->storage.length; i-- > 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i - 1]));
        }
    }

    // Insert the new element, also increasing our own size
    new(this->storage.elements) T();
    ++this->storage.length;
}

/* Adds a new element of type T to the front of the array, pushing the rest back. The element is copied using its copy constructor, which it is required to have. Also required to be move assignable to be moved around. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::push_front(const T& elem) -> std::enable_if_t<C && M, U> {
    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Move all elements one place back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + 1, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        for (uint32_t i = this->storage.length; i-- > 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i - 1]));
        }
    }

    // Insert the new element, also increasing our own size
    new(this->storage.elements) T(elem);
    ++this->storage.length;
}

/* Adds a new element of type T to the front of the array, pushing the rest back. The element is left in an usused state (moving it). Note that this requires the element to be move constructible. Also required to be move assignable to be moved around. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::push_front(T&& elem) -> std::enable_if_t<M, U> {
    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Move all elements one place back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + 1, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        for (uint32_t i = this->storage.length; i-- > 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i - 1]));
        }
    }

    // Insert the new element, also increasing our own size
    new(this->storage.elements) T(std::move(elem));
    ++this->storage.length;
}

/* Removes the first element from the array, moving the rest one index to the front. Needs to be move assignable to do the moving. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::pop_front() -> std::enable_if_t<M, U> {
    // Check if there are any elements
    if (this->storage.length == 0) { return; }

    // Delete the first element if we need to
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        this->storage.elements[0].~T();
    }

    // If there are other elements, move them forward
    if (this->storage.length > 1) {
        if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
            memmove(this->storage.elements, this->storage.elements + 1, this->storage.length * sizeof(T));
        } else {
            for (uint32_t i = 0; i < this->storage.length - 1; i++) {
                new(this->storage.elements + i) T(std::move(this->storage.elements[i + 1]));
            }
        }
    }

    // Decrement the length
    --this->storage.length;
}



/* Inserts a new element at the given location, pushing all elements coming after it one index back. Since we initialise the element with its default constructor, we need that to be present for the Array's element type. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::insert(array_size_t index) -> std::enable_if_t<D && M, U> {
    // Check if the index is within bounds
    if (index >= this->storage.length) {
        throw std::out_of_range("Index " + std::to_string(index) + " is out-of-bounds for Array with size " + std::to_string(this->storage.length));
    }

    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Move all elements following the index one place back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + index + 1, this->storage.elements + index, (this->storage.length - index - 1) * sizeof(T));
    } else {
        for (uint32_t i = this->storage.length; i-- > index + 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i - 1]));
        }
    }

    // Insert the new element, also increasing our own size
    new(this->storage.elements + index) T();
    ++this->storage.length;
}

/* Inserts a copy of the given element at the given location, pushing all elements coming after it one index back. Requires the element to be copy constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::insert(array_size_t index, const T& elem) -> std::enable_if_t<C && M, U> {
    // Check if the index is within bounds
    if (index >= this->storage.length) {
        throw std::out_of_range("Index " + std::to_string(index) + " is out-of-bounds for Array with size " + std::to_string(this->storage.length));
    }

    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Move all elements following the index one place back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + index + 1, this->storage.elements + index, (this->storage.length - index - 1) * sizeof(T));
    } else {
        for (uint32_t i = this->storage.length; i-- > index + 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i - 1]));
        }
    }

    // Insert the new element, also increasing our own size
    new(this->storage.elements + index) T(elem);
    ++this->storage.length;
}

/* Inserts the given element at the given location, pushing all elements coming after it one index back. Requires the element to be move constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::insert(array_size_t index, T&& elem) -> std::enable_if_t<M, U> {
    // Check if the index is within bounds
    if (index >= this->storage.length) {
        throw std::out_of_range("Index " + std::to_string(index) + " is out-of-bounds for Array with size " + std::to_string(this->storage.length));
    }

    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Move all elements following the index one place back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + index + 1, this->storage.elements + index, (this->storage.length - index) * sizeof(T));
    } else {
        for (uint32_t i = this->storage.length; i-- > index + 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i - 1]));
        }
    }

    // Insert the new element, also increasing our own size
    new(this->storage.elements + index) T(std::move(elem));
    ++this->storage.length;
}

/* Erases an element with the given index from the array. Does nothing if the index is out-of-bounds. Because the elements need to be moved forward, the element is required to be move constructible. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::erase(array_size_t index) -> std::enable_if_t<M, U> {
    // Check if the index is within bounds
    if (index >= this->storage.length) { return; }

    // Otherwise, delete the element if needed
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        this->storage.elements[index].~T();
    }

    // Move the other elements one back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + index, this->storage.elements + index + 1, (this->storage.length - index - 1) * sizeof(T));
    } else {
        for (array_size_t i = index; i < this->storage.length - 1; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i + 1]));
        }
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
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        for (array_size_t i = start_index; i <= stop_index; i++) {
            this->storage.elements[i].~T();
        }
    }

    // Move all elements following it back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(this->storage.elements + start_index, this->storage.elements + stop_index + 1, (this->storage.length - stop_index - 1) * sizeof(T));
    } else {
        for (array_size_t i = stop_index + 1; i < this->storage.length; i++) {
            new(this->storage.elements + (i - (stop_index - start_index) - 1)) T(std::move(this->storage.elements[i]));
        }
    }

    // Decrease the length
    this->storage.length -= 1 + stop_index - start_index;
}



/* Adds a new element of type T to the array, initializing it with its default constructor. Only needs a default constructor to be present, but cannot resize itself without a move constructor. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::push_back() -> std::enable_if_t<D, U> {
    // Make sure the array has enough size
    if (this->storage.length >= this->storage.max_length) {
        if constexpr (M) {
            this->reserve(this->storage.length + 1);
        } else {
            throw std::out_of_range("Cannot add more elements to Array than reserved for without move constructor (Array has space for " + std::to_string(this->storage.length) + " elements).");
        }
    }

    // Add the element to the end of the array
    new(this->storage.elements + this->storage.length++) T();
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
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        this->storage.elements[this->storage.length - 1].~T();
    }

    // Decrement the length
    --this->storage.length;
}



/* Erases everything from the array, but leaves the internally located array intact. */
template <class T, bool D, bool C, bool M>
void Array<T, D, C, M>::clear() {
    // Delete everything in the Array if the type wants it to
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        for (array_size_t i = 0; i < this->storage.length; i++) {
            this->storage.elements[i].~T();
        }
    }

    // Set the new length
    this->storage.length = 0;
}

/* Erases everything from the array, even removing the internal allocated array. */
template <class T, bool D, bool C, bool M>
void Array<T, D, C, M>::reset() {
    // Delete everything in the Array if the type wants it to
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        for (array_size_t i = 0; i < this->storage.length; i++) {
            this->storage.elements[i].~T();
        }
    }
    free(this->storage.elements);

    // Set the new values
    this->storage.elements = nullptr;
    this->storage.length = 0;
    this->storage.max_length = 0;
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
    T* new_elements = (T*) malloc(new_size * sizeof(T));
    if (new_elements == nullptr) { throw std::bad_alloc(); }

    // Copy the elements over using their move constructor
    array_size_t n_to_copy = std::min(new_size, this->storage.length);
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(new_elements, this->storage.elements, n_to_copy * sizeof(T));
    } else {
        for (array_size_t i = 0; i < n_to_copy; i++) {
            new(new_elements + i) T(std::move(this->storage.elements[i]));
        }
    }

    // Delete the elements that are too many
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        for (array_size_t i = n_to_copy; i < this->storage.length; i++) {
            this->storage.elements[i].~T();
        }
    }
    free(this->storage.elements);

    // Finally, put the Array to the internal slot
    this->storage.elements = new_elements;
    this->storage.length = n_to_copy;
    this->storage.max_length = new_size;
}

/* Guarantees that the Array has at least min_size capacity after the call. Does so by reallocating the internal array if we currently have less, but leaving it untouched otherwise. Any new elements will be left unitialized. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::reserve_opt(array_size_t min_size) -> std::enable_if_t<M, U> {
    // Do some special cases for the new_size
    if (min_size == 0) {
        // Simply call reset
        this->reset();
        return;
    } else if (min_size <= this->storage.max_length) {
        // Do nothing
        return;
    }

    // Start by allocating space for a new array
    T* new_elements = (T*) malloc(min_size * sizeof(T));
    if (new_elements == nullptr) { throw std::bad_alloc(); }

    // Copy the elements over using their move constructor
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        memmove(new_elements, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        for (array_size_t i = 0; i < this->storage.length; i++) {
            new(new_elements + i) T(std::move(this->storage.elements[i]));
        }
    }

    // Clear the old list
    free(this->storage.elements);

    // Finally, put the Array to the internal slot
    this->storage.elements = new_elements;
    this->storage.max_length = min_size;
}

/* Resizes the array to the given size. Any leftover elements will be initialized with their default constructor (and thus requires the type to have one), and elements that won't fit will be deallocated. */
template <class T, bool D, bool C, bool M>
template <typename U>
auto Array<T, D, C, M>::resize(array_size_t new_size) -> std::enable_if_t<D && M, U> {
    // Simply reserve the space
    this->reserve(new_size);

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
    if (new_size != std::numeric_limits<array_size_t>::max()) { this->storage.length = new_size; }
    // Return the pointer
    return this->storage.elements;
}
