/* SWAPCHAIN INFO.hpp
 *   by Lut99
 *
 * Created:
 *   05/06/2021, 15:17:11
 * Last edited:
 *   05/06/2021, 15:17:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SwapchainInfo class, which bundles and manages
 *   information about the swapchain to which we render.
**/

#ifndef RENDERING_SWAPCHAIN_INFO_HPP
#define RENDERING_SWAPCHAIN_INFO_HPP

#include <vulkan/vulkan.h>

#include "tools/Logger.hpp"
#include "tools/Array.hpp"

namespace Rasterizer::Rendering {
    /* The SwapchainInfo class, which describes the kind of swapchains the GPU supports. */
    class SwapchainInfo {
    private:
        /* Logger for the SwapchainInfo class. */
        Tools::Logger logger;
        /* The capabilities of this device for the given surface. */
        VkSurfaceCapabilitiesKHR vk_capabilities;
        /* The formats supported by this device for this surface. */
        Tools::Array<VkSurfaceFormatKHR> vk_formats;
        /* The present modes supported by this device for this surface. */
        Tools::Array<VkPresentModeKHR> vk_present_modes;

    public:
        /* "Default" constructor for the SwapchainInfo class, which initializes this to "nothing supported". */
        SwapchainInfo(const Tools::Logger::InitData& init_data);
        /* Constructor for the SwapchainInfo class, which takes init data for its logger, a VkPhysicalDevice and a VkSurfaceKHR to populate itself appropriately. */
        SwapchainInfo(const Tools::Logger::InitData& init_data, VkPhysicalDevice vk_physical_device, VkSurfaceKHR vk_surface);

        /* Returns the capabilities of this device as a constant reference. */
        inline const VkSurfaceCapabilitiesKHR& capabilities() const { return this->vk_capabilities; }
        /* Returns a constant reference to the list of surface formats. */
        inline const Tools::Array<VkSurfaceFormatKHR>& formats() const { return this->vk_formats; }
        /* Returns a constant reference to the list of present modes. */
        inline const Tools::Array<VkPresentModeKHR>& present_modes() const { return this->vk_present_modes; }

    };
}

#endif
