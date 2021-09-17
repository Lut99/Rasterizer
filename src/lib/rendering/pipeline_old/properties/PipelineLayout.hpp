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
        /* Channel name for the PipelineLayout class. */
        static constexpr const char* channel = "PipelineLayout";

        /* GPU where the Pipeline layout lives. */
        const Rendering::GPU& gpu;

    private:
        /* The list of descriptor set layouts which we use to define the layout. */
        Tools::Array<Rendering::DescriptorSetLayout> descriptor_layouts;
        /* The list of VkDescriptorSetLayouts that are wrapped in the other list of layouts. */
        Tools::Array<VkDescriptorSetLayout> vk_descriptor_layouts;

        /* The list of VkPushConstantRanges, which combines the info in the list above. */
        Tools::Array<VkPushConstantRange> vk_push_constant_ranges;

        /* The VkPipelineLayout struct that we wrap. */
        VkPipelineLayout vk_layout;
    
    public:
        /* Constructor for the PipelineLayout class, which takes a GPU where it will live, a list of DescriptorSetLayouts and a list of pairs describing the layout of the push constants: each constant has a shader stage where it is accessed and a size, in bytes. */
        PipelineLayout(const Rendering::GPU& gpu, const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constant_layouts);
        /* Copy constructor for the PipelineLayout class. */
        PipelineLayout(const PipelineLayout& other);
        /* Move constructor for the PipelineLayout class. */
        PipelineLayout(PipelineLayout&& other);
        /* Destructor for the PipelineLayout class. */
        ~PipelineLayout();

        /* Explicitly returns the internal VkPipelineLayout object. */
        inline const VkPipelineLayout& layout() const { return this->vk_layout; }
        /* Implicitly returns the internal VkPipelineLayout object. */
        inline operator const VkPipelineLayout&() const { return this->vk_layout; }

        /* Copy assignment operator for the PipelineLayout class. */
        inline PipelineLayout& operator=(const PipelineLayout& other) { return *this = PipelineLayout(other); }
        /* Move assignment operator for the PipelineLayout class. */
        inline PipelineLayout& operator=(PipelineLayout&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineLayout class. */
        friend void swap(PipelineLayout& pl1, PipelineLayout& pl2);

    };
    
    /* Swap operator for the PipelineLayout class. */
    void swap(PipelineLayout& pl1, PipelineLayout& pl2);

}

#endif
