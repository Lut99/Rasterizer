/* SWAPCHAIN INFO.cpp
 *   by Lut99
 *
 * Created:
 *   05/06/2021, 15:16:58
 * Last edited:
 *   05/06/2021, 15:16:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SwapchainInfo class, which bundles and manages
 *   information about the swapchain to which we render.
**/

#include "../auxillary/ErrorCodes.hpp"
#include "SwapchainInfo.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** SWAPCHAININFO CLASS *****/
/* "Default" constructor for the SwapchainInfo class, which initializes this to "nothing supported". */
SwapchainInfo::SwapchainInfo(const Tools::Logger::InitData& init_data) :
    logger(init_data, "SwapchainInfo"),
    vk_capabilities({})
{}

/* Constructor for the SwapchainInfo class, which takes init data for its logger, a VkPhysicalDevice and a VkSurfaceKHR to populate itself appropriately. */
SwapchainInfo::SwapchainInfo(const Tools::Logger::InitData& init_data, VkPhysicalDevice vk_physical_device, VkSurfaceKHR vk_surface) :
    SwapchainInfo(init_data)
{
    this->logger.log(Verbosity::debug, "Fetching swapchain information...");

    // First, getch the capabilities of the device/surface pair
    VkResult vk_result;
    if ((vk_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, vk_surface, &this->vk_capabilities)) != VK_SUCCESS) {
        this->logger.fatal("Could not get physical device surface capabilities: " + vk_error_map[vk_result]);
    }

    // Next, get the list of formats
    uint32_t n_formats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_surface, &n_formats, nullptr);
    if (n_formats > 0) {
        this->vk_formats.reserve(n_formats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_surface, &n_formats, this->vk_formats.wdata(n_formats));
    }

    // Finally, get the list of present modes
    uint32_t n_modes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_surface, &n_modes, nullptr);
    if (n_modes > 0) {
        this->vk_present_modes.reserve(n_modes);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_surface, &n_modes, this->vk_present_modes.wdata(n_modes));
    }
}

