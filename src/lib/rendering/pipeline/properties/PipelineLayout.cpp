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





/***** PIPELINELAYOUT CLASS *****/
/* Default constructor for the PipelineLayout class. */
PipelineLayout::PipelineLayout() :
    PipelineLayout({}, {})
{}

/* Constructor for the PipelineLayout class, which takes a list of DescriptorSetLayouts and a list of pairs describing the layout of the push constants: each constant has a shader stage where it is accessed and a size, in bytes. */
PipelineLayout::PipelineLayout(const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constant_layouts) :
    descriptor_layouts(descriptor_layouts),
    push_constant_layouts(push_constant_layouts)
{}



/* Casts the internal list of descriptor set layouts to VkDescriptorSetLayout objects. */
Tools::Array<VkDescriptorSetLayout> PipelineLayout::get_layouts() const {
    Tools::Array<VkDescriptorSetLayout> result(this->descriptor_layouts.size());
    for (uint32_t i = 0; i < this->descriptor_layouts.size(); i++) {
        result.push_back(this->descriptor_layouts[i].descriptor_set_layout());
    }
    return result;
}

/* Casts the internal list of push constant layouts to VkPushConstantRanges. */
Tools::Array<VkPushConstantRange> PipelineLayout::get_ranges() const {
    uint32_t offset = 0;
    Tools::Array<VkPushConstantRange> result(this->push_constant_layouts.size());
    for (uint32_t i = 0; i < this->push_constant_layouts.size(); i++) {
        // For this push constant, align the size to four
        uint32_t aligned_size = this->push_constant_layouts[i].second;
        if (aligned_size % 4 != 0) { aligned_size += (4 - aligned_size % 4); }

        // Populate the struct
        result.push_back({});
        populate_push_constant_range(result[i], offset, aligned_size, this->push_constant_layouts[i].first);

        // Before we continue, update the offset
        offset += aligned_size;
    }
    return result;
}
