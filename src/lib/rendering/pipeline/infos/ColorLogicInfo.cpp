/* COLOR LOGIC INFO.cpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 15:06:07
 * Last edited:
 *   19/09/2021, 15:06:07
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorLogicInfo, which bundles a
 *   VkPipelineColorBlendStateCreateInfo struct and the structs it needs to
 *   exist in one place.
**/

#include "ColorLogicInfo.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineColorBlendStateCreateInfo struct based on the given enabled status, logic op and color blend states. */
static void populate_color_blend_state_info(VkPipelineColorBlendStateCreateInfo& color_blend_state_info, VkBool32 vk_logic_enabled, VkLogicOp vk_logic_op, const VkPipelineColorBlendAttachmentState* vk_color_blend_states, uint32_t vk_color_blend_states_size) {
    // Set to default
    color_blend_state_info = {};
    color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    // Describe the logic operations to do
    color_blend_state_info.logicOpEnable = vk_logic_enabled;
    color_blend_state_info.logicOp = vk_logic_op;
    // Attach the per-framebuffer attachments
    color_blend_state_info.attachmentCount = vk_color_blend_states_size;
    color_blend_state_info.pAttachments = vk_color_blend_states;
    // Hardcode the constants, since I don't rlly know what they do lmao gottem
    color_blend_state_info.blendConstants[0] = 0.0f;
    color_blend_state_info.blendConstants[1] = 0.0f;
    color_blend_state_info.blendConstants[2] = 0.0f;
    color_blend_state_info.blendConstants[3] = 0.0f;
}





/***** COLORLOGICINFO CLASS *****/
/* Constructor for the ColorLogicInfo class, which takes a normal ColorLogic object to initialize itself with. */
ColorLogicInfo::ColorLogicInfo(const Rendering::ColorLogic& color_logic) {
    // Allocate the list of color blends first
    this->vk_color_blend_states_size = color_logic.color_blends.size();
    this->vk_color_blend_states = new VkPipelineColorBlendAttachmentState[this->vk_color_blend_states_size];

    // Populate the array
    for (uint32_t i = 0; i < color_logic.color_blends.size(); i++) {
        // Define the index for the local array as the framebuffer index
        uint32_t fi = color_logic.color_blends[i].framebuffer;

        // Set the struct to default values
        this->vk_color_blend_states[fi] = {};

        // Populate the struct's enabled state
        this->vk_color_blend_states[fi].blendEnable = color_logic.color_blends[i].enabled;

        // Populate the struct for the color blending
        this->vk_color_blend_states[fi].srcColorBlendFactor = color_logic.color_blends[i].src_color_factor;
        this->vk_color_blend_states[fi].dstColorBlendFactor = color_logic.color_blends[i].dst_color_factor;
        this->vk_color_blend_states[fi].colorBlendOp = color_logic.color_blends[i].color_op;
        
        // Populate the struct for the alpha blending
        this->vk_color_blend_states[fi].srcAlphaBlendFactor = color_logic.color_blends[i].src_alpha_factor;
        this->vk_color_blend_states[fi].dstAlphaBlendFactor = color_logic.color_blends[i].dst_alpha_factor;
        this->vk_color_blend_states[fi].alphaBlendOp = color_logic.color_blends[i].alpha_op;

        // Set the color mask too, to all channels
        this->vk_color_blend_states[fi].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }

    // With the array populated, populate the internal struct
    populate_color_blend_state_info(this->vk_color_blend_state_info, color_logic.enabled, color_logic.logic_op, this->vk_color_blend_states, this->vk_color_blend_states_size);
}

/* Copy constructor for the ColorLogicInfo class. */
ColorLogicInfo::ColorLogicInfo(const ColorLogicInfo& other) :
    vk_color_blend_states_size(other.vk_color_blend_states_size),
    vk_color_blend_state_info(other.vk_color_blend_state_info)
{
    // Copy the color blends list first
    this->vk_color_blend_states = new VkPipelineColorBlendAttachmentState[this->vk_color_blend_states_size];
    memcpy(this->vk_color_blend_states, other.vk_color_blend_states, this->vk_color_blend_states_size * sizeof(VkPipelineColorBlendAttachmentState));

    // Update the pointers in the main struct
    this->vk_color_blend_state_info.pAttachments = this->vk_color_blend_states;
}

/* Move constructor for the ColorLogicInfo class. */
ColorLogicInfo::ColorLogicInfo(ColorLogicInfo&& other) :
    vk_color_blend_states(other.vk_color_blend_states),
    vk_color_blend_states_size(other.vk_color_blend_states_size),

    vk_color_blend_state_info(other.vk_color_blend_state_info)
{
    // Prevent deallocation
    other.vk_color_blend_states = nullptr;
}

/* Destructor for the ColorLogicInfo class. */
ColorLogicInfo::~ColorLogicInfo() {
    if (this->vk_color_blend_states != nullptr) {
        delete[] this->vk_color_blend_states;
    }
}



/* Swap operator for the ColorLogicInfo class. */
void Rendering::swap(ColorLogicInfo& cli1, ColorLogicInfo& cli2) {
    using std::swap;

    swap(cli1.vk_color_blend_states, cli2.vk_color_blend_states);
    swap(cli1.vk_color_blend_states_size, cli2.vk_color_blend_states_size);

    swap(cli1.vk_color_blend_state_info, cli2.vk_color_blend_state_info);
}
