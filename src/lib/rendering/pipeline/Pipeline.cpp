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
/* Constructor for the Pipeline class, which takes its properties by a series of structs.
 *
 * @param gpu The GPU on which the pipeline should be created
 * @param shader_stages A list of ShaderStages to initialize, each containing custom shader code
 * @param input_assembly_state The description of what to do with the input vertices
 * @param depth_testing Whether to do depth testing or not and, if so, how so
 * @param viewport_transformation How the resulting viewport is sized/cutoff
 * @param rasterization What to do during the rasterization stage
 * @param multisampling How the pipeline should deal with multisampling
 * @param color_logic How to deal with pixels already present in the target framebuffer(s)
 */
Pipeline::Pipeline(const Rendering::GPU& gpu,
                   const Tools::Array<Rendering::ShaderStage>& shader_stages,
                   const Rendering::InputAssemblyState& input_assembly_state,
                   const Rendering::DepthTesting& depth_testing,
                   const Rendering::ViewportTransformation& viewport_transformation,
                   const Rendering::Rasterization& rasterization,
                   const Rendering::Multisampling& multisampling,
                   const Rendering::ColorLogic& color_logic) :
    gpu(gpu)
{}

/* Destructor for the Pipeline class. */
Pipeline::~Pipeline() {}
