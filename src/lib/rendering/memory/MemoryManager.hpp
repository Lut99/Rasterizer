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
#include "../views/ImageViewPool.hpp"
#include "../pipeline/PipelinePool.hpp"

namespace Makma3D::Rendering {
    /* The MemoryManager class, which bundles different kind of pools into one place. */
    class MemoryManager {
    public:
        /* Channel name for the MemoryManager class. */
        static constexpr const char* channel = "MemoryManager";

        /* The GPU on which the MemoryManager is based. */
        const Rendering::GPU& gpu;

    public:
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

        /* The pipeline pool used to efficiently allocate pipelines. */
        Rendering::PipelinePool pipeline_pool;
        /* The ImageView pool used to allocate image views. */
        Rendering::ImageViewPool view_pool;

        /* A command buffer for memory transfer operations. */
        Rendering::CommandBuffer* copy_cmd;

    public:
        /* Constructor for the MemoryManager class, which takes the GPU where it is defined for and the sizes of the two memory pools. */
        MemoryManager(const Rendering::GPU& gpu, VkDeviceSize draw_pool_size, VkDeviceSize stage_pool_size);
        /* Copy constructor for the MemoryManager class, which is deleted. */
        MemoryManager(const MemoryManager& other) = delete;;
        /* Move constructor for the MemoryManager class. */
        MemoryManager(MemoryManager&& other);
        /* Destructor for the MemoryManager class. */
        ~MemoryManager();

        /* Copy assignment operator for the MemoryManager class, which is deleted. */
        MemoryManager& operator=(const MemoryManager& other) = delete;
        /* Move assignment operator for the MemoryManager class. */
        inline MemoryManager& operator=(MemoryManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the MemoryManager class. */
        friend void swap(MemoryManager& mm1, MemoryManager& mm2);

    };

    /* Swap operator for the MemoryManager class. */
    void swap(MemoryManager& mm1, MemoryManager& mm2);

}

#endif
