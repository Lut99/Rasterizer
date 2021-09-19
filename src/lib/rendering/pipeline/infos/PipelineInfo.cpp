/* PIPELINE INFO.cpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 15:48:27
 * Last edited:
 *   9/19/2021, 6:03:45 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineInfo class, which is a helper class for the
 *   Constructor that bundles all the vulkan structs needed for a full
 *   VkGraphicsPipelineCreateInfo struct in one place (and more
 *   importantly, bundles the memory management in one place).
**/

#include "tools/Logger.hpp"
#include "../../auxillary/ErrorCodes.hpp"

#include "PipelineInfo.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkGraphicsPipelineCreateInfo struct. */
static void populate_pipeline_info(VkGraphicsPipelineCreateInfo& pipeline_info,
                                   const VkPipelineShaderStageCreateInfo* vk_shader_stages, uint32_t vk_shader_stages_size,
                                   const VkPipelineVertexInputStateCreateInfo* vk_vertex_input_state,
                                   const VkPipelineInputAssemblyStateCreateInfo* vk_input_assembly_state,
                                   const VkPipelineDepthStencilStateCreateInfo* vk_depth_testing,
                                   const VkPipelineViewportStateCreateInfo* vk_viewport_transformation,
                                   const VkPipelineRasterizationStateCreateInfo* vk_rasterization,
                                   const VkPipelineMultisampleStateCreateInfo* vk_multisampling,
                                   const VkPipelineColorBlendStateCreateInfo* vk_color_logic,
                                   const VkRenderPass& vk_render_pass, uint32_t first_subpass,
                                   const VkPipeline& vk_base_handle, int32_t base_index,
                                   VkPipelineCreateFlags create_flags)
{
    // Set to default
    pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // Set the shader stages
    pipeline_info.stageCount = vk_shader_stages_size;
    pipeline_info.pStages = vk_shader_stages;

    // Next, attach the fixed-function structures
    pipeline_info.pVertexInputState = vk_vertex_input_state;
    pipeline_info.pInputAssemblyState = vk_input_assembly_state;
    pipeline_info.pDepthStencilState = vk_depth_testing;
    pipeline_info.pViewportState = vk_viewport_transformation;
    pipeline_info.pRasterizationState = vk_rasterization;
    pipeline_info.pMultisampleState = vk_multisampling;
    pipeline_info.pColorBlendState = vk_color_logic;
    pipeline_info.pDynamicState = nullptr;

    // We don't set the layout yet, but wait until it's created
    pipeline_info.layout = nullptr;

    // Finally, set the renderpass and its first subpass
    pipeline_info.renderPass = vk_render_pass;
    pipeline_info.subpass = first_subpass;

    // Note: we won't use pipeline derivation for now
    pipeline_info.basePipelineHandle = vk_base_handle;
    pipeline_info.basePipelineIndex = base_index;

    // Finally, set the create flags
    pipeline_info.flags = create_flags;
}





/***** PIPELINEINFO CLASS *****/
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
PipelineInfo::PipelineInfo(const Tools::Array<Rendering::ShaderStage>& shaders,
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
                           VkPipelineCreateFlags create_flags) :
    shader_stage_infos(shaders.size()),
    vertex_input_state_info(vertex_input_state),
    color_logic_info(color_logic),
    pipeline_layout_info(pipeline_layout)
{
    // Initialize the list of shader stage infos first, immediately casting to to the vklist
    this->vk_shader_stage_infos = new VkPipelineShaderStageCreateInfo[shaders.size()];
    for (uint32_t i = 0; i < shaders.size(); i++) {
        this->shader_stage_infos.push_back(ShaderStageInfo(shaders[i]));
        this->vk_shader_stage_infos[i] = this->shader_stage_infos[i].vulkan();
    }

    // Assign the other pointers
    this->vk_vertex_input_state_info = &this->vertex_input_state_info.vulkan();
    this->vk_input_assembly_state_info = new VkPipelineInputAssemblyStateCreateInfo(input_assembly_state.get_info());
    this->vk_depth_testing_info = new VkPipelineDepthStencilStateCreateInfo(depth_testing.get_info());
    this->vk_viewport_transformation_info = new VkPipelineViewportStateCreateInfo(viewport_transformation.get_info());
    this->vk_rasterization_info = new VkPipelineRasterizationStateCreateInfo(rasterization.get_info());
    this->vk_multisampling_info = new VkPipelineMultisampleStateCreateInfo(multisampling.get_info());
    this->vk_color_logic_info = &this->color_logic_info.vulkan();
    this->vk_pipeline_layout_info =&this->pipeline_layout_info.vulkan();

    // With all that, we can try to populate the graphics pipeline info (at least for so far)
    populate_pipeline_info(
        this->vk_pipeline_info,
        this->vk_shader_stage_infos, this->shader_stage_infos.size(),
        this->vk_vertex_input_state_info,
        this->vk_input_assembly_state_info,
        this->vk_depth_testing_info,
        this->vk_viewport_transformation_info,
        this->vk_rasterization_info,
        this->vk_multisampling_info,
        this->vk_color_logic_info,
        render_pass.vulkan(), first_subpass,
        vk_base_handle, base_index,
        create_flags
    );
}

/* Copy constructor for the PipelineInfo class. */
PipelineInfo::PipelineInfo(const PipelineInfo& other) :
    shader_stage_infos(other.shader_stage_infos),
    vertex_input_state_info(other.vertex_input_state_info),
    color_logic_info(other.color_logic_info),
    pipeline_layout_info(other.pipeline_layout_info),

    vk_pipeline_info(other.vk_pipeline_info)
{
    // Re-initialize the list of shader stage infos first, immediately casting to to the vklist
    this->vk_shader_stage_infos = new VkPipelineShaderStageCreateInfo[this->shader_stage_infos.size()];
    for (uint32_t i = 0; i < this->shader_stage_infos.size(); i++) {
        this->vk_shader_stage_infos[i] = this->shader_stage_infos[i].vulkan();
    }

    // Copy all the pointers
    this->vk_vertex_input_state_info = &this->vertex_input_state_info.vulkan();
    this->vk_input_assembly_state_info = new VkPipelineInputAssemblyStateCreateInfo(*other.vk_input_assembly_state_info);
    this->vk_depth_testing_info = new VkPipelineDepthStencilStateCreateInfo(*other.vk_depth_testing_info);
    this->vk_viewport_transformation_info = new VkPipelineViewportStateCreateInfo(*other.vk_viewport_transformation_info);
    this->vk_rasterization_info = new VkPipelineRasterizationStateCreateInfo(*other.vk_rasterization_info);
    this->vk_multisampling_info = new VkPipelineMultisampleStateCreateInfo(*other.vk_multisampling_info);
    this->vk_color_logic_info = &this->color_logic_info.vulkan();
    this->vk_pipeline_layout_info = &this->pipeline_layout_info.vulkan();

    // Update the pointers in the pipeline info
    this->vk_pipeline_info.pStages = this->vk_shader_stage_infos;
    this->vk_pipeline_info.pVertexInputState = this->vk_vertex_input_state_info;
    this->vk_pipeline_info.pInputAssemblyState = this->vk_input_assembly_state_info;
    this->vk_pipeline_info.pDepthStencilState = this->vk_depth_testing_info;
    this->vk_pipeline_info.pViewportState = this->vk_viewport_transformation_info;
    this->vk_pipeline_info.pRasterizationState = this->vk_rasterization_info;
    this->vk_pipeline_info.pMultisampleState = this->vk_multisampling_info;
    this->vk_pipeline_info.pColorBlendState = this->vk_color_logic_info;
    this->vk_pipeline_info.pDynamicState = nullptr;
}

/* Move constructor for the PipelineInfo class. */
PipelineInfo::PipelineInfo(PipelineInfo&& other) :
    shader_stage_infos(other.shader_stage_infos),
    vertex_input_state_info(other.vertex_input_state_info),
    color_logic_info(other.color_logic_info),
    pipeline_layout_info(other.pipeline_layout_info),

    vk_shader_stage_infos(other.vk_shader_stage_infos),
    vk_vertex_input_state_info(other.vk_vertex_input_state_info),
    vk_input_assembly_state_info(other.vk_input_assembly_state_info),
    vk_depth_testing_info(other.vk_depth_testing_info),
    vk_viewport_transformation_info(other.vk_viewport_transformation_info),
    vk_rasterization_info(other.vk_rasterization_info),
    vk_multisampling_info(other.vk_multisampling_info),
    vk_color_logic_info(other.vk_color_logic_info),
    vk_pipeline_layout_info(other.vk_pipeline_layout_info),

    vk_pipeline_info(other.vk_pipeline_info)
{
    // Prevent everything from deallocating
    other.vk_shader_stage_infos = nullptr;
    other.vk_input_assembly_state_info = nullptr;
    other.vk_depth_testing_info = nullptr;
    other.vk_viewport_transformation_info = nullptr;
    other.vk_rasterization_info = nullptr;
    other.vk_multisampling_info = nullptr;
}

/* Destructor for the PipelineInfo class. */
PipelineInfo::~PipelineInfo() {
    // The vk_pipeline_layout_info is managed by our internal pipeline_layout_info
    // The vk_color_logic_info is managed by our internal color_logic_info
    if (this->vk_multisampling_info != nullptr) { delete this->vk_multisampling_info; }
    if (this->vk_rasterization_info != nullptr) { delete this->vk_rasterization_info; }
    if (this->vk_viewport_transformation_info != nullptr) { delete this->vk_viewport_transformation_info; }
    if (this->vk_depth_testing_info != nullptr) { delete this->vk_depth_testing_info; }
    if (this->vk_input_assembly_state_info != nullptr) { delete this->vk_input_assembly_state_info; }
    // The vk_vertex_input_state_info is managed by our internal vk_vertex_input_state_info
    if (this->vk_shader_stage_infos != nullptr) { delete[] this->vk_shader_stage_infos; }
}



/* Swap operator for the PipelineInfo class. */
void Rendering::swap(PipelineInfo& pi1, PipelineInfo& pi2) {
    using std::swap;

    swap(pi1.shader_stage_infos, pi2.shader_stage_infos);
    swap(pi1.vertex_input_state_info, pi2.vertex_input_state_info);
    swap(pi1.color_logic_info, pi2.color_logic_info);
    swap(pi1.pipeline_layout_info, pi2.pipeline_layout_info);

    swap(pi1.vk_shader_stage_infos, pi2.vk_shader_stage_infos);
    swap(pi1.vk_vertex_input_state_info, pi2.vk_vertex_input_state_info);
    swap(pi1.vk_input_assembly_state_info, pi2.vk_input_assembly_state_info);
    swap(pi1.vk_depth_testing_info, pi2.vk_depth_testing_info);
    swap(pi1.vk_viewport_transformation_info, pi2.vk_viewport_transformation_info);
    swap(pi1.vk_rasterization_info, pi2.vk_rasterization_info);
    swap(pi1.vk_multisampling_info, pi2.vk_multisampling_info);
    swap(pi1.vk_color_logic_info, pi2.vk_color_logic_info);
    swap(pi1.vk_pipeline_layout_info, pi2.vk_pipeline_layout_info);

    swap(pi1.vk_pipeline_info, pi2.vk_pipeline_info);
}
