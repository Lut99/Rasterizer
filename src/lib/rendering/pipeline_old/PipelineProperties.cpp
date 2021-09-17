/* PIPELINE PROPERTIES.cpp
 *   by Lut99
 *
 * Created:
 *   13/09/2021, 22:16:16
 * Last edited:
 *   13/09/2021, 22:16:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that bundles all pipeline properties in one place.
**/

#include "PipelineProperties.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** PIPELINEPROPERTIES CLASS *****/
/* Constructor for the PipelineProperties class, which simply takes all properties in order and copies them.
 *
 * @param shader_stages A list of ShaderStages to initialize, each containing custom shader code
 * @param vertex_input_state The description of where the vertex buffers are bound and how they are laid out
 * @param input_assembly_state The description of what to do with the input vertices
 * @param depth_testing Whether to do depth testing or not and, if so, how so
 * @param viewport_transformation How the resulting viewport is sized/cutoff
 * @param rasterization What to do during the rasterization stage
 * @param multisampling How the pipeline should deal with multisampling
 * @param color_logic How to deal with pixels already present in the target framebuffer(s)
 * @param pipeline_layout How the data in the pipeline looks like, in terms of descriptors and push constants
 */
PipelineProperties::PipelineProperties(const Tools::Array<Rendering::ShaderStage>& shader_stages,
                                       const Rendering::VertexInputState& vertex_input_state,
                                       const Rendering::InputAssemblyState& input_assembly_state,
                                       const Rendering::DepthTesting& depth_testing,
                                       const Rendering::ViewportTransformation& viewport_transformation,
                                       const Rendering::Rasterization& rasterization,
                                       const Rendering::Multisampling& multisampling,
                                       const Rendering::ColorLogic& color_logic,
                                       const Rendering::PipelineLayout& pipeline_layout) :
    shader_stages(shader_stages),
    vertex_input_state(vertex_input_state),
    input_assembly_state(input_assembly_state),
    depth_testing(depth_testing),
    viewport_transformation(viewport_transformation),
    rasterization(rasterization),
    multisampling(multisampling),
    color_logic(color_logic),
    pipeline_layout(pipeline_layout)
{}

/* Constructor for the PipelineProperties class, which simply takes all properties in order and moves them.
 *
 * @param shader_stages A list of ShaderStages to initialize, each containing custom shader code
 * @param vertex_input_state The description of where the vertex buffers are bound and how they are laid out
 * @param input_assembly_state The description of what to do with the input vertices
 * @param depth_testing Whether to do depth testing or not and, if so, how so
 * @param viewport_transformation How the resulting viewport is sized/cutoff
 * @param rasterization What to do during the rasterization stage
 * @param multisampling How the pipeline should deal with multisampling
 * @param color_logic How to deal with pixels already present in the target framebuffer(s)
 * @param pipeline_layout How the data in the pipeline looks like, in terms of descriptors and push constants
 */
PipelineProperties::PipelineProperties(Tools::Array<Rendering::ShaderStage>&& shader_stages,
                                       Rendering::VertexInputState&& vertex_input_state,
                                       Rendering::InputAssemblyState&& input_assembly_state,
                                       Rendering::DepthTesting&& depth_testing,
                                       Rendering::ViewportTransformation&& viewport_transformation,
                                       Rendering::Rasterization&& rasterization,
                                       Rendering::Multisampling&& multisampling,
                                       Rendering::ColorLogic&& color_logic,
                                       Rendering::PipelineLayout&& pipeline_layout) :
    shader_stages(std::move(shader_stages)),
    vertex_input_state(std::move(vertex_input_state)),
    input_assembly_state(std::move(input_assembly_state)),
    depth_testing(std::move(depth_testing)),
    viewport_transformation(std::move(viewport_transformation)),
    rasterization(std::move(rasterization)),
    multisampling(std::move(multisampling)),
    color_logic(std::move(color_logic)),
    pipeline_layout(std::move(pipeline_layout))
{}

/* Copy constructor for the PipelineProperties class. */
PipelineProperties::PipelineProperties(const PipelineProperties& other) :
    shader_stages(other.shader_stages),
    vertex_input_state(other.vertex_input_state),
    input_assembly_state(other.input_assembly_state),
    depth_testing(other.depth_testing),
    viewport_transformation(other.viewport_transformation),
    rasterization(other.rasterization),
    multisampling(other.multisampling),
    color_logic(other.color_logic),
    pipeline_layout(other.pipeline_layout)
{}

/* Move constructor for the PipelineProperties class. */
PipelineProperties::PipelineProperties(PipelineProperties&& other) :
    shader_stages(std::move(other.shader_stages)),
    vertex_input_state(std::move(other.vertex_input_state)),
    input_assembly_state(std::move(other.input_assembly_state)),
    depth_testing(std::move(other.depth_testing)),
    viewport_transformation(std::move(other.viewport_transformation)),
    rasterization(std::move(other.rasterization)),
    multisampling(std::move(other.multisampling)),
    color_logic(std::move(other.color_logic)),
    pipeline_layout(std::move(other.pipeline_layout))
{}

/* Destructor for the PipelineProperties class. */
PipelineProperties::~PipelineProperties() {}



/* Swap operator for the PipelineProperties class */
void Rendering::swap(PipelineProperties& pp1, PipelineProperties& pp2) {
    using std::swap;

    swap(pp1.shader_stages, pp2.shader_stages);
    swap(pp1.vertex_input_state, pp2.vertex_input_state);
    swap(pp1.input_assembly_state, pp2.input_assembly_state);
    swap(pp1.depth_testing, pp2.depth_testing);
    swap(pp1.viewport_transformation, pp2.viewport_transformation);
    swap(pp1.rasterization, pp2.rasterization);
    swap(pp1.multisampling, pp2.multisampling);
    swap(pp1.color_logic, pp2.color_logic);
    swap(pp1.pipeline_layout, pp2.pipeline_layout);
}
