/* STATIC ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   12/08/2021, 15:48:25
 * Last edited:
 *   12/08/2021, 15:48:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the StaticArray class, which is like a static array except
 *   that it lives in heap memory.
**/

#ifndef TOOLS_STATIC_ARRAY_HPP
#define TOOLS_STATIC_ARRAY_HPP

#include <cstdint>
#include <type_traits>
#include <initializer_list>
#include <vector>

namespace Tools {
    /* The datatype used for the size of the StaticArray. */
    using static_array_size_t = uint32_t;


    
    /* Namespace used for internal stuff. */
    namespace _static_array_intern {
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



        /* The StaticArrayStorage class, which basically implements the StaticArray's standard operators. */
        template <class T, static_array_size_t MAX_SIZE>
        class StaticArrayStorage {
        public:
            /* Pointer to the elements stored in the StaticArray. */
            T* elements;
            /* The number of elements we have currently storedh. */
            static_array_size_t length;
            /* The number of elements we have allocated space for. */
            static constexpr const static_array_size_t max_length = MAX_SIZE;
        
        public:
            /* Default constructor for the StaticArrayStorage class, which initializes itself to 0. */
            StaticArrayStorage();
            /* Copy constructor for the StaticArrayStorage class. */
            StaticArrayStorage(const StaticArrayStorage& other);
            /* Move constructor for the StaticArrayStorage class. */
            StaticArrayStorage(StaticArrayStorage&& other);
            /* Destructor for the StaticArrayStorage class. */
            ~StaticArrayStorage();

            /* Copy assignment operator for the StaticArrayStorage class */
            inline StaticArrayStorage& operator=(const StaticArrayStorage& other) { return *this = StaticArrayStorage(other); }
            /* Move assignment operator for the StaticArrayStorage class */
            inline StaticArrayStorage& operator=(StaticArrayStorage&& other) { if (this != &other) { swap(*this, other); } return *this; }
            /* Swap operator for the StaticArrayStorage class. */
            friend void swap(StaticArrayStorage& as1, StaticArrayStorage& as2) {
                using std::swap;

                swap(as1.elements, as2.elements);
                swap(as1.length, as2.length);
            }

        };
    }



    /* The StaticArray class, which is like a static C-array except that it lives on the heap. */
    template <class T, static_array_size_t MAX_SIZE, bool D = std::is_default_constructible<T>::value, bool C = std::is_copy_constructible<T>::value, bool M = std::is_move_constructible<T>::value>
    class StaticArray: public _static_array_intern::CopyControl<T> {
    private:
        /* The storage class of the static array. */
        _static_array_intern::StaticArrayStorage<T, MAX_SIZE> storage;
    
    public:
        /* Default constructor for the StaticArray class, which initializes all elements to unitialized. */
        StaticArray();
        /* Constructor for the StaticArray class, which takes a single element and repeats it n_repeats times. Makes use of the element's copy constructor. Throws errors if the length is longer than the StaticArray's length. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        StaticArray(const T& elem, static_array_size_t n_repeats);
        /* Constructor for the StaticArray class, which takes an initializer_list to initialize the StaticArray with. Makes use of the element's copy constructor. Throws errors if the length is longer than the StaticArray's length. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        StaticArray(const std::initializer_list<T>& list);
        /* Constructor for the StaticArray class, which takes a raw C-style vector to copy elements from. Note that the Array's element type must have a copy custructor defined. Throws errors if the length is longer than the StaticArray's length. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        StaticArray(const T* list, static_array_size_t list_size);
        /* Constructor for the StaticArray class, which takes a C++-style vector. Note that the StaticArray's element type must have a copy custructor defined. Throws errors if the length is longer than the StaticArray's length. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        StaticArray(const std::vector<T>& list);

        /* Adds a new element of type T to the front of the static array, pushing the rest back. The element is initialized with with its default constructor. Only needs a default constructor to be present. */
        template <typename U = void>
        auto push_front() -> std::enable_if_t<D, U>;
        /* Adds a new element of type T to the front of the static array, pushing the rest back. The element is copied using its copy constructor, which it is required to have. */
        template <typename U = void>
        auto push_front(const T& elem) -> std::enable_if_t<C, U>;
        /* Adds a new element of type T to the front of the static array, pushing the rest back. The element is left in an usused state (moving it). Note that this requires the element to be move constructible. */
        template <typename U = void>
        auto push_front(T&& elem) -> std::enable_if_t<M, U>;
        /* Removes the first element from the static array, moving the rest one to the front. */
        void pop_front();

        /* Adds a new element of type T to the end of the static array, initializing it with its default constructor. Only needs a default constructor to be present. */
        template <typename U = void>
        auto push_back() -> std::enable_if_t<D, U>;
        /* Adds a new element of type T to the end of the static array, copying it. Note that this requires the element to be copy constructible. */
        template <typename U = void>
        auto push_back(const T& elem) -> std::enable_if_t<C, U>;
        /* Adds a new element of type T to the end of the static array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
        template <typename U = void>
        auto push_back(T&& elem) -> std::enable_if_t<M, U>;
        /* Removes the last element from the static array. */
        void pop_back();

        /* Erases an element with the given index from the array. Does nothing if the index is out-of-bounds. */
        template <typename U = void>
        auto erase(array_size_t index) -> std::enable_if_t<M, U>;
        /* Erases multiple elements in the given (inclusive) range from the array. Does nothing if the any index is out-of-bounds or if the start_index is larger than the stop_index. */
        template <typename U = void>
        auto erase(array_size_t start_index, array_size_t stop_index) -> std::enable_if_t<M, U>;
        /* Erases everything from the array, but leaves the internally located array intact. */
        void clear();

        /* Returns a muteable reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline T& operator[](static_array_size_t index) { return this->storage.elements[index]; }
        /* Returns a constant reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline const T& operator[](static_array_size_t index) const { return this->storage.elements[index]; }
        /* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        T& at(static_array_size_t index);
        /* Returns a constant reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        inline const T& at(static_array_size_t index) const { return this->at(index); }
        /* Returns the first element in the list. */
        inline T& first() { return this->storage.elements[0]; }
        /* Returns the first element in the list. */
        inline const T& first() const { return this->storage.elements[0]; }
        /* Returns the last element in the list. */
        inline T& last() { return this->storage.elements[this->storage.length - 1]; }
        /* Returns the last element in the list. */
        inline const T& last() const { return this->storage.elements[this->storage.length - 1]; }

        /* Returns a muteable pointer to the internal data struct. Use this to fill the array using C-libraries, but beware that the array needs to have enough space reserved. */
        inline T* wdata() { return this->storage.elements; };
        /* Returns a constant pointer to the internal data struct. Use this to read from the array using C-libraries, but beware that the array needs to have enough space reserved. */
        inline const T* rdata() const { return this->storage.elements; }
        /* Returns the number of elements stored in this Array. */
        inline array_size_t size() const { return this->storage.length; }
        /* Returns the number of elements stored in this Array. */
        inline constexpr array_size_t capcity() const { return this->storage.max_length; }

        /* Swap operator for the Array class. */
        friend void swap(StaticArray& a1, StaticArray& a2) {
            using std::swap;

            swap(a1.storage, a2.storage);
        }

    };

}

#endif
