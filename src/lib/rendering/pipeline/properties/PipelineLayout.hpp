/* PIPELINE LAYOUT.hpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 11:23:10
 * Last edited:
 *   12/09/2021, 11:23:10
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineLayout class, which describes the layout of the
 *   pipeline in terms of push constants and descriptors.
**/

#ifndef RENDERING_PIPELINE_LAYOUT_HPP
#define RENDERING_PIPELINE_LAYOUT_HPP

#include <cstdint>
#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "../../gpu/GPU.hpp"
#include "../../descriptors/DescriptorSetLayout.hpp"

namespace Makma3D::Rendering {
    /* The PipelineLayout class, which describes the layout of the pipeline (wow). */
    class PipelineLayout {
    public:
        /* The list of descriptor set layouts which we use to define the layout. */
        Tools::Array<Rendering::DescriptorSetLayout> descriptor_layouts;
        /* The list of push constant layouts with which we define the layout. */
        Tools::Array<std::pair<VkShaderStageFlags, uint32_t>> push_constant_layouts;

    public:
        /* Default constructor for the PipelineLayout class. */
        PipelineLayout();
        /* Constructor for the PipelineLayout class, which takes a list of DescriptorSetLayouts and a list of pairs describing the layout of the push constants: each constant has a shader stage where it is accessed and a size, in bytes. */
        PipelineLayout(const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constant_layouts);

        /* Casts the internal list of descriptor set layouts to VkDescriptorSetLayout objects. */
        Tools::Array<VkDescriptorSetLayout> get_layouts() const;
        /* Casts the internal list of push constant layouts to VkPushConstantRanges. */
        Tools::Array<VkPushConstantRange> get_ranges() const;

    };
}

#endif
