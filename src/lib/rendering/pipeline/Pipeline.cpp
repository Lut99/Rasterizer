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
/* Constructor for the Pipeline class, which takes a GPU where it lives, the VkPipeline to wrap and its matching layout. */
Pipeline::Pipeline(const Rendering::GPU& gpu, const VkPipeline& vk_pipeline, const VkPipelineLayout& vk_pipeline_layout) :
    gpu(gpu),

    vk_pipeline(vk_pipeline),
    vk_pipeline_layout(vk_pipeline_layout)
{}

/* Move constructor for the Pipeline class, which is deleted. */
Pipeline::Pipeline(Pipeline&& other) :
    gpu(other.gpu),

    vk_pipeline(other.vk_pipeline),
    vk_pipeline_layout(other.vk_pipeline_layout)
{
    other.vk_pipeline = nullptr;
    other.vk_pipeline_layout = nullptr;
}

/* Destructor for the Pipeline class. */
Pipeline::~Pipeline() {
    if (this->vk_pipeline != nullptr) {
        vkDestroyPipeline(this->gpu, this->vk_pipeline, nullptr);
    }
    if (this->vk_pipeline_layout != nullptr) {
        vkDestroyPipelineLayout(this->gpu, this->vk_pipeline_layout, nullptr);
    }
}



/* Swap operator for the Pipeline class. */
void Rendering::swap(Pipeline& p1, Pipeline& p2) {
    #ifndef NDEBUG
    if (p1.gpu != p2.gpu) { logger.fatalc(Pipeline::channel, "Could not swap pipelines with different GPUs"); }
    #endif

    using std::swap;

    swap(p1.vk_pipeline, p2.vk_pipeline);
    swap(p1.vk_pipeline_layout, p2.vk_pipeline_layout);
}
