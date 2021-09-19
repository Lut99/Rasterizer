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

#include <cstdio>
#include "Rasterization.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** RASTERIZATION CLASS *****/
/* Default constructor for the Rasterization class. */
Rasterization::Rasterization() :
    Rasterization(VK_TRUE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE)
{}

/* Constructor for the Rasterization class, which takes the rasterizer-specific properties.
 *
 * @param enabled Whether to enable the rasterization stage or not. Technically discards the result if disabled, so the other properties still have to be valid.
 * @param cull_mode Describes how to cull faces that are behind others.
 * @param front_face Describes what the front face of an object is (i.e., which side to cull).
 * @param depth_clamp Describes whether to clamp objects that are too far or too near to their plane instead of simply not rendering them. Needs a special GPU feature to enable.
 * @param polygon_mode Describes how to "colour" the given geometry. For example, can be set to fill the whole vertex, or only its lines, etc. Needs a special GPU feature for anything other than FILL.
 * @param line_width The width of the lines that the rasterizer draws. Needs a special GPU feature to grow beyond 1.0f.
 */
Rasterization::Rasterization(VkBool32 enabled, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp, VkPolygonMode polygon_mode, float line_width) :
    enabled(enabled),
    cull_mode(cull_mode),
    front_face(front_face),
    depth_clamp(depth_clamp),
    polygon_mode(polygon_mode),
    line_width(line_width)
{
    printf("Initialized Rasterization\n");
}



/* Creates a VkPipelineRasterizationStateCreateInfo struct from the internal properties and returns it. */
VkPipelineRasterizationStateCreateInfo Rasterization::get_info() const {
    // Initialize the VkPipelineRasterizationStateCreateInfo struct to default
    VkPipelineRasterizationStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // Set the cullmode
    info.cullMode = this->cull_mode;
    info.frontFace = this->front_face;
    // Set whether or not to clamp distant objects
    info.depthClampEnable = this->depth_clamp;
    // Set the polygon mode
    info.polygonMode = this->polygon_mode;
    // Set the line width
    info.lineWidth = this->line_width;
    // Set the depth bias, which we (for now) hardcode to always be false
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0.0f;
    info.depthBiasClamp = 0.0f;
    info.depthBiasSlopeFactor = 0.0f;
    // Set the rasterizer enabled status
    info.rasterizerDiscardEnable = !this->enabled;

    // Done
    return info;
}
