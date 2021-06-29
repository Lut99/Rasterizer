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
#include "tools/CppDebugger.hpp"

#include "vulkan/Formats.hpp"
#include "vulkan/ErrorCodes.hpp"

#include "Swapchain.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VkSwapchainCreateInfo struct. */
static void populate_swapchain_info(VkSwapchainCreateInfoKHR& swapchain_info, VkSurfaceKHR vk_surface, const VkSurfaceCapabilitiesKHR& surface_capabilities, const VkSurfaceFormatKHR& surface_format, const VkPresentModeKHR& surface_present_mode, const VkExtent2D surface_extent, uint32_t image_count, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE) {
    DENTER("populate_swapchain_info");

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

    // Done!
    DRETURN;
}

/* Populates a given VkImageViewCreateInfo struct. */
static void populate_view_info(VkImageViewCreateInfo& view_info, const VkImage& vk_image, const VkFormat& vk_format) {
    DENTER("populate_view_info");

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

    // We're done
    DRETURN;
}





/***** SELECTION FUNCTIONS *****/
/* Given a list of supported formats, returns our most desireable one. */
static VkSurfaceFormatKHR choose_swapchain_format(const Tools::Array<VkSurfaceFormatKHR>& formats) {
    DENTER("choose_swapchain_format");

    for (uint32_t i = 0; i < formats.size(); i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            DLOG(info, "Using format: " + vk_format_map[formats[i].format]);
            DRETURN formats[i];
        }
    }

    // Otherwise, return the first one - assuming there is one
    if (formats.size() == 0) {
        DLOG(fatal, "No surface formats given");
    }
    DLOG(info, "Using format: " + vk_format_map[formats[0].format]);
    DRETURN formats[0];
}

/* Given a list of supported presentation modes, returns our most desireable one. */
static VkPresentModeKHR choose_swapchain_present_mode(const Tools::Array<VkPresentModeKHR>& modes) {
    DENTER("choose_swapchain_present_mode");

    // Since default, blocking VSYNC mode is guaranteed to exist, we'll pick that
    VkPresentModeKHR result = VK_PRESENT_MODE_FIFO_KHR;
    (void) modes;
    
    // Done, return
    DRETURN result;
}

/* Given the capabilities of a given surface and given the target window, returns the best extent (size) of the swapchain. Takes higher DPI monitors into account, where one pixel != one coordinate. */
static VkExtent2D choose_swapchain_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* glfw_window) {
    DENTER("choose_swapchain_extent");

    // If the currentExtent is already another value, then ez fix
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        DRETURN capabilities.currentExtent;
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
    DRETURN result;
}





/***** SWAPCHAIN CLASS *****/
/* Constructor for the Swapchain class, which takes the GPU where it will be constructed and the window to which it shall present. */
Swapchain::Swapchain(const GPU& gpu, GLFWwindow* glfw_window, const Surface& surface) :
    gpu(gpu),
    surface(surface)
{
    DENTER("Compute::Swapchain::Swapchain");
    DLOG(info, "Initializing Swapchain...");
    DINDENT;

    

    // We begin by selecting appropriate formats
    DLOG(info, "Preparing Swapchain creation...");
    // Choose the options
    this->vk_surface_format = choose_swapchain_format(this->gpu.swapchain_info().formats());
    this->vk_surface_present_mode = choose_swapchain_present_mode(this->gpu.swapchain_info().present_modes());
    this->vk_surface_extent = choose_swapchain_extent(this->gpu.swapchain_info().capabilities(), glfw_window);
    this->vk_desired_image_count = this->gpu.swapchain_info().capabilities().minImageCount + 1;
    if (this->gpu.swapchain_info().capabilities().maxImageCount > 0 && this->vk_desired_image_count > this->gpu.swapchain_info().capabilities().maxImageCount) {
        this->vk_desired_image_count = this->gpu.swapchain_info().capabilities().maxImageCount;
    }



    // Next, we can move to construction of the swapchain
    DLOG(info, "Constructing swapchain...");
    DINDENT;
    DLOG(info, "Swapchain image size  : " + std::to_string(this->vk_surface_extent.width) + "x" + std::to_string(this->vk_surface_extent.height));
    DLOG(info, "Swapchain image count : " + std::to_string(this->vk_desired_image_count));
    DDEDENT;

    // Populate the create info
    VkSwapchainCreateInfoKHR swapchain_info;
    populate_swapchain_info(swapchain_info, this->surface, this->gpu.swapchain_info().capabilities(), this->vk_surface_format, this->vk_surface_present_mode, this->vk_surface_extent, this->vk_desired_image_count);

    // Use that to actually create the swapchain
    VkResult vk_result;
    if ((vk_result = vkCreateSwapchainKHR(this->gpu, &swapchain_info, nullptr, &this->vk_swapchain)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create swapchain: " + vk_error_map[vk_result]);
    }



    // Retrieve the images that are part of the swapchain
    DLOG(info, "Retrieving images...");

    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, nullptr);
    this->vk_swapchain_images.reserve(this->vk_actual_image_count);
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, this->vk_swapchain_images.wdata(this->vk_actual_image_count));
    DINDENT;
    DLOG(info, "Retrieved " + std::to_string(this->vk_actual_image_count) + " images");
    DDEDENT;

    // Also re-create the image views and framebuffers
    this->create_views(this->vk_swapchain_images, this->vk_surface_format.format);



    DDEDENT;
    DLEAVE;
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
    DENTER("Compute::Swapchain::Swapchain(copy)");

    // We copy the swapchain by-recreating it using the standard options
    VkSwapchainCreateInfoKHR swapchain_info;
    populate_swapchain_info(swapchain_info, this->surface, this->gpu.swapchain_info().capabilities(), this->vk_surface_format, this->vk_surface_present_mode, this->vk_surface_extent, this->vk_desired_image_count);

    // Use that to actually create the swapchain
    VkResult vk_result;
    if ((vk_result = vkCreateSwapchainKHR(this->gpu, &swapchain_info, nullptr, &this->vk_swapchain)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create swapchain: " + vk_error_map[vk_result]);
    }

    // Fetch new images
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, nullptr);
    this->vk_swapchain_images.reserve(this->vk_actual_image_count);
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, this->vk_swapchain_images.wdata(this->vk_actual_image_count));

    // Also re-create the image views and framebuffers
    this->create_views(this->vk_swapchain_images, this->vk_surface_format.format);



    DLEAVE;
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
    vk_swapchain_images(other.vk_swapchain_images),
    vk_swapchain_views(other.vk_swapchain_views)
{
    // Set the deallocatable objects to nullptrs to avoid them, well, being deallocation
    other.vk_swapchain = nullptr;
    other.vk_swapchain_views.clear();
}

/* Destructor for the Swapchain class. */
Swapchain::~Swapchain() {
    DENTER("Compute::Swapchain::~Swapchain");
    DLOG(info, "Cleaning Swapchain...");
    DINDENT;

    if (this->vk_swapchain_views.size() > 0) {
        DLOG(info, "Destroying image views...");
        for (uint32_t i = 0; i < this->vk_swapchain_views.size(); i++) {
            vkDestroyImageView(this->gpu, this->vk_swapchain_views[i], nullptr);
        }
    }

    if (this->vk_swapchain != nullptr) {
        DLOG(info, "Destroying internal swapchain object...");
        vkDestroySwapchainKHR(this->gpu, this->vk_swapchain, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Private helper function that re-creates image views and frame buffers from the given list of images. */
void Swapchain::create_views(const Tools::Array<VkImage>& vk_images, const VkFormat& vk_format) {
    DENTER("Vulkan::Swapchain::create_views");

    // Create new image views for these fellers
    DLOG(info, "Creating swapchain image views...");
    this->vk_swapchain_views.resize(vk_images.size());
    for (uint32_t i = 0; i < vk_images.size(); i++) {
        // First, create the create info
        VkImageViewCreateInfo view_info;
        populate_view_info(view_info, vk_images[i], vk_format);

        // Create 'em
        VkResult vk_result;
        if ((vk_result = vkCreateImageView(this->gpu, &view_info, nullptr, &this->vk_swapchain_views[i])) != VK_SUCCESS) {
            DLOG(fatal, "Could not create image view for image " + std::to_string(i) + ": " + vk_error_map[vk_result]);
        }
    }

    DRETURN;
}



/* Resizes the swapchain. Note that this also re-creates it, so any existing handle to the internal VkSwapchain will be invalid. */
void Swapchain::resize(uint32_t new_width, uint32_t new_height) {
    DENTER("Vulkan::Swapchain::resize");
    DLOG(info, "Re-creating swapchain...");
    DINDENT;



    // First, delete old stuff
    DLOG(info, "Deallocating old swapchain...");
    for (uint32_t i = 0; i < this->vk_swapchain_views.size(); i++) {
        vkDestroyImageView(this->gpu, this->vk_swapchain_views[i], nullptr);
    }
    this->vk_swapchain_views.clear();
    this->vk_swapchain_images.clear();
    vkDestroySwapchainKHR(this->gpu, this->vk_swapchain, nullptr);



    // We set the new extent
    DLOG(info, "Re-creating new swapchain...");
    DINDENT; DLOG(info, "New swapchain size: " + std::to_string(new_width) + "x" + std::to_string(new_height)); DDEDENT;
    this->vk_surface_extent = VkExtent2D({ new_width, new_height });

    // Populate the create info
    VkSwapchainCreateInfoKHR swapchain_info;
    populate_swapchain_info(swapchain_info, this->surface, this->gpu.swapchain_info().capabilities(), this->vk_surface_format, this->vk_surface_present_mode, this->vk_surface_extent, this->vk_desired_image_count);

    // Use that to actually create the swapchain
    VkResult vk_result;
    if ((vk_result = vkCreateSwapchainKHR(this->gpu, &swapchain_info, nullptr, &this->vk_swapchain)) != VK_SUCCESS) {
        DLOG(fatal, "Could not re-create swapchain: " + vk_error_map[vk_result]);
    }



    // Retrieve the images that are part of the swapchain
    DLOG(info, "Retrieving new images...");
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, nullptr);
    this->vk_swapchain_images.reserve(this->vk_actual_image_count);
    vkGetSwapchainImagesKHR(this->gpu, this->vk_swapchain, &this->vk_actual_image_count, this->vk_swapchain_images.wdata(this->vk_actual_image_count));
    DINDENT;
    DLOG(info, "Retrieved " + std::to_string(this->vk_actual_image_count) + " images");
    DDEDENT;

    // Also re-create the image views and framebuffers
    this->create_views(this->vk_swapchain_images, this->vk_surface_format.format);



    DDEDENT;
    DRETURN;
}

/* Resizes the swapchain to the size of the given window. Note that this also re-creates it, so any existing handle to the internal VkSwapchain will be invalid. */
void Swapchain::resize(GLFWwindow* glfw_window) {
    DENTER("Vulkan::Swapchain::resize(window)");

    // Fetch the new size from the window
    VkExtent2D new_extent = choose_swapchain_extent(this->gpu.swapchain_info().capabilities(), glfw_window);
    // Call the other resize to do the actual work
    this->resize(new_extent.width, new_extent.height);

    // Done
    DRETURN;
}



/* Swap operator for the Swapchain class. */
void Vulkan::swap(Swapchain& s1, Swapchain& s2) {
    DENTER("Compute::swap(Swapchain)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (s1.gpu != s2.gpu) {
        DLOG(fatal, "Cannot swap swapchains with different GPUs");
    }
    // If the surface is not the same...
    if (s1.surface != s2.surface) {
        DLOG(fatal, "Cannot swap swapchains with different surfaces");
    }
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
    swap(s1.vk_swapchain_views, s2.vk_swapchain_views);

    // Done
    DRETURN;
}
