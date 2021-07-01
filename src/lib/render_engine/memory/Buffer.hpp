/* BUFFER.hpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:19:56
 * Last edited:
 *   19/06/2021, 12:19:56
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Buffer class, which wraps a pre-allocated VkBuffer
 *   object. Only usable in the context of the MemoryPool.
**/

#ifndef RENDERING_BUFFER_HPP
#define RENDERING_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "render_engine/gpu/GPU.hpp"
#include "render_engine/commandbuffers/CommandBuffer.hpp"

#include "MemoryHandle.hpp"

namespace Rasterizer::Rendering {
    /* Handle for buffer objects, which is used to reference buffers. */
    using buffer_h = memory_h;



    /* The Buffer class, which is a reference to a Buffer allocated by the MemoryPool. Do NOT change any fields in this class directly, as memory etc is managed by the memory pool. */
    class Buffer {
    private:
        /* Handle for this buffer object. */
        buffer_h vk_handle;

        /* The actual VkBuffer object constructed. */
        VkBuffer vk_buffer;

        /* Describes the usage flags set for this buffer. */
        VkBufferUsageFlags vk_usage_flags;
        /* Describes the sharing mode for this buffer. */
        VkSharingMode vk_sharing_mode;
        /* Describes the create flags for this buffer. */
        VkBufferCreateFlags vk_create_flags;

        /* Reference to the large memory block where this buffer is allocated. */
        VkDeviceMemory vk_memory;
        /* The offset of the internal buffer. */
        VkDeviceSize vk_memory_offset;
        /* The size of the internal buffer. */
        VkDeviceSize vk_memory_size;
        /* The actual size of the internal buffer, as reported by memory_requirements.size. */
        VkDeviceSize vk_req_memory_size;
        /* The properties of the memory for this buffer. */
        VkMemoryPropertyFlags vk_memory_properties;

        /* Private constructor for the Buffer class, which takes the buffer, the buffer's size and the properties of the pool's memory. */
        Buffer(buffer_h handle, VkBuffer buffer, VkBufferUsageFlags vk_usage_flags, VkSharingMode vk_sharing_mode, VkBufferCreateFlags vk_create_flags, VkDeviceMemory vk_memory, VkDeviceSize memory_offset, VkDeviceSize memory_size, VkDeviceSize req_memory_size, VkMemoryPropertyFlags memory_properties);
        
        /* Mark the MemoryPool as friend. */
        friend class MemoryPool;

    public:
        /* Sets the buffer using an intermediate staging buffer. The staging buffer is copied using the given command buffer on the given queue. */
        void set(const Rendering::GPU& gpu, const Rendering::CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const;
        /* Gets n_bytes data from this buffer using an intermediate staging buffer. The buffers are copied over using the given command buffer on the given queue. The result is put in the given pointer. */
        void get(const Rendering::GPU& gpu, const Rendering::CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const;

        /* Maps the buffer to host-memory so it can be written to. Only possible if the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set for the memory of this buffer's pool. Note that the memory is NOT automatically unmapped if the Buffer object is destroyed. */
        void map(const Rendering::GPU& gpu, void** mapped_memory) const;
        /* Flushes all unflushed memory operations done on mapped memory. If the memory of this buffer has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then nothing is done as the memory is already automatically flushed. */
        void flush(const Rendering::GPU& gpu) const;
        /* Unmaps buffer's memory. */
        void unmap(const Rendering::GPU& gpu) const;

        /* Copies this buffer's content to another given buffer on the given memory-enabled GPU queue. The given command pool (which must be a pool for the memory-enabled queue) is used to schedule the copy. Optionally waits until the queue is idle before returning. Note that the given buffer needn't come from the same memory pool. */
        inline void copyto(const Rendering::CommandBuffer& command_buffer, VkQueue vk_queue, const Buffer& destination, bool wait_queue_idle = true) const { return copyto(destination, command_buffer, vk_queue, std::numeric_limits<VkDeviceSize>::max(), 0, wait_queue_idle); }
        /* Copies this buffer's content to another given buffer on the given memory-enabled GPU queue. The given command pool (which must be a pool for the memory-enabled queue) is used to schedule the copy. Optionally waits until the queue is idle before returning. Note that the given buffer needn't come from the same memory pool. */
        void copyto(const Buffer& destination, const Rendering::CommandBuffer& command_buffer, VkQueue vk_queue, VkDeviceSize n_bytes, VkDeviceSize target_offset = 0, bool wait_queue_idle = true) const;

        /* Returns the size of the buffer, in bytes. */
        inline VkDeviceSize size() const { return this->vk_memory_size; }
        /* Returns the memory offset of the buffer, in bytes. */
        inline VkDeviceSize offset() const { return this->vk_memory_offset; }
        /* Explicit retrieval of the internal buffer object. */
        inline const VkBuffer& buffer() const { return this->vk_buffer; }
        /* Implicit retrieval of the internal buffer object. */
        inline operator VkBuffer() const { return this->vk_buffer; }
        /* Explicit retrieval of the internal handle. */
        inline const buffer_h& handle() const { return this->vk_handle; }
        /* Implicit retrieval of the internal handle. */
        inline operator buffer_h() const { return this->vk_handle; }

        /* Swap operator for the Buffer class. */
        friend void swap(Buffer& b1, Buffer& b2);
    };

    /* Swap operator for the Buffer class. */
    void swap(Buffer& b1, Buffer& b2);
}

#endif
