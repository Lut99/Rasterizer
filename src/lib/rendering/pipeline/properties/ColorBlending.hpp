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

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The ColorBlending class, which describes how a Pipeline should deal with pixels already present in the target framebuffer. Note that it only describes the action for a single framebuffer. */
    struct ColorBlending {
    public:
        /* The framebuffer for which this color blending holds. */
        uint32_t framebuffer;
        /* Whether or not color blending is enabled for this framebuffer. */
        VkBool32 enabled;

        /* How much weight the new color will have while blending. */
        VkBlendFactor src_color_factor;
        /* How much weight the existing color will have while blending. */
        VkBlendFactor dst_color_factor;
        /* How to blend the new and existing colours. */
        VkBlendOp color_op;

        /* How much weight the new alpha channel will have while blending. */
        VkBlendFactor src_alpha_factor;
        /* How much weight the existing alpha channel will have while blending. */
        VkBlendFactor dst_alpha_factor;
        /* How to blend the new and existing alpha channels. */
        VkBlendOp alpha_op;

    public:
        /* Constructor for the ColorBlending class, which takes multiple parameters for a single framebuffer. This overload enables colorblending for the given framebuffer.
         *
         * @param framebuffer The index of the framebuffer for which these settings hold
         * @param enabled Whether or not to enable color blending for this framebuffer. If disabled, just replaces the damn thing.
         * @param src_color_factor How much weight the new color will have while blending
         * @param dst_color_factor How much weight the existing color will have while blending
         * @param color_op How to combine the two colors into one
         * @param src_alpha_factor How much weight the new alpha value will have while blending
         * @param dst_alpha_factor How much weight the existing alpha value will have while blending
         * @param alpha_op How to combine the two alpha channels into one
         */
        ColorBlending(uint32_t framebuffer, VkBool32 enabled, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op) :
            framebuffer(framebuffer), enabled(enabled),
            src_color_factor(src_color_factor), dst_color_factor(dst_color_factor), color_op(color_op),
            src_alpha_factor(src_alpha_factor), dst_alpha_factor(dst_alpha_factor), alpha_op(alpha_op)
        {}

    };
}

#endif
