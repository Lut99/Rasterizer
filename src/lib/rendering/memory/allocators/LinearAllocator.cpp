/* LINEAR ALLOCATOR.cpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 14:28:41
 * Last edited:
 *   30/09/2021, 14:28:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the LinearAllocator class, which allocates memory as simple
 *   as possible. Doesn't allow things to be deallocated individually, 
 *   but only everything in one go.
**/

#include <limits>

#include "LinearAllocator.hpp"


/***** LINEARALLOCATOR CLASS *****/
/* Constructor for the LinearAllocator class, which takes its total size. */
template <class T>
Makma3D::Rendering::LinearAllocator<T>::LinearAllocator(T capacity) :
    _size(0),
    _capacity(capacity)
{}



/* Allocates a new block of memory of the given size and with the given alignment in the Allocator. Returns the offset. */
template <class T>
T Makma3D::Rendering::LinearAllocator<T>::allocate(T size, T alignment) {
    // Compute how many bytes offset we have
    T align_bytes = alignment - this->_size % alignment;
    if (align_bytes >= alignment) { align_bytes = 0; }

    // Check if there's enough space available in the allocator
    if (this->_size + align_bytes + size > this->_capacity) {
        return std::numeric_limits<T>::max();
    }

    // Get the offset of the block (with alignment)
    T result = this->_size + align_bytes;
    // Increment the size
    this->_size += align_bytes + size;

    // Done!
    return result;
}



/* Completely wipes the LinearAllocator, deallocating all blocks. */
template <class T>
void Makma3D::Rendering::LinearAllocator<T>::clear() {
    // Simply set the size to 0, done
    this->_size = 0;
}
