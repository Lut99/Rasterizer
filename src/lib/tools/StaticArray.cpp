/* STATIC ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   12/08/2021, 15:48:18
 * Last edited:
 *   12/08/2021, 15:48:18
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the StaticArray class, which is like a static array except
 *   that it lives in heap memory.
**/

#include <string>
#include <cstring>
#include <stdexcept>

#include "StaticArray.hpp"


/***** STATICARRAYSTORAGE CLASS *****/
/* Default constructor for the StaticArrayStorage class, which initializes itself to 0. */
template <class T, Tools::static_array_size_t MAX_SIZE>
Tools::_static_array_intern::StaticArrayStorage<T, MAX_SIZE>::StaticArrayStorage() :
    elements((T*) malloc(this->max_length * sizeof(T))),
    length(0)
{
    if (this->elements == nullptr) { throw std::bad_alloc(); }
}

/* Copy constructor for the StaticArrayStorage class. */
template <class T, Tools::static_array_size_t MAX_SIZE>
Tools::_static_array_intern::StaticArrayStorage<T, MAX_SIZE>::StaticArrayStorage(const StaticArrayStorage& other) :
    elements((T*) malloc(this->max_length * sizeof(T))),
    length(other.length)
{
    // Make sure the malloc went alright
    if (this->elements == nullptr) { throw std::bad_alloc(); }

    // Copy the elements
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        // Simply call memcpy
        memcpy(this->elements, other.elements, this->length * sizeof(T));
    } else {
        // Copy the elements one-by-one
        for (static_array_size_t i = 0; i < this->length; i++) {
            new(this->elements + i) T(other.elements[i]);
        }
    }
}

/* Move constructor for the StaticArrayStorage class. */
template <class T, Tools::static_array_size_t MAX_SIZE>
Tools::_static_array_intern::StaticArrayStorage<T, MAX_SIZE>::StaticArrayStorage(StaticArrayStorage&& other) :
    elements(other.elements),
    length(other.length)
{
    other.elements = nullptr;
}

/* Destructor for the StaticArrayStorage class. */
template <class T, Tools::static_array_size_t MAX_SIZE>
Tools::_static_array_intern::StaticArrayStorage<T, MAX_SIZE>::~StaticArrayStorage() {
    if (this->elements != nullptr) {
        if constexpr (std::conjunction<std::is_destructible, std::negation<std::is_trivially_destructible<T>>>::value) {
            for (static_array_size_t i = 0; i < this->length; i++) {
                this->elements[i].~T();
            }
        }
        free(this->elements);
    }
}





/***** STATICARRAY CLASS *****/
/* Default constructor for the StaticArray class, which initializes all elements to unitialized. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
Tools::StaticArray<T, MAX_SIZE, D, C, M>::StaticArray() {}

/* Constructor for the StaticArray class, which takes a single element and repeats it n_repeats times. Makes use of the element's copy constructor. Throws errors if the length is longer than the StaticArray's length. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U, typename>
Tools::StaticArray<T, MAX_SIZE, D, C, M>::StaticArray(const T& elem, static_array_size_t n_repeats) {
    // Simply copy the element enough times
    for (static_array_size_t i = 0; i < n_repeats; i++) {
        new(this->storage.elements + i) T(elem);
    }
    this->storage.length = n_repeats;
}

/* Constructor for the StaticArray class, which takes an initializer_list to initialize the StaticArray with. Makes use of the element's copy constructor. Throws errors if the length is longer than the StaticArray's length. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U, typename>
Tools::StaticArray<T, MAX_SIZE, D, C, M>::StaticArray(const std::initializer_list<T>& list):
    StaticArray(list.begin(), static_cast<static_array_size_t>(list.size()))
{}

/* Constructor for the StaticArray class, which takes a raw C-style vector to copy elements from. Note that the Array's element type must have a copy custructor defined. Throws errors if the length is longer than the StaticArray's length. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U, typename>
Tools::StaticArray<T, MAX_SIZE, D, C, M>::StaticArray(const T* list, static_array_size_t list_size) {
    // Copy the list's elements over
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(this->storage.elements, list, list_size * sizeof(T));
    } else {
        for (static_array_size_t i = 0; i < list_size; i++) {
            new(this->storage.elements + i) T(list[i]);
        }
    }
    this->storage.length = list_size;
}

/* Constructor for the StaticArray class, which takes a C++-style vector. Note that the StaticArray's element type must have a copy custructor defined. Throws errors if the length is longer than the StaticArray's length. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U, typename>
Tools::StaticArray<T, MAX_SIZE, D, C, M>::StaticArray(const std::vector<T>& list) :
    StaticArray(list.data(), static_cast<static_array_size_t>(list.size()))
{}



/* Adds a new element of type T to the front of the static array, pushing the rest back. The element is initialized with with its default constructor. Only needs a default constructor to be present. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::push_front() -> std::enable_if_t<D, U> {
    // Throw an error if we don't have enough space
    if (this->storage.length >= this->storage.max_length) {
        throw std::out_of_range("Cannot push_front() to fully populate StaticArray of size " + std::to_string(this->storage.max_length));
    }

    // Otherwise, move the rest one element forward
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        // Use memmove to move them all one up
        memmove(this->storage.elements + 1, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        // Note that the loop begins at this->storage.length - 1
        for (static_array_size_t i = this->storage.length; i-- > 0 ;) {
            new(this->storage.elements + i + 1) T(std::move(this->storage[i]));
        }
    }

    // Insert the element at the start
    new(this->storage.elements) T();
    ++this->storage.length;
}

/* Adds a new element of type T to the front of the static array, pushing the rest back. The element is copied using its copy constructor, which it is required to have. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::push_front(const T& elem) -> std::enable_if_t<C, U> {
    // Throw an error if we don't have enough space
    if (this->storage.length >= this->storage.max_length) {
        throw std::out_of_range("Cannot push_front(const T&) to fully populate StaticArray of size " + std::to_string(this->storage.max_length));
    }

    // Otherwise, move the rest one element forward
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        // Use memmove to move them all one up
        memmove(this->storage.elements + 1, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        // Note that the loop begins at this->storage.length - 1
        for (static_array_size_t i = this->storage.length; i-- > 0 ;) {
            new(this->storage.elements + i + 1) T(std::move(this->storage[i]));
        }
    }

    // Insert the element at the start
    new(this->storage.elements) T(elem);
    ++this->storage.length;
}

/* Adds a new element of type T to the front of the static array, pushing the rest back. The element is left in an usused state (moving it). Note that this requires the element to be move constructible. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::push_front(T&& elem) -> std::enable_if_t<M, U> {
    // Throw an error if we don't have enough space
    if (this->storage.length >= this->storage.max_length) {
        throw std::out_of_range("Cannot push_front(T&&) to fully populate StaticArray of size " + std::to_string(this->storage.max_length));
    }

    // Otherwise, move the rest one element forward
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        // Use memmove to move them all one up
        memmove(this->storage.elements + 1, this->storage.elements, this->storage.length * sizeof(T));
    } else {
        // Note that the loop begins at this->storage.length - 1
        for (static_array_size_t i = this->storage.length; i-- > 0 ;) {
            new(this->storage.elements + i + 1) T(std::move(this->storage[i]));
        }
    }

    // Insert the element at the start
    new(this->storage.elements) T(std::move(elem));
    ++this->storage.length;
}

/* Removes the first element from the static array, moving the rest one to the front. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
void Tools::StaticArray<T, MAX_SIZE, D, C, M>::pop_front() {
    // Do nothing if empty
    if (this->storage.length == 0) { return; }

    // Delete the first element
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>::value) {
        this->storage.elements[0].~T();
    }
    --this->storage.length;

    // Move all other elements one backward
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        // Use memmove to move them all one up
        memmove(this->storage.elements, this->storage.elements + 1, this->storage.length * sizeof(T));
    } else {
        for (static_array_size_t i = 0; i < this->storage.length; i++) {
            new(this->storage.elements + i) T(std::move(this->storage[i + 1]));
        }
    }

    // Done
}



/* Adds a new element of type T to the end of the static array, initializing it with its default constructor. Only needs a default constructor to be present. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::push_back() -> std::enable_if_t<D, U> {
    // Throw an error if we don't have enough space
    if (this->storage.length >= this->storage.max_length) {
        throw std::out_of_range("Cannot push_back() to fully populate StaticArray of size " + std::to_string(this->storage.max_length));
    }

    // Otherwise, put it at the end
    new(this->storage.elements + this->storage.length++) T();
}

/* Adds a new element of type T to the end of the static array, copying it. Note that this requires the element to be copy constructible. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::push_back(const T& elem) -> std::enable_if_t<C, U> {
    // Throw an error if we don't have enough space
    if (this->storage.length >= this->storage.max_length) {
        throw std::out_of_range("Cannot push_back(const T&) to fully populate StaticArray of size " + std::to_string(this->storage.max_length));
    }

    // Otherwise, put it at the end
    new(this->storage.elements + this->storage.length++) T(elem);
}

/* Adds a new element of type T to the end of the static array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::push_back(T&& elem) -> std::enable_if_t<M, U> {
    // Throw an error if we don't have enough space
    if (this->storage.length >= this->storage.max_length) {
        throw std::out_of_range("Cannot push_back(T&&) to fully populate StaticArray of size " + std::to_string(this->storage.max_length));
    }

    // Otherwise, put it at the end
    new(this->storage.elements + this->storage.length++) T(std::move(elem));
}

/* Removes the last element from the static array. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
void Tools::StaticArray<T, MAX_SIZE, D, C, M>::pop_back() {
    // Do nothing if empty
    if (this->storage.length == 0) { return; }

    // Delete the last element
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>::value) {
        this->storage.elements[this->storage.length - 1].~T();
    }
    --this->storage.length;

    // Done
}



/* Erases an element with the given index from the array. Does nothing if the index is out-of-bounds. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::erase(array_size_t index) -> std::enable_if_t<M, U> {
    // Make sure we're within range
    if (index >= this->storage.length) { return; }

    // If so, delete the element that is indexed
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>::value) {
        this->storage.elements[index].~T();
    }
    --this->storage.length;

    // Next, move everything after that one back
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        // Use memmove to move them all one up
        memmove(this->storage.elements + index + 1, this->storage.elements + index, (this->storage.length - index) * sizeof(T));
    } else {
        for (array_size_t i = index; i < this->storage.length; i++) {
            new(this->storage.elements + i) T(std::move(this->storage.elements[i + 1]));
        }
    }
}

/* Erases multiple elements in the given (inclusive) range from the array. Does nothing if the any index is out-of-bounds or if the start_index is larger than the stop_index. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
template <typename U>
auto Tools::StaticArray<T, MAX_SIZE, D, C, M>::erase(array_size_t start_index, array_size_t stop_index) -> std::enable_if_t<M, U> {
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

/* Erases everything from the array, but leaves the internally located array intact. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
void Tools::StaticArray<T, MAX_SIZE, D, C, M>::clear() {
    // Delete all elements if needed
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>::value) {
        for (static_array_size_t i = 0; i < this->storage.length; i++) {
            this->storage.elements[i].~T();
        }
    }

    // Set the length to 0, done
    this->storage.length = 0;
}



/* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
template <class T, Tools::static_array_size_t MAX_SIZE, bool D, bool C, bool M>
T& Tools::StaticArray<T, MAX_SIZE, D, C, M>::at(static_array_size_t index) {
    if (index >= this->storage.length) { throw std::out_of_range("Index " + std::to_string(index) + " is out-of-bounds for StaticArray of size " + std::to_string(this->storage.length)); }
    return this->storage.elements[index];
}
