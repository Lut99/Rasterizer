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


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkViewport struct. */
static void populate_viewport(VkViewport& vk_viewport, const VkOffset2D& vk_offset, const VkExtent2D& vk_extent) {
    // Set the default
    vk_viewport = {};

    // Set the offset
    vk_viewport.x = (float) vk_offset.x;
    vk_viewport.y = (float) vk_offset.y;

    // Set the extent dimensions
    vk_viewport.width = (float) vk_extent.width;
    vk_viewport.height = (float) vk_extent.height;

    // Set the depth (hardcoded)
    vk_viewport.minDepth = 0.0f;
    vk_viewport.maxDepth = 1.0f;
}

/* Populates the given VkRect2D struct using an offset and an extent. */
static void populate_rect(VkRect2D& vk_rect, const VkOffset2D& vk_offset, const VkExtent2D& vk_extent) {
    // Set to default
    vk_rect = {};

    // Set the offset & extent directly
    vk_rect.offset = vk_offset;
    vk_rect.extent = vk_extent;
}

/* Populates the given VkPipelineViewportStateCreateInfo struct. */
static void populate_viewport_state(VkPipelineViewportStateCreateInfo& viewport_state_info, const VkViewport& vk_viewport, const VkRect2D& vk_scissor) {
    // Set to default
    viewport_state_info = {};
    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // Set the rectangle and scissors
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = &vk_viewport;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = &vk_scissor;
}





/***** VIEWPORTTRANSFORMATION CLASS *****/
/* Constructor for the ViewportTransformation class, which takes the coordinates and the extent for the target viewport (which will stretch the result) and the target scissor (which will cutoff the result). */
ViewportTransformation::ViewportTransformation(VkOffset2D viewport_offset, VkExtent2D viewport_extent, VkOffset2D scissor_offset, VkExtent2D scissor_extent) {
    // Create the viewport definition
    populate_viewport(this->vk_viewport, viewport_offset, viewport_extent);
    // Create the scissor definition
    populate_rect(this->vk_scissor, scissor_offset, scissor_extent);

    // Use those to populate the viewport state
    populate_viewport_state(this->vk_viewport_state, this->vk_viewport, this->vk_scissor);
}

/* Copy constructor for the ViewportTransformation class. */
ViewportTransformation::ViewportTransformation(const ViewportTransformation& other) :
    vk_viewport(other.vk_viewport),
    vk_scissor(other.vk_scissor)
{
    // Re-initialize the viewport state itself
    populate_viewport_state(this->vk_viewport_state, this->vk_viewport, this->vk_scissor);
}

/* Move constructor for the ViewportTransformation class. */
ViewportTransformation::ViewportTransformation(ViewportTransformation&& other) :
    vk_viewport(other.vk_viewport),
    vk_scissor(other.vk_scissor)
{
    // Re-initialize the viewport state itself
    populate_viewport_state(this->vk_viewport_state, this->vk_viewport, this->vk_scissor);
}

/* Destructor for the ViewportTransformation class. */
ViewportTransformation::~ViewportTransformation() {}



/* Swap operator for the ViewportTransformation class. */
void Rendering::swap(ViewportTransformation& vt1, ViewportTransformation& vt2) {
    using std::swap;

    swap(vt1.vk_viewport, vt2.vk_viewport);
    swap(vt1.vk_scissor, vt2.vk_scissor);

    swap(vt1.vk_viewport_state, vt2.vk_viewport_state);
}
