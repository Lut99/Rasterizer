/* DEPTH STENCIL.cpp
 *   by Lut99
 *
 * Created:
 *   17/07/2021, 12:57:09
 * Last edited:
 *   17/07/2021, 12:57:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DepthStencil class, which wraps around an image and can
 *   be used by the pipeline to perform depth testing.
**/

#include "../auxillary/ErrorCodes.hpp"

#include "DepthStencil.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** HELPER FUNCTIONS *****/
/* Finds a suitable format for our depth image. */
static VkFormat find_depth_format(const Rendering::GPU& gpu) {
    

    // Go through the list of suggested formats
    Tools::Array<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    for (uint32_t i = 0; i < candidates.size(); i++) {
        // Query the properties of this format on the device
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(gpu, candidates[i], &properties);

        // If the format is supported for depth stencil attachments, pick it
        if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return candidates[i];
        }
    }

    // Otherwise, no format we like
    DLOG(fatal, "Could not find a supported format.");
    return VK_FORMAT_MAX_ENUM;
}





/***** POPULATE FUNCTIONS *****/
/* Populates a given VkImageViewCreateInfo struct. */
static void populate_view_info(VkImageViewCreateInfo& view_info, const VkImage& vk_image, const VkFormat& vk_format) {
    

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
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    // We're done
    return;
}





/***** DEPTHSTENCIL CLASS *****/
/* Constructor for the DepthStencil class, which takes the GPU where the DepthStencil lives, a memory pool to allocate the internal image from and the size of the depth stencil (as an VkExtent2D). */
DepthStencil::DepthStencil(const Rendering::GPU& gpu, Rendering::MemoryPool& draw_pool, const VkExtent2D& image_extent) :
    gpu(gpu),
    draw_pool(draw_pool)
{
    
    DLOG(info, "Initializing depth stencil...");
    DINDENT;



    // Allocate the image - both the memory and the Vulkan memory
    DLOG(info, "Allocating internal image...");
    this->rendering_image = this->draw_pool.allocate(image_extent, find_depth_format(this->gpu), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);



    // Initialize the create info for the VkImageView
    DLOG(info, "Initializing internal image view...");
    VkImageViewCreateInfo view_info;
    populate_view_info(view_info, this->rendering_image->image(), this->rendering_image->format());

    // Actually create the view info
    VkResult vk_result;
    if ((vk_result = vkCreateImageView(this->gpu, &view_info, nullptr, &this->vk_image_view)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create image view: " + vk_error_map[vk_result]);
    }



    // Done
    DDEDENT;
}

/* Copy constructor for the DepthStencil class. */
DepthStencil::DepthStencil(const DepthStencil& other) :
    gpu(other.gpu),
    draw_pool(other.draw_pool)
{
    



    // Copy the image object
    this->rendering_image = this->draw_pool.allocate(other.rendering_image);



    // Initialize the create info for the VkImageView
    VkImageViewCreateInfo view_info;
    populate_view_info(view_info, this->rendering_image->image(), this->rendering_image->format());

    // Actually create the view info
    VkResult vk_result;
    if ((vk_result = vkCreateImageView(this->gpu, &view_info, nullptr, &this->vk_image_view)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create image view: " + vk_error_map[vk_result]);
    }
}

/* Move constructor for the DepthStencil class. */
DepthStencil::DepthStencil(DepthStencil&& other) :
    gpu(other.gpu),
    draw_pool(other.draw_pool),
    rendering_image(other.rendering_image),
    vk_image_view(other.vk_image_view)
{
    // Set the other's image and image view to non-deallocatable
    other.rendering_image = nullptr;
    other.vk_image_view = nullptr;
}

/* Destructor for the DepthStencil class. */
DepthStencil::~DepthStencil() {
    

    // Deallocate the image view if we need to
    if (this->vk_image_view != nullptr) {
        vkDestroyImageView(this->gpu, this->vk_image_view, nullptr);
    }
    // Deallocate the image too if we need to
    if (this->rendering_image != nullptr) {
        this->draw_pool.free(this->rendering_image);
    }
}



/* Resizes the depth stencil to the given size. */
void DepthStencil::resize(const VkExtent2D& new_extent) {
    

    // Deallocate the old image view
    vkDestroyImageView(this->gpu, this->vk_image_view, nullptr);
    // Deallocate the old image
    VkFormat old_format = this->rendering_image->format();
    this->draw_pool.free(this->rendering_image);

    // Allocate a new image
    this->rendering_image = this->draw_pool.allocate(new_extent, old_format, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    // Also re-create the image view
    VkImageViewCreateInfo view_info;
    populate_view_info(view_info, this->rendering_image->image(), this->rendering_image->format());
    VkResult vk_result;
    if ((vk_result = vkCreateImageView(this->gpu, &view_info, nullptr, &this->vk_image_view)) != VK_SUCCESS) {
        DLOG(fatal, "Could not resize image view: " + vk_error_map[vk_result]);
    }

    // Done
    return;
}



/* Swap operator for the DepthStencil class. */
void Rendering::swap(DepthStencil& ds1, DepthStencil& ds2) {
    

    #ifndef NDEBUG
    // Make sure the GPU and the pool match
    if (ds1.gpu != ds2.gpu) {
        DLOG(fatal, "Cannot swap depth stencils with different GPUs.");
    }
    if (&ds1.draw_pool != &ds2.draw_pool) {
        DLOG(fatal, "Cannot swap depth stencils with different draw pools.");
    }
    #endif

    // Swap all fields
    using std::swap;

    swap(ds1.rendering_image, ds2.rendering_image);
    swap(ds1.vk_image_view, ds2.vk_image_view);

    // Done
    return;
}

