/* IMAGE LAYOUTS.hpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 15:52:18
 * Last edited:
 *   9/8/2021, 3:08:57 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains mappings from vulkan VkImageLayout to readable names.
**/

#ifndef RENDERING_IMAGE_LAYOUTS_HPP
#define RENDERING_IMAGE_LAYOUTS_HPP

#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>

/* Simple macro that sets the given enum to its string representation. */
#define MAP_VK_IMAGE_LAYOUT(FORMAT) \
    { (FORMAT), (#FORMAT) }

namespace Makma3D::Rendering {
    /* Static map of VkFormats to their respective string representations. */
    static std::unordered_map<VkImageLayout, std::string> vk_image_layout_map({
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_UNDEFINED),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_GENERAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_PREINITIALIZED),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        #ifdef VK_ENABLE_BETA_EXTENSIONS
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR),
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR),
        #endif
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR),
        #ifdef VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR),
        #endif
        MAP_VK_IMAGE_LAYOUT(VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT)
    });
}

#endif
