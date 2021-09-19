/* VIEWPORT TRANSFORMATION.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 17:40:29
 * Last edited:
 *   11/09/2021, 17:40:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ViewportTransformation class, which is a collection of
 *   properties for a Pipeline that describe where the target framebuffer
 *   should be filled in exactly.
**/

#include "ViewportTransformation.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** VIEWPORTTRANSFORMATION CLASS *****/
/* Default constructor for the ViewportTransformation class. */
ViewportTransformation::ViewportTransformation() :
    ViewportTransformation({ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 })
{}

/* Constructor for the ViewportTransformation class, which takes the coordinates and the extent for the target viewport (which will stretch the result) and the target scissor (which will cutoff the result). */
ViewportTransformation::ViewportTransformation(VkOffset2D viewport_offset, VkExtent2D viewport_extent, VkOffset2D scissor_offset, VkExtent2D scissor_extent) :
    viewport({}),
    scissor({})
{
    // Set the viewport's properties first
    this->viewport.x = static_cast<float>(viewport_offset.x);
    this->viewport.y = static_cast<float>(viewport_offset.y);
    this->viewport.width = (float) viewport_extent.width;
    this->viewport.height = (float) viewport_extent.height;
    this->viewport.minDepth = 0.0f;
    this->viewport.maxDepth = 1.0f;

    // Then, set the scissor
    this->scissor.offset = scissor_offset;
    this->scissor.extent = scissor_extent;

    // Done
    printf("Initialized ViewportTransformation\n");
}



/* Returns a VkPipelineViewportStateCreateInfo struct populated with the internal properties. */
VkPipelineViewportStateCreateInfo ViewportTransformation::get_info() const {
    // Initialize to default
    VkPipelineViewportStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // Set the rectangle and the scissors
    info.viewportCount = 1;
    info.pViewports = &this->viewport;
    info.scissorCount = 1;
    info.pScissors = &this->scissor;

    // Done
    return info;
}
