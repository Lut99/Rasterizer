/* COLOR BLEND STATE.hpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 14:16:24
 * Last edited:
 *   29/06/2021, 14:16:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorBlendState class, which manages the ColorBlendState
 *   attachment and associated memory in the Pipeline class.
**/

#ifndef VULKAN_COLOR_BLEND_STATE_HPP
#define VULKAN_COLOR_BLEND_STATE_HPP

#include <vulkan/vulkan.h>

namespace Rasterizer::Vulkan {
    /* The ColorBlendState class, which manages an associated VkPipelineColorBlendStateCreateInfo object. */
    class ColorBlendState {
    private:
        /* The VkPipelineColorBlendStateCreateInfo object we wrap. */
        VkPipelineColorBlendStateCreateInfo vk_color_blend_state;

        /* List of the per-attachment color blend states. */
        VkPipelineColorBlendAttachmentState* attachments;
        /* Count of the per-attachment color blend states. */
        uint32_t n_attachments;

    public:
        /* Constructor for the ColorBlendState class, which takes 

    };
}

#endif
