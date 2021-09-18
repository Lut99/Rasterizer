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


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineLayoutCreateInfo struct. */
static void populate_layout_info(VkPipelineLayoutCreateInfo& layout_info, const Tools::Array<VkDescriptorSetLayout>& vk_layouts, const Tools::Array<VkPushConstantRange>& vk_push_constants) {
    // Set to default first
    layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Bind the descriptor set layouts
    layout_info.setLayoutCount = vk_layouts.size();
    layout_info.pSetLayouts = vk_layouts.rdata();

    // Bind the push constant descriptions
    layout_info.pushConstantRangeCount = vk_push_constants.size();
    layout_info.pPushConstantRanges = vk_push_constants.rdata();
}

/* Populates the given VkGraphicsPipelineCreateInfo struct. */
static void populate_pipeline_info(VkGraphicsPipelineCreateInfo& pipeline_info,
                                   const Tools::Array<VkPipelineShaderStageCreateInfo>& vk_shader_stages,
                                   const VkPipelineVertexInputStateCreateInfo& vk_vertex_input_state,
                                   const VkPipelineInputAssemblyStateCreateInfo& vk_input_assembly_state,
                                   const VkPipelineDepthStencilStateCreateInfo& vk_depth_testing,
                                   const VkPipelineViewportStateCreateInfo& vk_viewport_transformation,
                                   const VkPipelineRasterizationStateCreateInfo& vk_rasterization,
                                   const VkPipelineMultisampleStateCreateInfo& vk_multisampling,
                                   const VkPipelineColorBlendStateCreateInfo& vk_color_logic,
                                   const VkPipelineLayout& vk_pipeline_layout,
                                   const VkRenderPass& vk_render_pass, uint32_t first_subpass,
                                   const VkPipeline& vk_base_handle, int32_t base_index,
                                   VkPipelineCreateFlags create_flags)
{
    // Set to default
    pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // Set the shader stages
    pipeline_info.stageCount = vk_shader_stages.size();
    pipeline_info.pStages = vk_shader_stages.rdata();

    // Next, attach the fixed-function structures
    pipeline_info.pVertexInputState = &vk_vertex_input_state;
    pipeline_info.pInputAssemblyState = &vk_input_assembly_state;
    pipeline_info.pDepthStencilState = &vk_depth_testing;
    pipeline_info.pViewportState = &vk_viewport_transformation;
    pipeline_info.pRasterizationState = &vk_rasterization;
    pipeline_info.pMultisampleState = &vk_multisampling;
    pipeline_info.pColorBlendState = &vk_color_logic;
    pipeline_info.pDynamicState = nullptr;

    // We next set the pipeline layout
    pipeline_info.layout = vk_pipeline_layout;

    // Finally, set the renderpass and its first subpass
    pipeline_info.renderPass = vk_render_pass;
    pipeline_info.subpass = first_subpass;

    // Note: we won't use pipeline derivation for now
    pipeline_info.basePipelineHandle = vk_base_handle;
    pipeline_info.basePipelineIndex = base_index;

    // Finally, set the create flags
    pipeline_info.flags = create_flags;
}





/***** PIPELINECONSTRUCTOR CLASS *****/
/* Constructor for the PipelineConstructor class, which takes a GPU to create the pipelines on and a PipelineCache we use for creating its pipelines. */
PipelineConstructor::PipelineConstructor(const Rendering::GPU& gpu, const Rendering::PipelineCache& pipeline_cache) :
    gpu(gpu),
    pipeline_cache(pipeline_cache)
{}

/* Copy constructor for the PipelineConstructor class. */
PipelineConstructor::PipelineConstructor(const PipelineConstructor& other) :
    gpu(other.gpu),
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
    gpu(other.gpu),
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



/* Creates a new Pipeline with the internal properties and the given RenderPass & first subpass. Optionally takes create flags for the VkPipeline, too. */
Rendering::Pipeline PipelineConstructor::construct(const Rendering::RenderPass& render_pass, uint32_t first_subpass, VkPipelineCreateFlags create_flags) const {
    VkResult vk_result;

    // Get the shader stages as a list of VkShaderStages
    Tools::Array<std::pair<void*, size_t>> vk_shader_datas(this->shaders.size());
    Tools::Array<std::pair<VkSpecializationMapEntry*, size_t>> vk_shader_entries(this->shaders.size());
    Tools::Array<VkSpecializationInfo> vk_shader_specialization_infos(this->shaders.size());
    Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_stages(this->shaders.size());
    for (uint32_t i = 0; i < this->shaders.size(); i++) {
        // Get a reference to the shader stage object
        const ShaderStage& shader_stage = this->shaders[i];

        // Flatten the specialization map for this shader
        vk_shader_entries.push_back(shader_stage.flatten_specialization_entries());
        vk_shader_datas.push_back(shader_stage.flatten_specialization_values(vk_shader_entries[i]));

        // Create the specialization info and with that, the stage info
        vk_shader_specialization_infos.push_back(shader_stage.create_specialization_info(vk_shader_entries[i], vk_shader_datas[i]));
        vk_shader_stages.push_back(shader_stage.create_info(vk_shader_specialization_infos[i]));
    }



    // Get the vertex input info
    Tools::Array<VkVertexInputBindingDescription> vk_vertex_bindings = this->vertex_input_state.get_bindings();
    Tools::Array<VkVertexInputAttributeDescription> vk_vertex_attributes = this->vertex_input_state.get_attributes();
    VkPipelineVertexInputStateCreateInfo vk_vertex_input_state = this->vertex_input_state.get_info(vk_vertex_bindings, vk_vertex_attributes);
    // Get the input assembly info
    VkPipelineInputAssemblyStateCreateInfo vk_input_assembly_state = this->input_assembly_state.get_info();
    // Get the depth testing info
    VkPipelineDepthStencilStateCreateInfo vk_depth_testing_state = this->depth_testing.get_info();
    // Get the viewport transformation info
    VkPipelineViewportStateCreateInfo vk_viewport_state = this->viewport_transformation.get_info();
    // Get the rasterization info
    VkPipelineRasterizationStateCreateInfo vk_rasterization_state = this->rasterization.get_info();
    // Get the multisampling info
    VkPipelineMultisampleStateCreateInfo vk_multisample_state = this->multisampling.get_info();
    // Create the color logic info
    Tools::Array<VkPipelineColorBlendAttachmentState> vk_color_blends = this->color_logic.get_color_blends();
    VkPipelineColorBlendStateCreateInfo vk_color_logic_state = this->color_logic.get_info(vk_color_blends);

    // Create the pipeline layout
    Tools::Array<VkDescriptorSetLayout> vk_descriptor_layouts = this->pipeline_layout.get_layouts();
    Tools::Array<VkPushConstantRange> vk_push_constant_ranges = this->pipeline_layout.get_ranges();
    VkPipelineLayoutCreateInfo layout_info;
    populate_layout_info(layout_info, vk_descriptor_layouts, vk_push_constant_ranges);
    VkPipelineLayout vk_pipeline_layout;
    if ((vk_result = vkCreatePipelineLayout(this->gpu, &layout_info, nullptr, &vk_pipeline_layout)) != VK_SUCCESS) {
        logger.fatalc(PipelineConstructor::channel, "Could not create pipeline layout: ", vk_error_map[vk_result]);
    }



    // With that done, populate the pipeline create info
    VkGraphicsPipelineCreateInfo pipeline_info;
    populate_pipeline_info(
        pipeline_info,
        vk_shader_stages,
        vk_vertex_input_state,
        vk_input_assembly_state,
        vk_depth_testing_state,
        vk_viewport_state,
        vk_rasterization_state,
        vk_multisample_state,
        vk_color_logic_state,
        vk_pipeline_layout,
        render_pass, first_subpass,
        VK_NULL_HANDLE, -1,
        create_flags
    );

    // And then create the pipeline
    VkPipeline vk_pipeline;
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, this->pipeline_cache, 1, &pipeline_info, nullptr, &vk_pipeline)) != VK_SUCCESS) {
        logger.fatalc(PipelineConstructor::channel, "Could not create pipeline: ", vk_error_map[vk_result]);
    }

    // Wrap it in a Pipeline class and we're as good as done
    Pipeline result(this->gpu, vk_pipeline, vk_pipeline_layout);

}



/* Swap operator for the PipelineConstructor class. */
void Rendering::swap(PipelineConstructor& pc1, PipelineConstructor& pc2) {
    #ifndef NDEBUG
    if (pc1.gpu != pc2.gpu) { logger.fatalc(PipelineConstructor::channel, "Cannot swap pipeline constructors with different GPUs"); }
    if (&pc1.pipeline_cache != &pc2.pipeline_cache) { logger.fatalc(PipelineConstructor::channel, "Cannot swap pipeline constructors with different pipeline caches"); }
    #endif

    using std::swap;

    swap(pc1.shaders, pc2.shaders);
}
