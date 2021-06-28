/* FENCE.cpp
 *   by Lut99
 *
 * Created:
 *   28/06/2021, 16:57:15
 * Last edited:
 *   28/06/2021, 16:57:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that wraps a VkFence object.
**/

#include "tools/CppDebugger.hpp"
#include "vulkan/ErrorCodes.hpp"

#include "Fence.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkFenceCreateInfo struct. */
static void populate_fence_info(VkFenceCreateInfo& fence_info, VkFenceCreateFlags create_flags) {
    DENTER("populate_fence_info");

    // Set to default
    fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    // Set the flags
    fence_info.flags = create_flags;

    // And done!
    DRETURN;
}





/***** FENCE CLASS *****/
/* Constructor for the Fence class, which takes completely nothing! (except for the GPU where it lives, obviously, and optionally some flags) */
Fence::Fence(const Vulkan::GPU& gpu, VkFenceCreateFlags create_flags) :
    gpu(gpu),
    vk_create_flags(create_flags)
{
    DENTER("Vulkan::Fence::Fence");

    // Populate the create info
    VkFenceCreateInfo fence_info;
    populate_fence_info(fence_info, this->vk_create_flags);

    // Create the semaphore
    VkResult vk_result;
    if ((vk_result = vkCreateFence(this->gpu, &fence_info, nullptr, &this->vk_fence)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create fence: " + vk_error_map[vk_result]);
    }

    // Done
    DLEAVE;
}

/* Copy constructor for the Fence class. */
Fence::Fence(const Fence& other) :
    gpu(other.gpu),
    vk_create_flags(other.vk_create_flags)
{
    DENTER("Vulkan::Fence::Fence(copy)");

    // Populate the create info
    VkFenceCreateInfo fence_info;
    populate_fence_info(fence_info, this->vk_create_flags);

    // Create the semaphore
    VkResult vk_result;
    if ((vk_result = vkCreateFence(this->gpu, &fence_info, nullptr, &this->vk_fence)) != VK_SUCCESS) {
        DLOG(fatal, "Could not re-create fence: " + vk_error_map[vk_result]);
    }

    // Done
    DLEAVE;
}

/* Move constructor for the Fence class. */
Fence::Fence(Fence&& other) :
    gpu(other.gpu),
    vk_fence(other.vk_fence),
    vk_create_flags(other.vk_create_flags)
{
    // Prevent the fence from being deallocated
    other.vk_fence = nullptr;
}

/* Destructor for the Fence class. */
Fence::~Fence() {
    DENTER("Vulkan::Fence::~Fence");

    if (this->vk_fence != nullptr) {
        vkDestroyFence(this->gpu, this->vk_fence, nullptr);
    }

    DRETURN;
}



/* Swap operator for the Fence class. */
void Vulkan::swap(Fence& f1, Fence& f2) {
    DENTER("Vulkan::swap(Fence)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (f1.gpu != f2.gpu) {
        DLOG(fatal, "Cannot swap fences with different GPUs");
    }
    #endif

    // Swap EVERYTHING but the GPU
    using std::swap;
    swap(f1.vk_fence, f2.vk_fence);
    swap(f1.vk_create_flags, f2.vk_create_flags);

    // Done
    DRETURN;
}