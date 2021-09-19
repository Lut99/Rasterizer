/* PIPELINE INFO.hpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 14:01:13
 * Last edited:
 *   19/09/2021, 14:01:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineInfo class, which is a helper class for the
 *   Constructor that bundles all the vulkan structs needed for a full
 *   VkGraphicsPipelineCreateInfo struct in one place (and more
 *   importantly, bundles the memory management in one place).
**/

#ifndef RENDERING_PIPELINE_INFO_HPP
#define RENDERING_PIPELINE_INFO_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "../../renderpass/RenderPass.hpp"

#include "../properties/ShaderStage.hpp"
#include "../properties/VertexInputState.hpp"
#include "../properties/InputAssemblyState.hpp"
#include "../properties/DepthTesting.hpp"
#include "../properties/ViewportTransformation.hpp"
#include "../properties/Rasterization.hpp"
#include "../properties/Multisampling.hpp"
#include "../properties/ColorLogic.hpp"
#include "../properties/PipelineLayout.hpp"

#include "ShaderStageInfo.hpp"
#include "VertexInputStateInfo.hpp"
#include "ColorLogicInfo.hpp"
#include "PipelineLayoutInfo.hpp"

namespace Makma3D::Rendering {
    /* The PipelineInfo class, which manages a VkGraphicsPipelineCreateInfo and all its associated Vulkan structs. */
    class PipelineInfo {
    public:
        /* The channel name for the PipelineInfo class. */
        static constexpr const char* channel = "PipelineInfo";
    
    private:
        /* The wrapper around the data needed for a VkPipelineShaderStageStateCreateInfo struct. */
        Tools::Array<Rendering::ShaderStageInfo> shader_stage_infos;
        /* Wraps all data we need for the VkPipelineVertexInputStateCreateInfo struct. */
        Rendering::VertexInputStateInfo vertex_input_state_info;
        /* Wraps the relevant data for the VkPipelineColorBlendStateCreateInfo struct. */
        Rendering::ColorLogicInfo color_logic_info;
        /* Describes what we need for the VkPipelineLayoutCreateInfo struct. */
        Rendering::PipelineLayoutInfo pipeline_layout_info;

    public:
        /* The list of VkPipelineShaderStageCreateInfo we need to describe all the shaders bound to this pipeline. Has the same size as shader_stage_infos. */
        VkPipelineShaderStageCreateInfo* vk_shader_stage_infos;
        /* The VkPipelineVertexInputStateCreateInfo struct that describes how the input vertex buffers look like. */
        const VkPipelineVertexInputStateCreateInfo* vk_vertex_input_state_info;
        /* The VkPipelineInputAssemblyStateCreateInfo struct that describes what to do with the given vertices. */
        const VkPipelineInputAssemblyStateCreateInfo* vk_input_assembly_state_info;
        /* The VkPipelineDepthStencilStateCreateInfo struct that describes if and how to perform depth teseting. */
        const VkPipelineDepthStencilStateCreateInfo* vk_depth_testing_info;
        /* The VkPipelineViewportStateCreateInfo struct that describes the resulting viewport size and scaling. */
        const VkPipelineViewportStateCreateInfo* vk_viewport_transformation_info;
        /* The VkPipelineRasterizationStateCreateInfo struct that describes how the rasterizer stage behaves. */
        const VkPipelineRasterizationStateCreateInfo* vk_rasterization_info;
        /* The VkPipelineMultisampleStateCreateInfo struct that describes if and how to do multisampling. */
        const VkPipelineMultisampleStateCreateInfo* vk_multisampling_info;
        /* The VkPipelineColorBlendStateCreateInfo struct that describes what to do with pixels already present in the resulting framebuffer. */
        const VkPipelineColorBlendStateCreateInfo* vk_color_logic_info;
        /* The VkPipelineLayoutCreateInfo used to create the pipeline layout for the pipeline we'll create. */
        const VkPipelineLayoutCreateInfo* vk_pipeline_layout_info;

        /* The VkGraphicsPipelineCreateInfo we actually wrap. */
        VkGraphicsPipelineCreateInfo vk_pipeline_info;

    public:
        /* Constructor for the PipelineInfo class, which takes all the necessary properties to create the PipelineInfo.
         *
         * @param shaders A list of ShaderStages to initialize, each containing custom shader code.
         * @param vertex_input_state The description of where the vertex buffers are bound and how they are laid out.
         * @param input_assembly_state The description of what to do with the input vertices.
         * @param depth_testing Whether to do depth testing or not and, if so, how so.
         * @param viewport_transformation How the resulting viewport is sized/cutoff.
         * @param rasterization What to do during the rasterization stage.
         * @param multisampling How the pipeline should deal with multisampling.
         * @param color_logic How to deal with pixels already present in the target framebuffer(s).
         * @param pipeline_layout How the data in the pipeline looks like, in terms of descriptors and push constants.
         * @param render_pass The RenderPass to which to bind the PipelineInfo.
         * @param first_subpass The index of the first subpass that should be executed.
         * @param vk_base_handle Base handle for another, already constructed VkPipeline which could be used to speed up pipeline creation. Means that this pipeline officially becomes a child of the given one.
         * @param base_index In case this PipelineInfo is used in a mass allocation, use the pipeline info at the given index in that list as a parent for this pipeline. Requires the index to be less than our own.
         * @param create_flags Optional create flags for the pipeline.
         */
        PipelineInfo(const Tools::Array<Rendering::ShaderStage>& shaders,
                     const Rendering::VertexInputState& vertex_input_state,
                     const Rendering::InputAssemblyState& input_assembly_state,
                     const Rendering::DepthTesting& depth_testing,
                     const Rendering::ViewportTransformation& viewport_transformation,
                     const Rendering::Rasterization& rasterization,
                     const Rendering::Multisampling& multisampling,
                     const Rendering::ColorLogic& color_logic,
                     const Rendering::PipelineLayout& pipeline_layout,
                     const Rendering::RenderPass& render_pass, uint32_t first_subpass,
                     const VkPipeline& vk_base_handle, int32_t base_index,
                     VkPipelineCreateFlags create_flags = 0);
        /* Copy constructor for the PipelineInfo class. */
        PipelineInfo(const PipelineInfo& other);
        /* Move constructor for the PipelineInfo class. */
        PipelineInfo(PipelineInfo&& other);
        /* Destructor for the PipelineInfo class. */
        ~PipelineInfo();

        /* Copy assignment operator for the PipelineInfo class. */
        inline PipelineInfo& operator=(const PipelineInfo& other) { return *this = PipelineInfo(other); }
        /* Move assignment operator for the PipelineInfo class. */
        inline PipelineInfo& operator=(PipelineInfo&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineInfo class. */
        friend void swap(PipelineInfo& pi1, PipelineInfo& pi2);

    };

    /* Swap operator for the PipelineInfo class. */
    void swap(PipelineInfo& pi1, PipelineInfo& pi2);
}

#endif
