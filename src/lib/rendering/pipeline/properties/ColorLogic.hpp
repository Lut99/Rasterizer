/* COLOR LOGIC.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:38:53
 * Last edited:
 *   11/09/2021, 18:38:53
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorLogic class, which describes what the Pipeline
 *   should do with pixels in the target framebuffer(s).
**/

#ifndef RENDERING_COLOR_LOGIC_HPP
#define RENDERING_COLOR_LOGIC_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"

#include "ColorBlending.hpp"

namespace Makma3D::Rendering {
    /* The ColorLogic class, which describes what to do with pixels already in framebuffer(s). */
    class ColorLogic {
    public:
        /* Whether or not to enable the global logic blending. */
        VkBool32 enabled;
        /* How to blend the colors globally. */
        VkLogicOp logic_op;
        /* The list of ColorBlending struct which we used to describe the color blend state for each framebuffer. */
        Tools::Array<Rendering::ColorBlending> color_blends;

    public:
        /* Default constructor for the ColorLogic class. */
        ColorLogic();
        /* Constructor for the ColorLogic class, which takes whether or not to enable the logic operation, which operation to apply if it is and the list of per-framebuffer ColorBlending properties. Note that there has to be one ColorBlending struct per framebuffer, whether that framebuffer has it enabled or not. */
        ColorLogic(VkBool32 enabled, VkLogicOp logic_op, const Tools::Array<Rendering::ColorBlending>& color_blend_attachments);

        /* Casts the internal list of ColorBlending objects to VkPipelineColorBlendAttachmentState structs. */
        Tools::Array<VkPipelineColorBlendAttachmentState> get_color_blends() const;
        /* Given a list of VkPipelineColorBlendAttachmentStates, returns a VkPipelineColorBlendStateCreateInfo based on that and internal properties. */
        VkPipelineColorBlendStateCreateInfo get_info(const Tools::Array<VkPipelineColorBlendAttachmentState>& vk_color_blends) const;

    };
}

#endif
