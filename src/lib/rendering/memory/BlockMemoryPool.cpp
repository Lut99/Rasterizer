/* BLOCK MEMORY POOL.cpp
 *   by Lut99
 *
 * Created:
 *   30/09/2021, 16:06:43
 * Last edited:
 *   30/09/2021, 16:06:43
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the BlockMemoryPool class, which implements a memory pool
 *   with a block allocator: a more complicated and versatile yet slow
 *   allocator.
**/

#include "BlockMemoryPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** BLOCKMEMORYPOOL CLASS *****/
/* Constructor for the BlockMemoryPool class, which takes the GPU where it lives, the size of its memory, the memory properties and optionally some buffer memory usage flags to take into account when selecting memory. */
BlockMemoryPool::BlockMemoryPool(const Rendering::GPU& gpu, VkDeviceSize pool_size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage, VkImageUsageFlags image_usage) :
    MemoryPool(gpu, pool_size, memory_properties, buffer_usage, image_usage),
    allocator(pool_size)
{}

/* Move constructor for the BlockMemoryPool class. */
BlockMemoryPool::BlockMemoryPool(BlockMemoryPool&& other) :
    MemoryPool(std::move(other)),
    allocator(std::move(other.allocator))
{}

/* Destructor for the BlockMemoryPool class. */
BlockMemoryPool::~BlockMemoryPool() {}



/* Private helper function that does the actual memory allocation part using the internal allocator. */
VkDeviceSize BlockMemoryPool::_allocate(const VkMemoryRequirements& requirements) {
    // Try to reserve memory in the freelist
    VkDeviceSize offset = this->allocator.allocate(requirements.size, requirements.alignment);
    if (offset == std::numeric_limits<VkDeviceSize>::max()) { logger.fatalc(MemoryPool::channel, "Could not allocate new memory object: not enough space left in pool (need ", Tools::bytes_to_string(requirements.size), ", but ", Tools::bytes_to_string(this->allocator.capacity() - this->allocator.size()), " free)"); }
    else if (offset == std::numeric_limits<VkDeviceSize>::max() - 1) { logger.fatalc(MemoryPool::channel, "Could not allocate new memory object: enough space but bad fragmentation"); }

    // Well that's it
    return offset;
}

/* Private helper function that does the actual memory freeing part in the internal allocator. */
void BlockMemoryPool::_free(VkDeviceSize offset) {
    logger.fatalc(MemoryPool::channel, "Cannot free individual objects in a MemoryPool with a block allocator.");
}

/* Private helper function that resets the internal allocator. */
void BlockMemoryPool::_reset() {
    this->allocator.clear();
}



/* Swap operator for the BlockMemoryPool class. */
void Rendering::swap(BlockMemoryPool& bmp1, BlockMemoryPool& bmp2) {
    using std::swap;

    swap((MemoryPool&) bmp1, (MemoryPool&) bmp2);
    swap(bmp1.allocator, bmp2.allocator);
}
