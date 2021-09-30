/* BLOCK MEMORY POOL.hpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 16:06:41
 * Last edited:
 *   30/09/2021, 16:06:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the BlockMemoryPool class, which implements a memory pool
 *   with a block allocator: a more complicated and versatile yet slow
 *   allocator.
**/

#ifndef RENDERING_BLOCK_MEMORY_POOL_HPP
#define RENDERING_BLOCK_MEMORY_POOL_HPP

#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

#include "allocators/BlockAllocator.hpp"
#include "MemoryPool.hpp"

namespace Makma3D::Rendering {
    /* The BlockMemoryPool class, which implements a MemoryPool with a block allocator. */
    class BlockMemoryPool: public MemoryPool {
    private:
        /* The BlockAllocator which we use for allocation. */
        BlockAllocator<VkDeviceSize> allocator;


        /* Private helper function that does the actual memory allocation part using the internal allocator. */
        virtual VkDeviceSize _allocate(const VkMemoryRequirements& requirements);
        /* Private helper function that does the actual memory freeing part in the internal allocator. */
        virtual void _free(VkDeviceSize offset);
        /* Private helper function that resets the internal allocator. */
        virtual void _reset();
    
    public:
        /* Constructor for the BlockMemoryPool class, which takes the GPU where it lives, the size of its memory, the memory properties and optionally some buffer memory usage flags to take into account when selecting memory. */
        BlockMemoryPool(const Rendering::GPU& gpu, VkDeviceSize pool_size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage = 0, VkImageUsageFlags image_usage = 0);
        /* Copy constructor for the BlockMemoryPool class, which is deleted. */
        BlockMemoryPool(const BlockMemoryPool& other) = delete;
        /* Move constructor for the BlockMemoryPool class. */
        BlockMemoryPool(BlockMemoryPool&& other);
        /* Destructor for the BlockMemoryPool class. */
        virtual ~BlockMemoryPool();

        /* Returns the number of bytes used in the MemoryPool. */
        inline VkDeviceSize size() const { return this->allocator.size(); }
        /* Returns the total number of bytes in the MemoryPool. */
        inline VkDeviceSize capacity() const { return this->allocator.capacity(); }

        /* Copy assignment operator for the BlockMemoryPool class, which is deleted. */
        BlockMemoryPool& operator=(const BlockMemoryPool& other) = delete;
        /* Move assignment operator for the BlockMemoryPool class. */
        inline BlockMemoryPool& operator=(BlockMemoryPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the BlockMemoryPool class. */
        friend void swap(BlockMemoryPool& bmp1, BlockMemoryPool& bmp2);

    };

    /* Swap operator for the LinearMemoryPool class. */
    void swap(BlockMemoryPool& bmp1, BlockMemoryPool& bmp2);

}

#endif
