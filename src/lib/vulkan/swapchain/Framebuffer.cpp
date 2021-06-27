/* FRAMEBUFFER.cpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 14:11:46
 * Last edited:
 *   27/06/2021, 14:11:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a wrapper for the VkFramebuffer class. Is created by the
 *   RenderPass class, since its associated with its attachments.
**/

#include "tools/CppDebugger.hpp"
#include "vulkan/ErrorCodes.hpp"

#include "Framebuffer.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkFramebufferCreateInfo struct. */
static void populate_framebuffer_info(VkFramebufferCreateInfo& framebuffer_info, const VkRenderPass& vk_render_pass, const VkImageView& vk_image_view, const VkExtent2D& vk_extent) {
    DENTER("populate_framebuffer_info");

    // Set to default
    framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

    // Attach the render pass
    framebuffer_info.renderPass = vk_render_pass;

    // Attach the image view as a single attachment
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = &vk_image_view;

    // Finally, extract the framebuffer size from the extent
    framebuffer_info.width = vk_extent.width;
    framebuffer_info.height = vk_extent.height;
    framebuffer_info.layers = 1;

    // Done
    DRETURN;
}





/***** FRAMEBUFFER CLASS *****/
/* Constructor for the Framebuffer class, which takes a GPU to allocate it on, a renderpass to bind to, a VkImageView to wrap around and an extent describing the buffer's size. */
Framebuffer::Framebuffer(const Vulkan::GPU& gpu, const VkRenderPass& vk_render_pass, const VkImageView& vk_image_view, const VkExtent2D& vk_extent) :
    gpu(gpu),
    vk_extent(vk_extent),
    vk_image_view(vk_image_view)
{
    DENTER("Vulkan::Framebuffer::Framebuffer");

    // Populate the create info
    populate_framebuffer_info(this->vk_framebuffer_info, vk_render_pass, this->vk_image_view, this->vk_extent);

    // Use that to create the internal framebuffer
    VkResult vk_result;
    if ((vk_result = vkCreateFramebuffer(this->gpu, &this->vk_framebuffer_info, nullptr, &this->vk_framebuffer)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create framebuffer: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Copy constructor for the Framebuffer class. */
Framebuffer::Framebuffer(const Framebuffer& other) :
    gpu(other.gpu),
    vk_extent(other.vk_extent),
    vk_image_view(other.vk_image_view),
    vk_framebuffer_info(other.vk_framebuffer_info)
{
    DENTER("Vulkan::Framebuffer::Framebuffer(copy)");

    // Re-create the internal framebuffer
    VkResult vk_result;
    if ((vk_result = vkCreateFramebuffer(this->gpu, &this->vk_framebuffer_info, nullptr, &this->vk_framebuffer)) != VK_SUCCESS) {
        DLOG(fatal, "Could not re-create framebuffer: " + vk_error_map[vk_result]);
    }

    DRETURN;
}

/* Move constructor for the Framebuffer class. */
Framebuffer::Framebuffer(Framebuffer&& other) :
    gpu(other.gpu),
    vk_framebuffer(other.vk_framebuffer),
    vk_extent(other.vk_extent),
    vk_image_view(other.vk_image_view),
    vk_framebuffer_info(other.vk_framebuffer_info)
{
    // Mark the framebuffer as do-not-deallocate
    other.vk_framebuffer = nullptr;
}

/* Destructor for the Framebuffer class. */
Framebuffer::~Framebuffer() {
    DENTER("Vulkan::Framebuffer::~Framebuffer");

    if (this->vk_framebuffer != nullptr) {
        vkDestroyFramebuffer(this->gpu, this->vk_framebuffer, nullptr);
    }

    DRETURN;
}



/* Swap operator for the Framebuffer class. */
void Vulkan::swap(Vulkan::Framebuffer& fb1, Vulkan::Framebuffer& fb2) {
    DENTER("Vulkan::swap(Framebuffer)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (fb1.gpu != fb2.gpu) {
        DLOG(fatal, "Cannot swap framebuffers with different GPUs");
    }
    #endif

    // Swap EVERYTHING but the GPU
    using std::swap;
    swap(fb1.vk_framebuffer, fb2.vk_framebuffer);
    swap(fb1.vk_extent, fb2.vk_extent);
    swap(fb1.vk_image_view, fb2.vk_image_view);
    swap(fb1.vk_framebuffer_info, fb2.vk_framebuffer_info);

    // Done
    DRETURN;
}
