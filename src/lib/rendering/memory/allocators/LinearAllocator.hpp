/* LINEAR ALLOCATOR.hpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 14:27:46
 * Last edited:
 *   30/09/2021, 14:27:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the LinearAllocator class, which allocates memory as simple
 *   as possible. Doesn't allow things to be deallocated individually, 
 *   but only everything in one go.
**/

#ifndef RENDERING_LINEAR_ALLOCATOR_HPP
#define RENDERING_LINEAR_ALLOCATOR_HPP

#include <cstdint>

namespace Makma3D::Rendering {
    /* The LinearAllocator class, which can allocate things in the simplest (and thus fastest) way. Doesn't allow most deallocations, though. */
    template<class T = uint64_t>
    class LinearAllocator {
    public:
        /* Channel name for the LinearAllocator class. */
        static constexpr const char* channel = "LinearAllocator";

        /* The datatype used for size. */
        using size_t = T;
    
    private:
        /* The used space in the allocator, which is also the pointer to the first (and only) free block. */
        T _size;
        /* The total space available in the allocator. */
        T _capacity;

    public:
        /* Constructor for the LinearAllocator class, which takes its total size. */
        LinearAllocator(T capacity);
        
        /* Allocates a new block of memory of the given size and with the given alignment in the Allocator. Returns the offset if successfull, or std::numeric_limits<size_t>::max() if there wasn't enough space. */
        T allocate(T size, T alignment = 1);

        /* Completely wipes the LinearAllocator, deallocating all blocks. */
        void clear();

        /* Returns the used space in the Allocator. */
        inline T size() const { return this->_size; }
        /* Returns the total space in the Allocator. */
        inline T capacity() const { return this->_capacity; }

    };

}

/* Don't forget to include the .cpp file. */
#include "LinearAllocator.cpp"

#endif
