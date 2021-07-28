/* ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 16:45:17
 * Last edited:
 *   27/07/2021, 16:45:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a renewed iteration of the Array class, which is supposed to
 *   be a lot cleverer by disabling functions using SFINAE instead of
 *   template specializations.
**/

#ifndef TOOLS_ARRAY_HPP
#define TOOLS_ARRAY_HPP

#include <cstdint>
#include <type_traits>
#include <initializer_list>
#include <vector>

namespace Tools {
    /* The datatype used for the size of the Array. */
    using array_size_t = uint32_t;


    
    /* Namespace used for internal stuff. */
    namespace _intern {
        /* The CopyControl class, which uses template specializations to select an appropriate set of copy constructors for the Array class.
         * Note that this works because we're utilizing the fact that the compiler cannot generate the standard copy constructor/assignment operator for Array if it doesn't exist in its parent - which we control here. This way, we avoid specializating the entire Array or ArrayStorage class. */
        template <bool C>
        class CopyControl;

        template <>
        class CopyControl<true> {
        public:
            /* Default constructor for the CopyControl class. */
            CopyControl() = default;

            /* Copy constructor for the CopyControl class. */
            CopyControl(const CopyControl&) = default;
            /* Copy assignment for the CopyControl class. */
            CopyControl& operator=(const CopyControl&) = default;

            /* Move constructor for the CopyControl class. */
            CopyControl(CopyControl&&) = default;
            /* Move assignment for the CopyControl class. */
            CopyControl& operator=(CopyControl&&) = default;
        };

        template <>
        class CopyControl<false> {
        public:
            /* Default constructor for the CopyControl class. */
            CopyControl() = default;

            /* Copy constructor for the CopyControl class. */
            CopyControl(const CopyControl&) = delete;
            /* Copy assignment for the CopyControl class. */
            CopyControl& operator=(const CopyControl&) = delete;

            /* Move constructor for the CopyControl class. */
            CopyControl(CopyControl&&) = default;
            /* Move assignment for the CopyControl class. */
            CopyControl& operator=(CopyControl&&) = default;
        };



        /* The ArrayStorage class, which basically implements the Array's standard operators. */
        template <class T>
        class ArrayStorage {
        public:
            /* Pointer to the elements stored in the Array. */
            T* elements;
            /* The number of elements we have currently stored. */
            array_size_t length;
            /* The number of elements we have allocated space for. */
            array_size_t max_length;
        
        public:
            /* Default constructor for the ArrayStorage class, which initializes itself to 0. */
            ArrayStorage();
            /* Copy constructor for the ArrayStorage class. */
            ArrayStorage(const ArrayStorage& other);
            /* Move constructor for the ArrayStorage class. */
            ArrayStorage(ArrayStorage&& other);
            /* Destructor for the ArrayStorage class. */
            ~ArrayStorage();

            /* Copy assignment operator for the ArrayStorage class */
            inline ArrayStorage& operator=(const ArrayStorage& other) { return *this = ArrayStorage(other); }
            /* Move assignment operator for the ArrayStorage class */
            inline ArrayStorage& operator=(ArrayStorage&& other) { if (this != &other) { swap(*this, other); } return *this; }
            /* Swap operator for the ArrayStorage class. */
            friend void swap(ArrayStorage& as1, ArrayStorage& as2);

        };

        /* Swap operator for the ArrayStorage class. */
        template <class T>
        void swap(ArrayStorage<T>& as1, ArrayStorage<T>& as2);

    }



    /* The Array class, which can be used as a container for many things. It's optimized to work for containers that rarely (but still sometimes) have to resize. */
    template <class T, bool D = std::is_default_constructible<T>::value, bool C = std::is_copy_constructible<T>::value, bool M = std::is_move_constructible<T>::value>
    class Array: public _intern::CopyControl<C> {
    private:
        /* The internal data as wrapped by ArrayStorage. */
        _intern::ArrayStorage<T> storage;
    
    public:
        /* Default constructor for the Array class, which initializes it to not having any elements. */
        Array();
        /* Constructor for the Array class, which takes an initial amount to set its capacity to. Each element will thus be uninitialized. */
        Array(array_size_t initial_size);
        /* Constructor for the Array class, which takes a single element and repeats that the given amount of times. Makes use of the element's copy constructor. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        Array(const T& elem, array_size_t n_repeats);
        /* Constructor for the Array class, which takes an initializer_list to initialize the Array with. Makes use of the element's copy constructor. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        Array(const std::initializer_list<T>& list);
        /* Constructor for the Array class, which takes a raw C-style vector to copy elements from and its size. Note that the Array's element type must have a copy custructor defined. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        Array(T* list, array_size_t list_size);
        /* Constructor for the Array class, which takes a C++-style vector. Note that the Array's element type must have a copy custructor defined. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        Array(const std::vector<T>& list);

        /* Creates a new array that is a copy of this array with the elements in the given array copied and appended to them. Note that this requires the elements to be copy constructible. */
        template <typename U = Array<T>>
        inline auto operator+(const Array<T>& elems) const -> std::enable_if_t<M, U> { return Array<T>(*this).operator+=(elems); }
        /* Creates a new array that is a copy of this array with the elements in the given array appended to them (moved). Note that this still requires the elements to have some form of a move constructor defined. */
        template <typename U = Array<T>>
        inline auto operator+(Array<T>&& elems) const -> std::enable_if_t<M, U> { return Array<T>(*this).operator+=(std::move(elems)); }
        /* Adds a whole array worth of new elements to the array, copying them. Note that this requires the elements to be copy constructible. */
        template <typename U = Array<T>&>
        auto operator+=(const Array<T>& elems) -> std::enable_if_t<C, U>;
        /* Adds a whole array worth of new elements to the array, leaving the original array in an unused state (moving it). Note that this still requires the elements to have some form of a move constructor defined. */
        template <typename U = Array<T>&>
        auto operator+=(Array<T>&& elems) -> std::enable_if_t<M, U>;

        /* Adds a new element of type T to the array, copying it. Note that this requires the element to be copy constructible. */
        template <typename U = void>
        auto push_back(const T& elem) -> std::enable_if_t<C, U>;
        /* Adds a new element of type T to the array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
        template <typename U = void>
        auto push_back(T&& elem) -> std::enable_if_t<M, U>;
        /* Removes the last element from the array. */
        void pop_back();
        
        /* Erases an element with the given index from the array. Does nothing if the index is out-of-bounds. */
        template <typename U = void>
        auto erase(array_size_t index) -> std::enable_if_t<M, U>;
        /* Erases multiple elements in the given (inclusive) range from the array. Does nothing if the any index is out-of-bounds or if the start_index is larger than the stop_index. */
        template <typename U = void>
        auto erase(array_size_t start_index, array_size_t stop_index) -> std::enable_if_t<M, U>;
        /* Erases everything from the array, but leaves the internally located array intact. */
        void clear();
        /* Erases everything from the array, even removing the internal allocated array. */
        void reset();

        /* Re-allocates the internal array to the given size. Any leftover elements will be left unitialized, and elements that won't fit will be deallocated. */
        template <typename U = void>
        auto reserve(array_size_t new_size) -> std::enable_if_t<M, U>;
        /* Resizes the array to the given size. Any leftover elements will be initialized with their default constructor (and thus requires the type to have one), and elements that won't fit will be deallocated. */
        template <typename U = void>
        auto resize(array_size_t new_size) -> std::enable_if_t<std::conjunction<std::integral_constant<bool, D>, std::integral_constant<bool, M>>::value, U>;

        /* Returns a muteable reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline T& operator[](array_size_t index) { return this->elements[index]; }
        /* Returns a constant reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline const T& operator[](array_size_t index) const { return this->elements[index]; }
        /* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        T& at(array_size_t index);
        /* Returns a constant reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        inline const T& at(array_size_t index) const { return this->at(index); }
        /* Returns the last element in the list. */
        inline T& last() { return this->elements[this->length - 1]; }
        /* Returns the last element in the list. */
        inline const T& last() const { return this->elements[this->length - 1]; }

        /* Returns a muteable pointer to the internal data struct. Use this to fill the array using C-libraries, but beware that the array needs to have enough space reserved. Also note that object put here will still be deallocated by the Array using ~T(). The optional new_size parameter is used to update the size() value of the array, so it knows what is initialized and what is not. Leave it at numeric_limits<array_size_t>::max() to leave the array size unchanged. */
        T* wdata(array_size_t new_size = std::numeric_limits<array_size_t>::max());
        /* Returns a constant pointer to the internal data struct. Use this to read from the array using C-libraries, but beware that the array needs to have enough space reserved. */
        inline const T* rdata() const { return this->elements; }
        /* Returns true if there are no elements in this array, or false otherwise. */
        inline bool empty() const { return this->length == 0; }
        /* Returns the number of elements stored in this Array. */
        inline array_size_t size() const { return this->length; }
        /* Returns the number of elements this Array can store before resizing. */
        inline array_size_t capacity() const { return this->max_length; }

        /* Swap operator for the Array class. */
        friend void swap(Array& a1, Array& a2);

    };
    
    /* Swap operator for the Array class. */
    template <class T>
    void swap(Array<T>& a1, Array<T>& a2);

}

// Also get the .cpp
#include "Array.cpp"

#endif
