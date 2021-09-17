/* FRAME MANAGER.cpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 23:33:43
 * Last edited:
 *   08/09/2021, 23:33:43
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the FrameManager class, which is in charge of retrieving a
 *   conceptual frame from the swapchain.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "FrameManager.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPresentInfo struct. */
static void populate_present_info(VkPresentInfoKHR& present_info, const Swapchain& swapchain, const uint32_t& swapchain_index, const Semaphore& wait_for_semaphore) {
    // Set to default
    present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Add which swapchain and which image to use
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain.swapchain();
    present_info.pImageIndices = &swapchain_index;
    
    // Set the semaphore count
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &wait_for_semaphore.semaphore();

    // Also set the results validation handlers to not needed, since we only have one swapchain
    present_info.pResults = nullptr;
}





/***** FRAMEMANAGER CLASS *****/
/* Constructor for the FrameManager class, which takes a MemoryManager for stuff allocation, a Swapchain to draw images from, a layout for the frame's global descriptor and a layout for the frame's per-object descriptors. */
FrameManager::FrameManager(Rendering::MemoryManager& memory_manager, const Rendering::Swapchain& swapchain, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& object_layout) :
    memory_manager(memory_manager),
    swapchain(swapchain),

    frame_index(0)
{
    logger.logc(Verbosity::important, FrameManager::channel, "Initializing...");

    // Create the conceptual frames
    logger.logc(Verbosity::details, FrameManager::channel, "Preparing ConceptualFrames...");
    this->conceptual_frames.reserve(FrameManager::max_frames_in_flight);
    for (uint32_t i = 0; i < FrameManager::max_frames_in_flight; i++) {
        this->conceptual_frames.push_back(ConceptualFrame(this->memory_manager, global_layout, object_layout));
    }

    logger.logc(Verbosity::important, FrameManager::channel, "Init success.");
}

/* Move constructor for the FrameManager class. */
FrameManager::FrameManager(FrameManager&& other) :
    memory_manager(other.memory_manager),
    swapchain(other.swapchain),

    swapchain_frames(std::move(other.swapchain_frames)),
    conceptual_frames(std::move(other.conceptual_frames)),

    frame_index(other.frame_index)
{
    // Array's move guarantees that it's empty after a move constructor
}

/* Destructor for the FrameManager class. */
FrameManager::~FrameManager() {
    logger.logc(Verbosity::important, FrameManager::channel, "Cleaning...");

    // Nothing yet, as C++'s default deallocators seem to deal with it

    logger.logc(Verbosity::important, FrameManager::channel, "Cleaned.");
}



/* Binds the FrameManager to a render pass and a depth stencil by retrieving the swapchain frames. Must be done at least once. */
void FrameManager::bind(const Rendering::RenderPass& render_pass, const Rendering::DepthStencil& depth_stencil) {
    logger.logc(Verbosity::details, FrameManager::channel, "Binding FrameManager to RenderPass @ ", &render_pass, " and DepthStencil @ ", &depth_stencil);

    // Clear any old frames
    this->swapchain_frames.clear();
    // Get the list of swapchain images
    this->swapchain_frames = this->swapchain.get_frames(render_pass, depth_stencil);

    // Reset the index for the frame we're gonna render to
    this->frame_index = 0;

    // Done
}

/* Returns a new ConceptualFrame to which the render system can render. Blocks until any such frame is available. */
Rendering::ConceptualFrame* FrameManager::get_frame() {
    // First, wait until the current conceptual frame is not in flight anymore
    Rendering::ConceptualFrame* conceptual_frame = &this->conceptual_frames[this->frame_index];
    conceptual_frame->in_flight_fence.wait();

    // Next, try to get a swapchain for this conceptual frame
    uint32_t swapchain_index;
    VkResult vk_result = vkAcquireNextImageKHR(this->swapchain.gpu, this->swapchain, UINT64_MAX, conceptual_frame->image_ready_semaphore, VK_NULL_HANDLE, &swapchain_index);
    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR) {
        // The swapchain is outdated or suboptimal (probably a resize); we resize to fit again
        return nullptr;
    } else if (vk_result != VK_SUCCESS) {
        logger.fatalc(FrameManager::channel, "Could not get frame from swapchain: ", vk_error_map[vk_result]);
    }

    // When we have an image, make sure it's not secretly being used by another conceptual frame
    Rendering::SwapchainFrame* swapchain_frame = &this->swapchain_frames[swapchain_index];
    if (swapchain_frame->in_flight_fence != nullptr) {
        swapchain_frame->in_flight_fence->wait();
    }
    swapchain_frame->in_flight_fence = &conceptual_frame->in_flight_fence;

    // Link the found swapchain image to the conceptualframe
    conceptual_frame->swapchain_frame = swapchain_frame;

    // Done, increment the frame index and return
    this->frame_index = (this->frame_index + 1) % FrameManager::max_frames_in_flight;
    return conceptual_frame;
}

/* Schedules the given frame for presentation once rendering to it has been completed. Returns whether or not the window needs to be resized. */
bool FrameManager::present_frame(Rendering::ConceptualFrame* conceptual_frame) {
    // Prepare the present info
    VkPresentInfoKHR present_info;
    populate_present_info(present_info, this->swapchain, conceptual_frame->swapchain_frame->index(), conceptual_frame->render_ready_semaphore);

    // Present it using the queue present function
    Tools::Array<VkQueue> present_queues = this->memory_manager.gpu.queues(QueueType::present);
    VkResult vk_result = vkQueuePresentKHR(present_queues[0], &present_info);
    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR) {
        // The swapchain is outdated or suboptimal (probably a resize); we resize to fit again
        return true;
    } else if (vk_result != VK_SUCCESS) {
        logger.fatalc(FrameManager::channel, "Could not present frame: ", vk_error_map[vk_result]);
    }

    // Done
    return false;
}



/* Swap operator for the FrameManager class. */
void Rendering::swap(FrameManager& fm1, FrameManager& fm2) {
    #ifndef NDEBUG
    if (&fm1.memory_manager != &fm2.memory_manager) { logger.fatalc(FrameManager::channel, "Cannot swap frame managers with different memory managers."); }
    if (&fm1.swapchain != &fm2.swapchain) { logger.fatalc(FrameManager::channel, "Cannot swap frame managers with different swapchains."); }
    #endif

    using std::swap;

    swap(fm1.swapchain_frames, fm2.swapchain_frames);
    swap(fm1.conceptual_frames, fm2.conceptual_frames);

    swap(fm1.frame_index, fm2.frame_index);
}
