/* COLOR LOGIC.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:38:56
 * Last edited:
 *   11/09/2021, 18:38:56
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorLogic class, which describes what the Pipeline
 *   should do with pixels in the target framebuffer - in general, so NOT
 *   per framebuffer.
**/

#include "tools/Array.hpp"
#include "ColorLogic.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineColorBlendStateCreateInfo struct. */
static void populate_color_blend_state(VkPipelineColorBlendStateCreateInfo& color_state_info, const Tools::Array<VkPipelineColorBlendAttachmentState>& color_attachments, VkBool32 enable_logic, VkLogicOp logic_op) {
    // Set to default
    color_state_info = {};
    color_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    // Describe the logic operations to do
    color_state_info.logicOpEnable = enable_logic;
    color_state_info.logicOp = logic_op;

    // Attach the per-framebuffer attachments
    color_state_info.attachmentCount = color_attachments.size();
    color_state_info.pAttachments = color_attachments.rdata();

    // Hardcode the constants, since I don't rlly know what they do lmao gottem
    color_state_info.blendConstants[0] = 0.0f;
    color_state_info.blendConstants[1] = 0.0f;
    color_state_info.blendConstants[2] = 0.0f;
    color_state_info.blendConstants[3] = 0.0f;
}





/***** COLORLOGIC CLASS *****/
/* Constructor for the ColorLogic class, which takes whether or not to enable the logic operation, which operation to apply if it is and the list of per-framebuffer ColorBlending properties. Note that there has to be one ColorBlending struct per framebuffer. */
ColorLogic::ColorLogic(VkBool32 enabled, VkLogicOp logic_op, const Tools::Array<Rendering::ColorBlending>& color_blend_attachments) :
    color_blend_attachments(color_blend_attachments)
{
    // First, 'cast' the ColorBlending structs to their Vulkan-wrapped objects
    this->vk_color_blend_attachments.reserve(this->color_blend_attachments.size());
    for (uint32_t i = 0; i < this->color_blend_attachments.size(); i++) {
        this->vk_color_blend_attachments.push_back(this->color_blend_attachments[i].info());
    }

    // Then populate the final state info struct
    populate_color_blend_state(this->vk_color_blend_state, this->vk_color_blend_attachments, enabled, logic_op);
}

/* Copy constructor for the ColorLogic class. */
ColorLogic::ColorLogic(const ColorLogic& other) :
    color_blend_attachments(other.color_blend_attachments)
{
    // First, 'cast' the ColorBlending structs to their Vulkan-wrapped objects
    this->vk_color_blend_attachments.reserve(this->color_blend_attachments.size());
    for (uint32_t i = 0; i < this->color_blend_attachments.size(); i++) {
        this->vk_color_blend_attachments.push_back(this->color_blend_attachments[i].info());
    }

    // Populate the internal struct manually
    populate_color_blend_state(this->vk_color_blend_state, this->vk_color_blend_attachments, other.vk_color_blend_state.logicOpEnable, other.vk_color_blend_state.logicOp);
}

/* Move constructor for the ColorLogic class. */
ColorLogic::ColorLogic(ColorLogic&& other) :
    color_blend_attachments(std::move(other.color_blend_attachments))
{
    // First, 'cast' the ColorBlending structs to their Vulkan-wrapped objects
    this->vk_color_blend_attachments.reserve(this->color_blend_attachments.size());
    for (uint32_t i = 0; i < this->color_blend_attachments.size(); i++) {
        this->vk_color_blend_attachments.push_back(this->color_blend_attachments[i].info());
    }

    // Populate the internal struct manually
    populate_color_blend_state(this->vk_color_blend_state, this->vk_color_blend_attachments, other.vk_color_blend_state.logicOpEnable, other.vk_color_blend_state.logicOp);
}

/* Destructor for the ColorLogic class. */
ColorLogic::~ColorLogic() {}



/* Swap operator for the ColorLogic class. */
void Rendering::swap(ColorLogic& cl1, ColorLogic& cl2) {
    using std::swap;

    swap(cl1.color_blend_attachments, cl2.color_blend_attachments);
    swap(cl1.vk_color_blend_attachments, cl2.vk_color_blend_attachments);

    swap(cl1.vk_color_blend_state, cl2.vk_color_blend_state);
}
