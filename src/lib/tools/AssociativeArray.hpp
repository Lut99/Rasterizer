/* ASSOCIATIVE ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 11:15:20
 * Last edited:
 *   10/09/2021, 11:15:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the AssociativeArray class, which wraps around an Array and
 *   is indexable by some extra reference as well as normal indices.
**/

#ifndef TOOLS_ASSOCIATIVE_ARRAY_HPP
#define TOOLS_ASSOCIATIVE_ARRAY_HPP

#include <type_traits>
#include <unordered_map>

#include "Array.hpp"

namespace Tools {
    /* The AssociativeArray class, which wraps around a normal Array to make it indexable not just by index, but also by an abstractive layer. */
    template <class ABSTRACT, class T, bool D = std::is_default_constructible<T>::value, bool C = std::is_copy_constructible<T>::value, bool M = std::is_move_constructible<T>::value>
    class AssociativeArray: public _intern::CopyControl {
    public:
        /* The abstract type that we use for indexing. */
        using abstract_t = ABSTRACT;

    private:
        /* The Array class, which we wrap and use for storing the actual data. */
        Array<T, D, C, M> storage;

        /* Maps abstract indices to normal indices. */
        std::unordered_map<abstract_t, array_size_t> abstract_index_map;
        /* Maps normal indices to abstract indices. */
        std::unordered_map<array_size_t, abstract_t> index_abstract_map;


    public:
        /* Default constructor for the AssociativeArray class, which initializes it to not having any elements. */
        AssociativeArray();
        /* Constructor for the AssociativeArray class, which takes an initial amount to set its capacity to. Each element will thus be uninitialized. */
        AssociativeArray(array_size_t initial_size);

        /* Adds a new element to the AssociativeArray with the given abstract index. The element will be initialized with its default constructor, which it is thus required to have. */
        template <class U = void>
        auto add(abstract_t abstract_index) -> std::enable_if_t<D, U>;
        /* Adds a copy of the given element to the AssociativeArray with the given abstract index. Obviously requires the element to be copy constructible. */
        template <class U = void>
        auto add(abstract_t abstract_index, const T& elem) -> std::enable_if_t<C, U>;
        /* Adds a the given element to the AssociativeArray with the given abstract index. Requires the element to be move constructible. */
        template <class U = void>
        auto add(abstract_t abstract_index, T&& elem) -> std::enable_if_t<M, U>;
        /* Removes the element with the given abstract index from the AssociativeArray. Will throw errors if no such element exists, and requires the element to be move constructible to re-order the array. */
        template <class U = void>
        auto remove(abstract_t abstract_index) -> std::enable_if_t<M, U>;

        /* Erases everything from the array, but leaves the internally located array intact. */
        inline void clear() { return this->storage.clear(); }
        /* Erases everything from the array, even removing the internal allocated array. */
        inline void reset() { return this->storage.reset(); }

        /* Re-allocates the internal array to the given size. Any leftover elements will be left unitialized, and elements that won't fit will be deallocated. */
        template <typename U = void>
        inline auto reserve(array_size_t new_size) -> std::enable_if_t<M, U> { return this->storage.reserve(new_size); }
        /* Guarantees that the AssociativeArray has at least min_size capacity after the call. Does so by reallocating the internal array if we currently have less, but leaving it untouched otherwise. Any new elements will be left unitialized. */
        template <typename U = void>
        inline auto reserve_opt(array_size_t min_size) -> std::enable_if_t<M, U> { return this->storage.reserve_opt(min_size); }

        /* Returns a muteable reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline T& operator[](array_size_t index) { return this->storage[index]; }
        /* Returns a constant reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline const T& operator[](array_size_t index) const { return this->storage[index]; }
        /* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        inline T& at(array_size_t index) { return this->storage.at(index); }
        /* Returns a constant reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        inline const T& at(array_size_t index) const { return this->storage.at(index); }
        /* Returns the first element in the list. */
        inline T& first() { return this->storage.first(); }
        /* Returns the first element in the list. */
        inline const T& first() const { return this->storage.first(); }
        /* Returns the last element in the list. */
        inline T& last() { return this->storage.last(); }
        /* Returns the last element in the list. */
        inline const T& last() const { return this->storage.last(); }

        /* Returns a muteable reference to the element with the given abstraction index. Does not perform any checking if that index exists. */
        inline T& get(abstract_t abstract_index) { return this->storage[this->abstract_index_map.at(abstract_index)]; }
        /* Returns an immuteable reference to the element with the given abstraction index. Does not perform any checking if that index exists. */
        inline const T& get(abstract_t abstract_index) const { return this->storage[this->abstract_index_map.at(abstract_index)]; }
        /* Returns a muteable reference to the element with the given abstraction index. Performs checks to see if that index exists. */
        T& get_at(abstract_t abstract_index);
        /* Returns an immuteable reference to the element with the given abstraction index. Performs checks to see if that index exists. */
        inline const T& get_at(abstract_t abstract_index) const { return const_cast<AssociativeArray*>(this)->get_at(abstract_index); }

        /* Returns whether or not the AssociativeArray has the given abstract index mapped. */
        inline bool contains(abstract_t abstract_index) const { return this->abstract_index_map.find(abstract_index) != this->abstract_index_map.end(); }
        /* Returns the index behind the given abstract index. */
        inline uint32_t get_index(abstract_t abstract_index) const { return this->abstract_index_map.at(abstract_index); }
        /* Returns the abstract index behind the given normal index. */
        inline uint32_t get_abstract_index(array_size_t index) const { return this->index_abstract_map.at(index); }

        /* Returns a constant pointer to the internal data struct. Use this to read from the array using C-libraries, but beware that the array needs to have enough space reserved. */
        inline const T* rdata() const { return this->storage.rdata(); }
        /* Returns true if there are no elements in this array, or false otherwise. */
        inline bool empty() const { return this->storage.size() == 0; }
        /* Returns the number of elements stored in this AssociativeArray. */
        inline array_size_t size() const { return this->storage.size(); }
        /* Returns the number of elements this AssociativeArray can store before resizing. */
        inline array_size_t capacity() const { return this->storage.capacity(); }

        /* Returns a constant iterator to iterate over the internal abstract indices. Note that it will not iterate in order, so to get the indices you should call get_index(). */
        inline std::unordered_map<abstract_t, array_size_t>::const_iterator& begin() const { return this->abstract_index_map.begin(); }
        /* Returns a constant iterator to the end of the internal abstract indices. Note that it will not iterate in order, so to get the indices you should call get_index(). */
        inline std::unordered_map<abstract_t, array_size_t>::const_iterator& end() const { return this->abstract_index_map.end(); }
        /* Returns a constant iterator to iterate over the internal abstract indices. Note that it will not iterate in order, so to get the indices you should call get_index(). */
        inline std::unordered_map<abstract_t, array_size_t>::const_iterator& cbegin() const { return this->abstract_index_map.cbegin(); }
        /* Returns a constant reverse iterator to the end of the internal abstract indices. Note that it will not iterate in order, so to get the indices you should call get_index(). */
        inline std::unordered_map<abstract_t, array_size_t>::const_iterator& cend() const { return this->abstract_index_map.cend(); }

        /* Swap operator for the AssociativeArray class. */
        friend void swap(AssociativeArray& a1, AssociativeArray& a2) {
            using std::swap;

            swap(a1.storage, a2.storage);
            swap(a1.abstract_index_map, s2.abstract_index_map);
            swap(a1.index_abstract_map, s2.index_abstract_map);
        }

    };

}

#endif
