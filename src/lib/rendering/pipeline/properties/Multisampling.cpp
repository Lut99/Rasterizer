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

#include <cstdio>
#include "Multisampling.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** MULTISAMPLING CLASS *****/
// /* Default constructor for the Multisampling class. */
// Multisampling::Multisampling() :
//     Multisampling()
// {}

/* Constructor for the Multisampling class, which takes nothing as we don't do that for now. */
Multisampling::Multisampling() {
    printf("Initialized Multisampling\n");
}



/* Returns a VkPipelineMultisamplingStateCreateInfo struct based on the internal properties. */
VkPipelineMultisampleStateCreateInfo Multisampling::get_info() const {
    // INitialize to the default struct
    VkPipelineMultisampleStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    // For now, hardcode the multisampling to be off
    info.sampleShadingEnable = VK_FALSE;
    info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.minSampleShading = 1.0f;
    info.pSampleMask = nullptr;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;

    // Done
    return info;
}
