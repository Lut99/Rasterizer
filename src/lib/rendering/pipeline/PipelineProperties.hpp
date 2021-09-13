/* PIPELINE PROPERTIES.hpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:39:31
 * Last edited:
 *   12/09/2021, 14:39:31
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that bundles all pipeline properties in one place.
**/

#ifndef RENDERING_PIPELINE_PROPERTIES_HPP
#define RENDERING_PIPELINE_PROPERTIES_HPP

#include "properties/ShaderStage.hpp"
#include "properties/VertexInputState.hpp"
#include "properties/InputAssemblyState.hpp"
#include "properties/DepthTesting.hpp"
#include "properties/ViewportTransformation.hpp"
#include "properties/Rasterization.hpp"
#include "properties/Multisampling.hpp"
#include "properties/ColorLogic.hpp"
#include "properties/PipelineLayout.hpp"

namespace Makma3D::Rendering {
    /* The PipelineProperties struct, which bundles the properties in one place. */
    class PipelineProperties {
    public:
        /* The list of shader stages for this pipeline. */
        Tools::Array<Rendering::ShaderStage> shader_stages;
        /* Describes how the vertex data presented to the pipeline is organised .*/
        Rendering::VertexInputState vertex_input_state;
        /* The properties for the pipeline's input assembly. */
        Rendering::InputAssemblyState input_assembly_state;
        /* The properties for the pipeline's depth testing. */
        Rendering::DepthTesting depth_testing;
        /* The properties for the pipeline's viewport size/cutoff. */
        Rendering::ViewportTransformation viewport_transformation;
        /* The properties for the pipeline's rasterization stage. */
        Rendering::Rasterization rasterization;
        /* The properties for the pipeline's multisampling. */
        Rendering::Multisampling multisampling;
        /* The properties for how the pipeline deals with existing pixels in the target framebuffer(s). */
        Rendering::ColorLogic color_logic;
        /* The layout of the pipeline in terms of descriptors and push constants. */
        Rendering::PipelineLayout pipeline_layout;

    public:
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
        PipelineProperties(const Tools::Array<Rendering::ShaderStage>& shader_stages,
                           const Rendering::VertexInputState& vertex_input_state,
                           const Rendering::InputAssemblyState& input_assembly_state,
                           const Rendering::DepthTesting& depth_testing,
                           const Rendering::ViewportTransformation& viewport_transformation,
                           const Rendering::Rasterization& rasterization,
                           const Rendering::Multisampling& multisampling,
                           const Rendering::ColorLogic& color_logic,
                           const Rendering::PipelineLayout& pipeline_layout);
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
        PipelineProperties(Tools::Array<Rendering::ShaderStage>&& shader_stages,
                           Rendering::VertexInputState&& vertex_input_state,
                           Rendering::InputAssemblyState&& input_assembly_state,
                           Rendering::DepthTesting&& depth_testing,
                           Rendering::ViewportTransformation&& viewport_transformation,
                           Rendering::Rasterization&& rasterization,
                           Rendering::Multisampling&& multisampling,
                           Rendering::ColorLogic&& color_logic,
                           Rendering::PipelineLayout&& pipeline_layout);
        /* Copy constructor for the PipelineProperties class. */
        PipelineProperties(const PipelineProperties& other);
        /* Move constructor for the PipelineProperties class. */
        PipelineProperties(PipelineProperties&& other);
        /* Destructor for the PipelineProperties class. */
        ~PipelineProperties();

        /* Copy assignment operator for the PipelineProperties class. */
        inline PipelineProperties& operator=(const PipelineProperties& other) { return *this = PipelineProperties(other); }
        /* Move assignment operator for the PipelineProperties class. */
        inline PipelineProperties& operator=(PipelineProperties&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineProperties class */
        friend void swap(PipelineProperties& pp1, PipelineProperties& pp2);

    };

    /* Swap operator for the PipelineProperties class */
    void swap(PipelineProperties& pp1, PipelineProperties& pp2);

}

#endif
