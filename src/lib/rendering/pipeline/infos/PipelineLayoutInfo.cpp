/* PIPELINE LAYOUT INFO.cpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 15:25:06
 * Last edited:
 *   19/09/2021, 15:25:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "PipelineLayoutInfo.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineLayoutCreateInfo struct. */
static void populate_pipeline_layout_info(VkPipelineLayoutCreateInfo& pipeline_layout_info, const VkDescriptorSetLayout* vk_layouts, uint32_t vk_layouts_size, const VkPushConstantRange* vk_push_constants, uint32_t vk_push_constants_size) {
    // Set to default first
    pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Bind the descriptor set layouts
    pipeline_layout_info.setLayoutCount = vk_layouts_size;
    pipeline_layout_info.pSetLayouts = vk_layouts;

    // Bind the push constant descriptions
    pipeline_layout_info.pushConstantRangeCount = vk_push_constants_size;
    pipeline_layout_info.pPushConstantRanges = vk_push_constants;
}





/***** PIPELINELAYOUTINFO CLASS *****/
/* Constructor for the PipelineLayoutInfo class, which takes a normal PipelineLayout object to initialize itself with. */
PipelineLayoutInfo::PipelineLayoutInfo(const Rendering::PipelineLayout& pipeline_layout) :
    descriptor_set_layouts(pipeline_layout.descriptor_layouts)
{
    // First, initialize the list of vulkan descriptor set layouts
    this->vk_descriptor_set_layouts_size = this->descriptor_set_layouts.size();
    this->vk_descriptor_set_layouts = new VkDescriptorSetLayout[this->vk_descriptor_set_layouts_size];
    for (uint32_t i = 0; i < this->vk_descriptor_set_layouts_size; i++) {
        this->vk_descriptor_set_layouts[i] = this->descriptor_set_layouts[i].vulkan();
    }

    // Next, prepare the list of push constants
    this->vk_push_constant_ranges_size = pipeline_layout.push_constant_layouts.size();
    this->vk_push_constant_ranges = new VkPushConstantRange[this->vk_push_constant_ranges_size];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < pipeline_layout.push_constant_layouts.size(); i++) {
        // For this push constant, align the size to four
        uint32_t aligned_size = pipeline_layout.push_constant_layouts[i].second;
        if (aligned_size % 4 != 0) { aligned_size += (4 - aligned_size % 4); }

        // Initialize the push constant range to default
        this->vk_push_constant_ranges[i] = {};

        // Set the offset + size of the push constant
        this->vk_push_constant_ranges[i].offset = offset;
        this->vk_push_constant_ranges[i].size = aligned_size;

        // Set the stage flags
        this->vk_push_constant_ranges[i].stageFlags = pipeline_layout.push_constant_layouts[i].first;

        // Before we continue, update the offset
        offset += aligned_size;
    }

    // With both lists populated, create the info
    populate_pipeline_layout_info(this->vk_pipeline_layout_info, this->vk_descriptor_set_layouts, this->vk_descriptor_set_layouts_size, this->vk_push_constant_ranges, this->vk_push_constant_ranges_size);
}

/* Copy constructor for the PipelineLayoutInfo class. */
PipelineLayoutInfo::PipelineLayoutInfo(const PipelineLayoutInfo& other) :
    descriptor_set_layouts(other.descriptor_set_layouts),

    vk_descriptor_set_layouts_size(other.vk_descriptor_set_layouts_size),
    vk_push_constant_ranges_size(other.vk_push_constant_ranges_size),
    vk_pipeline_layout_info(other.vk_pipeline_layout_info)
{
    // Copy the list of descriptor set layouts first
    this->vk_descriptor_set_layouts = new VkDescriptorSetLayout[this->vk_descriptor_set_layouts_size];
    for (uint32_t i = 0; i < this->vk_descriptor_set_layouts_size; i++) {
        this->vk_descriptor_set_layouts[i] = this->descriptor_set_layouts[i].vulkan();
    }

    // Next, copy the list of push constants ranges
    this->vk_push_constant_ranges = new VkPushConstantRange[this->vk_push_constant_ranges_size];
    memcpy(this->vk_push_constant_ranges, other.vk_push_constant_ranges, this->vk_push_constant_ranges_size * sizeof(VkPushConstantRange));

    // Finally, edit the pointers in the main info
    this->vk_pipeline_layout_info.pSetLayouts = this->vk_descriptor_set_layouts;
    this->vk_pipeline_layout_info.pPushConstantRanges = this->vk_push_constant_ranges;
}

/* Move constructor for the PipelineLayoutInfo class. */
PipelineLayoutInfo::PipelineLayoutInfo(PipelineLayoutInfo&& other) :
    descriptor_set_layouts(std::move(other.descriptor_set_layouts)),

    vk_descriptor_set_layouts(other.vk_descriptor_set_layouts),
    vk_descriptor_set_layouts_size(other.vk_descriptor_set_layouts_size),

    vk_push_constant_ranges(other.vk_push_constant_ranges),
    vk_push_constant_ranges_size(other.vk_push_constant_ranges_size),

    vk_pipeline_layout_info(other.vk_pipeline_layout_info)
{
    // Prevent deallocation on the other's side
    other.vk_descriptor_set_layouts = nullptr;
    other.vk_push_constant_ranges = nullptr;
}

/* Destructor for the PipelineLayoutInfo class. */
PipelineLayoutInfo::~PipelineLayoutInfo() {
    if (this->vk_push_constant_ranges != nullptr) {
        delete[] this->vk_push_constant_ranges;
    }
    if (this->vk_descriptor_set_layouts != nullptr) {
        delete[] this->vk_descriptor_set_layouts;
    }
}



/* Swap operator for the PipelineLayoutInfo class. */
void Rendering::swap(PipelineLayoutInfo& pli1, PipelineLayoutInfo& pli2) {
    using std::swap;

    swap(pli1.descriptor_set_layouts, pli2.descriptor_set_layouts);

    swap(pli1.vk_descriptor_set_layouts, pli2.vk_descriptor_set_layouts);
    swap(pli1.vk_descriptor_set_layouts_size, pli2.vk_descriptor_set_layouts_size);
    
    swap(pli1.vk_push_constant_ranges, pli2.vk_push_constant_ranges);
    swap(pli1.vk_push_constant_ranges_size, pli2.vk_push_constant_ranges_size);

    swap(pli1.vk_pipeline_layout_info, pli2.vk_pipeline_layout_info);
}
