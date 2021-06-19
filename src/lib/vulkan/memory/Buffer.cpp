/* BUFFER.cpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:19:53
 * Last edited:
 *   19/06/2021, 12:19:53
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Buffer class, which wraps a pre-allocated VkBuffer
 *   object. Only usable in the context of the MemoryPool.
**/

#include "tools/CppDebugger.hpp"
#include "tools/Common.hpp"

#include "vulkan/ErrorCodes.hpp"

#include "Buffer.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VkMappedMemoryRange struct. */
static void populate_memory_range(VkMappedMemoryRange& memory_range, VkDeviceMemory vk_memory, VkDeviceSize vk_memory_offset, VkDeviceSize vk_memory_size) {
    DENTER("populate_memory_range");

    // Set to default
    memory_range = {};
    memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;

    // Tell it the memory and which part of the device memory is used
    memory_range.memory = vk_memory;
    memory_range.offset = vk_memory_offset;
    memory_range.size = vk_memory_size;

    // Done, return
    DRETURN;
}





/***** BUFFER CLASS *****/
/* Private constructor for the Buffer class, which takes the buffer, the buffer's size and the properties of the pool's memory. */
Buffer::Buffer(buffer_h handle, VkBuffer buffer, VkBufferUsageFlags vk_usage_flags, VkSharingMode vk_sharing_mode, VkBufferCreateFlags vk_create_flags, VkDeviceMemory vk_memory, VkDeviceSize memory_offset, VkDeviceSize memory_size, VkDeviceSize req_memory_size, VkMemoryPropertyFlags memory_properties) :
    vk_handle(handle),
    vk_buffer(buffer),
    vk_usage_flags(vk_usage_flags),
    vk_sharing_mode(vk_sharing_mode),
    vk_create_flags(vk_create_flags),
    vk_memory(vk_memory),
    vk_memory_offset(memory_offset),
    vk_memory_size(memory_size),
    vk_req_memory_size(req_memory_size),
    vk_memory_properties(memory_properties)
{}



/* Sets n_bytes data to this buffer using an intermediate staging buffer. The staging buffer is copied using the given command buffer on the given queue. */
void Buffer::set(const GPU& gpu, const CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const {
    DENTER("Vulkan::Buffer::set");

    // First, map the staging buffer to an CPU-reachable area
    void* mapped_area;
    staging_buffer.map(gpu, &mapped_area);

    // Next, copy the data to the buffer
    memcpy(mapped_area, data, n_bytes);

    // Flush and then unmap the staging buffer
    staging_buffer.flush(gpu);
    staging_buffer.unmap(gpu);

    // Use the command buffer to copy the data around
    staging_buffer.copyto(command_buffer, vk_queue, *this, (VkDeviceSize) n_bytes);

    // Done
    DRETURN;
}

/* Gets n_bytes data from this buffer using an intermediate staging buffer. The buffers are copied over using the given command buffer on the given queue. The result is put in the given pointer. */
void Buffer::get(const GPU& gpu, const CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const {
    DENTER("Vulkan::Buffer::set");

    // First, copy the data we have to the staging buffer
    this->copyto(command_buffer, vk_queue, staging_buffer, (VkDeviceSize) n_bytes);

    // Then, map the staging buffer to an CPU-reachable area
    void* mapped_area;
    staging_buffer.map(gpu, &mapped_area);

    // Next, copy the data to a user-defined location
    memcpy(data, mapped_area, n_bytes);

    // Unmap the staging buffer. No need to flush cuz we didn't change anything
    staging_buffer.unmap(gpu);

    // Done
    DRETURN;
}



/* Maps the buffer to host-memory so it can be written to. Only possible if the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set for the memory of this buffer's pool. Note that the memory is NOT automatically unmapped if the Buffer object is destroyed. */
void Buffer::map(const GPU& gpu, void** mapped_memory) const {
    DENTER("Vulkan::Buffer::map");

    // If this buffer does not have the host bit set, then we stop immediatement
    if (!(this->vk_memory_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        DLOG(fatal, "Cannot map a buffer that is not visible by the CPU.");
    }

    // Now, we map the memory to a bit of host-side memory
    VkResult vk_result;
    if ((vk_result = vkMapMemory(gpu, this->vk_memory, this->vk_memory_offset, this->vk_req_memory_size, 0, mapped_memory)) != VK_SUCCESS) {
        DLOG(fatal, "Could not map buffer memory to CPU-memory: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Flushes all unflushed memory operations done on mapped memory. If the memory of this buffer has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then nothing is done as the memory is already automatically flushed. */
void Buffer::flush(const GPU& gpu) const {
    DENTER("Vulkan::Buffer::flush");

    // If this buffer is coherent, quite immediately
    if (!(this->vk_memory_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        DRETURN;
    }

    // Prepare the call to the flush function
    VkMappedMemoryRange memory_range;
    populate_memory_range(memory_range, this->vk_memory, this->vk_memory_offset, this->vk_req_memory_size);

    // Do the flush call
    VkResult vk_result;
    if ((vk_result = vkFlushMappedMemoryRanges(gpu, 1, &memory_range)) != VK_SUCCESS) {
        DLOG(fatal, "Could not flush mapped buffer memory: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Unmaps buffer's memory. */
void Buffer::unmap(const GPU& gpu) const {
    DENTER("Vulkan::Buffer::unmap");

    // Simply call unmap, done
    vkUnmapMemory(gpu, this->vk_memory);

    DRETURN;
}



/* Copies this buffer's content to another given buffer. The given command pool (which must be a pool for the memory-enabled queue) is used to schedule the copy. Note that the given buffer needn't come from the same memory pool. */
void Buffer::copyto(const Buffer& destination, const CommandBuffer& command_buffer, VkQueue vk_queue, VkDeviceSize n_bytes, VkDeviceSize target_offset, bool wait_queue_idle) const {
    DENTER("Vulkan::Buffer::copyto");

    // If the number of bytes to transfer is the max, default to the buffer size
    if (n_bytes == numeric_limits<VkDeviceSize>::max()) {
        n_bytes = this->vk_memory_size;
    }

    // First, check if the destination buffer is large enough
    if (destination.vk_memory_size - target_offset < n_bytes) {
        DLOG(fatal, "Cannot copy " + Tools::bytes_to_string(n_bytes) + " to buffer of only " + Tools::bytes_to_string(destination.vk_memory_size) + " (with offset=" + std::to_string(target_offset) + ").");
    }

    // Next, make sure they have the required flags
    if (!(this->vk_usage_flags & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
        DLOG(fatal, "Source buffer does not have VK_BUFFER_USAGE_TRANSFER_SRC_BIT-flag set.");
    }
    if (!(destination.vk_usage_flags & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
        DLOG(fatal, "Destination buffer does not have VK_BUFFER_USAGE_TRANSFER_DST_BIT-flag set.");
    }

    // Next, start recording the given command buffer, and we'll tell Vulkan we use this recording only once
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // We schedule the copy by populating a struct
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = target_offset;
    copy_region.size = n_bytes;
    vkCmdCopyBuffer(command_buffer, this->vk_buffer, destination.vk_buffer, 1, &copy_region);

    // Since that's all, submit the queue. Note that we only return once the copy is 
    command_buffer.end(vk_queue, wait_queue_idle);

    DRETURN;
}



/* Swap operator for the Buffer class. */
void Vulkan::swap(Buffer& b1, Buffer& b2) {
    using std::swap;

    // Swap all fields
    swap(b1.vk_handle, b2.vk_handle);
    swap(b1.vk_buffer, b2.vk_buffer);
    swap(b1.vk_usage_flags, b2.vk_usage_flags);
    swap(b1.vk_sharing_mode, b2.vk_sharing_mode);
    swap(b1.vk_create_flags, b2.vk_create_flags);
    swap(b1.vk_memory, b2.vk_memory);
    swap(b1.vk_memory_offset, b2.vk_memory_offset);
    swap(b1.vk_memory_size, b2.vk_memory_size);
    swap(b1.vk_req_memory_size, b2.vk_req_memory_size);
    swap(b1.vk_memory_properties, b2.vk_memory_properties);
}

