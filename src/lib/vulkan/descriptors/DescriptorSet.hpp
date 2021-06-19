/* DESCRIPTOR SET.hpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:47:50
 * Last edited:
 *   19/06/2021, 12:47:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorSet class, which wraps a pre-allocated
 *   VkDescriptorSet object. Only usable in the context of the
 *   DescriptorPool.
**/

#ifndef VULKAN_DESCRIPTOR_SET_HPP
#define VULKAN_DESCRIPTOR_SET_HPP

#include <vulkan/vulkan.h>

#include "vulkan/gpu/GPU.hpp"
#include "vulkan/memory/Buffer.hpp"
#include "vulkan/commandbuffers/CommandBuffer.hpp"

namespace Rasterizer::Vulkan {
    /* Handle for DescriptorSets, which can be used to retrieve them from the Pool. Note that each pool has its own set of handles. */
    using descriptor_set_h = uint32_t;

    /* The DescriptorSet class, which represents a reference to a single VkDescriptorSet. */
    class DescriptorSet {
    private:
        /* The handle of this set. */
        descriptor_set_h vk_handle;
        /* The VkDescriptorSet object that this class wraps. */
        VkDescriptorSet vk_descriptor_set;

        /* Constructor for the DescriptorSet class, which takes the vk_descriptor_set it wraps. */
        DescriptorSet(descriptor_set_h handle, VkDescriptorSet vk_descriptor_set);

        /* Mark the DescriptorPool as friend. */
        friend class DescriptorPool;

    public:
        /* Binds this descriptor set with the contents of a given buffer to the given bind index. */
        void bind(const Vulkan::GPU& gpu, VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<Vulkan::Buffer>& buffers) const;
        /* Binds this descriptor set with the contents of a given image view to the given bind index. Must be enough views to actually populate all bindings of the given type. */
        void bind(const Vulkan::GPU& gpu, VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<VkImageView>& image_views) const;
        /* Binds the descriptor to the given (compute) command buffer. We assume that the recording already started. */
        void record(const Vulkan::CommandBuffer& buffer, VkPipelineLayout pipeline_layout) const;

        /* Explicity returns the internal VkDescriptorSet object. */
        inline const VkDescriptorSet& descriptor_set() const { return this->vk_descriptor_set; }
        /* Implicitly returns the internal VkDescriptorSet object. */
        inline operator VkDescriptorSet() const { return this->vk_descriptor_set; }
        /* Explicity returns the internal descriptor set handle. */
        inline const descriptor_set_h& handle() const { return this->vk_handle; }
        /* Implicitly returns the internal descriptor set handle. */
        inline operator descriptor_set_h() const { return this->vk_handle; }

        /* Swap operator for the DescriptorSet. */
        friend void swap(DescriptorSet& ds1, DescriptorSet& ds2);
        
    };

    /* Swap operator for the DescriptorSet. */
    void swap(DescriptorSet& ds1, DescriptorSet& ds2);
}

#endif
