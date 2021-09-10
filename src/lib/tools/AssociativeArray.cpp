/* ASSOCIATIVE ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 11:15:26
 * Last edited:
 *   10/09/2021, 11:15:26
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the AssociativeArray class, which wraps around an Array and
 *   is indexable by some extra reference as well as normal indices.
**/

#include "AssociativeArray.hpp"


/***** ASSOCIATIVEARRAY CLASS *****/
/* Default constructor for the AssociativeArray class, which initializes it to not having any elements. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
AssociativeArray<ABSTRACT, T, D, C, M>::AssociativeArray() :
    storage()
{}

/* Constructor for the AssociativeArray class, which takes an initial amount to set its capacity to. Each element will thus be uninitialized. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
AssociativeArray<ABSTRACT, T, D, C, M>::AssociativeArray(array_size_t initial_size) :
    storage(initial_size)
{}



/* Adds a new element to the AssociativeArray with the given abstract index. The element will be initialized with its default constructor, which it is thus required to have. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
template <class U>
auto AssociativeArray<ABSTRACT, T, D, C, M>::add(abstract_t abstract_index) -> std::enable_if_t<D, U> {
    // Generate the mappings for this element
    array_size_t index = this->storage.size();
    this->abstract_index_map.insert({ abstract_index, index });
    this->index_abstract_map.insert({ index, abstract_index });

    // Insert the element in the list
    this->storage.push_back();
}

/* Adds a copy of the given element to the AssociativeArray with the given abstract index. Obviously requires the element to be copy constructible. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
template <class U>
auto AssociativeArray<ABSTRACT, T, D, C, M>::add(abstract_t abstract_index, const T& elem) -> std::enable_if_t<C, U> {
    // Generate the mappings for this element
    array_size_t index = this->storage.size();
    this->abstract_index_map.insert({ abstract_index, index });
    this->index_abstract_map.insert({ index, abstract_index });

    // Insert the element in the list
    this->storage.push_back(elem);
}

/* Adds a the given element to the AssociativeArray with the given abstract index. Requires the element to be move constructible. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
template <class U>
auto AssociativeArray<ABSTRACT, T, D, C, M>::add(abstract_t abstract_index, T&& elem) -> std::enable_if_t<M, U> {
    // Generate the mappings for this element
    array_size_t index = this->storage.size();
    this->abstract_index_map.insert({ abstract_index, index });
    this->index_abstract_map.insert({ index, abstract_index });

    // Insert the element in the list
    this->storage.push_back(elem);
}

/* Removes the element with the given abstract index from the AssociativeArray. Will throw errors if no such element exists, and requires the element to be move constructible to re-order the array. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
template <class U>
auto AssociativeArray<ABSTRACT, T, D, C, M>::remove(abstract_t abstract_index) -> std::enable_if_t<M, U> {
    // Try to find the abstract index first
    std::unordered_map<abstract_t, array_size_t>::iterator iter = this->abstract_index_map.find(abstract_index);
    if (iter == this->abstract_index_map.end()) {
        throw std::out_of_range("No element with abstract index " + std::to_string(abstract_index) + " is present in the AssociativeArray.");
    }

    // Remove the mappings for this element
    this->abstract_index_map.erase(iter);
    this->index_abstract_map.erase((*iter).second);

    // Update the mappings for all other elements
    for (array_size_t i = (*iter).second + 1; i < this->storage.size(); i++) {
        abstract_t a = this->index_abstract_map.at(i);
        this->abstract_index_map.at(a) -= 1;
        this->index_abstract_map.erase(i);
        this->index_abstract_map.insert({ i - 1, a });
    }

    // Remove the element from the list
    this->storage.erase((*iter).second);
}



/* Returns a muteable reference to the element with the given abstraction index. Performs checks to see if that index exists. */
template <class ABSTRACT, class T, bool D, bool C, bool M>
T& AssociativeArray<ABSTRACT, T, D, C, M>::get_at(abstract_t abstract_index) {
    // First, check if the index exists
    std::unordered_map<abstract_t, array_size_t>::iterator iter = this->abstract_index_map.find(abstract_index);
    if (iter == this->abstract_index_map.end()) {
        throw std::out_of_range("No element with abstract index " + std::to_string(abstract_index) + " is present in the AssociativeArray.");
    }

    // Then return the value
    return this->storage[(*iter).second];
}
