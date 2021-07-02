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
    public:
        /* Handle for the GPU object that the Buffer is allocated. */
        const Rendering::GPU& gpu;

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
        Buffer(const Rendering::GPU& gpu, buffer_h handle, VkBuffer buffer, VkBufferUsageFlags vk_usage_flags, VkSharingMode vk_sharing_mode, VkBufferCreateFlags vk_create_flags, VkDeviceMemory vk_memory, VkDeviceSize memory_offset, VkDeviceSize memory_size, VkDeviceSize req_memory_size, VkMemoryPropertyFlags memory_properties);
        
        /* Mark the MemoryPool as friend. */
        friend class MemoryPool;

    public:
        /* Sets the buffer using an intermediate staging buffer. The staging buffer is copied using the given command buffer on the given queue. */
        void set(const Rendering::CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const;
        /* Gets n_bytes data from this buffer using an intermediate staging buffer. The buffers are copied over using the given command buffer on the given queue. The result is put in the given pointer. */
        void get(const Rendering::CommandBuffer& command_buffer, const Buffer& staging_buffer, VkQueue vk_queue, void* data, uint32_t n_bytes) const;

        /* Maps the buffer to host-memory so it can be written to. Only possible if the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set for the memory of this buffer's pool. Note that the memory is NOT automatically unmapped if the Buffer object is destroyed. */
        void map(void** mapped_memory) const;
        /* Flushes all unflushed memory operations done on mapped memory. If the memory of this buffer has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then nothing is done as the memory is already automatically flushed. */
        void flush() const;
        /* Unmaps buffer's memory. */
        void unmap() const;

        /* Copies this buffer's contents to the given Buffer, scheduling the command on the given command buffer. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. Also, it can be opted to not wait until the given queue is idle again but to return immediately. */
        inline void copyto(const Buffer& destination, const Rendering::CommandBuffer& command_buffer, VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const { return copyto(destination, VK_WHOLE_SIZE, 0, 0, command_buffer, vk_queue, wait_queue_idle); }
        /* Copies this buffer's contents to the given Buffer, scheduling the command on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. Also, it can be opted to not wait until the given queue is idle again but to return immediately. */
        void copyto(const Buffer& destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, VkDeviceSize target_offset, const Rendering::CommandBuffer& command_buffer, VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const;

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
