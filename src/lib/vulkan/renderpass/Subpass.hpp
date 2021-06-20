/* SUBPASS.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 15:23:55
 * Last edited:
 *   20/06/2021, 15:23:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Subpass class, which describes the buffer layouts for
 *   each subpass of a large renderpass.
**/

#ifndef VULKAN_SUBPASS_HPP
#define VULKAN_SUBPASS_HPP

#include <vulkan/vulkan.h>

namespace Rasterizer::Vulkan {
    /* The Subpass class, which describes the memory layout in a single subpass in a larger renderpass. */
    class Subpass {
    private:
        /* The VkSubpassDescription that does the actual description work. */
        VkSubpassDescription vk_subpass;

        

    };
}

#endif
