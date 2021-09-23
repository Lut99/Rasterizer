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

#ifndef RENDERING_COMMAND_BUFFER_HPP
#define RENDERING_COMMAND_BUFFER_HPP

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Makma3D::Rendering {
    /* The CommandBuffer class, which acts as a reference to an allocated CommandBuffer in the CommandPool. Can thus be comfortably deallocated and then later re-acquired by its matching handle. */
    class CommandBuffer {
    public:
        /* Channel name for the CommandBuffer class. */
        static constexpr const char* channel = "CommandBuffer";

    private:
        /* The VkCommandBuffer object that we wrap. */
        VkCommandBuffer vk_command_buffer;

        /* Mark the CommandPool class as friend. */
        friend class CommandPool;

        /* Private constructor for the CommandBuffer, which only takes the VkCommandBuffer object to wrap. */
        CommandBuffer(VkCommandBuffer vk_command_buffer);
        /* Private destructor for the CommandBuffer class. */
        ~CommandBuffer();

    public:
        /* Copy constructor for the CommandBuffer class, which is deleted. */
        CommandBuffer(const CommandBuffer& other) = delete;
        /* Move constructor for the CommandBuffer class, which is deleted. */
        CommandBuffer(CommandBuffer&& other) = delete;

        /* Begins recording the command buffer. Overwrites whatever is already recorded here, for some reason. Takes optional usage flags for this recording. */
        void begin(VkCommandBufferUsageFlags usage_flags = 0) const;
        /* Ends recording the command buffer, but does not yet submit to any queue unless one is given. If so, then you can optionally specify to wait or not to wait for the queue to become idle. */
        void end(VkQueue vk_queue = nullptr, bool wait_queue_idle = true) const;
        /* Return the VkSubmitInfo for this command buffer. */
        VkSubmitInfo get_submit_info() const;

        /* Explititly returns the internal VkCommandBuffer object. */
        inline const VkCommandBuffer& vulkan() const { return this->vk_command_buffer; }
        /* Implicitly returns the internal VkCommandBuffer object. */
        inline operator const VkCommandBuffer&() const { return this->vk_command_buffer; }

        /* Copy assignment operator for the CommandBuffer class, which is deleted. */
        CommandBuffer& operator=(const CommandBuffer& other) = delete;
        /* Move assignment operator for the CommandBuffer class, which is deleted. */
        CommandBuffer& operator=(CommandBuffer&& other) = delete;

    };
}

#endif
