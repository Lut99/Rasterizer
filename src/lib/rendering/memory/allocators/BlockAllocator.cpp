/* BLOCK ALLOCATOR.cpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 14:37:37
 * Last edited:
 *   30/09/2021, 14:37:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the BlockAllocator class, which allocates memory in blocks.
 *   Is much more complicated than the LinearAllocator, which means that
 *   any block can be removed instead of just all of them - but as a price,
 *   is much slower in allocation/deallocation.
**/

#include <limits>

#include "tools/Common.hpp"

#include "BlockAllocator.hpp"


/***** BLOCKALLOCATOR CLASS *****/
/* Constructor for the BlockAllocator class, which takes its total size. */
template <class T>
Makma3D::Rendering::BlockAllocator<T>::BlockAllocator(T capacity) :
    _blocks(16),
    _size(0),
    _capacity(capacity)
{}



/* Allocates a new block of memory of the given size and with the given alignment in the Allocator. Returns the offset. */
template <class T>
T Makma3D::Rendering::BlockAllocator<T>::allocate(T size, T alignment) {
    // Check if there's enough space available in the allocator
    if (this->_size + size > this->_capacity) {
        return std::numeric_limits<size_t>::max();
    }

    // If there aren't any blocks yet, we do know there's enough size, so add
    if (this->_blocks.empty()) {
        T result = 0;
        this->_blocks.push_back(MemoryBlock{ result, size });

        // Update the used size, then we're done
        this->_size += size;
        return result;
    }

    // If we have the basic size, try to find a free block
    T after_block = 0;
    for (array_size_t i = 0; i < this->_blocks.size(); i++) {
        // Compute the number of alignment bytes we'd need if we were to put it after the last block
        T align_bytes = alignment - after_block % alignment;
        if (align_bytes >= alignment) { align_bytes = 0; }

        // If the free space between this block and the last used byte is enough, put it here
        if (align_bytes + size <= this->_blocks[i].offset - after_block) {
            // Insert the block here
            T result = after_block + align_bytes;
            while (this->_blocks.size() >= this->_blocks.capacity()) { this->_blocks.reserve(2 * this->_blocks.capacity()); }
            this->_blocks.insert(i, MemoryBlock{ result, size });

            // Update the used size, then we're done
            this->_size += align_bytes + size;
            return result;
        }

        // Otherwise, mark the end of this block as the last used one
        after_block = this->_blocks[i].offset + this->_blocks[i].size;
    }

    // Last chance: is there space after the last block?
    T align_bytes = alignment - after_block % alignment;
    if (align_bytes >= alignment) { align_bytes = 0; }
    if (align_bytes + size <= this->_capacity - (this->_blocks.last().offset + this->_blocks.last().size)) {
        // There is, so put it after the last block
        T result = after_block + align_bytes;
        while (this->_blocks.size() >= this->_blocks.capacity()) { this->_blocks.reserve(2 * this->_blocks.capacity()); }
        this->_blocks.push_back(MemoryBlock{ result, size });

        // Update the used size, then we're done
        this->_size += align_bytes + size;
        return result;
    }

    // If we get here, we didn't find a free block with enough size, so return the max - 1
    return std::numeric_limits<T>::max() - 1;
}

/* Frees the block of memory at the given offset. Throws errors if such a block if not allocated. */
template <class T>
void Makma3D::Rendering::BlockAllocator<T>::free(T offset) {
    // Try to found a valid block
    T after_block = 0;
    for (array_size_t i = 0; i < this->_blocks.size(); i++) {
        // If this offset is within this block, we gottem
        T block_offset = this->_blocks[i].offset;
        T block_size   = this->_blocks[i].size;
        if (offset >= block_offset && offset < block_offset + block_size) {
            // Simply remove the block from the list
            this->_blocks.erase(i);

            // Shrink the size appropriately
            this->_size -= (block_offset - after_block) + block_size;

            // Done
            return;
        }

        // Otherwise, update the pointer to after the last block
        after_block = block_offset + block_size;
    }

    // If we got here, no block contained the offset.
    logger.fatalc(BlockAllocator::channel, "Cannot remove block with offset ", offset, ", as no such block is allocated.");
}



/* Completely wipes the BlockAllocator, deallocating all blocks. */
template <class T>
void Makma3D::Rendering::BlockAllocator<T>::clear() {
    // Clear the list of blocks
    this->_blocks.clear();
    // Reset the size
    this->_size = 0;
}
