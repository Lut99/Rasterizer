/* LINEAR MEMORY POOL.hpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 15:44:30
 * Last edited:
 *   30/09/2021, 15:44:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the LinearMemoryPool class, which implements a memory pool
 *   with a lighting fast but not-so-versatile linear allocator.
**/

#ifndef RENDERING_LINEAR_MEMORY_POOL_HPP
#define RENDERING_LINEAR_MEMORY_POOL_HPP

#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

#include "allocators/LinearAllocator.hpp"
#include "MemoryPool.hpp"

namespace Makma3D::Rendering {
    /* The LinearMemoryPool class, which implements a MemoryPool with a linear allocator. */
    class LinearMemoryPool: public MemoryPool {
    private:
        /* The LinearAllocator which we use for allocation. */
        LinearAllocator<VkDeviceSize> allocator;


        /* Private helper function that does the actual memory allocation part using the internal allocator. */
        virtual VkDeviceSize _allocate(const VkMemoryRequirements& requirements);
        /* Private helper function that does the actual memory freeing part in the internal allocator. */
        virtual void _free(VkDeviceSize offset);
        /* Private helper function that resets the internal allocator. */
        virtual void _reset();
    
    public:
        /* Constructor for the LinearMemoryPool class, which takes the GPU where it lives, the size of its memory, the memory properties and optionally some buffer memory usage flags to take into account when selecting memory. */
        LinearMemoryPool(const Rendering::GPU& gpu, VkDeviceSize pool_size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage = 0, VkImageUsageFlags image_usage = 0);
        /* Copy constructor for the LinearMemoryPool class, which is deleted. */
        LinearMemoryPool(const LinearMemoryPool& other) = delete;
        /* Move constructor for the LinearMemoryPool class. */
        LinearMemoryPool(LinearMemoryPool&& other);
        /* Destructor for the LinearMemoryPool class. */
        virtual ~LinearMemoryPool();

        /* Returns the number of bytes used in the MemoryPool. */
        inline VkDeviceSize size() const { return this->allocator.size(); }
        /* Returns the total number of bytes in the MemoryPool. */
        inline VkDeviceSize capacity() const { return this->allocator.capacity(); }

        /* Copy assignment operator for the LinearMemoryPool class, which is deleted. */
        LinearMemoryPool& operator=(const LinearMemoryPool& other) = delete;
        /* Move assignment operator for the LinearMemoryPool class. */
        inline LinearMemoryPool& operator=(LinearMemoryPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the LinearMemoryPool class. */
        friend void swap(LinearMemoryPool& lmp1, LinearMemoryPool& lmp2);

    };

    /* Swap operator for the LinearMemoryPool class. */
    void swap(LinearMemoryPool& lmp1, LinearMemoryPool& lmp2);

}

#endif
