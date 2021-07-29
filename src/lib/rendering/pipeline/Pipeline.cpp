/* PIPELINE.cpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 12:29:37
 * Last edited:
 *   28/06/2021, 22:28:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Implements our warpper around the pipeline that is used to define the
 *   rendering process.
**/

#include "tools/CppDebugger.hpp"
#include "../auxillary/ErrorCodes.hpp"
#include "../auxillary/ShaderStages.hpp"

#include "Pipeline.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineVertexInputStateCreateInfo struct. */
static void populate_vertex_state_info(VkPipelineVertexInputStateCreateInfo& vertex_state_info, const VkVertexInputBindingDescription& vk_binding_description, const Tools::Array<VkVertexInputAttributeDescription>& vk_attribute_descriptions) {
    DENTER("populate_vertex_state_info");

    // Set to default
    vertex_state_info = {};
    vertex_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Set the bindings
    vertex_state_info.vertexBindingDescriptionCount = 1;
    vertex_state_info.pVertexBindingDescriptions = &vk_binding_description;
    
    // Set the attributes
    vertex_state_info.vertexAttributeDescriptionCount = vk_attribute_descriptions.size();
    vertex_state_info.pVertexAttributeDescriptions = vk_attribute_descriptions.rdata();

    // We're done
    DRETURN;
}

/* Populates the given VkPipelineInputAssemblyStateCreateInfo struct. */
static void populate_assembly_state_info(VkPipelineInputAssemblyStateCreateInfo& assembly_state_info, VkPrimitiveTopology topology, VkBool32 primitive_restart) {
    DENTER("populate_assembly_state_info");

    // Set to default
    assembly_state_info = {};
    assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // Pass the arguments
    assembly_state_info.topology = topology;
    assembly_state_info.primitiveRestartEnable = primitive_restart;

    // DOne
    DRETURN;
}

/* Populates the given VkPipelineDepthStencilStateCreateInfo struct. */
static void populate_depth_stencil_state_info(VkPipelineDepthStencilStateCreateInfo& depth_stencil_state_info, VkBool32 vk_enable, VkCompareOp vk_compare_op) {
    DENTER("populate_depth_stencil_state_info");

    // Set to default
    depth_stencil_state_info = {};
    depth_stencil_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Pass the arguments
    depth_stencil_state_info.depthTestEnable = vk_enable;
    depth_stencil_state_info.depthWriteEnable = vk_enable;

    // Define how to compare the values of the depth stencil
    depth_stencil_state_info.depthCompareOp = vk_compare_op;

    // We can optionally specify to only keep elements within a certain bound; but we don't
    depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state_info.minDepthBounds = 0.0f;
    depth_stencil_state_info.maxDepthBounds = 1.0f;

    // We can also do stencils tests(?)
    depth_stencil_state_info.stencilTestEnable = VK_FALSE;
    depth_stencil_state_info.front = {};
    depth_stencil_state_info.back = {};

    // Done
    DRETURN;
}

/* Populates the given VkPipelineViewportStateCreateInfo struct. */
static void populate_viewport_state_info(VkPipelineViewportStateCreateInfo& viewport_state_info, const VkViewport& vk_viewport, const VkRect2D& vk_scissor) {
    DENTER("populate_viewport_state_info");

    // Set to default
    viewport_state_info = {};
    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // Set the rectangle and scissors
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports = &vk_viewport;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = &vk_scissor;

    // Done
    DRETURN;
}

/* Populates the given VkPipelineRasterizationStateCreateInfo struct. */
static void populate_rasterizer_state_info(VkPipelineRasterizationStateCreateInfo& rasterizer_state_info, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp, VkPolygonMode polygon_mode, float line_width, VkBool32 disable_rasterizer) {
    DENTER("populate_rasterizer_state_info");

    // Set the struct to default
    rasterizer_state_info = {};
    rasterizer_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // Set the cullmode
    rasterizer_state_info.cullMode = cull_mode;
    rasterizer_state_info.frontFace = front_face;

    // Set whether or not to clamp distant objects
    rasterizer_state_info.depthClampEnable = depth_clamp;
    
    // Set the polygon mode
    rasterizer_state_info.polygonMode = polygon_mode;

    // Set the line width
    rasterizer_state_info.lineWidth = line_width;

    // Set the depth bias, which we (for now) hardcode to always be false
    rasterizer_state_info.depthBiasEnable = VK_FALSE;
    rasterizer_state_info.depthBiasConstantFactor = 0.0f;
    rasterizer_state_info.depthBiasClamp = 0.0f;
    rasterizer_state_info.depthBiasSlopeFactor = 0.0f;

    // Set the rasterizer enabled status
    rasterizer_state_info.rasterizerDiscardEnable = disable_rasterizer;

    DRETURN;
}

/* Populate sthe given VkPipelineMultisampleStateCreateInfo struct. */
static void populate_multisample_state(VkPipelineMultisampleStateCreateInfo& multisample_state_info) {
    DENTER("populate_multisample_state");

    // Define the standard stuff
    multisample_state_info = {};
    multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    // For now, hardcode the multisampling to be off
    multisample_state_info.sampleShadingEnable = VK_FALSE;
    multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state_info.minSampleShading = 1.0f;
    multisample_state_info.pSampleMask = nullptr;
    multisample_state_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_info.alphaToOneEnable = VK_FALSE;

    // Done
    DRETURN;
}

/* Populates the given VkPipelineColorBlendAttachmentState struct. */
static void populate_color_attachment_state(VkPipelineColorBlendAttachmentState& color_attachment_state, VkBool32 enable_blending, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op) {
    DENTER("populate_color_attachment_state");

    // Set to default
    color_attachment_state = {};

    // Set whether or not it's enabled
    color_attachment_state.blendEnable = enable_blending;

    // Set the write mask, which we hardcode to everything always
    color_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    // Set the color blending
    color_attachment_state.srcColorBlendFactor = src_color_factor;
    color_attachment_state.dstColorBlendFactor = dst_color_factor;
    color_attachment_state.colorBlendOp = color_op;

    // Set the alpha blending
    color_attachment_state.srcAlphaBlendFactor = src_alpha_factor;
    color_attachment_state.dstAlphaBlendFactor = dst_alpha_factor;
    color_attachment_state.alphaBlendOp = alpha_op;

    // Done
    DRETURN;
}

/* Populates the given VkPipelineColorBlendStateCreateInfo struct. */
static void populate_color_state_info(VkPipelineColorBlendStateCreateInfo& color_state_info, const Tools::Array<VkPipelineColorBlendAttachmentState>& color_attachments, VkBool32 enable_logic, VkLogicOp logic_op) {
    DENTER("populate_color_state_info");

    // Set to default
    color_state_info = {};
    color_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    // Describe the logic operations to do
    color_state_info.logicOpEnable = enable_logic;
    color_state_info.logicOp = logic_op;

    // Attach the per-framebuffer attachments
    color_state_info.attachmentCount = color_attachments.size();
    color_state_info.pAttachments = color_attachments.rdata();

    // Hardcode the constants, since I don't rlly know what they do lmao gottem
    color_state_info.blendConstants[0] = 0.0f;
    color_state_info.blendConstants[1] = 0.0f;
    color_state_info.blendConstants[2] = 0.0f;
    color_state_info.blendConstants[3] = 0.0f;

    // Done
    DRETURN;
}

/* Populates the given VkPipelineLayoutCreateInfo struct. */
static void populate_layout_info(VkPipelineLayoutCreateInfo& layout_info, const Tools::Array<VkDescriptorSetLayout>& vk_layouts, const Tools::Array<VkPushConstantRange>& vk_push_constants) {
    DENTER("populate_layout_info");

    // Set to default first
    layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Bind the descriptor set layouts
    layout_info.setLayoutCount = vk_layouts.size();
    layout_info.pSetLayouts = vk_layouts.rdata();

    // Bind the push constant descriptions
    layout_info.pushConstantRangeCount = vk_push_constants.size();
    layout_info.pPushConstantRanges = vk_push_constants.rdata();

    // Done
    DRETURN;
}





/***** PIPELINE CLASS *****/
/* Constructor for the Pipeline class, which takes the GPU on which it should be created. */
Pipeline::Pipeline(const Rendering::GPU& gpu) :
    gpu(gpu),
    vk_pipeline_layout(nullptr)
{
    DENTER("Rendering::Pipeline::Pipeline");
    DLOG(info, "Started pipeline initialization.");
    DLEAVE;
}

/* Copy constructor for the Pipeline class. */
Pipeline::Pipeline(const Pipeline& other) :
    gpu(other.gpu),

    vk_descriptor_set_layouts(other.vk_descriptor_set_layouts),
    vk_push_constants(other.vk_push_constants),
    vk_layout_info(other.vk_layout_info),
    vk_pipeline_layout(other.vk_pipeline_layout),

    shaders(other.shaders),
    shader_stages(other.shader_stages),

    vk_vertex_state_info(other.vk_vertex_state_info),
    vk_assembly_state_info(other.vk_assembly_state_info),
    vk_depth_stencil_state_info(other.vk_depth_stencil_state_info),
    vk_viewport(other.vk_viewport),
    vk_scissor(other.vk_scissor),
    vk_viewport_state_info(other.vk_viewport_state_info),
    vk_rasterizer_state_info(other.vk_rasterizer_state_info),
    vk_multisample_state_info(other.vk_multisample_state_info),
    vk_color_blending(other.vk_color_blending),
    vk_color_state_info(other.vk_color_state_info)
{
    DENTER("Rendering::Pipeline::Pipeline(copy)");

    // Re-create the pipeline layout if needed using the create info we got
    if (this->vk_pipeline_layout != nullptr) {
        VkResult vk_result;
        if ((vk_result = vkCreatePipelineLayout(this->gpu, &this->vk_layout_info, nullptr, &this->vk_pipeline_layout)) != VK_SUCCESS) {
            DLOG(fatal, "Could not re-create pipeline layout: " + vk_error_map[vk_result]);
        }
    }

    DLEAVE;
}

/* Move constructor for the Pipeline class. */
Pipeline::Pipeline(Pipeline&& other) :
    gpu(other.gpu),

    vk_pipeline(other.vk_pipeline),

    vk_descriptor_set_layouts(std::move(other.vk_descriptor_set_layouts)),
    vk_push_constants(std::move(other.vk_push_constants)),
    vk_layout_info(std::move(other.vk_layout_info)),
    vk_pipeline_layout(std::move(other.vk_pipeline_layout)),

    shaders(std::move(other.shaders)),
    shader_stages(std::move(other.shader_stages)),

    vk_vertex_state_info(std::move(other.vk_vertex_state_info)),
    vk_assembly_state_info(std::move(other.vk_assembly_state_info)),
    vk_depth_stencil_state_info(std::move(other.vk_depth_stencil_state_info)),
    vk_viewport(std::move(other.vk_viewport)),
    vk_scissor(std::move(other.vk_scissor)),
    vk_viewport_state_info(std::move(other.vk_viewport_state_info)),
    vk_rasterizer_state_info(std::move(other.vk_rasterizer_state_info)),
    vk_multisample_state_info(std::move(other.vk_multisample_state_info)),
    vk_color_blending(std::move(other.vk_color_blending)),
    vk_color_state_info(std::move(other.vk_color_state_info))
{
    // Set the deallocatable fields to nullptrs
    other.vk_pipeline = nullptr;
    other.vk_pipeline_layout = nullptr;
}

/* Destructor for the Pipeline class. */
Pipeline::~Pipeline() {
    DENTER("Rendering::Pipeline::~Pipeline");
    DLOG(info, "Destroying Pipeline...")
    DINDENT;

    if (this->vk_pipeline_layout != nullptr) {
        DLOG(info, "Destroying pipeline layout...");
        vkDestroyPipelineLayout(this->gpu, this->vk_pipeline_layout, nullptr);
    }

    if (this->vk_pipeline != nullptr) {
        DLOG(info, "Destroying internal VkPipeline...")
        vkDestroyPipeline(this->gpu, this->vk_pipeline, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Loads a shader in the given shader stage mask. */
void Pipeline::init_shader_stage(const Rendering::Shader& shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, BinaryString>& specialization_constants) {
    DENTER("Rendering::Pipeline::init_shader_stage");

    // Store the shader
    this->shaders.push_back(shader);

    // Create a new shader stage and store that
    this->shader_stages.push_back(ShaderStage(shader, shader_stage, specialization_constants));

    // We're done
    DINDENT;
    DLOG(info, "Initialized Pipeline shader for the " + vk_shader_stage_map[shader_stage] + " stage");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how its vertex input looks like. Takes struct describing how it looks like. */
void Pipeline::init_vertex_input(const VkVertexInputBindingDescription& vk_input_binding_description, const Tools::Array<VkVertexInputAttributeDescription>& vk_input_attribute_descriptions) {
    DENTER("Rendering::Pipeline::init_vertex_input");

    // Prepare the interal VkPipelineVertexInputStateCreateInfo struct
    populate_vertex_state_info(this->vk_vertex_state_info, vk_input_binding_description, vk_input_attribute_descriptions);

    DINDENT;
    DLOG(info, "Initialized Pipeline vertex input");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline what to do with the vertex we gave it. */
void Pipeline::init_input_assembly(VkPrimitiveTopology topology, VkBool32 restart_enable) {
    DENTER("Rendering::Pipeline::init_input_assembly");

    // Prepare the interal VkPipelineInputAssemblyStateCreateInfo struct
    populate_assembly_state_info(this->vk_assembly_state_info, topology, restart_enable);

    DINDENT;
    DLOG(info, "Initialized Pipeline input assembly");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline what to do with depth stencil testing. */
void Pipeline::init_depth_testing(VkBool32 enable_testing, VkCompareOp compare_op) {
    DENTER("Rendering::Pipeline::init_depth_testing");

    populate_depth_stencil_state_info(this->vk_depth_stencil_state_info, enable_testing, compare_op);

    DINDENT;
    DLOG(info, "Initialized Pipeline depth testing");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how the render the output frame. The Viewport is used to stretch it, and the Scissor is used to cut it off. */
void Pipeline::init_viewport_transformation(const Rectangle& viewport, const Rectangle& scissor) {
    DENTER("Rendering::Pipeline::init_viewport_transformation");

    // First, prepare the VkViewport object
    this->vk_viewport = {};
    this->vk_viewport.x = viewport.x;
    this->vk_viewport.y = viewport.y;
    this->vk_viewport.width = viewport.w;
    this->vk_viewport.height = viewport.h;
    this->vk_viewport.minDepth = 0.0f;
    this->vk_viewport.maxDepth = 1.0f;

    // Then, prepare the scissor rectangle
    this->vk_scissor = {};
    this->vk_scissor.offset = scissor.offset();
    this->vk_scissor.extent = scissor.extent();

    // Finally, use those to populate the ViewportState struct
    populate_viewport_state_info(this->vk_viewport_state_info, this->vk_viewport, this->vk_scissor);

    DINDENT;
    DLOG(info, "Initialized Pipeline viewport");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how to configure the Rasterizer stage.
 *
 * @param cull_mode Describes how to cull faces that are behind others
 * @param front_face Describes what the front face of an object is (i.e., which side to cull)
 * @param depth_clamp Describes whether to clamp objects that are too far or too near to their plane instead of simply not rendering them. Needs a special GPU feature to enable.
 * @param polygon_mode Describes how to "colour" the given geometry. For example, can be set to fill the whole vertex, or only its lines, etc. Needs a special GPU feature for anything other than FILL.
 * @param line_width The width of the lines that the rasterizer draws. Needs a special GPU feature to grow beyond 1.0f.
 * @param disable_rasterizer If set to TRUE, then discards the output of the rasterizer, disabling it in effect.
 */
void Pipeline::init_rasterizer(VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp, VkPolygonMode polygon_mode, float line_width, VkBool32 disable_rasterizer) {
    DENTER("Rendering::Pipeline::init_rasterizer");

    // Simply populate the correct struct
    populate_rasterizer_state_info(this->vk_rasterizer_state_info, cull_mode, front_face, depth_clamp, polygon_mode, line_width, disable_rasterizer);

    DINDENT;
    DLOG(info, "Initialized Pipeline rasterizer");
    DDEDENT;
    DRETURN;
}

/* Tells the pipeline how to handle multi-sampling. For now, perpetuably disabled. */
void Pipeline::init_multisampling() {
    DENTER("Rendering::Pipeline::init_multisampling");

    // Populate the struct
    populate_multisample_state(this->vk_multisample_state_info);

    // Done
    DINDENT;
    DLOG(info, "Initialized Pipeline multisampling");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how to blend the new, computed color of a pixel with the one already in the framebuffer. This is per output framebuffer, and may thus need to be called multiple times.
 *
 * @param framebuffer The index of the framebuffer for which these settings hold.
 * @param enabled_blending Whether or not to enable color blending.
 * @param src_color_factor How much weight the new color will have while blending
 * @param dst_color_factor How much weight the existing color will have while blending
 * @param color_op How to combine the two colors into one
 * @param src_alpha_factor How much weight the new alpha value will have while blending
 * @param dst_alpha_factor How much weight the existing alpha value will have while blending
 * @param alpha_op How to combine the two alpha channels into one
 */
void Pipeline::init_color_blending(uint32_t framebuffer, VkBool32 enable_blending, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op) {
    DENTER("Rendering::Pipeline::init_color_blending");

    // See if we have a framebuffer with this index yet
    if (framebuffer == this->vk_color_blending.size()) {
        // If it's just one larger, add it
        this->vk_color_blending.push_back({});
    } else if (framebuffer > this->vk_color_blending.size()) {
        DLOG(fatal, "Cannot create more than one new framebuffer at a time (most recent framebuffer is " + std::to_string(this->vk_color_blending.size() - 1) + ", got " + std::to_string(framebuffer) + ")");
    }

    // Populate the attachment for this buffer
    populate_color_attachment_state(this->vk_color_blending[framebuffer], enable_blending, src_color_factor, dst_color_factor, color_op, src_alpha_factor, dst_alpha_factor, alpha_op);

    DINDENT;
    DLOG(info, "Initialized Pipeline color blending for framebuffer " + std::to_string(framebuffer));
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how to generally blend the new, computed color of a pixel with the one already there. This one uses logical operations instead of a blending one, and holds for ALL framebuffers.
 * Note that if given, any framebuffer-specific operations will be ignored.
 *
 * @param enable_logic Whether or not this global logic operator should be enabled.
 * @param logic_op Which logic operation to apply.
 */
void Pipeline::init_color_logic(VkBool32 enable_logic, VkLogicOp logic_op) {
    DENTER("Rendering::Pipeline::init_color_logic");

    // Populate the color blend state with the array we collected with init_color_blending() calls
    populate_color_state_info(this->vk_color_state_info, this->vk_color_blending, enable_logic, logic_op);

    DINDENT;
    DLOG(info, "Initialized Pipeline general color blending");
    DDEDENT;
    DRETURN;
}

/* Initiates the pipeline layout based on the list of descriptor set layouts and on the list of push constants. */
void Pipeline::init_pipeline_layout(const Tools::Array<DescriptorSetLayout>& layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constants) {
    DENTER("Rendering::Pipeline::init_pipeline_layout");

    // Begin by converting the list of layouts to vulkan VkDescriptorSetLayouts
    this->vk_descriptor_set_layouts.resize(layouts.size());
    for (uint32_t i = 0; i < layouts.size(); i++) {
        this->vk_descriptor_set_layouts[i] = layouts[i];
    }

    // Next, convert the push constants to a usable format
    uint32_t offset = 0;
    this->vk_push_constants.resize(push_constants.size());
    for (uint32_t i = 0; i < push_constants.size(); i++) {
        // For this push constant, align the size to four
        uint32_t aligned_size = push_constants[i].second;
        if (aligned_size % 4 != 0) { aligned_size += (4 - aligned_size % 4); }

        // Populate the new struct with this info
        this->vk_push_constants[i] = {};
        this->vk_push_constants[i].stageFlags = push_constants[i].first;
        this->vk_push_constants[i].offset = offset;
        this->vk_push_constants[i].size = aligned_size;

        // Before we continue, update the offset
        offset += aligned_size;
    }

    // Populate the internal create info
    populate_layout_info(this->vk_layout_info, this->vk_descriptor_set_layouts, this->vk_push_constants);

    // Finally, create the actual pipeline layout
    VkResult vk_result;
    if ((vk_result = vkCreatePipelineLayout(this->gpu, &this->vk_layout_info, nullptr, &this->vk_pipeline_layout)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create pipeline layout: " + vk_error_map[vk_result]);
    }

    // We're done
    DINDENT;
    DLOG(info, "Initialized Pipeline layout");
    DDEDENT;
    DRETURN;
}

/* When called, completes the pipeline with the settings given by the other initialization functions. */
void Pipeline::finalize(const RenderPass& render_pass, uint32_t first_subpass) {
    DENTER("Rendering::Pipeline::finalize");

    // Finally, begin prepare the create info
    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // First, attach the shaders
    Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_stages(this->shader_stages.size());
    for (uint32_t i = 0; i < this->shader_stages.size(); i++) {
        vk_shader_stages.push_back(this->shader_stages[i].shader_stage());
    }
    pipeline_info.stageCount = vk_shader_stages.size();
    pipeline_info.pStages = vk_shader_stages.rdata();

    // Next, attach the fixed-function structures
    pipeline_info.pVertexInputState = &this->vk_vertex_state_info;
    pipeline_info.pInputAssemblyState = &this->vk_assembly_state_info;
    pipeline_info.pViewportState = &this->vk_viewport_state_info;
    pipeline_info.pRasterizationState = &this->vk_rasterizer_state_info;
    pipeline_info.pMultisampleState = &this->vk_multisample_state_info;
    pipeline_info.pDepthStencilState = &this->vk_depth_stencil_state_info;
    pipeline_info.pColorBlendState = &this->vk_color_state_info;
    pipeline_info.pDynamicState = nullptr;

    // We next set the pipeline layout
    pipeline_info.layout = this->vk_pipeline_layout;

    // Finally, set the renderpass and its first subpass
    pipeline_info.renderPass = render_pass.render_pass();
    pipeline_info.subpass = first_subpass;

    // Note: we won't use pipeline derivation for now
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    // And that's it! Time to create it!
    VkResult vk_result;
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->vk_pipeline)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create graphics pipeline: " + vk_error_map[vk_result]);
    }

    // Done :)
    DRETURN;
}



/* Schedules the pipeline to be run and thus drawn in the given command buffer. Optionally takes another bind point. */
void Pipeline::schedule(const Rendering::CommandBuffer& cmd, VkPipelineBindPoint vk_bind_point) {
    DENTER("Rendering::Pipeline::schedule");

    // Simply bind the pipeline at this point in the command buffer
    vkCmdBindPipeline(cmd, vk_bind_point, this->vk_pipeline);

    // We're done
    DRETURN;
}

/* Schedules a new push constant to be pushed to the shader(s) in the pipeline. */
void Pipeline::schedule_push_constants(const Rendering::CommandBuffer& cmd, VkShaderStageFlags shader_stage, uint32_t offset, uint32_t size, void* data) {
    DENTER("Rendering::Pipeline::schedule_push_constants");

    // Simply schedule it
    vkCmdPushConstants(cmd, this->vk_pipeline_layout, shader_stage, offset, size, data);

    DRETURN;
}

/* Schedules the draw call for the pipeline, with the given numer of vertices, instances and vertex & instance offset. */
void Pipeline::schedule_draw(const Rendering::CommandBuffer& cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    DENTER("Rendering::Pipeline::schedule_draw");

    // Simply do the draw call
    vkCmdDraw(cmd, vertex_count, instance_count, first_vertex, first_instance);

    // We're done
    DRETURN;
}

/* Schedules the draw call for the pipeline, except that is uses an index buffer instead of just a list of vertices. Takes the number of indices, the number of instances, the first vertex, the first index and the first instance. */
void Pipeline::schedule_draw_indexed(const Rendering::CommandBuffer& cmd, uint32_t index_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance) {
    DENTER("Rendering::Pipeline::schedule_draw_indexed");

    // Simply do the draw call
    vkCmdDrawIndexed(cmd, index_count, instance_count, first_index, first_vertex, first_instance);

    // We're done
    DRETURN;
}



/* Swap operator for the Pipeline class. */
void Rendering::swap(Pipeline& p1, Pipeline& p2) {
    DENTER("Rendering::swap(Pipeline)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (p1.gpu != p2.gpu) {
        DLOG(fatal, "Cannot swap pipelines with different GPUs");
    }
    #endif

    // Simply swap all fields
    using std::swap;
    swap(p1.vk_pipeline, p2.vk_pipeline);

    swap(p1.vk_descriptor_set_layouts, p2.vk_descriptor_set_layouts);
    swap(p1.vk_push_constants, p2.vk_push_constants);
    swap(p1.vk_layout_info, p2.vk_layout_info);
    swap(p1.vk_pipeline_layout, p2.vk_pipeline_layout);

    swap(p1.shaders, p2.shaders);
    swap(p1.shader_stages, p2.shader_stages);

    swap(p1.vk_vertex_state_info, p2.vk_vertex_state_info);
    swap(p1.vk_assembly_state_info, p2.vk_assembly_state_info);
    swap(p1.vk_depth_stencil_state_info, p2.vk_depth_stencil_state_info);
    swap(p1.vk_viewport, p2.vk_viewport);
    swap(p1.vk_scissor, p2.vk_scissor);
    swap(p1.vk_viewport_state_info, p2.vk_viewport_state_info);
    swap(p1.vk_rasterizer_state_info, p2.vk_rasterizer_state_info);
    swap(p1.vk_multisample_state_info, p2.vk_multisample_state_info);
    swap(p1.vk_color_blending, p2.vk_color_blending);
    swap(p1.vk_color_state_info, p2.vk_color_state_info);

    // Done
    DRETURN;
}

