/* BUFFER.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 15:03:35
 * Last edited:
 *   16/08/2021, 15:03:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Buffer class, which wraps a VkBuffer allocated by the
 *   MemoryPool.
**/

#ifndef RENDERING_BUFFER_HPP
#define RENDERING_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "../commandbuffers/CommandBuffer.hpp"

#include "MemoryObject.hpp"
#include "Image.hpp"

namespace Rasterizer::Rendering {
    /* The Buffer class, which wraps a VkBuffer object and who's memory is managerd by the MemoryPool class. */
    class Buffer: public MemoryObject {
    public:
        /* Channel name for the Buffer class. */
        static constexpr const char* channel = "Buffer";

    private:
        /* The InitData struct, which is used to group everything needed for copying buffers. */
        struct InitData {
            /* The usage flags for this buffer. */
            VkBufferUsageFlags buffer_usage;
            /* The sharing mode for this buffer. */
            VkSharingMode sharing_mode;
            /* The create flags for this buffer. */
            VkBufferCreateFlags create_flags;
        };

    private:
        /* The Vulkan buffer object we wrap. */
        VkBuffer vk_buffer;
        /* The given size at the creation of the buffer. */
        VkDeviceSize buffer_size;
        /* The memory requirements of this specific object, including its real size (in bytes). */
        VkMemoryRequirements vk_requirements;

        /* Other data needed only for the buffer to be copyable. */
        InitData init_data;

        /* Declare the random MemoryPool class as friend. */
        friend class MemoryPool;

        
        /* Constructor for the Buffer class, which takes the pool where it was allocated, the buffer object to wrap, the offset of this buffer in the main memory pool, its desired size and its memory properties. Also takes other stuff that's needed to copy the buffer. */
        Buffer(const MemoryPool& pool, VkBuffer vk_buffer, VkDeviceSize offset, VkDeviceSize buffer_size, const VkMemoryRequirements& vk_requirements, VkBufferUsageFlags buffer_usage, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags);
        /* Destructor for the Buffer class. */
        ~Buffer();
    
    public:
        /* Copy constructor for the Buffer class, which is deleted. */
        Buffer(const Buffer& other) = delete;
        /* Move constructor for the Buffer class, which is deleted. */
        Buffer(Buffer&& other) = delete;

        /* Sets the buffer using an intermediate staging buffer. The staging buffer is copied using the given command buffer on the given queue. */
        void set(void* data, uint32_t n_bytes, const Buffer* staging_buffer, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue = nullptr) const;
        /* Gets n_bytes data from this buffer using an intermediate staging buffer. The buffers are copied over using the given command buffer on the given queue. The result is put in the given pointer. */
        void get(void* data, uint32_t n_bytes, const Buffer* staging_buffer, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue = nullptr) const;

        /* Maps the buffer to host-memory so it can be written to. Only possible if the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT is set for the memory of this buffer's pool. Note that the memory is NOT automatically unmapped if the Buffer object is destroyed. */
        void map(void** mapped_memory, VkMemoryMapFlags map_flags = 0) const;
        /* Flushes all unflushed memory operations done on mapped memory. If the memory of this buffer has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then nothing is done as the memory is already automatically flushed. */
        void flush(VkDeviceSize n_bytes = VK_WHOLE_SIZE) const;
        /* Unmaps buffer's memory. */
        inline void unmap() const { vkUnmapMemory(this->gpu, this->pool.memory()); }

        /* Schedules a copy to the given buffer on the given command buffer. */
        inline void schedule_copyto(const Buffer* destination, const Rendering::CommandBuffer* command_buffer) const { return this->schedule_copyto(destination, VK_WHOLE_SIZE, 0, 0, command_buffer); }
        /* Schedules a copy to the given buffer on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given. */
        void schedule_copyto(const Buffer* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, VkDeviceSize target_offset, const Rendering::CommandBuffer* command_buffer) const;
        /* Schedules a copy to the given image on the given command buffer. */
        inline void schedule_copyto(Image* destination, const Rendering::CommandBuffer* command_buffer) const { return this->schedule_copyto(destination, VK_WHOLE_SIZE, 0, VkOffset3D({0, 0, 0}), command_buffer); }
        /* Schedules a copy to the given image on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given (the latter of which is three dimensional). */
        void schedule_copyto(Image* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, const VkOffset3D& target_offset, const Rendering::CommandBuffer* command_buffer) const;

        /* Copies this buffer's contents immediately to the given Buffer using the given command buffer. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. Also, it can be opted to not wait until the given queue is idle again but to return immediately. */
        inline void copyto(const Buffer* destination, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const { return this->copyto(destination, VK_WHOLE_SIZE, 0, 0, command_buffer, vk_queue, wait_queue_idle); }
        /* Copies this buffer's contents immediately to the given Buffer using the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source and target buffers can be given. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. Also, it can be opted to not wait until the given queue is idle again but to return immediately. */
        void copyto(const Buffer* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, VkDeviceSize target_offset, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const;
        /* Copies this buffer's contents to the given Image, scheduling the command on the given command buffer. Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. */
        inline void copyto(Image* destination, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const { return this->copyto(destination, VK_WHOLE_SIZE, 0, VkOffset3D({0, 0, 0}), command_buffer, vk_queue, wait_queue_idle); }
        /* Copies this buffer's contents to the given Image, scheduling the command on the given command buffer. Only part of the source buffer can be copied by specifying a size other than VK_WHOLE_SIZE, and also an offset in the source buffer and target image can be given. (the latter in three dimensions). Optionally, a queue can be given to run the commands on, which will otherwise default to the first memory queue of the internal GPU. */
        void copyto(Image* destination, VkDeviceSize n_bytes, VkDeviceSize source_offset, const VkOffset3D& target_offset, const Rendering::CommandBuffer* command_buffer, VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const;

        /* Returns the memory offset of the buffer, in bytes. */
        inline VkDeviceSize offset() const { return this->object_offset; }
        /* Returns the size of the buffer. */
        inline VkDeviceSize size() const { return this->buffer_size; }
        /* Returns the actual size of the buffer as allocated. */
        inline VkDeviceSize rsize() const { return this->vk_requirements.size; }
        /* Explicitly returns the internal VkBuffer object. */
        inline const VkBuffer& buffer() const { return this->vk_buffer; }
        /* Implicitly returns the internal VkBuffer object. */
        inline operator const VkBuffer&() const { return this->vk_buffer; }

        /* Copy assignment operator for the Buffer class, which is deleted. */
        Buffer& operator=(const Buffer& other) = delete;
        /* Copy assignment operator for the Buffer class, which is deleted. */
        Buffer& operator=(Buffer&& other) = delete;

    };

}

#endif
