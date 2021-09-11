/* PIPELINE.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 12:29:41
 * Last edited:
 *   28/06/2021, 22:26:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Implements our warpper around the pipeline that is used to define the
 *   rendering process.
**/

#ifndef RENDERING_PIPELINE_HPP
#define RENDERING_PIPELINE_HPP

#include "tools/Array.hpp"
#include "../gpu/GPU.hpp"

#include "properties/ShaderStage.hpp"
#include "properties/InputAssemblyState.hpp"
#include "properties/DepthTesting.hpp"
#include "properties/ViewportTransformation.hpp"
#include "properties/Rasterization.hpp"
#include "properties/Multisampling.hpp"
#include "properties/ColorLogic.hpp"

namespace Makma3D::Rendering {
    /* The Pipeline class, which functions a as a more convenient wrapper for the internal VkPipeline object. */
    class Pipeline {
    public:
        /* Channel name of the Pipeline class. */
        static constexpr const char* channel = "Pipeline";

        /* The GPU on which the Pipeline lives. */
        const Rendering::GPU& gpu;

    private:
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
        Pipeline(const Rendering::GPU& gpu,
                 const Tools::Array<Rendering::ShaderStage>& shader_stages,
                 const Rendering::InputAssemblyState& input_assembly_state,
                 const Rendering::DepthTesting& depth_testing,
                 const Rendering::ViewportTransformation& viewport_transformation,
                 const Rendering::Rasterization& rasterization,
                 const Rendering::Multisampling& multisampling,
                 const Rendering::ColorLogic& color_logic);
        /* Destructor for the Pipeline class. */
        ~Pipeline();

    public:
        /* Copy constructor for the Pipeline class, which is deleted. */
        Pipeline(const Pipeline& other) = delete;
        /* Move constructor for the Pipeline class, which is deleted. */
        Pipeline(Pipeline&& other) = delete;

        /* Copy assignment operator for the Pipeline class, which is deleted. */
        Pipeline& operator=(const Pipeline& other) = delete;
        /* Move assignment operator for the Pipeline class, which is deleted. */
        Pipeline& operator=(Pipeline&& other) = delete;

    };

}

#endif
