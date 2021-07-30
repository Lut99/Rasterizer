/* LINKED ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 14:52:35
 * Last edited:
 *   30/07/2021, 14:52:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the LinkedArray class, which is our implementation of a
 *   linked list.
**/

#ifndef TOOLS_LINKED_ARRAY_HPP
#define TOOLS_LINKED_ARRAY_HPP

#include <cstdint>
#include <type_traits>
#include <initializer_list>
#include <vector>

namespace Tools {
    /* The datatype used for the size of the Array. */
    using linked_array_size_t = uint32_t;




    /* The LinkedArray class, which implements a LinkedList. */
    template <class T, bool D = std::is_default_constructible<T>::value, bool C = std::is_copy_constructible<T>::value, bool M = std::is_move_constructible<T>::value>
    class LinkedArray;

    /* Namespace used for internal stuff to the LinkedArray class. */
    namespace _linked_array_intern {
        /* The CopyControl class, which uses template specializations to select an appropriate set of copy constructors for the LinkedArray class.
         * Note that this works because we're utilizing the fact that the compiler cannot generate the standard copy constructor/assignment operator for LinkedArray if it doesn't exist in its parent - which we control here. This way, we avoid specializating the entire LinkedArray or LinkedArrayStorage class. */
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



        /* A single link in the LinkedArray. */
        template <class T>
        struct LinkedArrayLink {
            /* The value stored at this location in the LinkedArray. */
            T value;
            /* Pointer to the next link in the LinkedArray. */
            LinkedArrayLink* next;
            /* Pointer to the previous link in the LinkedArray. */
            LinkedArrayLink* prev;
        };

        /* The LinkedArrayStorage class, which basically implements the rule of 7 for the LinkedArray. */
        template <class T>
        class LinkedArrayStorage {
        private:
            /* The first node in the array. */
            LinkedArrayLink<T>* head;
            /* The last node in the array. */
            LinkedArrayLink<T>* tail;
            /* The number of elements in the array. */
            linked_array_size_t length;
        
            /* Declare the LinkedArray as friend. */
            friend class LinkedArray<T>;

        public:
            /* Default constructor for the LinkedArrayStorage, which initializes it to empty. */
            LinkedArrayStorage();
            /* Copy constructor for the LinkedArrayStorage class. */
            LinkedArrayStorage(const LinkedArrayStorage& other);
            /* Move constructor for the LinkedArrayStorage class. */
            LinkedArrayStorage(LinkedArrayStorage&& other);
            /* Destructor for the LinkedArrayStorage class. */
            ~LinkedArrayStorage();

            /* Copy assignment operator for the LinkedArrayStorage class. */
            inline LinkedArrayStorage& operator=(const LinkedArrayStorage& other) { return *this = LinkedArrayStorage(other); }
            /* Move assignment operator for the LinkedArrayStorage class. */
            inline LinkedArrayStorage& operator=(LinkedArrayStorage&& other) { if (this != &other) { swap(*this, other); } return *this; }
            /* Swap operator for the LinkedArrayStorage class. */
            friend void swap(LinkedArrayStorage& las1, LinkedArrayStorage& las2) {
                using std::swap;

                swap(las1.head, las2.head);
                swap(las1.tail, las2.tail);
                swap(las1.length, las2.length);
            }

        };



        /* Iterator for the LinkedArray class. */
        template <class T>
        class LinkedArrayIterator {
        protected:
            /* The node where we current are. */
            LinkedArrayLink<T>* link;
        
            /* Declare the LinkedArray class as friend. */
            friend class LinkedArray<T>;

        public:
            /* Constructor for the LinkedArrayIterator class, which takes the linked array where it should start (or nullptr to indicate end()). */
            LinkedArrayIterator(LinkedArrayLink<T>* link): link(link) {}

            /* Allows the iterator to be compared with the given one. */
            inline bool operator==(const LinkedArrayIterator& other) const { return this->link == other.link; }
            /* Allows the iterator to be compared with the given one, by inequality. */
            inline bool operator!=(const LinkedArrayIterator& other) const { return !(this->link == other.link); }

            /* Increments the iterator by one (prefix). */
            LinkedArrayIterator& operator++();
            /* Increments the iterator by one (postfix). */
            LinkedArrayIterator operator++(int) const;
            /* Decrements the iterator by one (prefix). */
            LinkedArrayIterator& operator--();
            /* Decrements the iterator by one (postfix). */
            LinkedArrayIterator operator--(int) const;

            /* Increments the iterator by N steps. */
            LinkedArrayIterator& operator+=(linked_array_size_t N);
            /* Returns a new iterator that is N steps ahead of this one. */
            inline LinkedArrayIterator operator+(linked_array_size_t N) const { return LinkedArrayIterator(*this) += N; }
            /* Decrements the iterator by N steps. */
            LinkedArrayIterator& operator-=(linked_array_size_t N);
            /* Returns a new iterator that is N steps behind this one. */
            inline LinkedArrayIterator operator-(linked_array_size_t N) const { return LinkedArrayIterator(*this) -= N; }

            /* Dereferences the iterator. */
            inline T& operator*() { return link->value; }
            /* Dereferences the iterator immutably. */
            inline const T& operator*() const { return link->value; }

        };

        /* Constant iterator for the LinkedArray class, which simply inherits from the normal iterator except that it has no mutable dereferencer. */
        template <class T>
        class LinkedArrayConstIterator: public LinkedArrayIterator<T> {
        private:
            /* Declare the LinkedArray class as friend. */
            friend class LinkedArray<T>;

        public:
            /* Constructor for the LinkedArrayConstIterator class, which takes the linked array where it should start (or nullptr to indicate end()). */
            LinkedArrayConstIterator(const LinkedArrayLink<T>* link): LinkedArrayIterator<T>(const_cast<LinkedArrayLink<T>*>(link)) {}

            /* Allows the iterator to be compared with the given one. */
            inline bool operator==(const LinkedArrayConstIterator& other) const { return LinkedArrayIterator<T>::operator==(other); }
            /* Allows the iterator to be compared with the given one, by inequality. */
            inline bool operator!=(const LinkedArrayConstIterator& other) const { return LinkedArrayIterator<T>::operator!=(other); }

            /* Increments the iterator by one (prefix). */
            inline LinkedArrayConstIterator& operator++() { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator++(); }
            /* Increments the iterator by one (postfix). */
            inline LinkedArrayConstIterator operator++(int) const { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator++(); }
            /* Decrements the iterator by one (prefix). */
            inline LinkedArrayConstIterator& operator--() { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator--(); }
            /* Decrements the iterator by one (postfix). */
            inline LinkedArrayConstIterator operator--(int) const { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator--(); }

            /* Increments the iterator by N steps. */
            inline LinkedArrayConstIterator& operator+=(linked_array_size_t N) { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator+=(N); }
            /* Returns a new iterator that is N steps ahead of this one. */
            inline LinkedArrayConstIterator operator+(linked_array_size_t N) const { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator+(N); }
            /* Decrements the iterator by N steps. */
            inline LinkedArrayConstIterator& operator-=(linked_array_size_t N) { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator-=(N); }
            /* Returns a new iterator that is N steps behind this one. */
            inline LinkedArrayConstIterator operator-(linked_array_size_t N) const { return (LinkedArrayConstIterator&) LinkedArrayIterator<T>::operator-(N); }

            /* Dereferences the iterator, but is deleted for the constant iterator. */
            T& operator*() = delete;

        };

        /* Reverse iterator for the LinkedArray class. */
        template <class T>
        class LinkedArrayReverseIterator: public LinkedArrayIterator<T> {
        private:
            /* Declare the LinkedArray class as friend. */
            friend class LinkedArray<T>;
            
        public:
            /* Constructor for the LinkedArrayReverseIterator class, which takes the linked array where it should start (or nullptr to indicate end()). */
            LinkedArrayReverseIterator(LinkedArrayLink<T>* link): LinkedArrayIterator<T>(link) {}

            /* Allows the iterator to be compared with the given one. */
            inline bool operator==(const LinkedArrayReverseIterator& other) const { return LinkedArrayIterator<T>::operator==(other); }
            /* Allows the iterator to be compared with the given one, by inequality. */
            inline bool operator!=(const LinkedArrayReverseIterator& other) const { return LinkedArrayIterator<T>::operator!=(other); }

            /* Increments the iterator by one (prefix). */
            inline LinkedArrayReverseIterator& operator++() { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator--(); }
            /* Increments the iterator by one (postfix). */
            inline LinkedArrayReverseIterator operator++(int) const { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator--(); }
            /* Decrements the iterator by one (prefix). */
            inline LinkedArrayReverseIterator& operator--() { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator++(); }
            /* Decrements the iterator by one (postfix). */
            inline LinkedArrayReverseIterator operator--(int) const { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator++(); }

            /* Increments the iterator by N steps. */
            inline LinkedArrayReverseIterator& operator+=(linked_array_size_t N) { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator-=(N); }
            /* Returns a new iterator that is N steps ahead of this one. */
            inline LinkedArrayReverseIterator operator+(linked_array_size_t N) const { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator-(N); }
            /* Decrements the iterator by N steps. */
            inline LinkedArrayReverseIterator& operator-=(linked_array_size_t N) { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator+=(N); }
            /* Returns a new iterator that is N steps behind this one. */
            inline LinkedArrayReverseIterator operator-(linked_array_size_t N) const { return (LinkedArrayReverseIterator&) LinkedArrayIterator<T>::operator+(N); }

        };

        /* Reverse constant iterator for the LinkedArray class. */
        template <class T>
        class LinkedArrayReverseConstantIterator: public LinkedArrayReverseIterator<T> {
        private:
            /* Declare the LinkedArray class as friend. */
            friend class LinkedArray<T>;
            
        public:
            /* Constructor for the LinkedArrayReverseConstantIterator class, which takes the linked array where it should start (or nullptr to indicate end()). */
            LinkedArrayReverseConstantIterator(const LinkedArrayLink<T>* link): LinkedArrayReverseIterator<T>(const_cast<LinkedArrayLink<T>*>(link)) {}

            /* Allows the iterator to be compared with the given one. */
            inline bool operator==(const LinkedArrayReverseConstantIterator& other) const { return LinkedArrayReverseIterator<T>::operator==(other); }
            /* Allows the iterator to be compared with the given one, by inequality. */
            inline bool operator!=(const LinkedArrayReverseConstantIterator& other) const { return LinkedArrayReverseIterator<T>::operator!=(other); }

            /* Increments the iterator by one (prefix). */
            inline LinkedArrayReverseConstantIterator& operator++() { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator++(); }
            /* Increments the iterator by one (postfix). */
            inline LinkedArrayReverseConstantIterator operator++(int) const { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator++(); }
            /* Decrements the iterator by one (prefix). */
            inline LinkedArrayReverseConstantIterator& operator--() { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator--(); }
            /* Decrements the iterator by one (postfix). */
            inline LinkedArrayReverseConstantIterator operator--(int) const { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator--(); }

            /* Increments the iterator by N steps. */
            inline LinkedArrayReverseConstantIterator& operator+=(linked_array_size_t N) { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator+=(N); }
            /* Returns a new iterator that is N steps ahead of this one. */
            inline LinkedArrayReverseConstantIterator operator+(linked_array_size_t N) const { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator+(N); }
            /* Decrements the iterator by N steps. */
            inline LinkedArrayReverseConstantIterator& operator-=(linked_array_size_t N) { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator-=(N); }
            /* Returns a new iterator that is N steps behind this one. */
            inline LinkedArrayReverseConstantIterator operator-(linked_array_size_t N) const { return (LinkedArrayReverseConstantIterator&) LinkedArrayReverseIterator<T>::operator-(N); }

            /* Dereferences the iterator, but is deleted for the constant iterator. */
            T& operator*() = delete;

        };

    };



    /* The LinkedArray class, which implements a LinkedList. */
    template <class T, bool D, bool C, bool M>
    class LinkedArray: public _linked_array_intern::CopyControl<C> {
    public:
        /* Muteable iterator for the LinkedArray class. */
        using iterator = _linked_array_intern::LinkedArrayIterator<T>;
        /* Immuteable iterator for the LinkedArray class. */
        using const_iterator = _linked_array_intern::LinkedArrayConstIterator<T>;
        /* Muteable reverse iterator for the LinkedArray class. */
        using reverse_iterator = _linked_array_intern::LinkedArrayReverseIterator<T>;
        /* Immuteable reverse iterator for the LinkedArray class. */
        using reverse_const_iterator = _linked_array_intern::LinkedArrayReverseConstantIterator<T>;

    private:
        /* The data stored in the LinkedArray. */
        _linked_array_intern::LinkedArrayStorage<T> storage;


        /* Private helper function that actually appends a given element to the linked array. */
        void _push_back(_linked_array_intern::LinkedArrayLink<T>* new_link);
        /* Private helper function that actually prepends a given element to the linked array. */
        void _push_front(_linked_array_intern::LinkedArrayLink<T>* new_link);

    public:
        /* Default constructor for the LinkedArray, which initializes it to an empty list. */
        LinkedArray();
        /* Constructor for the LinkedArray class, which takes a single element and repeats that the given amount of times. Makes use of the element's copy constructor. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        LinkedArray(const T& elem, linked_array_size_t n_repeats);
        /* Constructor for the LinkedArray class, which takes an initializer_list to initialize the LinkedArray with. Makes use of the element's copy constructor. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        LinkedArray(const std::initializer_list<T>& list);
        /* Constructor for the LinkedArray class, which takes a raw C-style vector to copy elements from and its size. Note that the Array's element type must have a copy custructor defined. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        LinkedArray(const T* list, linked_array_size_t list_size);
        /* Constructor for the LinkedArray class, which takes a C++-style vector. Note that the Array's element type must have a copy custructor defined. */
        template <typename U = void, typename = std::enable_if_t<C, U>>
        LinkedArray(const std::vector<T>& list);

        /* Creates a new linked array that is a copy of this linked array with the elements in the given array copied and appended to them. Note that this requires the elements to be copy constructible. */
        template <typename U = LinkedArray<T>>
        inline auto operator+(const LinkedArray<T>& elems) const -> std::enable_if_t<M, U> { return LinkedArray<T>(*this).operator+=(elems); }
        /* Creates a new linked array that is a copy of this linked array with the elements in the given array appended to them (moved). Does not require a move constructor to be defined. */
        inline void operator+(LinkedArray<T>&& elems) const { return LinkedArray<T>(*this).operator+=(std::move(elems)); }
        /* Adds a whole linked array worth of new elements to this linked array, copying them. Note that this requires the elements to be copy constructible. */
        template <typename U = LinkedArray<T>&>
        auto operator+=(const LinkedArray<T>& elems) -> std::enable_if_t<C, U>;
        /* Adds a whole linked array worth of new elements to this linked array, leaving the original array in an unused state (moving it). Does not require a move constructor to be defined. */
        void operator+=(LinkedArray<T>&& elems);

        /* Adds a new element of type T to the back of the linked array, initializing it with its default constructor. Only needs a default constructor to be present. */
        template <typename U = void>
        auto push_back() -> std::enable_if_t<D, U>;
        /* Adds a new element of type T to the back of the linked array, copying it. Note that this requires the element to be copy constructible. */
        template <typename U = void>
        auto push_back(const T& elem) -> std::enable_if_t<C, U>;
        /* Adds a new element of type T to the back of the linked array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
        template <typename U = void>
        auto push_back(T&& elem) -> std::enable_if_t<M, U>;
        /* Removes the last element from the array. */
        void pop_back();

        /* Adds a new element of type T to the start of the linked array, initializing it with its default constructor. Only needs a default constructor to be present. */
        template <typename U = void>
        auto push_front() -> std::enable_if_t<D, U>;
        /* Adds a new element of type T to the start of the linked array, copying it. Note that this requires the element to be copy constructible. */
        template <typename U = void>
        auto push_front(const T& elem) -> std::enable_if_t<C, U>;
        /* Adds a new element of type T to the start of the linked array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
        template <typename U = void>
        auto push_front(T&& elem) -> std::enable_if_t<M, U>;
        /* Removes the first element from the array. */
        void pop_front();

        /* Erases an element with the given index from the linked array. Does nothing if the index is out-of-bounds. */
        void erase(linked_array_size_t index);
        /* Erases an element referenced by the given iterator. Does nothing if the index is out-of-bounds. */
        void erase(const iterator& iter);
        /* Erases an element referenced by the given constant iterator. Does nothing if the index is out-of-bounds. */
        inline void erase(const const_iterator& iter) { return this->erase((const iterator&) iter); }
        /* Erases an element referenced by the given reverse iterator. Does nothing if the index is out-of-bounds. */
        inline void erase(const reverse_iterator& iter) { return this->erase((const iterator&) iter); }
        /* Erases an element referenced by the given reverse, constant iterator. Does nothing if the index is out-of-bounds. */
        inline void erase(const reverse_const_iterator& iter) { return this->erase((const iterator&) iter); }
        /* Erases all elements until (and including) the element referenced by the given iterator. */
        void erase_until(const iterator& iter);
        /* Erases all elements until (and including) the element referenced by the given constant iterator. */
        inline void erase_until(const const_iterator& iter) { return this->erase_until((const iterator&) iter); }
        /* Erases all elements until (and including) the element referenced by the given reverse iterator. */
        inline void erase_until(const reverse_iterator& iter) { return this->erase_from((const iterator&) iter); }
        /* Erases all elements until (and including) the element referenced by the given reverse, constant iterator. */
        inline void erase_until(const reverse_const_iterator& iter) { return this->erase_from((const iterator&) iter); }
        /* Erases all elements from (and including) the element referenced by the given iterator. */
        void erase_from(const iterator& iter);
        /* Erases all elements from (and including) the element referenced by the given constant iterator. */
        inline void erase_from(const const_iterator& iter) { return this->erase_from((const iterator&) iter); }
        /* Erases all elements from (and including) the element referenced by the given reverse iterator. */
        inline void erase_from(const reverse_iterator& iter) { return this->erase_until((const iterator&) iter); }
        /* Erases all elements from (and including) the element referenced by the given reverse, constant iterator. */
        inline void erase_from(const reverse_const_iterator& iter) { return this->erase_until((const iterator&) iter); }
        /* Erases everything from the linked array, thus deallocating all its elements. */
        void reset();

        /* Resizes the linked array to the given size. Any leftover elements will be initialized with their default constructor (and thus requires the type to have one), and elements that won't fit will be deallocated. */
        template <typename U = void>
        auto resize(linked_array_size_t new_size) -> std::enable_if_t<D, U>;

        /* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        T& at(linked_array_size_t index);
        /* Returns a constant reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        inline const T& at(linked_array_size_t index) const { return this->at(index); }
        /* Returns the first element in the list. */
        inline T& first() { return *this->storage.head; }
        /* Returns the first element in the list. */
        inline const T& first() const { return *this->storage.head; }
        /* Returns the last element in the list. */
        inline T& last() { return *this->storage.tail; }
        /* Returns the last element in the list. */
        inline const T& last() const { return *this->storage.tail; }

        /* Returns true if there are no elements in this array, or false otherwise. */
        inline bool empty() const { return this->storage.length == 0; }
        /* Returns the number of elements stored in this Array. */
        inline linked_array_size_t size() const { return this->storage.length; }

        /* Returns a muteable iterator to the beginning of the LinkedArray. */
        inline iterator begin() { return iterator(this->storage.head); }
        /* Returns a muteable iterator to the end of the LinkedArray. */
        inline iterator end() { return iterator(this->storage.tail); }
        /* Returns an immuteable iterator to the beginning of the LinkedArray. */
        inline const_iterator begin() const { return const_iterator(this->storage.head); }
        /* Returns an immuteable iterator to the end of the LinkedArray. */
        inline const_iterator end() const { return const_iterator(this->storage.tail); }
        /* Returns an immuteable iterator to the beginning of the LinkedArray. */
        inline const_iterator cbegin() const { return this->begin(); }
        /* Returns an immuteable iterator to the end of the LinkedArray. */
        inline const_iterator cend() const { return this->end(); }
        /* Returns a muteable iterator to the beginning of the LinkedArray. */
        inline reverse_iterator rbegin() { return reverse_iterator(this->storage.tail); }
        /* Returns a muteable reverse iterator to the end of the LinkedArray. */
        inline reverse_iterator rend() { return reverse_iterator(this->storage.head); }
        /* Returns an immuteable reverse iterator to the beginning of the LinkedArray. */
        inline reverse_const_iterator rbegin() const { return reverse_const_iterator(this->storage.tail); }
        /* Returns an immuteable reverse iterator to the end of the LinkedArray. */
        inline reverse_const_iterator rend() const { return reverse_const_iterator(this->storage.head); }
        /* Returns an immuteable reverse iterator to the beginning of the LinkedArray. */
        inline reverse_const_iterator crbegin() const { return this->rbegin(); }
        /* Returns an immuteable reverse iterator to the end of the LinkedArray. */
        inline reverse_const_iterator crend() const { return this->rend(); }

        /* Swap operator for the LinkedArray class. */
        friend void swap(LinkedArray& la1, LinkedArray& la2) {
            using std::swap;

            swap(la1.storage, la2.storage);
        }

    };
}

// Include the .cpp since it's all templated
#include "LinkedArray.cpp"

#endif
