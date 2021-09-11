/* COLOR BLENDING.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:24:51
 * Last edited:
 *   11/09/2021, 18:24:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorBlending class, which describes how a pipeline
 *   should blend the new pixels with the pixels already present in the
 *   target framebuffer. Note that this is per framebuffer, so likely multiple
 *   of these bad boys will be needed.
**/

#include "ColorBlending.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineColorBlendAttachmentState struct. */
static void populate_color_blend_attachment_state(VkPipelineColorBlendAttachmentState& color_attachment_state, VkBool32 enable_blending, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op) {
    // Set to default
    color_attachment_state = {};

    // Set whether or not it's enabled
    color_attachment_state.blendEnable = enable_blending;

    // Set the write mask, which we hardcode to everything always
    color_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    // Set the color blending
    color_attachment_state.srcColorBlendFactor = src_color_factor;
    color_attachment_state.dstColorBlendFactor = dst_color_factor;
    color_attachment_state.colorBlendOp = color_op;

    // Set the alpha blending
    color_attachment_state.srcAlphaBlendFactor = src_alpha_factor;
    color_attachment_state.dstAlphaBlendFactor = dst_alpha_factor;
    color_attachment_state.alphaBlendOp = alpha_op;
}





/***** COLORBLENDING CLASS *****/
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
ColorBlending::ColorBlending(uint32_t framebuffer, const std::true_type&, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op) {
    // Populate the internal struct with the given values
    populate_color_blend_attachment_state(this->vk_color_blend_attachment_state, VK_TRUE, src_color_factor, dst_color_factor, color_op, src_alpha_factor, dst_alpha_factor, alpha_op);
}

/* Constructor for the ColorBlending class, which takes multiple parameters for a single framebuffer. This overload disables colorblending for the given framebuffer, meaning we don't need to know anything else. */
ColorBlending::ColorBlending(uint32_t framebuffer, const std::false_type&) {
    // Populate the internal struct with the default values for OFF
    populate_color_blend_attachment_state(this->vk_color_blend_attachment_state, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
}
