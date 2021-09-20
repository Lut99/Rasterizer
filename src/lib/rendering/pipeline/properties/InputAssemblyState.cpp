/* INPUT ASSEMBLY STATE.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 17:19:33
 * Last edited:
 *   11/09/2021, 17:19:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the InputAssemblyState class, which is a group of properties of
 *   a Pipeline relating to how the vertices are treated.
**/

#include <cstdio>
#include "InputAssemblyState.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** INPUTASSEMBLYSTATE CLASS *****/
/* Default constructor for the InputAssemblyState class. */
InputAssemblyState::InputAssemblyState() :
    InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE)
{}

/* Constructor for the InputAssemblyState class, which takes the topology of the vertices given (i.e., points, lines, triangles, etc) and whether or not a special vertex index is defined that 'restarts' drawing a primitive, i.e., quits drawing a triangle prematurely. Note that any topology other than triangles probably requires GPU extensions. */
InputAssemblyState::InputAssemblyState(VkPrimitiveTopology topology, VkBool32 restart_enable) :
    topology(topology),
    restart_enable(restart_enable)
{}



/* Returns a VkPipelineInputAssemblyStateCreateInfo struct containing the settings in this InputAssemblyState. */
VkPipelineInputAssemblyStateCreateInfo InputAssemblyState::get_info() const {
    // Initialize the struct to default
    VkPipelineInputAssemblyStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // Set the topology and whether or not to enable the restart
    info.topology = this->topology;
    info.primitiveRestartEnable = this->restart_enable;

    // Done
    return info;
}
