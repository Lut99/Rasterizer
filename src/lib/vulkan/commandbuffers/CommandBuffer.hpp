/* COMMAND BUFFER.hpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:13:06
 * Last edited:
 *   19/06/2021, 12:13:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the CommandBuffer class, which is a wrapper around an already
 *   allocated VkCommandBuffer object. Only usable in the context of the
 *   CommandPool.
**/

#ifndef VULKAN_COMMAND_BUFFER_HPP
#define VULKAN_COMMAND_BUFFER_HPP

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Rasterizer::Vulkan {
    /* Handle for CommandBuffers, which can be used to retrieve them from the Pool. Note that each pool has its own set of handles. */
    using command_buffer_h = uint32_t;

    /* The CommandBuffer class, which acts as a reference to an allocated CommandBuffer in the CommandPool. Can thus be comfortably deallocated and then later re-acquired by its matching handle. */
    class CommandBuffer {
    private:
        /* The handle for this buffer. */
        command_buffer_h vk_handle;
        /* The VkCommandBuffer object that we wrap. */
        VkCommandBuffer vk_command_buffer;

        /* Private constructor for the CommandBuffer, which only takes the handle to this buffer and the VkCommandBuffer object to wrap. */
        CommandBuffer(command_buffer_h handle, VkCommandBuffer vk_command_buffer);

        /* Mark the CommandPool class as friend. */
        friend class CommandPool;

    public:
        /* Initializes the CommandBuffer to a default, unusable state. */
        CommandBuffer();

        /* Begins recording the command buffer. Overwrites whatever is already recorded here, for some reason. Takes optional usage flags for this recording. */
        void begin(VkCommandBufferUsageFlags usage_flags = 0) const;
        /* Ends recording the command buffer, but does not yet submit to any queue unless one is given. If so, then you can optionally specify to wait or not to wait for the queue to become idle. */
        void end(VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const;
        /* Return the VkSubmitInfo for this command buffer. */
        VkSubmitInfo get_submit_info() const;

        /* Explititly returns the internal VkCommandBuffer object. */
        inline const VkCommandBuffer& command_buffer() const { return this->vk_command_buffer; }
        /* Implicitly returns the internal VkCommandBuffer object. */
        inline operator VkCommandBuffer() const { return this->vk_command_buffer; }
        /* Explicitly returns the internal handle. */
        inline const command_buffer_h& handle() const { return this->vk_handle; }
        /* Implicitly returns the internal handle. */
        inline operator command_buffer_h() const { return this->vk_handle; }

    };
}

#endif
