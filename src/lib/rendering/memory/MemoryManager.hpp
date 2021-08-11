/* MEMORY MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 16:22:54
 * Last edited:
 *   8/1/2021, 5:05:57 PM
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

        /* A command buffer for memory transfer operations. */
        Rendering::command_buffer_h copy_cmd_h;


        /* Constructor for the MemoryManager class, which takes the GPU where it is defined for and the sizes of the two memory pools. */
        MemoryManager(const Rendering::GPU& gpu, VkDeviceSize draw_pool_size, VkDeviceSize stage_pool_size);
        /* Copy constructor for the MemoryManager class. */
        MemoryManager(const MemoryManager& other);
        /* Move constructor for the MemoryManager class. */
        MemoryManager(MemoryManager&& other);
        /* Destructor for the MemoryManager class. */
        ~MemoryManager();

        /* Get a reference to the internal copy command buffer. */
        inline Rendering::CommandBuffer copy_cmd() { return this->mem_cmd_pool.deref(this->copy_cmd_h); }

        /* Copy assignment operator for the MemoryManager class. */
        inline MemoryManager& operator=(const MemoryManager& other) { return *this = MemoryManager(other); }
        /* Move assignment operator for the MemoryManager class. */
        inline MemoryManager& operator=(MemoryManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the MemoryManager class. */
        friend void swap(MemoryManager& mm1, MemoryManager& mm2);

    };

    /* Swap operator for the MemoryManager class. */
    void swap(MemoryManager& mm1, MemoryManager& mm2);

}

#endif
