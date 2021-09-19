/* SWAPCHAIN FRAME.cpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 15:38:52
 * Last edited:
 *   08/09/2021, 15:38:52
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains data needed to draw to a single frame returned by the
 *   swapchain.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "SwapchainFrame.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the frame's color-aspect image view create info. */
static void populate_color_view_info(VkImageViewCreateInfo& view_info, const VkImage& vk_image, VkFormat vk_format) {
    // Set the struct's default values
    view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    // Link the image
    view_info.image = vk_image;

    // Set the type and format of the image
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = vk_format;

    // Set the components of the swapchain. For now, all of them are just themselves
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Set the subresource range's properties: what kind of aspect we're interested in, how many bitmaps this image has and how many layers
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
}

/* Populates the given VkFramebufferCreateInfo struct. */
static void populate_framebuffer_info(VkFramebufferCreateInfo& framebuffer_info, const VkRenderPass& vk_render_pass, const Tools::Array<VkImageView>& attachments, const VkExtent2D& vk_extent) {
    // Set to default
    framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

    // Attach the render pass
    framebuffer_info.renderPass = vk_render_pass;

    // Attach the image view as a single attachment
    framebuffer_info.attachmentCount = attachments.size();
    framebuffer_info.pAttachments = attachments.rdata();

    // Finally, extract the framebuffer size from the extent
    framebuffer_info.width = vk_extent.width;
    framebuffer_info.height = vk_extent.height;
    framebuffer_info.layers = 1;
}





/***** SWAPCHAINFRAME CLASS *****/
/* Constructor for the SwapchainFrame class, which takes a GPU where it lives, a renderpass to bind to, the index of the swapchain image we wrap, the image itself, its format, its size and a depth-aspect image view originating from a DepthStencil. */
SwapchainFrame::SwapchainFrame(const Rendering::GPU& gpu, const Rendering::RenderPass& render_pass, uint32_t vk_image_index, const VkImage& vk_image, VkFormat vk_image_format, const VkExtent2D& vk_image_extent, const VkImageView& vk_depth_view) :
    gpu(gpu),
    render_pass(render_pass),

    vk_image_index(vk_image_index),
    vk_image(vk_image),
    vk_format(vk_image_format),
    vk_extent(vk_image_extent),

    vk_depth_view(vk_depth_view),

    in_flight_fence(nullptr)
{
    // logger.logc(Verbosity::details, SwapchainFrame::channel, "Initializing...");
    VkResult vk_result;

    // Create the color aspect image view
    // logger.logc(Verbosity::details, SwapchainFrame::channel, "Creating image view...");
    VkImageViewCreateInfo color_view_info;
    populate_color_view_info(color_view_info, this->vk_image, this->vk_format);
    if ((vk_result = vkCreateImageView(this->gpu, &color_view_info, nullptr, &this->vk_color_view)) != VK_SUCCESS) {
        logger.fatalc(SwapchainFrame::channel, "Could not create color-aspect image view: ", vk_error_map[vk_result]);
    }

    // Create the framebuffer
    // logger.logc(Verbosity::details, SwapchainFrame::channel, "Creating framebuffer...");
    Tools::Array<VkImageView> framebuffer_views = { this->vk_color_view, this->vk_depth_view };
    VkFramebufferCreateInfo framebuffer_info;
    populate_framebuffer_info(framebuffer_info, this->render_pass.vulkan(), framebuffer_views, this->vk_extent);
    if ((vk_result = vkCreateFramebuffer(this->gpu, &framebuffer_info, nullptr, &this->vk_framebuffer)) != VK_SUCCESS) {
        logger.fatalc(SwapchainFrame::channel, "Could not create framebuffer: ", vk_error_map[vk_result]);
    }
    
    // logger.logc(Verbosity::details, SwapchainFrame::channel, "Init success.");
}

/* Move constructor for the SwapchainFrame class. */
SwapchainFrame::SwapchainFrame(SwapchainFrame&& other) :
    gpu(other.gpu),
    render_pass(other.render_pass),

    vk_image_index(other.vk_image_index),
    vk_image(other.vk_image),
    vk_format(other.vk_format),
    vk_extent(other.vk_extent),

    vk_color_view(other.vk_color_view),
    vk_depth_view(other.vk_depth_view),
    vk_framebuffer(other.vk_framebuffer),

    in_flight_fence(other.in_flight_fence)
{
    // Mark the color image view as non-deallocatable
    other.vk_color_view = nullptr;
    // Also the framebuffer
    other.vk_framebuffer = nullptr;
}

/* Destructor for the SwapchainFrame class. */
SwapchainFrame::~SwapchainFrame() {
    // logger.logc(Verbosity::details, SwapchainFrame::channel, "Cleaning...");

    if (this->vk_framebuffer != nullptr) {
        // logger.logc(Verbosity::details, SwapchainFrame::channel, "Cleaning framebuffer...");
        vkDestroyFramebuffer(this->gpu, this->vk_framebuffer, nullptr);
    }
    if (this->vk_color_view != nullptr) {
        // logger.logc(Verbosity::details, SwapchainFrame::channel, "Cleaning color image view...");
        vkDestroyImageView(this->gpu, this->vk_color_view, nullptr);
    }
    
    // logger.logc(Verbosity::details, SwapchainFrame::channel, "Cleaned.");
}



/* Swap operator for the SwapchainFrame class. */
void Rendering::swap(SwapchainFrame& sf1, SwapchainFrame& sf2) {
    #ifndef NDEBUG
    if (sf1.gpu != sf2.gpu) { logger.fatalc(SwapchainFrame::channel, "Cannot swap swapchain frames with different GPUs."); }
    if (&sf1.render_pass != &sf2.render_pass) { logger.fatalc(SwapchainFrame::channel, "Cannot swap swapchain frames with different render passes."); }
    #endif

    using std::swap;

    swap(sf1.vk_image_index, sf2.vk_image_index);
    swap(sf1.vk_image, sf2.vk_image);
    swap(sf1.vk_format, sf2.vk_format);
    swap(sf1.vk_extent, sf2.vk_extent);
    
    swap(sf1.vk_color_view, sf2.vk_color_view);
    swap(sf1.vk_depth_view, sf2.vk_depth_view);
    swap(sf1.vk_framebuffer, sf2.vk_framebuffer);

    swap(sf1.in_flight_fence, sf2.in_flight_fence);
}
