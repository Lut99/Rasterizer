/* BUFFER.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 16:03:24
 * Last edited:
 *   16/08/2021, 16:03:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Buffer class, which wraps a VkBuffer allocated by the
 *   MemoryPool.
**/

#include <cstring>
#include "tools/CppDebugger.hpp"
#include "tools/Common.hpp"

#include "../auxillary/ErrorCodes.hpp"

#include "MemoryPool.hpp"
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

/* Populates the given VkBufferImageCopy struct. */
static void populate_buffer_image_copy(VkBufferImageCopy& buffer_image_copy, VkDeviceSize buffer_offset, uint32_t buffer_pitch, VkOffset3D image_offset, VkExtent3D image_extent) {
    DENTER("populate_buffer_image_copy");

    // Set to default
    buffer_image_copy = {};

    // Set the buffer region. The pitch defines the length of each image row in the buffer (unless it's 0, in which case we assume it's tightly packed), and the same for the image height.
    buffer_image_copy.bufferOffset = buffer_offset;
    buffer_image_copy.bufferRowLength = buffer_pitch;
    buffer_image_copy.bufferImageHeight = 0;

    // Set the image region
    buffer_image_copy.imageOffset = image_offset;
    buffer_image_copy.imageExtent = image_extent;
    
    // Set the image subresource
    buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    buffer_image_copy.imageSubresource.mipLevel = 0;
    buffer_image_copy.imageSubresource.baseArrayLayer = 0;
    buffer_image_copy.imageSubresource.layerCount = 1;

    // Done
    DRETURN;
}





/***** BUFFER CLASS *****/
/* Constructor for the Buffer class, which takes the pool where it was allocated, the buffer object to wrap, the offset of this buffer in the main memory pool, its desired size and its memory properties. Also takes other stuff that's needed to copy the buffer. */
Buffer::Buffer(const MemoryPool& pool, VkBuffer vk_buffer, VkDeviceSize offset, VkDeviceSize buffer_size, const VkMemoryRequirements& vk_requirements, VkBufferUsageFlags buffer_usage, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags) :
    MemoryObject(pool, MemoryObjectType::buffer, offset),
    vk_buffer(vk_buffer),
    buffer_size(buffer_size),
    vk_requirements(vk_requirements),
    init_data({ buffer_usage, sharing_mode, create_flags })
{}

/* Destructor for the Buffer class. */
Buffer::~Buffer() {}



/* Sets the buffer using an intermediate staging buffer. The staging buffer is copied using the given command buffer on the given queue. */
void Buffer::set(void* data, uint32_t n_bytes, const Buffer* staging_buffer, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue) const {
    DENTER("Rendering::Buffer::set");

    // First, map the staging buffer to an CPU-reachable area
    void* mapped_area;
    staging_buffer->map(&mapped_area);

    // Next, copy the data to the buffer
    std::memcpy(mapped_area, data, n_bytes);

    // Flush and then unmap the staging buffer
    staging_buffer->flush();
    staging_buffer->unmap();

    // Use the command buffer to copy the data around
    staging_buffer->copyto(this, static_cast<VkDeviceSize>(n_bytes), 0, 0, command_buffer, vk_queue);

    // Done
    DRETURN;
}

/* Gets n_bytes data from this buffer using an intermediate staging buffer. The buffers are copied over using the given command buffer on the given queue. The result is put in the given pointer. */
void Buffer::get(void* data, uint32_t n_bytes, const Buffer* staging_buffer, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue) const {
    DENTER("Rendering::Buffer::set");

    // First, copy the data we have to the staging buffer
    this->copyto(staging_buffer, static_cast<VkDeviceSize>(n_bytes), 0, 0, command_buffer, vk_queue);

    // Then, map the staging buffer to an CPU-reachable area
    void* mapped_area;
    staging_buffer->map(&mapped_area);

    // Next, copy the data to a user-defined location
    std::memcpy(data, mapped_area, n_bytes);

    // Unmap the staging buffer. No need to flush cuz we didn't change anything
    staging_buffer->unmap();

    // Done
    DRETURN;
}



/* Maps the buffer to host-memory so it can be written to. Only possible if the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set for the memory of this buffer's pool. Note that the memory is NOT automatically unmapped if the Buffer object is destroyed. */
void Buffer::map(void** mapped_memory, VkMemoryMapFlags map_flags) const {
    DENTER("Rendering::Buffer::map");
    
    // If this buffer does not have the host bit set, then we stop immediatement
    if (!(this->pool.properties() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        DLOG(fatal, "Cannot map a buffer that is not visible by the CPU.");
    }

    // Now, we map the memory to a bit of host-side memory
    VkResult vk_result;
    if ((vk_result = vkMapMemory(this->gpu, this->pool.memory(), this->object_offset, this->vk_requirements.size, map_flags, mapped_memory)) != VK_SUCCESS) {
        DLOG(fatal, "Could not map buffer memory to CPU-memory: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Flushes all unflushed memory operations done on mapped memory. If the memory of this buffer has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then nothing is done as the memory is already automatically flushed. */
void Buffer::flush() const {
    DENTER("Rendering::Buffer::flush");

    // If this buffer is coherent, quite immediately
    if (!(this->pool.properties() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        DRETURN;
    }

    // Prepare the call to the flush function
    VkMappedMemoryRange memory_range;
    populate_memory_range(memory_range, this->pool.memory(), this->object_offset, this->vk_requirements.size);

    // Do the flush call
    VkResult vk_result;
    if ((vk_result = vkFlushMappedMemoryRanges(this->gpu, 1, &memory_range)) != VK_SUCCESS) {
        DLOG(fatal, "Could not flush mapped buffer memory: " + vk_error_map[vk_result]);
    }

    // D0ne
    DRETURN;
}

/* Unmaps buffer's memory. */
void Buffer::unmap() const {
    DENTER("Rendering::Buffer::unmap");

    // Simply call unmap, done
    vkUnmapMemory(this->gpu, this->pool.memory());

    // Done
    DRETURN;
}



/* Schedules a copy to the given buffer on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given. */
void Buffer::schedule_copyto(const Buffer* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, VkDeviceSize target_offset, const Rendering::CommandBuffer* command_buffer) const {
    DENTER("Rendering::Buffer::schedule_copyto(Buffer)");

    #ifndef NDEBUG
    // Check if the offsets are valid
    if (source_offset >= this->vk_requirements.size) {
        DLOG(fatal, "Source offset of " + std::to_string(source_offset) + " is out of range for buffer of size " + std::to_string(this->vk_requirements.size) + ".");
    }
    if (target_offset >= destination->vk_requirements.size) {
        DLOG(fatal, "Target offset of " + std::to_string(target_offset) + " is out of range for buffer of size " + std::to_string(destination->vk_requirements.size) + ".");
    }
    #endif

    // Check if the sizes is not too large
    n_bytes = n_bytes == VK_WHOLE_SIZE ? this->vk_requirements.size : n_bytes;
    #ifndef NDEBUG
    if (n_bytes > this->vk_requirements.size - source_offset) {
        DLOG(fatal, "Cannot copy " + Tools::bytes_to_string(n_bytes) + " from buffer of only " + Tools::bytes_to_string(this->vk_requirements.size) + " (with offset=" + std::to_string(source_offset) + ").");
    }
    if (n_bytes > destination->vk_requirements.size - target_offset) {
        DLOG(fatal, "Cannot copy " + Tools::bytes_to_string(n_bytes) + " to buffer of only " + Tools::bytes_to_string(destination->vk_requirements.size) + " (with offset=" + std::to_string(target_offset) + ").");
    }
    #endif

    // Make sure they have the required flags
    #ifndef NDEBUG
    if (!(this->init_data.buffer_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
        DLOG(fatal, "Source buffer does not have VK_BUFFER_USAGE_TRANSFER_SRC_BIT-flag set.");
    }
    if (!(destination->init_data.buffer_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
        DLOG(fatal, "Destination buffer does not have VK_BUFFER_USAGE_TRANSFER_DST_BIT-flag set.");
    }
    #endif

    // We schedule the copy by populating a struct
    VkBufferCopy copy_region{};
    copy_region.srcOffset = source_offset;
    copy_region.dstOffset = target_offset;
    copy_region.size = n_bytes;
    vkCmdCopyBuffer(command_buffer->command_buffer(), this->vk_buffer, destination->vk_buffer, 1, &copy_region);

    // D0ne
    DRETURN;
}

/* Schedules a copy to the given image on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given (the latter of which is three dimensional). */
void Buffer::schedule_copyto(Image* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, const VkOffset3D& target_offset, const Rendering::CommandBuffer* command_buffer) const {
    DENTER("Rendering::Buffer::schedule_copyto(Image)");

    #ifndef NDEBUG
    // Check if the offsets are valid
    if (source_offset >= this->vk_requirements.size) {
        DLOG(fatal, "Source offset of " + std::to_string(source_offset) + " is out of range for buffer of size " + std::to_string(this->vk_requirements.size) + ".");
    }
    if (static_cast<uint32_t>(target_offset.x) >= destination->vk_extent.width) {
        DLOG(fatal, "Target offset.x of " + std::to_string(target_offset.x) + " is out of range for image of width " + std::to_string(destination->vk_extent.width) + ".");
    }
    if (static_cast<uint32_t>(target_offset.y) >= destination->vk_extent.height) {
        DLOG(fatal, "Target offset.y of " + std::to_string(target_offset.y) + " is out of range for image of height " + std::to_string(destination->vk_extent.height) + ".");
    }
    #endif

    #ifndef NDEBUG
    // Check if the buffer is not too small
    if (4 * destination->vk_extent.width * destination->vk_extent.height > this->vk_requirements.size) {
        DLOG(fatal, "Cannot copy " + std::to_string(n_bytes) + " pixels from buffer of only " + Tools::bytes_to_string(this->vk_requirements.size) + " (with offset=" + std::to_string(source_offset) + ").");
    }
    #endif

    // Make sure they have the required flags
    #ifndef NDEBUG
    if (!(this->init_data.buffer_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
        DLOG(fatal, "Source buffer does not have VK_BUFFER_USAGE_TRANSFER_SRC_BIT-flag set.");
    }
    if (!(destination->init_data.image_usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
        DLOG(fatal, "Destination image does not have VK_IMAGE_USAGE_TRANSFER_DST_BIT-flag set.");
    }
    #endif

    // We schedule the copy by populating a struct
    VkExtent3D vk_extent = { destination->vk_extent.width, destination->vk_extent.height, 1 };
    VkBufferImageCopy buffer_image_copy;
    populate_buffer_image_copy(buffer_image_copy, source_offset, 0, target_offset, vk_extent);

    // Schedule the copy
    vkCmdCopyBufferToImage(
        command_buffer->command_buffer(),
        this->vk_buffer, destination->vk_image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &buffer_image_copy
    );

    // Update the layout in the image, then D0ne
    destination->vk_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    DRETURN;
}



/* Copies this buffer's contents to the given Buffer, scheduling the command on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. Also, it can be opted to not wait until the given queue is idle again but to return immediately. */
void Buffer::copyto(const Buffer* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, VkDeviceSize target_offset, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue, bool wait_queue_idle) const {
    DENTER("Rendering::Buffer::copyto(Buffer)");

    // Start recording the given command buffer, and we'll tell Vulkan we use this recording only once
    command_buffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    // We schedule the copy
    this->schedule_copyto(destination, n_bytes, source_offset, target_offset, command_buffer);
    // Since that's all, submit the queue. Note that we only return once the copy is 
    command_buffer->end(vk_queue, wait_queue_idle);

    // D0ne
    DRETURN;
}

/* Copies this buffer's contents to the given Image, scheduling the command on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source buffer and target image can be given. (the latter in three dimensions). Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. */
void Buffer::copyto(Image* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, const VkOffset3D& target_offset, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue, bool wait_queue_idle) const {
     DENTER("Rendering::Buffer::copyto(Image)");

    // Start recording the given command buffer, and we'll tell Vulkan we use this recording only once
    command_buffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    // We schedule the copy
    this->schedule_copyto(destination, n_bytes, source_offset, target_offset, command_buffer);
    // Since that's all, submit the queue. Note that we only return once the copy is 
    command_buffer->end(vk_queue, wait_queue_idle);

    // D0ne
    DRETURN;
}
