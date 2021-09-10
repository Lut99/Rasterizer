/* COMMAND POOL.hpp
 *   by Lut99
 *
 * Created:
 *   27/04/2021, 13:03:55
 * Last edited:
 *   10/09/2021, 10:55:56
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

#include "tools/Array.hpp"
#include "../gpu/GPU.hpp"

#include "CommandBuffer.hpp"

namespace Makma3D::Rendering {
    /* The CommandPool class, which manages CommandBuffers for a single device queue. */
    class CommandPool {
    public:
        /* Channel name for the CommandPool class. */
        static constexpr const char* channel = "CommandPool";

        /* Constant reference to the device that we're managing the pool for. */
        const Rendering::GPU& gpu;

    private:
        /* The command pool object that we wrap. */
        VkCommandPool vk_command_pool;
        /* The device queue index of this pool. */
        uint32_t vk_queue_index;
        /* The create flags used to allocate the pool. */
        VkCommandPoolCreateFlags vk_create_flags;

        /* List of the CommandBuffers allocated with this pool. */
        Tools::Array<CommandBuffer*> command_buffers;

    public:
        /* Constructor for the CommandPool class, which takes the GPU to allocate for, the queue index for which this pool allocates buffers and optionally create flags for the pool. */
        CommandPool(const Rendering::GPU& gpu, uint32_t queue_index, VkCommandPoolCreateFlags create_flags = 0);
        /* Copy constructor for the CommandPool class. */
        CommandPool(const CommandPool& other);
        /* Move constructor for the CommandPool class. */
        CommandPool(CommandPool&& other);
        /* Destructor for the CommandPool class. */
        ~CommandPool();

        /* Allocates a single, new command buffer of the given level. Returns a new buffer object. */
        CommandBuffer* allocate(VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        /* Allocates N new command buffers of the given level. Returns by handles. */
        Tools::Array<CommandBuffer*> nallocate(uint32_t n_buffers, VkCommandBufferLevel buffer_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        
        /* Deallocates the CommandBuffer behind the given handle. Note that all buffers are deallocated automatically when the CommandPool is destructed, but this could save you memory. */
        void free(const CommandBuffer* buffer);
        /* Deallocates an array of given command buffers. */
        void nfree(const Tools::Array<CommandBuffer*>& buffers);

        /* Returns the queue family index for this command pool. */
        inline uint32_t queue_index() const { return this->vk_queue_index; }

        /* Expliticly returns the internal VkCommandPool object. */
        inline const VkCommandPool& command_pool() const { return this->vk_command_pool; }
        /* Implicitly returns the internal VkCommandPool object. */
        inline operator VkCommandPool() const { return this->vk_command_pool; }

        /* Copy assignment operator for the CommandPool class. */
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
