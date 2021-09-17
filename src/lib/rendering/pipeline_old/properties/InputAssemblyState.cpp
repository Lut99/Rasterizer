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

#include "InputAssemblyState.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/* Populates the given VkPipelineInputAssemblyStateCreateInfo struct. */
static void populate_input_assembly_state(VkPipelineInputAssemblyStateCreateInfo& assembly_state_info, VkPrimitiveTopology topology, VkBool32 primitive_restart) {
    // Set to default
    assembly_state_info = {};
    assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // Pass the arguments
    assembly_state_info.topology = topology;
    assembly_state_info.primitiveRestartEnable = primitive_restart;
}





/***** INPUTASSEMBLYSTATE CLASS *****/
/* Constructor for the InputAssemblyState class, which takes the topology of the vertices given (i.e., points, lines, triangles, etc) and whether or not a special vertex index is defined that 'restarts' drawing a primitive, i.e., quits drawing a triangle prematurely. Note that any topology other than triangles probably requires GPU extensions. */
InputAssemblyState::InputAssemblyState(VkPrimitiveTopology topology, VkBool32 restart_enable) {
    // Simply populate the struct and we're done
    populate_input_assembly_state(this->vk_input_assembly_state, topology, restart_enable);
}
