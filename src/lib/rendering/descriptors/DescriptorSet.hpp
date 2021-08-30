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

#ifndef RENDERING_DESCRIPTOR_SET_HPP
#define RENDERING_DESCRIPTOR_SET_HPP

#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"
#include "../memory/Buffer.hpp"
#include "../commandbuffers/CommandBuffer.hpp"

namespace Rasterizer::Rendering {
    /* The DescriptorSet class, which represents a reference to a single VkDescriptorSet. */
    class DescriptorSet {
    public:
        /* The GPU object the set lives on. */
        const Rendering::GPU& gpu;

    private:
        /* The VkDescriptorSet object that this class wraps. */
        VkDescriptorSet vk_descriptor_set;

        /* Constructor for the DescriptorSet class, which only takes the GPU where it lives and the vk_descriptor_set it wraps. */
        DescriptorSet(const Rendering::GPU& gpu, VkDescriptorSet vk_descriptor_set);
        /* Private destructor for the DescriptorSet classs. */
        ~DescriptorSet();

        /* Mark the DescriptorPool as friend. */
        friend class DescriptorPool;

    public:
        /* Copy constructor for the DescriptorSet class, which is deleted. */
        DescriptorSet(const DescriptorSet& other) = delete;
        /* Move constructor for the DescriptorSet class, which is deleted. */
        DescriptorSet(DescriptorSet&& other) = delete;

        /* Binds this descriptor set with the contents of a given buffer to the given bind index. Must be enough buffers to actually populate all bindings of the given type. */
        void bind(VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<Rendering::Buffer*>& buffers) const;
        /* Binds this descriptor set with the contents of a given image view to the given bind index. Must be enough views to actually populate all bindings of the given type. */
        void bind(VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<std::tuple<VkImageView, VkImageLayout>>& image_views) const;
        /* Binds this descriptor set with the contents of a given image view & sampler pair to the given bind index. Must be enough views to actually populate all bindings of the given type. */
        void bind(VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<std::tuple<VkImageView, VkImageLayout, VkSampler>>& view_sampler_pairs) const;
        /* Binds the descriptor to the given (compute) command buffer. We assume that the recording already started. */
        void schedule(const Rendering::CommandBuffer* buffer, VkPipelineLayout pipeline_layout) const;

        /* Explicity returns the internal VkDescriptorSet object. */
        inline const VkDescriptorSet& descriptor_set() const { return this->vk_descriptor_set; }
        /* Implicitly returns the internal VkDescriptorSet object. */
        inline operator const VkDescriptorSet&() const { return this->vk_descriptor_set; }

        /* Copy assignment operator for the DescriptorSet class, which is deleted. */
        DescriptorSet& operator=(const DescriptorSet& other) = delete;
        /* Move assignment operator for the DescriptorSet class, which is deleted. */
        DescriptorSet& operator=(DescriptorSet&& other) = delete;

    };
}

#endif
