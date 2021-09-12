/* PIPELINE.cpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 12:29:37
 * Last edited:
 *   8/1/2021, 3:48:45 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Implements our warpper around the pipeline that is used to define the
 *   rendering process.
**/

#include "tools/Logger.hpp"

#include "Pipeline.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** PIPELINE CLASS *****/
/* Constructor for the Pipeline class, which takes a GPU where it lives, the VkPipeline to wrap and its properties for copying purposes. */
Pipeline::Pipeline(const Rendering::GPU& gpu, const VkPipeline& vk_pipeline, Rendering::PipelineProperties&& properties) :
    gpu(gpu),

    vk_pipeline(vk_pipeline),
    properties(properties)
{}

/* Destructor for the Pipeline class. */
Pipeline::~Pipeline() {}
