/* COMMAND POOL.hpp
 *   by Lut99
 *
 * Created:
 *   27/04/2021, 13:03:55
 * Last edited:
 *   01/07/2021, 13:43:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the CommandPool class, which handles allocating and
 *   destroying command buffers for a single device queue.
**/

#ifndef RENDERING_COMMAND_POOL_HPP
#define RENDERING_COMMAND_POOL_HPP

#include <vulkan/vulkan.h>
#include <unordered_map>

#include "render_engine/gpu/GPU.hpp"
#include "tools/Array.hpp"

#include "CommandBuffer.hpp"

namespace Rasterizer::Rendering {
    /* The CommandPool class, which manages CommandBuffers for a single device queue. */
    class CommandPool {
    public:
        /* Constant reference to the device that we're managing the pool for. */
        const Rendering::GPU& gpu;

    private:
        /* The command pool object that we wrap. */
        VkCommandPool vk_command_pool;
        /* The device queue index of this pool. */
        uint32_t vk_queue_index;
        /* The create flags used to allocate the pool. */
        VkCommandPoolCreateFlags vk_create_flags;

        /* Map of the CommandBuffers allocated with this pool. */
        std::unordered_map<command_buffer_h, VkCommandBuffer> vk_command_buffers;

    public:
        /* The null handle for the pool. */
        const static constexpr command_buffer_h NullHandle = 0;


        /* Constructor for the CommandPool class, which takes the GPU to allocate for, the queue index for which this pool allocates buffers and optionally create flags for the pool. */
        CommandPool(const Rendering::GPU& gpu, uint32_t queue_index, VkCommandPoolCreateFlags create_flags = 0);
        /* Copy constructor for the CommandPool class. */
        CommandPool(const CommandPool& other);
        /* Move constructor for the CommandPool class. */
        CommandPool(CommandPool&& other);
        /* Destructor for the CommandPool class. */
        ~CommandPool();

        /* Returns a CommandBuffer from the given handle, which can be used as a CommandBuffer. Does not perform any checks on the handle validity. */
        inline CommandBuffer deref(command_buffer_h buffer) const { return CommandBuffer(buffer, this->vk_command_buffers.at(buffer)); }

        /* Allocates a single, new command buffer of the given level. Returns a new buffer object. */
        inline CommandBuffer allocate(VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) { return this->deref(this->allocate_h(buffer_level)); }
        /* Allocates a single, new command buffer of the given level. Returns by handle. */
        command_buffer_h allocate_h(VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        /* Allocates N new command buffers of the given level. Returns by handles. */
        Tools::Array<CommandBuffer> nallocate(uint32_t n_buffers, VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        /* Allocates N new command buffers of the given level. Returns new buffer objects. */
        Tools::Array<command_buffer_h> nallocate_h(uint32_t n_buffers, VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        
        /* Deallocates the CommandBuffer behind the given handle. Note that all buffers are deallocated automatically when the CommandPool is destructed, but this could save you memory. */
        void deallocate(command_buffer_h buffer);
        /* Deallocates an array of given command buffers. */
        void ndeallocate(const Tools::Array<CommandBuffer>& buffers);
        /* Deallocates an array of given command buffer handles. */
        void ndeallocate(const Tools::Array<command_buffer_h>& handles);

        /* Expliticly returns the internal VkCommandPool object. */
        inline const VkCommandPool& command_pool() const { return this->vk_command_pool; }
        /* Implicitly returns the internal VkCommandPool object. */
        inline operator VkCommandPool() const { return this->vk_command_pool; }

        /* Returns the queue family index for this command pool. */
        inline uint32_t queue_index() const { return this->vk_queue_index; }

        /* Copy assignment operator for the CommandPool class, which is deleted. */
        inline CommandPool& operator=(const CommandPool& other) { return *this = CommandPool(other); }
        /* Move assignment operator for the CommandPool class. */
        inline CommandPool& operator=(CommandPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the CommandPool class. */
        friend void swap(CommandPool& cp1, CommandPool& cp2);

    };

    /* Swap operator for the CommandPool class. */
    void swap(CommandPool& cp1, CommandPool& cp2);

}

#endif
