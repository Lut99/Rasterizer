/* MEMORY MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 16:22:54
 * Last edited:
 *   27/07/2021, 16:22:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MemoryManager class, which aims to be a centralized space
 *   for managing all sorts of Vulkan memory.
**/

#ifndef RENDERING_MEMORY_MANAGER_HPP
#define RENDERING_MEMORY_MANAGER_HPP

#include "tools/Array.hpp"
#include "../memory/MemoryPool.hpp"
#include "../commandbuffers/CommandPool.hpp"
#include "../descriptors/DescriptorPool.hpp"

namespace Rasterizer::Rendering {
    /* The MemoryManager class, which bundles different kind of pools into one memory manager. */
    class MemoryManager {
    public:
        /* The GPU on which the MemoryManager is based. */
        const Rendering::GPU& gpu;

    private:
        /* List of all memory pools in the memory manager. */
        Tools::Array<Rendering::MemoryPool> memory_pools;
        /* List of all the command pools in the memory manager. */
        Tools::Array<Rendering::CommandPool> command_pools;
        /* List of all the descriptor pools in the memory manager. */
        Tools::Array<Rendering::DescriptorPool> descriptor_pools;

    public:
        /* Constructor for the MemoryManager class, which takes the GPU where it is defined for. */
        MemoryManager(const Rendering::GPU& gpu);
        
        /* Adds a new memory pool to the manager that has the given memory properties and the given size (in bytes). Optionally, buffer usage flags can be given to properly configure the memory type chosen for the memory pool. */
        void add_memory(VkDeviceSize size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage = 0);
        /* Adds a new command pool to the manager for the given queue family and with the given VkCommandPoolCreateFlags. */
        void add_command(uint32_t queue_family, VkCommandPoolCreateFlags command_pool_flags = 0);
        /* Adds a new descriptor pool to the manager with the given number of descriptor sets and number of descriptors per set. */
        void add_descriptor(const Tools::Array<std::pair<VkDescriptorType, uint32_t>>& descriptor_types, uint32_t max_sets);



    };

}

#endif
