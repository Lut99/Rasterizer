/* MULTISAMPLING.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:19:06
 * Last edited:
 *   11/09/2021, 18:19:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Multisampling class, which describes how a Pipeline
 *   should deal with multisampling.
**/

#include "Multisampling.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populate sthe given VkPipelineMultisampleStateCreateInfo struct. */
static void populate_multisample_state(VkPipelineMultisampleStateCreateInfo& multisample_state_info) {
    // Define the standard stuff
    multisample_state_info = {};
    multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    // For now, hardcode the multisampling to be off
    multisample_state_info.sampleShadingEnable = VK_FALSE;
    multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state_info.minSampleShading = 1.0f;
    multisample_state_info.pSampleMask = nullptr;
    multisample_state_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_info.alphaToOneEnable = VK_FALSE;
}





/***** MULTISAMPLING CLASS *****/
/* Constructor for the Multisampling class, which takes nothing as we don't do that for now. */
Multisampling::Multisampling() {
    // Simply populate with default, disabling values
    populate_multisample_state(this->vk_multisample_state);
}
