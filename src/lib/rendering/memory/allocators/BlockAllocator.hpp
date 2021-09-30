/* BLOCK ALLOCATOR.hpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 14:37:22
 * Last edited:
 *   30/09/2021, 14:37:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the BlockAllocator class, which allocates memory in blocks.
 *   Is much more complicated than the LinearAllocator, which means that
 *   any block can be removed instead of just all of them - but as a price,
 *   is much slower in allocation/deallocation.
**/

#ifndef RENDERING_BLOCK_ALLOCATOR_HPP
#define RENDERING_BLOCK_ALLOCATOR_HPP

#include <cstdint>

#include "tools/Array.hpp"

namespace Makma3D::Rendering {
    /* The BlockAllocator class, which can allocate things the most versatile (but slow) way. */
    template<class T = uint64_t>
    class BlockAllocator {
    public:
        /* Channel name for the BlockAllocator class. */
        static constexpr const char* channel = "BlockAllocator";

        /* The datatype used for size. */
        using size_t = T;
    
    private:
        /* Small struct that describes a block of data. */
        struct MemoryBlock {
            /* The start index of the block. */
            T offset;
            /* The size of this block. */
            T size;
        };


        /* Dynamically sizeable list of free blocks. */
        Tools::Array<MemoryBlock> _blocks;
        /* The used space in the allocator, which is also the pointer to the first (and only) free block. */
        T _size;
        /* The total space available in the allocator. */
        T _capacity;

    public:
        /* Constructor for the BlockAllocator class, which takes its total size. */
        BlockAllocator(T capacity);
        
        /* Allocates a new block of memory of the given size and with the given alignment in the Allocator. Returns the offset. */
        T allocate(T size, T alignment = 1);
        /* Frees the block of memory at the given offset. Throws errors if such a block if not allocated. */
        void free(T offset);

        /* Completely wipes the BlockAllocator, deallocating all blocks. */
        void clear();

        /* Returns the used space in the Allocator. */
        inline T size() const { return this->_size; }
        /* Returns the total space in the Allocator. */
        inline T capacity() const { return this->_capacity; }

    };

}

/* Don't forget to include the .cpp file. */
#include "BlockAllocator.cpp"

#endif
