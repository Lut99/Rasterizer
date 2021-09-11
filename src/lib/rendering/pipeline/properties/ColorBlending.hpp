/* COLOR BLENDING.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:24:20
 * Last edited:
 *   11/09/2021, 18:24:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorBlending class, which describes how a pipeline
 *   should blend the new pixels with the pixels already present in the
 *   target framebuffer. Note that this is per framebuffer, so likely multiple
 *   of these bad boys will be needed.
**/

#ifndef RENDERING_COLOR_BLENDING_HPP
#define RENDERING_COLOR_BLENDING_HPP

#include <type_traits>
#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The ColorBlending class, which describes how a Pipeline should deal with pixels already present in the target framebuffer. Note that it only describes the action for a single framebuffer. */
    class ColorBlending {
    private:
        /* The internal VkPipelineColorBlendAttachmentState struct we wrap. */
        VkPipelineColorBlendAttachmentState vk_color_blend_attachment_state;
    
    public:
        /* Constructor for the ColorBlending class, which takes multiple parameters for a single framebuffer. This overload enables colorblending for the given framebuffer.
         *
         * @param framebuffer The index of the framebuffer for which these settings hold
         * @param src_color_factor How much weight the new color will have while blending
         * @param dst_color_factor How much weight the existing color will have while blending
         * @param color_op How to combine the two colors into one
         * @param src_alpha_factor How much weight the new alpha value will have while blending
         * @param dst_alpha_factor How much weight the existing alpha value will have while blending
         * @param alpha_op How to combine the two alpha channels into one
         */
        ColorBlending(uint32_t framebuffer, const std::true_type&, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op);
        /* Constructor for the ColorBlending class, which takes multiple parameters for a single framebuffer. This overload disables colorblending for the given framebuffer, meaning we don't need to know anything else. */
        ColorBlending(uint32_t framebuffer, const std::false_type&);

        /* Explicitly returns the internal VkPipelineColorBlendAttachmentState struct. */
        inline const VkPipelineColorBlendAttachmentState& info() const { return this->vk_color_blend_attachment_state; }
        /* Implicitly returns the internal VkPipelineColorBlendAttachmentState struct. */
        inline operator const VkPipelineColorBlendAttachmentState&() const { return this->vk_color_blend_attachment_state; }

    };
}

#endif
