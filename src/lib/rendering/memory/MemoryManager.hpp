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

#include <unordered_map>

#include "tools/Array.hpp"
#include "../memory/MemoryPool.hpp"
#include "../commandbuffers/CommandPool.hpp"
#include "../descriptors/DescriptorPool.hpp"

namespace Rasterizer::Rendering {
    /* The MemoryManager struct, which bundles different kind of pools into one place. */
    struct MemoryManager {
        /* The size of the draw command pool, in bytes. */
        static constexpr VkDeviceSize draw_pool_size = 1024 * 1024 * 1024;
        /* The size of the stage command pool, in bytes. */
        static constexpr VkDeviceSize stage_pool_size = 1024 * 1024 * 1024;


        /* The GPU on which the MemoryManager is based. */
        const Rendering::GPU& gpu;

        /* The command pool for the draw calls. */
        Rendering::CommandPool draw_cmd_pool;
        /* The command pool for memory calls. */
        Rendering::CommandPool mem_cmd_pool;

        /* The memory pool for GPU-only, speedier buffers. */
        Rendering::MemoryPool draw_pool;
        /* The memory pool for staging buffers. */
        Rendering::MemoryPool stage_pool;

        /* The descriptor pool used to manage the descriptors. */
        Rendering::DescriptorPool descr_pool;


        /* Constructor for the MemoryManager class, which takes the GPU where it is defined for. */
        MemoryManager(const Rendering::GPU& gpu);

    };

}

#endif
