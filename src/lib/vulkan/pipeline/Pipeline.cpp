/* PIPELINE.cpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 12:29:37
 * Last edited:
 *   20/06/2021, 12:29:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Implements our warpper around the pipeline that is used to define the
 *   rendering process.
**/

#include "tools/CppDebugger.hpp"
#include "vulkan/ErrorCodes.hpp"
#include "vulkan/ShaderStages.hpp"

#include "Pipeline.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Given an unordered map of constants, creates an array of VkSpecializationMapEntry's and a matching array of data as void*. */
static void flatten_specialization_map(const std::unordered_map<uint32_t, BinaryString>& constant_map, Tools::Array<VkSpecializationMapEntry>& map_entries, Tools::Array<uint8_t>& data) {
    DENTER("flatten_specialization_map");

    // Create an array of mapentries, and then a single, unified data array
    uint32_t offset = 0;
    map_entries.reserve(static_cast<Tools::array_size_t>(constant_map.size()));
    for(const std::pair<uint32_t, BinaryString>& p : constant_map) {
        // Start to intialize a map entry
        VkSpecializationMapEntry map_entry{};

        // Set the constant ID for this entry
        map_entry.constantID = p.first;

        // Set the offset & size in the global constant array
        map_entry.offset = offset;
        map_entry.size = p.second.size;

        // Add it to the array
        map_entries.push_back(map_entry);

        // Increment the offset
        offset += p.second.size;
    }

    // The offset is now also the size. Use that to populate the data array
    if (offset > 0) {
        data.resize(offset);
        offset = 0;
        for (const std::pair<uint32_t, BinaryString>& p : constant_map) {
            // Copy the element's data to the array
            memcpy((void*) (data.rdata() + offset), p.second.data, p.second.size);

            // Increment the offset once more
            offset += p.second.size;
        }
    } else {
        data.clear();
    }

    // Done
    DRETURN;
}





/***** POPULATE FUNCTIONS *****/
/* Function that populates a VkSpecializationInfo struct based on the given map of constant_ids to values. */
static void populate_specialization_info(VkSpecializationInfo& specialization_info, const Tools::Array<VkSpecializationMapEntry>& map_entries, const Tools::Array<uint8_t>& data) {
    DENTER("populate_specialization_info");

    // Initialize to default
    specialization_info = {};
    
    // Set the specialization map
    specialization_info.mapEntryCount = map_entries.size();
    specialization_info.pMapEntries = map_entries.rdata();

    // Set the data array
    specialization_info.dataSize = data.size();
    specialization_info.pData = (void*) data.rdata();

    // Done
    DRETURN;
}

/* Populates the given VkPipelineShaderStageCreateInfo struct. */
static void populate_shader_info(VkPipelineShaderStageCreateInfo& shader_info, VkShaderStageFlagBits vk_shader_stage, const Shader& shader, const VkSpecializationInfo& specialization_info) {
    DENTER("populate_shader_info");

    // Set to default
    shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    // Set the shader's stage
    shader_info.stage = vk_shader_stage;

    // Add the shader module and its entry function
    shader_info.module = shader.shader_module();
    shader_info.pName = shader.entry_function().c_str();

    // Finally, set the specialization info for this shader
    shader_info.pSpecializationInfo = &specialization_info;

    DRETURN;
}

/* Populates the given VkPipelineVertexInputStateCreateInfo struct. */
static void populate_vertex_state_info(VkPipelineVertexInputStateCreateInfo& vertex_state_info, const Tools::Array<VkVertexInputBindingDescription>& vk_binding_descriptions, const Tools::Array<VkVertexInputAttributeDescription>& vk_attribute_descriptions) {
    DENTER("populate_vertex_state_info");

    // Set to default
    vertex_state_info = {};
    vertex_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Set the bindings
    vertex_state_info.vertexBindingDescriptionCount = vk_binding_descriptions.size();
    vertex_state_info.pVertexBindingDescriptions = vk_binding_descriptions.rdata();
    
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
Pipeline::Pipeline(const Vulkan::GPU& gpu) :
    gpu(gpu),
    vk_pipeline_layout(nullptr)
{
    DENTER("Vulkan::Pipeline::Pipeline");
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
    vk_specialization_entries(other.vk_specialization_entries),
    vk_specialization_datas(other.vk_specialization_datas),
    vk_specialization_infos(other.vk_specialization_infos),
    vk_shader_infos(other.vk_shader_infos),

    vk_vertex_state_info(other.vk_vertex_state_info),
    vk_assembly_state_info(other.vk_assembly_state_info),
    vk_viewport(other.vk_viewport),
    vk_scissor(other.vk_scissor),
    vk_viewport_state_info(other.vk_viewport_state_info),
    vk_rasterizer_state_info(other.vk_rasterizer_state_info),
    vk_multisample_state_info(other.vk_multisample_state_info),
    vk_color_blending(other.vk_color_blending),
    vk_color_state_info(other.vk_color_state_info)
{
    DENTER("Vulkan::Pipeline::Pipeline(copy)");

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

    vk_descriptor_set_layouts(other.vk_descriptor_set_layouts),
    vk_push_constants(other.vk_push_constants),
    vk_layout_info(other.vk_layout_info),
    vk_pipeline_layout(other.vk_pipeline_layout),

    shaders(other.shaders),
    vk_specialization_entries(other.vk_specialization_entries),
    vk_specialization_datas(other.vk_specialization_datas),
    vk_specialization_infos(other.vk_specialization_infos),
    vk_shader_infos(other.vk_shader_infos),

    vk_vertex_state_info(other.vk_vertex_state_info),
    vk_assembly_state_info(other.vk_assembly_state_info),
    vk_viewport(other.vk_viewport),
    vk_scissor(other.vk_scissor),
    vk_viewport_state_info(other.vk_viewport_state_info),
    vk_rasterizer_state_info(other.vk_rasterizer_state_info),
    vk_multisample_state_info(other.vk_multisample_state_info),
    vk_color_blending(other.vk_color_blending),
    vk_color_state_info(other.vk_color_state_info)
{
    // Set the deallocatable fields to nullptrs
    other.vk_pipeline = nullptr;
    other.vk_pipeline_layout = nullptr;
}

/* Destructor for the Pipeline class. */
Pipeline::~Pipeline() {
    DENTER("Vulkan::Pipeline::~Pipeline");
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
void Pipeline::init_shader_stage(VkShaderStageFlagBits vk_shader_stage, const Vulkan::Shader& shader, const std::unordered_map<uint32_t, BinaryString>& specialization_constants) {
    DENTER("Vulkan::Pipeline::init_shader_stage");

    // Store the shader
    this->shaders.push_back(shader);

    // Flatten the specialization map first
    this->vk_specialization_entries.push_back(Tools::Array<VkSpecializationMapEntry>({}));
    this->vk_specialization_datas.push_back(Tools::Array<uint8_t>({}));
    flatten_specialization_map(specialization_constants, this->vk_specialization_entries.last(), this->vk_specialization_datas.last());
    // Populate the relevant specialization info from the flattened map
    this->vk_specialization_infos.push_back({});
    populate_specialization_info(this->vk_specialization_infos.last(), this->vk_specialization_entries.last(), this->vk_specialization_datas.last());

    // Spawn a new create info for this stage
    this->vk_shader_infos.push_back({});
    // Populate it with the shader
    populate_shader_info(this->vk_shader_infos.last(), vk_shader_stage, shader, this->vk_specialization_infos.last());

    // We're done
    DINDENT;
    DLOG(info, "Initialized Pipeline shader for the " + vk_shader_stage_map[vk_shader_stage] + " stage");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how its vertex input looks like. */
void Pipeline::init_vertex_input() {
    DENTER("Vulkan::Pipeline::init_vertex_input");

    // Prepare the interal VkPipelineVertexInputStateCreateInfo struct
    populate_vertex_state_info(this->vk_vertex_state_info, Tools::Array<VkVertexInputBindingDescription>({}), Tools::Array<VkVertexInputAttributeDescription>({}));

    DINDENT;
    DLOG(info, "Initialized Pipeline vertex input");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline what to do with the vertex we gave it. */
void Pipeline::init_input_assembly(VkPrimitiveTopology topology, VkBool32 restart_enable) {
    DENTER("Vulkan::Pipeline::init_input_assembly");

    // Prepare the interal VkPipelineInputAssemblyStateCreateInfo struct
    populate_assembly_state_info(this->vk_assembly_state_info, topology, restart_enable);

    DINDENT;
    DLOG(info, "Initialized Pipeline input assembly");
    DDEDENT;
    DRETURN;
}

/* Tells the Pipeline how the render the output frame. The Viewport is used to stretch it, and the Scissor is used to cut it off. */
void Pipeline::init_viewport_transformation(const Rectangle& viewport, const Rectangle& scissor) {
    DENTER("Vulkan::Pipeline::init_viewport_transformation");

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
    DENTER("Vulkan::Pipeline::init_rasterizer");

    // Simply populate the correct struct
    populate_rasterizer_state_info(this->vk_rasterizer_state_info, cull_mode, front_face, depth_clamp, polygon_mode, line_width, disable_rasterizer);

    DINDENT;
    DLOG(info, "Initialized Pipeline rasterizer");
    DDEDENT;
    DRETURN;
}

/* Tells the pipeline how to handle multi-sampling. For now, perpetuably disabled. */
void Pipeline::init_multisampling() {
    DENTER("Vulkan::Pipeline::init_multisampling");

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
    DENTER("Vulkan::Pipeline::init_color_blending");

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
    DENTER("Vulkan::Pipeline::init_color_logic");

    // Populate the color blend state with the array we collected with init_color_blending() calls
    populate_color_state_info(this->vk_color_state_info, this->vk_color_blending, enable_logic, logic_op);

    DINDENT;
    DLOG(info, "Initialized Pipeline general color blending");
    DDEDENT;
    DRETURN;
}

/* Initiates the pipeline layout based on the list of descriptor set layouts and on the list of push constants. */
void Pipeline::init_pipeline_layout(const Tools::Array<DescriptorSetLayout>& layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constants) {
    DENTER("Vulkan::Pipeline::init_pipeline_layout");

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
    DENTER("Vulkan::Pipeline::finalize");

    // Finally, begin prepare the create info
    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // First, attach the shaders
    pipeline_info.stageCount = this->vk_shader_infos.size();
    pipeline_info.pStages = this->vk_shader_infos.rdata();

    // Next, attach the fixed-function structures
    pipeline_info.pVertexInputState = &this->vk_vertex_state_info;
    pipeline_info.pInputAssemblyState = &this->vk_assembly_state_info;
    pipeline_info.pViewportState = &this->vk_viewport_state_info;
    pipeline_info.pRasterizationState = &this->vk_rasterizer_state_info;
    pipeline_info.pMultisampleState = &this->vk_multisample_state_info;
    pipeline_info.pDepthStencilState = nullptr;
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
    DLOG(info, "awesomer???");
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->vk_pipeline)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create graphics pipeline: " + vk_error_map[vk_result]);
    }
    DLOG(info, "awesomemest!");

    // Done :)
    DRETURN;
}



/* Schedules the pipeline to be run and thus drawn in the given command buffer. Optionally takes another bind point. */
void Pipeline::schedule(const Vulkan::CommandBuffer& cmd, VkPipelineBindPoint vk_bind_point) {
    DENTER("Vulkan::Pipeline::schedule");

    // Simply bind the pipeline at this point in the command buffer
    vkCmdBindPipeline(cmd, vk_bind_point, this->vk_pipeline);

    // We're done
    DRETURN;
}

/* Schedules the draw call for the pipeline, with the given numer of vertices, instances and vertex & instance offset. */
void Pipeline::schedule_draw(const Vulkan::CommandBuffer& cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    DENTER("Vulkan::Pipeline::schedule_draw");

    // Simply do the draw call
    vkCmdDraw(cmd, vertex_count, instance_count, first_vertex, first_instance);

    // We're done
    DRETURN;
}



/* Swap operator for the Pipeline class. */
void Vulkan::swap(Pipeline& p1, Pipeline& p2) {
    DENTER("Vulkan::swap(Pipeline)");

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
    swap(p1.vk_specialization_entries, p2.vk_specialization_entries);
    swap(p1.vk_specialization_datas, p2.vk_specialization_datas);
    swap(p1.vk_specialization_infos, p2.vk_specialization_infos);
    swap(p1.vk_shader_infos, p2.vk_shader_infos);

    swap(p1.vk_vertex_state_info, p2.vk_vertex_state_info);
    swap(p1.vk_assembly_state_info, p2.vk_assembly_state_info);
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

