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


/***** DEPTHTESTING CLASS *****/
/* Default constructor for the DepthTesting class. */
DepthTesting::DepthTesting() :
    DepthTesting(VK_FALSE, VK_COMPARE_OP_MAX_ENUM)
{}

/* Constructor for the DepthTesting class, which takes that it is enabled (true_type) and the compare operation for when a new fragment needs to be tested for depth. */
DepthTesting::DepthTesting(VkBool32 enabled, VkCompareOp compare_op) :
    enabled(enabled),
    compare_op(compare_op)
{}



/* Returns a VkPipelineDepthStencilStateCreateInfo populated with the internal properties. */
VkPipelineDepthStencilStateCreateInfo DepthTesting::get_info() const {
    // Initialize the struct to default
    VkPipelineDepthStencilStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Set whether to test and write (which we treat as the same)
    info.depthTestEnable = this->enabled;
    info.depthWriteEnable = this->enabled;

    // Define how to compare the values
    info.depthCompareOp = this->compare_op;

    // We can optionally specify to only keep elements within a certain bound; but we don't
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f;
    info.maxDepthBounds = 1.0f;

    // We can also do stencils tests(?)
    info.stencilTestEnable = VK_FALSE;
    info.front = {};
    info.back = {};

    // We're done here
    return info;
}
