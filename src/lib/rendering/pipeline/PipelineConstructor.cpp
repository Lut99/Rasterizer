/* PIPELINE CONSTRUCTOR.cpp
 *   by Lut99
 *
 * Created:
 *   18/09/2021, 11:41:08
 * Last edited:
 *   18/09/2021, 11:41:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "PipelineConstructor.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** PIPELINECONSTRUCTOR CLASS *****/
/* Constructor for the PipelineConstructor class, which takes a PipelineCache we use for creating its pipelines. */
PipelineConstructor::PipelineConstructor(const Rendering::PipelineCache& pipeline_cache) :
    pipeline_cache(pipeline_cache)
{}

/* Copy constructor for the PipelineConstructor class. */
PipelineConstructor::PipelineConstructor(const PipelineConstructor& other) :
    pipeline_cache(other.pipeline_cache),

    shaders(other.shaders),
    vertex_input_state(other.vertex_input_state),
    input_assembly_state(other.input_assembly_state),
    depth_testing(other.depth_testing),
    viewport_transformation(other.viewport_transformation),
    rasterization(other.rasterization),
    multisampling(other.multisampling),
    color_logic(other.color_logic),
    pipeline_layout(other.pipeline_layout)
{}

/* Move constructor for the PipelineConstructor class. */
PipelineConstructor::PipelineConstructor(PipelineConstructor&& other) :
    pipeline_cache(other.pipeline_cache),

    shaders(std::move(other.shaders)),
    vertex_input_state(std::move(other.vertex_input_state)),
    input_assembly_state(std::move(other.input_assembly_state)),
    depth_testing(std::move(other.depth_testing)),
    viewport_transformation(std::move(other.viewport_transformation)),
    rasterization(std::move(other.rasterization)),
    multisampling(std::move(other.multisampling)),
    color_logic(std::move(other.color_logic)),
    pipeline_layout(std::move(other.pipeline_layout))
{}

/* Destructor for the PipelineConstructor class. */
PipelineConstructor::~PipelineConstructor() {}



/* Swap operator for the PipelineConstructor class. */
void Rendering::swap(PipelineConstructor& pc1, PipelineConstructor& pc2) {
    #ifndef NDEBUG
    if (&pc1.pipeline_cache != &pc2.pipeline_cache) { logger.fatalc(PipelineConstructor::channel, "Cannot swap pipeline constructors with different pipeline caches"); }
    #endif

    using std::swap;

    swap(pc1.shaders, pc2.shaders);
}
