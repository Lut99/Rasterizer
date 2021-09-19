/* PIPELINE CONSTRUCTOR.hpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 21:43:24
 * Last edited:
 *   17/09/2021, 21:43:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineConstructor class, which can be used to define
 *   how a Pipeline needs to be created.
**/

#ifndef RENDERING_PIPELINE_CONSTRUCTOR_HPP
#define RENDERING_PIPELINE_CONSTRUCTOR_HPP

#include "tools/Array.hpp"

#include "../gpu/GPU.hpp"
#include "../renderpass/RenderPass.hpp"

#include "properties/ShaderStage.hpp"
#include "properties/VertexInputState.hpp"
#include "properties/InputAssemblyState.hpp"
#include "properties/DepthTesting.hpp"
#include "properties/ViewportTransformation.hpp"
#include "properties/Rasterization.hpp"
#include "properties/Multisampling.hpp"
#include "properties/ColorLogic.hpp"
#include "properties/PipelineLayout.hpp"
#include "PipelineCache.hpp"
#include "Pipeline.hpp"

namespace Makma3D::Rendering {
    /* The PipelineConstructor class, which is used to efficiently set a pipeline's properties. */
    class PipelineConstructor {
    public:
        /* Channel name for the PipelineCOnstructor class. */
        static constexpr const char* channel = "PipelineConstructor";

        /* Reference to the GPU where the PipelineConstructor will construct its pipelines. */
        const Rendering::GPU& gpu;
        /* Reference to the PipelineCache that we use for speeding up pipeline creation. */
        const Rendering::PipelineCache& pipeline_cache;

    public:
        /* The base pipeline with which new pipelines can be created faster. */
        const Rendering::Pipeline* base_pipeline;

        /* List of Shaders to load,each with their matching pipeline stage and map of specialization constants. */
        Tools::Array<Rendering::ShaderStage> shaders;
        /* Defines how the input vertex buffer is laid out. */
        Rendering::VertexInputState vertex_input_state;
        /* Defines what to do with the given vertices. */
        Rendering::InputAssemblyState input_assembly_state;
        /* Defines if and how to perform depthtesting. */
        Rendering::DepthTesting depth_testing;
        /* Defines how the resulting viewport looks like. */
        Rendering::ViewportTransformation viewport_transformation;
        /* Defines how the rasterization stage behaves. */
        Rendering::Rasterization rasterization;
        /* Defines if and how to multisample. */
        Rendering::Multisampling multisampling;
        /* Defines how to populate the resulting framebuffers. */
        Rendering::ColorLogic color_logic;
        /* Defines the layout of the pipeline in terms of descriptors and push constants. */
        Rendering::PipelineLayout pipeline_layout;

    public:
        /* Constructor for the PipelineConstructor class, which takes a GPU to create the pipelines on and a PipelineCache we use for creating its pipelines. */
        PipelineConstructor(const Rendering::GPU& gpu, const Rendering::PipelineCache& pipeline_cache);
        /* Copy constructor for the PipelineConstructor class. */
        PipelineConstructor(const PipelineConstructor& other);
        /* Move constructor for the PipelineConstructor class. */
        PipelineConstructor(PipelineConstructor&& other);
        /* Destructor for the PipelineConstructor class. */
        ~PipelineConstructor();

        /* Sets the base pipeline that can be a parent to any new pipelines. Set to a nullptr to remove them again. */
        inline void set_base_pipeline(const Rendering::Pipeline* pipeline) { this->base_pipeline = pipeline; }

        /* Creates a new Pipeline with the internal properties and the given RenderPass & first subpass. Optionally takes create flags for the VkPipeline, too. */
        Rendering::Pipeline* construct(const Rendering::RenderPass& render_pass, uint32_t first_subpass, VkPipelineCreateFlags create_flags = 0) const;
        /* Creates N new Pipelines with the internal properties and the given RenderPass & first subpass. Optionally takes create flags for the VkPipeline, too. */
        Tools::Array<Rendering::Pipeline*> nconstruct(uint32_t N, const Rendering::RenderPass& render_pass, uint32_t first_subpass, VkPipelineCreateFlags create_flags = 0) const;

        /* Copy assignment operator for the PipelineConstructor class. */
        inline PipelineConstructor& operator=(const PipelineConstructor& other) { return *this = PipelineConstructor(other); }
        /* Move assignment operator for the PipelineConstructor class. */
        inline PipelineConstructor& operator=(PipelineConstructor&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineConstructor class. */
        friend void swap(PipelineConstructor& pc1, PipelineConstructor& pc2);

    };

    /* Swap operator for the PipelineConstructor class. */
    void swap(PipelineConstructor& pc1, PipelineConstructor& pc2);

    // /* Static function that takes a list of PipelineConstructors and uses the information in them to create that many new pipelines at once. */
    // Tools::Array<Rendering::Pipeline*> joint_construct(const Tools::Array<PipelineConstructor>& constructors, const Tools::Array<RenderPass>& render_passes, const Tools::Array<uint32_t>& first_subpasses, const Tools::Array<VkPipelineCreateFlags>& create_flags);

}

#endif
