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



/* Binds the pipeline to a given command buffer for future draw calls. */
void Pipeline::bind(const Rendering::CommandBuffer* cmd, VkPipelineBindPoint vk_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS) {

}

/* Schedules the given value as one of the push constants on the given command buffer. The shader stage and offset determine which push constant, while the data and data_size relate to the push constant's value. */
void Pipeline::schedule_push_constant(const Rendering::CommandBuffer* cmd, VkShaderStageFlags shader_stage, uint32_t offset, void* data, size_t data_size) {

}

/* Schedules a draw for this pipeline with the given number of vertices and the given number of instances. Optionally, an offset can be given in either arrays. */
void Pipeline::schedule_draw(const Rendering::CommandBuffer* cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex = 0, uint32_t first_instance = 0) {

}

/* Schedules an indexed draw for this pipeline with the given number of indices and the given number of instances. Optionally, an offset can be given in any of the three arrays. */
void Pipeline::schedule_idraw(const Rendering::CommandBuffer* cmd, uint32_t index_count, uint32_t instance_count, uint32_t first_vertex = 0, uint32_t first_index = 0, uint32_t first_instance = 0) {

}
