/* LINEAR MEMORY POOL.cpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 15:44:33
 * Last edited:
 *   30/09/2021, 15:44:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the LinearMemoryPool class, which implements a memory pool
 *   with a lighting fast but not-so-versatile linear allocator.
**/

#include "tools/Common.hpp"
#include "LinearMemoryPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** LINEARMEMORYPOOL CLASS *****/
/* Constructor for the LinearMemoryPool class, which takes the GPU where it lives, the size of its memory, the memory properties and optionally some buffer memory usage flags to take into account when selecting memory. */
LinearMemoryPool::LinearMemoryPool(const Rendering::GPU& gpu, VkDeviceSize pool_size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage, VkImageUsageFlags image_usage) :
    MemoryPool(gpu, pool_size, memory_properties, buffer_usage, image_usage),
    allocator(pool_size)
{}

/* Move constructor for the LinearMemoryPool class. */
LinearMemoryPool::LinearMemoryPool(LinearMemoryPool&& other) :
    MemoryPool(std::move(other)),
    allocator(std::move(other.allocator))
{}

/* Destructor for the LinearMemoryPool class. */
LinearMemoryPool::~LinearMemoryPool() {}



/* Private helper function that does the actual memory allocation part using the internal allocator. */
VkDeviceSize LinearMemoryPool::_allocate(const VkMemoryRequirements& requirements) {
    // Try to reserve memory in the freelist
    VkDeviceSize offset = this->allocator.allocate(requirements.size, requirements.alignment);
    if (offset == std::numeric_limits<VkDeviceSize>::max()) { logger.fatalc(MemoryPool::channel, "Could not allocate new memory object: not enough space left in pool (need ", Tools::bytes_to_string(requirements.size), ", but ", Tools::bytes_to_string(this->allocator.capacity() - this->allocator.size()), " free)"); }

    // Well that's it
    return offset;
}

/* Private helper function that does the actual memory freeing part in the internal allocator. */
void LinearMemoryPool::_free(VkDeviceSize offset) {
    logger.fatalc(MemoryPool::channel, "Cannot free individual objects in a MemoryPool with a linear allocator.");
}

/* Private helper function that resets the internal allocator. */
void LinearMemoryPool::_reset() {
    this->allocator.clear();
}



/* Swap operator for the LinearMemoryPool class. */
void Rendering::swap(LinearMemoryPool& lmp1, LinearMemoryPool& lmp2) {
    using std::swap;

    swap((MemoryPool&) lmp1, (MemoryPool&) lmp2);
    swap(lmp1.allocator, lmp2.allocator);
}
