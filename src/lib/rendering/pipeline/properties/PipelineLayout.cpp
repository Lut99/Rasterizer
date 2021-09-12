/* PIPELINE LAYOUT.cpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 11:23:12
 * Last edited:
 *   12/09/2021, 11:23:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineLayout class, which describes the layout of the
 *   pipeline in terms of push constants and descriptors.
**/

#include "tools/Logger.hpp"
#include "../../auxillary/ErrorCodes.hpp"

#include "PipelineLayout.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPushConstantRange struct. */
static void populate_push_constant_range(VkPushConstantRange& push_constant_range, uint32_t offset, uint32_t size, VkShaderStageFlags vk_shader_stage) {
    // Set to default first
    push_constant_range = {};
    
    // Set the offset + size of the push constant
    push_constant_range.offset = offset;
    push_constant_range.size = size;

    // Set the stage flags
    push_constant_range.stageFlags = vk_shader_stage;
}

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





/***** PIPELINELAYOUT CLASS *****/
/* Constructor for the PipelineLayout class, which takes a GPU where it will live, a list of DescriptorSetLayouts and a list of pairs describing the layout of the push constants: each constant has a shader stage where it is accessed and a size, in bytes. */
PipelineLayout::PipelineLayout(const Rendering::GPU& gpu, const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constant_layouts) :
    gpu(gpu),

    descriptor_layouts(descriptor_layouts)
{
    logger.logc(Verbosity::details, PipelineLayout::channel, "Initializing...");

    // Start by 'casting' the array of DescriptorSetLayouts to VkDescriptorSetLayouts
    logger.logc(Verbosity::details, PipelineLayout::channel, "Preparing descriptor set layouts...");
    this->vk_descriptor_layouts.reserve(this->descriptor_layouts.size());
    for (uint32_t i = 0; i < this->descriptor_layouts.size(); i++) {
        this->vk_descriptor_layouts.push_back(this->descriptor_layouts[i].descriptor_set_layout());
    }

    // Next, construct the push constant ranges
    logger.logc(Verbosity::details, PipelineLayout::channel, "Preparing push constant ranges...");
    uint32_t offset = 0;
    this->vk_push_constant_ranges.resize(push_constant_layouts.size());
    for (uint32_t i = 0; i < push_constant_layouts.size(); i++) {
        // For this push constant, align the size to four
        uint32_t aligned_size = push_constant_layouts[i].second;
        if (aligned_size % 4 != 0) { aligned_size += (4 - aligned_size % 4); }

        // Populate the struct
        populate_push_constant_range(this->vk_push_constant_ranges[i], offset, aligned_size, push_constant_layouts[i].first);

        // Before we continue, update the offset
        offset += aligned_size;
    }

    // Finally, create the VkPipelineLayout itself
    logger.logc(Verbosity::details, PipelineLayout::channel, "Creating VkPipelineLayout...");
    VkPipelineLayoutCreateInfo layout_info;
    populate_layout_info(layout_info, this->vk_descriptor_layouts, this->vk_push_constant_ranges);

    VkResult vk_result;
    if ((vk_result = vkCreatePipelineLayout(this->gpu, &layout_info, nullptr, &this->vk_layout)) != VK_SUCCESS) {
        logger.fatalc(PipelineLayout::channel, "Could not create pipeline layout: ", vk_error_map[vk_result]);   
    }

    // Done
    logger.logc(Verbosity::details, PipelineLayout::channel, "Init success.");
}

/* Copy constructor for the PipelineLayout class. */
PipelineLayout::PipelineLayout(const PipelineLayout& other) :
    gpu(other.gpu),

    descriptor_layouts(other.descriptor_layouts),

    vk_push_constant_ranges(other.vk_push_constant_ranges)
{
    logger.logc(Verbosity::debug, PipelineLayout::channel, "Copying...");

    // Start by 'casting' the array of DescriptorSetLayouts to VkDescriptorSetLayouts
    this->vk_descriptor_layouts.reserve(this->descriptor_layouts.size());
    for (uint32_t i = 0; i < this->descriptor_layouts.size(); i++) {
        this->vk_descriptor_layouts.push_back(this->descriptor_layouts[i].descriptor_set_layout());
    }

    // Finally, recreate the VkPipelineLayout itself
    VkPipelineLayoutCreateInfo layout_info;
    populate_layout_info(layout_info, this->vk_descriptor_layouts, this->vk_push_constant_ranges);
    VkResult vk_result;
    if ((vk_result = vkCreatePipelineLayout(this->gpu, &layout_info, nullptr, &this->vk_layout)) != VK_SUCCESS) {
        logger.fatalc(PipelineLayout::channel, "Could not create pipeline layout: ", vk_error_map[vk_result]);   
    }
    
    logger.logc(Verbosity::debug, PipelineLayout::channel, "Copy success.");
}

/* Move constructor for the PipelineLayout class. */
PipelineLayout::PipelineLayout(PipelineLayout&& other) :
    gpu(other.gpu),

    descriptor_layouts(std::move(other.descriptor_layouts)),
    vk_descriptor_layouts(std::move(other.vk_descriptor_layouts)),

    vk_push_constant_ranges(std::move(other.vk_push_constant_ranges)),

    vk_layout(other.vk_layout)
{
    // Prevent the layout from being deallocated
    other.vk_layout = nullptr;
}

/* Destructor for the PipelineLayout class. */
PipelineLayout::~PipelineLayout() {
    logger.logc(Verbosity::details, PipelineLayout::channel, "Cleaning...");

    // Deallocate the layout if needed
    if (this->vk_layout != nullptr) {
        logger.logc(Verbosity::details, PipelineLayout::channel, "Cleaning VkPipelineLayout...");
        vkDestroyPipelineLayout(this->gpu, this->vk_layout, nullptr);
    }
    
    logger.logc(Verbosity::details, PipelineLayout::channel, "Cleaned.");
}



/* Swap operator for the PipelineLayout class. */
void Rendering::swap(PipelineLayout& pl1, PipelineLayout& pl2) {
    #ifndef NDEBUG
    if (pl1.gpu != pl2.gpu) { logger.fatalc(PipelineLayout::channel, "Cannot swap pipeline layouts with different GPUs"); }
    #endif

    using std::swap;

    swap(pl1.descriptor_layouts, pl2.descriptor_layouts);
    swap(pl1.vk_descriptor_layouts, pl2.vk_descriptor_layouts);

    swap(pl1.vk_push_constant_ranges, pl2.vk_push_constant_ranges);

    swap(pl1.vk_layout, pl2.vk_layout);
}
