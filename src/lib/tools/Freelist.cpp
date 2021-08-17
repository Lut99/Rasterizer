/* FREELIST.cpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 16:24:17
 * Last edited:
 *   01/07/2021, 16:24:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that can keep track of free space in an array of
 *   elements.
**/

#include <limits>
#include "Common.hpp"
#include "CppDebugger.hpp"

#include "Freelist.hpp"

using namespace std;
using namespace Tools;
using namespace CppDebugger::SeverityValues;


/***** FREELIST CLASS *****/
/* Constructor for the Freelist class, which takes the number of elements available in the target list. */
Freelist::Freelist(freelist_size_t target_size) :
    current_size(0),
    max_size(target_size)
{
    DENTER("Tools::Freelist::Freelist");

    // Add the free block for the entire list
    this->blocks.push_back(MemoryBlock({ false, 0, this->max_size }));

    // Done
    DLEAVE;
}



/* Reserves a bit of space in the freelist. If no such space is available, returns an offset of std::numeric_limits<freelist_size_t>::max(). If an offset of std::numeric_limits<freelist_size_t>::max() - 1 is returned, then there is enough space in the freelist but not continously. Note that the size may slightly differ due to the offset; use Freelist::align_size() to get the proper one. */
freelist_size_t Freelist::reserve(freelist_size_t size, freelist_size_t align) {
    DENTER("Tools::Freelist::reserve");

    // If the size is too much by definition, return the other error code
    if (size > this->max_size - this->current_size) {
        DRETURN std::numeric_limits<freelist_size_t>::max();
    }

    // Search the list for the first free block with enough size
    freelist_size_t result = std::numeric_limits<freelist_size_t>::max() - 1;
    for (array_size_t i = 0; i < this->blocks.size(); i++) {
        if (!this->blocks[i].used && this->blocks[i].size >= size) {
            // Compute how many bytes offset we have
            freelist_size_t align_bytes = align - this->blocks[i].offset % align;
            if (align_bytes >= align) { align_bytes = 0; }

            // If the block now doesn't have enough memory, continue searching to the next one
            if (align_bytes + size > this->blocks[i].size) { continue; }

            // Otherwise, shrink the free block
            result = align_bytes + this->blocks[i].offset;
            this->blocks[i].offset += align_bytes + size;
            this->blocks[i].size   -= align_bytes + size;

            // Spawn in the new used block
            this->blocks.resize(this->blocks.size() + 1);
            for (uint32_t j = i; j < this->blocks.size() - 1; j++) {
                this->blocks[j + 1] = this->blocks[j];
            }
            this->blocks[i] = MemoryBlock({ true, result, size });

            // Done
            break;
        }
    }

    // Update the size, then return the found (or not found) index
    this->current_size += size;
    DRETURN result;
}

/* Releases memory at the given location. Throws errors if that memory is not in use. */
void Freelist::release(freelist_size_t offset) {
    DENTER("Tools::Freelist::release");

    // Find the block where the offset belongs in
    for (array_size_t i = 0; i < this->blocks.size(); i++) {
        if (offset >= this->blocks[i].offset && offset <= this->blocks[i].offset + this->blocks[i].size) {
            // If it's a free block, fail
            freelist_size_t block_size = this->blocks[i].size;
            if (!this->blocks[i].used) { DLOG(fatal, "Cannot free block with offset " + std::to_string(offset) + " as it's already a free block (" + bytes_to_string(block_size) + ", starting at " + std::to_string(this->blocks[i].offset) + ")"); }

            // Otherwise, match on the applicable condition of the block's neighbour
            if (this->blocks.size() == 1) {
                // It's the only block, so simply revert back to free
                this->blocks[i].used = false;

            } else if ((i == 0 || this->blocks[i - 1].used) && !this->blocks[i + 1].used) {
                // Left is nothing mergeable but right is, so merge to right
                this->blocks[i + 1].offset -= block_size;
                this->blocks[i + 1].size   += block_size;
                this->blocks.erase(i);

            } else if (i > 0 && !this->blocks[i - 1].used && (i == this->blocks.size() - 1 || this->blocks[i + 1].used)) {
                // Left is mergeable but right right isn't, so merge to the left
                this->blocks[i - 1].size += block_size;
                this->blocks.erase(i);

            } else if (i > 0 && i < this->blocks.size() - 1 && !this->blocks[i - 1].used && !this->blocks[i + 1].used) {
                // Both sides are mergable
                this->blocks[i - 1].size += block_size + this->blocks[i + 1].size;
                this->blocks.erase(i + 1);
                this->blocks.erase(i);

            } else {
                // Nothing is mergeable, so simply revert back to free
                this->blocks[i].used = false;

            }

            // Update the size with the block size
            this->current_size -= block_size;

            // Done
            DRETURN;
        }
    }

    // If we reached here, the offset is out of bounds
    DLOG(fatal, "Offset " + std::to_string(offset) + " is out of bounds for freelist of " + std::to_string(this->max_size) + " elements.");
}



/* Clears the freelist, resetting it to empty. */
void Freelist::clear() {
    DENTER("Tools::Freelist::clear");

    // Clear the list
    this->blocks.clear();
    this->blocks.push_back(MemoryBlock({ false, 0, this->max_size }));

    // Also reset the used size
    this->current_size = 0;

    // Done
    DRETURN;
}



/* Swap operator for the Freelist class. */
void Tools::swap(Freelist& f1, Freelist& f2) {
    using std::swap;
    swap(f1.blocks, f2.blocks);
    swap(f1.current_size, f2.current_size);
    swap(f1.max_size, f2.max_size);
}
