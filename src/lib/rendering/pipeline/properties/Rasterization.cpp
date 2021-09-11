/* RASTERIZATION.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:05:09
 * Last edited:
 *   11/09/2021, 18:05:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Rasterization class, which is a collection of properties for
 *   how the Pipeline will rasterizer the vertices.
**/

#include "Rasterization.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineRasterizationStateCreateInfo struct. */
static void populate_rasterizer_state(VkPipelineRasterizationStateCreateInfo& rasterizer_state_info, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp, VkPolygonMode polygon_mode, float line_width, VkBool32 disable_rasterizer) {
    // Set the struct to default
    rasterizer_state_info = {};
    rasterizer_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // Set the cullmode
    rasterizer_state_info.cullMode = cull_mode;
    rasterizer_state_info.frontFace = front_face;

    // Set whether or not to clamp distant objects
    rasterizer_state_info.depthClampEnable = depth_clamp;
    
    // Set the polygon mode
    rasterizer_state_info.polygonMode = polygon_mode;

    // Set the line width
    rasterizer_state_info.lineWidth = line_width;

    // Set the depth bias, which we (for now) hardcode to always be false
    rasterizer_state_info.depthBiasEnable = VK_FALSE;
    rasterizer_state_info.depthBiasConstantFactor = 0.0f;
    rasterizer_state_info.depthBiasClamp = 0.0f;
    rasterizer_state_info.depthBiasSlopeFactor = 0.0f;

    // Set the rasterizer enabled status
    rasterizer_state_info.rasterizerDiscardEnable = disable_rasterizer;
}





/***** RASTERIZATION CLASS *****/
/* Constructor for the Rasterization class, which takes the rasterizer-specific properties. This overload enables the rasterizer output.
 *
 * @param cull_mode Describes how to cull faces that are behind others
 * @param front_face Describes what the front face of an object is (i.e., which side to cull)
 * @param depth_clamp Describes whether to clamp objects that are too far or too near to their plane instead of simply not rendering them. Needs a special GPU feature to enable.
 * @param polygon_mode Describes how to "colour" the given geometry. For example, can be set to fill the whole vertex, or only its lines, etc. Needs a special GPU feature for anything other than FILL.
 * @param line_width The width of the lines that the rasterizer draws. Needs a special GPU feature to grow beyond 1.0f.
 */
Rasterization::Rasterization(const std::true_type&, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp, VkPolygonMode polygon_mode, float line_width) {
    // Simply populate the struct, done
    populate_rasterizer_state(this->vk_rasterizer_state, cull_mode, front_face, depth_clamp, polygon_mode, line_width, VK_FALSE);
}

/* Constructor for the Rasterization class, which takes the rasterizer-specific properties. This overload disables the rasterizer output, making other arguments unnecessary. */
Rasterization::Rasterization(const std::false_type&) {
    // Populate the struct with a lot of default values
    populate_rasterizer_state(this->vk_rasterizer_state, VK_CULL_MODE_FRONT_AND_BACK, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, VK_POLYGON_MODE_FILL, 1.0f, VK_TRUE);
}
