/* SWAPCHAIN.cpp
 *   by Lut99
 *
 * Created:
 *   09/05/2021, 18:40:07
 * Last edited:
 *   25/05/2021, 18:14:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the wrapper around Vulkan's swapchain. Not only does it
 *   manage those resources, but also all memory-related buffer and image
 *   management to make it easy to manage those.
**/

#include <limits>

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"
#include "../auxillary/Formats.hpp"

#include "Swapchain.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VkSwapchainCreateInfo struct. */
static void populate_swapchain_info(VkSwapchainCreateInfoKHR& swapchain_info, VkSurfaceKHR vk_surface, const VkSurfaceCapabilitiesKHR& surface_capabilities, const VkSurfaceFormatKHR& surface_format, const VkPresentModeKHR& surface_present_mode, const VkExtent2D surface_extent, uint32_t image_count, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE) {
    // Set the standard stuff
    swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // Bind the surface
    swapchain_info.surface = vk_surface;

    // Next, set the images we chose
    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = surface_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Then, we select the sharing mode of the image. This is always exclusive, since we'll change ownership manually
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Set the transform (like flipping or w/e) to default to the surface's current
    swapchain_info.preTransform = surface_capabilities.currentTransform;
    // Set the usage of the alpha channel, which we'll just ignore
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // Set the presentation mode, with clipping (i.e., pixels behind windows may be ignored)
    swapchain_info.presentMode = surface_present_mode;
    swapchain_info.clipped = VK_TRUE;

    // FInally, set no old swapchain (for now, at least)
    swapchain_info.oldSwapchain = old_swapchain;
}

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
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
}





/***** SELECTION FUNCTIONS *****/
/* Given a list of supported formats, returns our most desireable one. */
static VkSurfaceFormatKHR choose_swapchain_format(const Tools::Array<VkSurfaceFormatKHR>& formats) {
    for (uint32_t i = 0; i < formats.size(); i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            logger.logc(Verbosity::debug, Swapchain::channel, "Using format: ", vk_format_map[formats[i].format]);
            return formats[i];
        }
    }

    // Otherwise, return the first one - assuming there is one
    if (formats.size() == 0) {
        logger.fatalc(Swapchain::channel, "No surface formats given");
    }
    logger.logc(Verbosity::debug, Swapchain::channel, "Using format: ", vk_format_map[formats[0].format]);
    return formats[0];
}

/* Given a list of supported presentation modes, returns our most desireable one. */
static VkPresentModeKHR choose_swapchain_present_mode(const Tools::Array<VkPresentModeKHR>& modes) {
    // Since default, blocking VSYNC mode is guaranteed to exist, we'll pick that
    VkPresentModeKHR result = VK_PRESENT_MODE_FIFO_KHR;
    (void) modes;
    
    // Done, return
    return result;
}

/* Given the capabilities of a given surface and given the target window, returns the best extent (size) of the swapchain. Takes higher DPI monitors into account, where one pixel != one coordinate. */
static VkExtent2D choose_swapchain_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* glfw_window) {
    // If the currentExtent is already another value, then ez fix
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    // Query the actual size of the frame buffer
    int width, height;
    glfwGetFramebufferSize(glfw_window, &width, &height);

    // Create a new extent that's the actual one
    VkExtent2D result{};
    result.width = static_cast<uint32_t>(width);
    result.height = static_cast<uint32_t>(height);

    // Make sure the new extent is bounded by the min & maximum
    if (result.width < capabilities.minImageExtent.width) {
        result.width = capabilities.minImageExtent.width;
    } else if (result.width > capabilities.maxImageExtent.width) {
        result.width = capabilities.maxImageExtent.width;
    }
    if (result.height < capabilities.minImageExtent.height) {
        result.height = capabilities.minImageExtent.height;
    } else if (result.height > capabilities.maxImageExtent.height) {
        result.height = capabilities.maxImageExtent.height;
    }

    // Now we can return in peace
    return result;
}





/***** SWAPCHAIN CLASS *****/
/* Constructor for the Swapchain class, which takes the GPU where it will be constructed and the window to which it shall present. */
Swapchain::Swapchain(const GPU& gpu, GLFWwindow* glfw_window, const Surface& surface) :
    gpu(gpu),
    surface(surface)
{
    logger.logc(Verbosity::important, Swapchain::channel, "Initializing...");

    

    // We begin by selecting appropriate formats
    logger.logc(Verbosity::details, Swapchain::channel, "Preparing Swapchain creation...");
    // Choose the options
    this->vk_surface_format = choose_swapchain_format(this->gpu.swapchain_info().formats());
    this->vk_surface_present_mode = choose_swapchain_present_mode(this->gpu.swapchain_info().present_modes());
    this->vk_surface_extent = choose_swapchain_extent(this->gpu.swapchain_info().capabilities(), glfw_window);
    this->vk_desired_image_count = this->gpu.swapchain_info().capabilities().minImageCount + 1;
    if (this->gpu.swapchain_info().capabilities().maxImageCount > 0 && this->vk_desired_image_count > this->gpu.swapchain_info().capabilities().maxImageCount) {
        this->vk_desired_image_count = this->gpu.swapchain_info().capabilities().maxImageCount;
    }



    // Next, we can move to construction of the swapchain
    logger.logc(Verbosity::details, Swapchain::channel, "Constructing swapchain...");
    logger.logc(Verbosity::debug, Swapchain::channel, "Swapchain image size  : ", this->vk_surface_extent.width, 'x', this->vk_surface_extent.height);
    logger.logc(Verbosity::debug, Swapchain::channel, "Swapchain image count : ", this->vk_desired_image_count);

    // Populate the create info
    VkSwapchainCreateInfoKHR swapchain_info;
    populate_swapchain_info(swapchain_info, this->surface, this->gpu.swapchain_info().capabilities(), this->vk_surface_format, this->vk_surface_present_mode, this->vk_surface_extent, this->vk_desired_image_count);

    // Use that to actually create the swapchain
    VkResult vk_result;
    if ((vk_result = vkCreateSwapchainKHR(this->gpu, &swapchain_info, nullptr, &this->vk_swapchain)) != VK_SUCCESS) {
        logger.fatalc(Swapchain::channel, "Could not create swapchain: " + vk_error_map[vk_result]);
    }



    // Retrieve the images that are part of the swapchain
    logger.logc(Verbosity::details, Swapchain::channel, "Retrieving images...");

    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, nullptr);
    this->vk_swapchain_images.reserve(this->vk_actual_image_count);
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, this->vk_swapchain_images.wdata(this->vk_actual_image_count));
    logger.logc(Verbosity::debug, Swapchain::channel, "Retrieved ", this->vk_actual_image_count, " images");



    logger.logc(Verbosity::important, Swapchain::channel, "Init success.");
}

/* Copy constructor for the Swapchain class. */
Swapchain::Swapchain(const Swapchain& other) :
    gpu(other.gpu),
    surface(other.surface),

    vk_surface_format(other.vk_surface_format),
    vk_surface_present_mode(other.vk_surface_present_mode),
    vk_surface_extent(other.vk_surface_extent),

    vk_actual_image_count(other.vk_actual_image_count),
    vk_desired_image_count(other.vk_desired_image_count)
{
    logger.logc(Verbosity::debug, Swapchain::channel, "Copying...");

    // We copy the swapchain by-recreating it using the standard options
    VkSwapchainCreateInfoKHR swapchain_info;
    populate_swapchain_info(swapchain_info, this->surface, this->gpu.swapchain_info().capabilities(), this->vk_surface_format, this->vk_surface_present_mode, this->vk_surface_extent, this->vk_desired_image_count);

    // Use that to actually create the swapchain
    VkResult vk_result;
    if ((vk_result = vkCreateSwapchainKHR(this->gpu, &swapchain_info, nullptr, &this->vk_swapchain)) != VK_SUCCESS) {
        logger.fatalc(Swapchain::channel, "Could not create swapchain: ", vk_error_map[vk_result]);
    }

    // Fetch new images
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, nullptr);
    this->vk_swapchain_images.reserve(this->vk_actual_image_count);
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, this->vk_swapchain_images.wdata(this->vk_actual_image_count));

    // Done
    logger.logc(Verbosity::debug, Swapchain::channel, "Copy success.");
}

/* Move constructor for the Swapchain class. */
Swapchain::Swapchain(Swapchain&& other) :
    gpu(other.gpu),
    vk_swapchain(other.vk_swapchain),
    surface(other.surface),
    vk_surface_format(other.vk_surface_format),
    vk_surface_present_mode(other.vk_surface_present_mode),
    vk_surface_extent(other.vk_surface_extent),
    vk_actual_image_count(other.vk_actual_image_count),
    vk_desired_image_count(other.vk_desired_image_count),
    vk_swapchain_images(other.vk_swapchain_images)
{
    // Set the deallocatable objects to nullptrs to avoid them, well, being deallocation
    other.vk_swapchain = nullptr;
}

/* Destructor for the Swapchain class. */
Swapchain::~Swapchain() {
    logger.logc(Verbosity::important, Swapchain::channel, "Cleaning...");

    if (this->vk_swapchain != nullptr) {
        logger.logc(Verbosity::details, Swapchain::channel, "Destroying internal swapchain object...");
        vkDestroySwapchainKHR(this->gpu, this->vk_swapchain, nullptr);
    }

    logger.logc(Verbosity::important, Swapchain::channel, "Cleaned.");
}



/* Returns a list of SwapchainFrames from the internal images. They will be bound to the given RenderPass and DepthStencil. */
Tools::Array<Rendering::SwapchainFrame> Swapchain::get_frames(const Rendering::RenderPass& render_pass, const Rendering::DepthStencil& depth_stencil) const {
    // Create the list with the initial size
    Tools::Array<Rendering::SwapchainFrame> result(this->vk_actual_image_count);
    for (uint32_t i = 0; i < this->vk_actual_image_count; i++) {
        result.push_back(Rendering::SwapchainFrame(
            this->gpu,
            render_pass,
            i,
            this->vk_swapchain_images[i],
            this->vk_surface_format.format,
            this->vk_surface_extent,
            depth_stencil.view()
        ));
    }

    // Return the list
    return result;
}



/* Resizes the swapchain. Note that this also re-creates it, so any existing handle to the internal VkSwapchain will be invalid. */
void Swapchain::resize(uint32_t new_width, uint32_t new_height) {
    logger.logc(Verbosity::important, Swapchain::channel, "Re-creating swapchain...");



    // First, delete old stuff
    logger.logc(Verbosity::details, Swapchain::channel, "Deallocating old swapchain...");
    this->vk_swapchain_images.clear();
    vkDestroySwapchainKHR(this->gpu, this->vk_swapchain, nullptr);



    // We set the new extent
    logger.logc(Verbosity::details, Swapchain::channel, "Re-creating new swapchain...");
    logger.logc(Verbosity::debug, Swapchain::channel, "New swapchain size: ", new_width, 'x', new_height);
    this->vk_surface_extent = VkExtent2D({ new_width, new_height });

    // Populate the create info
    VkSwapchainCreateInfoKHR swapchain_info;
    populate_swapchain_info(swapchain_info, this->surface, this->gpu.swapchain_info().capabilities(), this->vk_surface_format, this->vk_surface_present_mode, this->vk_surface_extent, this->vk_desired_image_count);

    // Use that to actually create the swapchain
    VkResult vk_result;
    if ((vk_result = vkCreateSwapchainKHR(this->gpu, &swapchain_info, nullptr, &this->vk_swapchain)) != VK_SUCCESS) {
        logger.fatalc(Swapchain::channel, "Could not re-create swapchain: " + vk_error_map[vk_result]);
    }



    // Retrieve the images that are part of the swapchain
    logger.logc(Verbosity::details, Swapchain::channel, "Retrieving new images...");
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, nullptr);
    this->vk_swapchain_images.reserve(this->vk_actual_image_count);
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, this->vk_swapchain_images.wdata(this->vk_actual_image_count));
    logger.logc(Verbosity::debug, Swapchain::channel, "Retrieved ", this->vk_actual_image_count, " images");



    logger.logc(Verbosity::important, Swapchain::channel, "Re-creation success.");
}

/* Resizes the swapchain to the size of the given window. Note that this also re-creates it, so any existing handle to the internal VkSwapchain will be invalid. */
void Swapchain::resize(GLFWwindow* glfw_window) {
    // Fetch the new size from the window
    VkExtent2D new_extent = choose_swapchain_extent(this->gpu.swapchain_info().capabilities(), glfw_window);
    // Call the other resize to do the actual work
    this->resize(new_extent.width, new_extent.height);
}



/* Swap operator for the Swapchain class. */
void Rendering::swap(Swapchain& s1, Swapchain& s2) {
    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (s1.gpu != s2.gpu) { logger.fatalc(Swapchain::channel, "Cannot swap swapchains with different GPUs"); }
    // If the surface is not the same...
    if (s1.surface != s2.surface) { logger.fatalc(Swapchain::channel, "Cannot swap swapchains with different surfaces"); }
    #endif

    // Swap all fields
    using std::swap;
    
    swap(s1.vk_swapchain, s2.vk_swapchain);

    swap(s1.vk_surface_format, s2.vk_surface_format);
    swap(s1.vk_surface_present_mode, s2.vk_surface_present_mode);
    swap(s1.vk_surface_extent, s2.vk_surface_extent);

    swap(s1.vk_actual_image_count, s2.vk_actual_image_count);
    swap(s1.vk_desired_image_count, s2.vk_desired_image_count);

    swap(s1.vk_swapchain_images, s2.vk_swapchain_images);
}
