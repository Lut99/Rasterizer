/* PIPELINE LAYOUT INFO.hpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 15:22:47
 * Last edited:
 *   19/09/2021, 15:22:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelineLayoutInfo class, which bundles everything we
 *   need to the VkPipelineLayoutCreateInfo struct.
**/

#ifndef RENDERING_PIPELINE_LAYOUT_INFO_HPP
#define RENDERING_PIPELINE_LAYOUT_INFO_HPP

#include <vulkan/vulkan.h>
#include "tools/Array.hpp"

#include "../../descriptors/DescriptorSetLayout.hpp"
#include "../properties/PipelineLayout.hpp"

namespace Makma3D::Rendering {
    /* The PipelineLayoutInfo class, which wraps a VkPipelineLayoutCreateInfo struct and all associated infos. */
    class PipelineLayoutInfo {
    private:
        /* The higher level list of descriptor set layouts with which we copy the low-level list. */
        Tools::Array<Rendering::DescriptorSetLayout> descriptor_set_layouts;

        /* The list of VkDescriptorSetLayouts that the pipeline layout needs to know. */
        VkDescriptorSetLayout* vk_descriptor_set_layouts;
        /* The number of layouts stored in the array. */
        uint32_t vk_descriptor_set_layouts_size;

        /* The list of VkPushConstantRanges that the pipeline layout needs to know. */
        VkPushConstantRange* vk_push_constant_ranges;
        /* The number of ranges stored in the array. */
        uint32_t vk_push_constant_ranges_size;

        /* The VkPipelineLayoutCreateInfo struct we wrap. */
        VkPipelineLayoutCreateInfo vk_pipeline_layout_info;

    public:
        /* Constructor for the PipelineLayoutInfo class, which takes a normal PipelineLayout object to initialize itself with. */
        PipelineLayoutInfo(const Rendering::PipelineLayout& pipeline_layout);
        /* Copy constructor for the PipelineLayoutInfo class. */
        PipelineLayoutInfo(const PipelineLayoutInfo& other);
        /* Move constructor for the PipelineLayoutInfo class. */
        PipelineLayoutInfo(PipelineLayoutInfo&& other);
        /* Destructor for the PipelineLayoutInfo class. */
        ~PipelineLayoutInfo();

        /* Expliticly returns the internal VkPipelineLayoutCreateInfo object. */
        inline const VkPipelineLayoutCreateInfo& vulkan() const { return this->vk_pipeline_layout_info; }
        /* Implicitly returns the internal VkPipelineLayoutCreateInfo object. */
        inline operator const VkPipelineLayoutCreateInfo&() const { return this->vk_pipeline_layout_info; }

        /* Copy assignment operator for the PipelineLayoutInfo class. */
        inline PipelineLayoutInfo& operator=(const PipelineLayoutInfo& other) { return *this = PipelineLayoutInfo(other); }
        /* Move assignment operator for the PipelineLayoutInfo class. */
        inline PipelineLayoutInfo& operator=(PipelineLayoutInfo&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineLayoutInfo class. */
        friend void swap(PipelineLayoutInfo& pli1, PipelineLayoutInfo& pli2);

    };
    
    /* Swap operator for the PipelineLayoutInfo class. */
    void swap(PipelineLayoutInfo& pli1, PipelineLayoutInfo& pli2);

}

#endif
