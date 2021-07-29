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

#include "../auxillary/ErrorCodes.hpp"

#include "Buffer.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
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
Buffer::Buffer(const Rendering::GPU& gpu, buffer_h handle, VkBuffer buffer, VkBufferUsageFlags vk_usage_flags, VkSharingMode vk_sharing_mode, VkBufferCreateFlags vk_create_flags, VkDeviceMemory vk_memory, VkDeviceSize memory_offset, VkDeviceSize memory_size, VkDeviceSize req_memory_size, VkMemoryPropertyFlags memory_properties) :
    gpu(gpu),
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
void Buffer::set(const CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const {
    DENTER("Rendering::Buffer::set");

    // First, map the staging buffer to an CPU-reachable area
    void* mapped_area;
    staging_buffer.map(&mapped_area);

    // Next, copy the data to the buffer
    memcpy(mapped_area, data, n_bytes);

    // Flush and then unmap the staging buffer
    staging_buffer.flush();
    staging_buffer.unmap();

    // Use the command buffer to copy the data around
    staging_buffer.copyto(*this, command_buffer, vk_queue, (VkDeviceSize) n_bytes);

    // Done
    DRETURN;
}

/* Gets n_bytes data from this buffer using an intermediate staging buffer. The buffers are copied over using the given command buffer on the given queue. The result is put in the given pointer. */
void Buffer::get(const CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const {
    DENTER("Rendering::Buffer::set");

    // First, copy the data we have to the staging buffer
    this->copyto(staging_buffer, command_buffer, vk_queue, (VkDeviceSize) n_bytes);

    // Then, map the staging buffer to an CPU-reachable area
    void* mapped_area;
    staging_buffer.map(&mapped_area);

    // Next, copy the data to a user-defined location
    memcpy(data, mapped_area, n_bytes);

    // Unmap the staging buffer. No need to flush cuz we didn't change anything
    staging_buffer.unmap();

    // Done
    DRETURN;
}



/* Maps the buffer to host-memory so it can be written to. Only possible if the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set for the memory of this buffer's pool. Note that the memory is NOT automatically unmapped if the Buffer object is destroyed. */
void Buffer::map(void** mapped_memory) const {
    DENTER("Rendering::Buffer::map");

    // If this buffer does not have the host bit set, then we stop immediatement
    if (!(this->vk_memory_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        DLOG(fatal, "Cannot map a buffer that is not visible by the CPU.");
    }

    // Now, we map the memory to a bit of host-side memory
    VkResult vk_result;
    if ((vk_result = vkMapMemory(this->gpu, this->vk_memory, this->vk_memory_offset, this->vk_req_memory_size, 0, mapped_memory)) != VK_SUCCESS) {
        DLOG(fatal, "Could not map buffer memory to CPU-memory: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Flushes all unflushed memory operations done on mapped memory. If the memory of this buffer has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then nothing is done as the memory is already automatically flushed. */
void Buffer::flush() const {
    DENTER("Rendering::Buffer::flush");

    // If this buffer is coherent, quite immediately
    if (!(this->vk_memory_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        DRETURN;
    }

    // Prepare the call to the flush function
    VkMappedMemoryRange memory_range;
    populate_memory_range(memory_range, this->vk_memory, this->vk_memory_offset, this->vk_req_memory_size);

    // Do the flush call
    VkResult vk_result;
    if ((vk_result = vkFlushMappedMemoryRanges(this->gpu, 1, &memory_range)) != VK_SUCCESS) {
        DLOG(fatal, "Could not flush mapped buffer memory: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Unmaps buffer's memory. */
void Buffer::unmap() const {
    DENTER("Rendering::Buffer::unmap");

    // Simply call unmap, done
    vkUnmapMemory(this->gpu, this->vk_memory);

    DRETURN;
}



/* Copies this buffer's contents to the given Buffer, scheduling the command on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. Also, it can be opted to not wait until the given queue is idle again but to return immediately. */
void Buffer::copyto(const Buffer& destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, VkDeviceSize target_offset, const Rendering::CommandBuffer& command_buffer, VkQueue vk_queue, bool wait_queue_idle) const {
    DENTER("Rendering::Buffer::copyto");

    // First, resolve the queue
    if (vk_queue == nullptr) {
        vk_queue = this->gpu.queues(QueueType::memory)[0];
    }

    // Check if the offsets are valid
    if (source_offset >= this->vk_memory_size) {
        DLOG(fatal, "Source offset of " + std::to_string(source_offset) + " is out of range for buffer of size " + std::to_string(this->vk_memory_size) + ".");
    }
    if (target_offset >= destination.vk_memory_size) {
        DLOG(fatal, "Target offset of " + std::to_string(target_offset) + " is out of range for buffer of size " + std::to_string(destination.vk_memory_size) + ".");
    }

    // Check if the sizes is not too large
    n_bytes = n_bytes == VK_WHOLE_SIZE ? this->vk_memory_size : n_bytes;
    if (n_bytes > this->vk_memory_size - source_offset) {
        DLOG(fatal, "Cannot copy " + Tools::bytes_to_string(n_bytes) + " from buffer of only " + Tools::bytes_to_string(this->vk_memory_size) + " (with offset=" + std::to_string(source_offset) + ").");
    }
    if (n_bytes > destination.vk_memory_size - target_offset) {
        DLOG(fatal, "Cannot copy " + Tools::bytes_to_string(n_bytes) + " to buffer of only " + Tools::bytes_to_string(destination.vk_memory_size) + " (with offset=" + std::to_string(target_offset) + ").");
    }

    // Make sure they have the required flags
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
    copy_region.srcOffset = source_offset;
    copy_region.dstOffset = target_offset;
    copy_region.size = n_bytes;
    vkCmdCopyBuffer(command_buffer, this->vk_buffer, destination.vk_buffer, 1, &copy_region);

    // Since that's all, submit the queue. Note that we only return once the copy is 
    command_buffer.end(vk_queue, wait_queue_idle);

    DRETURN;
}



/* Swap operator for the Buffer class. */
void Rendering::swap(Buffer& b1, Buffer& b2) {
    DENTER("Rendering::swap(Buffer)");

    #ifndef NDEBUG
    // Make sure the GPU is the same
    if (b1.gpu != b2.gpu) {
        DLOG(fatal, "Cannot swap buffers with different gpus");
    }
    #endif

    // Swap all fields
    using std::swap;
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

    // Done
    DRETURN;
}

