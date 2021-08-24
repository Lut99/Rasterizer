/* FREELIST.hpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 16:24:20
 * Last edited:
 *   01/07/2021, 16:24:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that can keep track of free space in an array of
 *   elements.
**/

#ifndef TOOLS_FREELIST_HPP
#define TOOLS_FREELIST_HPP

#include <cstdint>
#include "Array.hpp"

namespace Tools {
    /* The type we use for the indices. */
    using freelist_size_t = uint64_t;



    /* The FreeList class, which allows one to keep track of free space in a fragmented array of elements. */
    class Freelist {
    public:
        /* Channel for the Freelist. */
        static constexpr const char* channel = "Freelist";

    private:
        /* Small struct that describes a block of data. */
        struct MemoryBlock {
            /* Whether or not the block is used for memory. */
            bool used;
            /* The start index of the block. */
            freelist_size_t offset;
            /* The size of this block. */
            freelist_size_t size;
        };


        /* Dynamically sizeable list of free blocks. */
        Tools::Array<MemoryBlock> blocks;
        /* The used size in the conceptual array. */
        freelist_size_t current_size;
        /* The maximum size of the conceptual array. */
        freelist_size_t max_size;

    public:
        /* Constructor for the Freelist class, which takes the number of elements available in the target list. */
        Freelist(freelist_size_t target_size);

        /* Reserves a bit of space in the freelist. If no such space is available, returns an offset of std::numeric_limits<freelist_size_t>::max(). The alignment of the element may optionally be given as well. */
        freelist_size_t reserve(freelist_size_t size, freelist_size_t align = 1);
        /* Releases memory at the given location. Throws errors if that memory is not in use. */
        void release(freelist_size_t offset);

        /* Clears the freelist, resetting it to empty. */
        void clear();

        /* Returns the used space in the freelist. */
        inline freelist_size_t size() const { return this->current_size; }
        /* Returns the maximum allowed space in the freelist. */
        inline freelist_size_t capacity() const { return this->max_size; }

        /* Swap operator for the Freelist class. */
        friend void swap(Freelist& f1, Freelist& f2);

    };

    /* Swap operator for the Freelist class. */
    void swap(Freelist& f1, Freelist& f2);

}

#endif
