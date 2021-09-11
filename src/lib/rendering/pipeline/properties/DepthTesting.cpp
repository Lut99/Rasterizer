/* DEPTH TESTING.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 17:24:35
 * Last edited:
 *   11/09/2021, 17:24:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DepthTesting class, which describes the properties of the
 *   depthtesting part of a Pipeline.
**/

#include "DepthTesting.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineDepthStencilStateCreateInfo struct. */
static void populate_depth_stencil_state(VkPipelineDepthStencilStateCreateInfo& depth_stencil_state_info, VkBool32 vk_enable, VkCompareOp vk_compare_op) {
    // Set to default
    depth_stencil_state_info = {};
    depth_stencil_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Pass the arguments
    depth_stencil_state_info.depthTestEnable = vk_enable;
    depth_stencil_state_info.depthWriteEnable = vk_enable;

    // Define how to compare the values of the depth stencil
    depth_stencil_state_info.depthCompareOp = vk_compare_op;

    // We can optionally specify to only keep elements within a certain bound; but we don't
    depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state_info.minDepthBounds = 0.0f;
    depth_stencil_state_info.maxDepthBounds = 1.0f;

    // We can also do stencils tests(?)
    depth_stencil_state_info.stencilTestEnable = VK_FALSE;
    depth_stencil_state_info.front = {};
    depth_stencil_state_info.back = {};
}





/***** DEPTHTESTING CLASS *****/
/* Constructor for the DepthTesting class, which takes that it is enabled (true_type) and the compare operation for when a new fragment needs to be tested for depth. */
DepthTesting::DepthTesting(const std::true_type&, VkCompareOp compare_op) {
    // Populate the internal struct with a VK_TRUE
    populate_depth_stencil_state(this->vk_depth_stencil_state, VK_TRUE, compare_op);
}

/* Constructor for the DepthTesting class, which takes that it is disabled. */
DepthTesting::DepthTesting(const std::false_type&) {
    // Populate the internal struct with a VK_FALSE
    populate_depth_stencil_state(this->vk_depth_stencil_state, VK_FALSE, VK_COMPARE_OP_MAX_ENUM);
}
