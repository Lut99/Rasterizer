/* VULKAN SWAP.hpp
 *   by Lut99
 *
 * Created:
 *   13/09/2021, 21:45:47
 * Last edited:
 *   13/09/2021, 21:45:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains swap functions for certain Vulkan structs.
**/

#ifndef RENDERING_VULKAN_SWAP_HPP
#define RENDERING_VULKAN_SWAP_HPP

#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* Swaps two VkOffset2D structs. */
    void swap(VkOffset2D& o1, VkOffset2D& o2);
    /* Swaps two VkExtent2D structs. */
    void swap(VkExtent2D& e1, VkExtent2D& e2);

    /* Swaps two VkViewport structs. */
    void swap(VkViewport& v1, VkViewport& v2);
    /* Swaps two VkRect2D structs. */
    void swap(VkRect2D& r1, VkRect2D& r2);

    /* Swaps two VkPipelineViewportStateCreateInfo structs. */
    void swap(VkPipelineViewportStateCreateInfo& pvs1, VkPipelineViewportStateCreateInfo& pvs2);

}

#endif
