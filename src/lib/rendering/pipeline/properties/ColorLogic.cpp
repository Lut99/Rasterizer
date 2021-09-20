/* COLOR LOGIC.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:38:56
 * Last edited:
 *   9/19/2021, 5:49:51 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorLogic class, which describes what the Pipeline
 *   should do with pixels in the target framebuffer - in general, so NOT
 *   per framebuffer.
**/

#include "ColorLogic.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** COLORLOGIC CLASS *****/
/* Default constructor for the ColorLogic class. */
ColorLogic::ColorLogic() :
    ColorLogic(VK_FALSE, VK_LOGIC_OP_MAX_ENUM, Tools::Array<Rendering::ColorBlending>())
{}

/* Constructor for the ColorLogic class, which takes whether or not to enable the logic operation, which operation to apply if it is and the list of per-framebuffer ColorBlending properties. Note that there has to be one ColorBlending struct per framebuffer, whether that framebuffer has it enabled or not. */
ColorLogic::ColorLogic(VkBool32 enabled, VkLogicOp logic_op, const Tools::Array<Rendering::ColorBlending>& color_blend_attachments) :
    enabled(enabled),
    logic_op(logic_op),
    color_blends(color_blend_attachments)
{}



/* Casts the internal list of ColorBlending objects to VkPipelineColorBlendAttachmentState structs. */
Tools::Array<VkPipelineColorBlendAttachmentState> ColorLogic::get_color_blends() const {
    // Loop and create the structs
    Tools::Array<VkPipelineColorBlendAttachmentState> result(this->color_blends.size());
    for (uint32_t i = 0; i < this->color_blends.size(); i++) {
        // Initialize the empty struct
        result.push_back({});

        // Set whether or not the operation is enabled
        result[i].blendEnable = this->color_blends[i].enabled;

        // Set the colour blending properties
        result[i].srcColorBlendFactor = this->color_blends[i].src_color_factor;
        result[i].dstColorBlendFactor = this->color_blends[i].dst_color_factor;
        result[i].colorBlendOp = this->color_blends[i].color_op;
        // Set what parts of the rgba channels to write to
        result[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        
        // Set the alpha blending properties
        result[i].srcAlphaBlendFactor = this->color_blends[i].src_alpha_factor;
        result[i].dstAlphaBlendFactor = this->color_blends[i].dst_alpha_factor;
        result[i].alphaBlendOp = this->color_blends[i].alpha_op;
    }

    // Done!
    return result;
}

/* Given a list of VkPipelineColorBlendAttachmentStates, returns a VkPipelineColorBlendStateCreateInfo based on that and internal properties. */
VkPipelineColorBlendStateCreateInfo ColorLogic::get_info(const Tools::Array<VkPipelineColorBlendAttachmentState>& vk_color_blends) const {
    // Initialize the struct to its default state
    VkPipelineColorBlendStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    // Describe the logic operations to do
    info.logicOpEnable = this->enabled;
    info.logicOp = this->logic_op;
    // Attach the per-framebuffer attachments
    info.attachmentCount = vk_color_blends.size();
    info.pAttachments = vk_color_blends.rdata();
    // Hardcode the constants, since I don't rlly know what they do lmao gottem
    info.blendConstants[0] = 0.0f;
    info.blendConstants[1] = 0.0f;
    info.blendConstants[2] = 0.0f;
    info.blendConstants[3] = 0.0f;

    // Done
    return info;
}
