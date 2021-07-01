/* DESCRIPTOR POOL.hpp
 *   by Lut99
 *
 * Created:
 *   26/04/2021, 14:39:16
 * Last edited:
 *   01/07/2021, 13:43:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorPool class, which serves as a memory pool for
 *   descriptors, which in turn describe how a certain buffer or other
 *   piece of memory should be accessed on the GPU.
**/

#ifndef COMPUTE_DESCRIPTOR_POOL_HPP
#define COMPUTE_DESCRIPTOR_POOL_HPP

#include <unordered_map>
#include <vulkan/vulkan.h>

#include "render_engine/gpu/GPU.hpp"
#include "render_engine/memory/MemoryPool.hpp"
#include "render_engine/commandbuffers/CommandPool.hpp"
#include "tools/Array.hpp"

#include "DescriptorSetLayout.hpp"
#include "DescriptorSet.hpp"

namespace Rasterizer::Rendering {
    /* The DescriptorPool class, which is used to generate and manage descriptor(sets) for describing buffers. */
    class DescriptorPool {
    public:
        /* Constant reference to the GPU we allocate this pool on. */
        const Rendering::GPU& gpu;

    private:
        /* The internal pool used for allocating new pools. */
        VkDescriptorPool vk_descriptor_pool;
        /* Type of descriptors that can be allocated here and how many per type. */
        Tools::Array<std::pair<VkDescriptorType, uint32_t>> vk_descriptor_types;
        /* The maximum number of sets allowed in this pool. */
        uint32_t vk_max_sets;
        /* The create flags used to initialize this pool. */
        VkDescriptorPoolCreateFlags vk_create_flags;

        /* Internal list of Descriptors. */
        std::unordered_map<descriptor_set_h, VkDescriptorSet> vk_descriptor_sets;

    public:
        /* The null handle for the pool. */
        const static constexpr descriptor_set_h NullHandle = 0;


        /* Constructor for the DescriptorPool class, which takes the GPU to create the pool on, the type of descriptors, the number of descriptors we want to allocate in the pool, the maximum number of descriptor sets that can be allocated and optionally custom create flags. */
        DescriptorPool(const Rendering::GPU& gpu, const std::pair<VkDescriptorType, uint32_t>& descriptor_type, uint32_t max_sets, VkDescriptorPoolCreateFlags flags = 0);
        /* Constructor for the DescriptorPool class, which takes the GPU to create the pool on, a list of descriptor types and their counts, the maximum number of descriptor sets that can be allocated and optionally custom create flags. */
        DescriptorPool(const Rendering::GPU& gpu, const Tools::Array<std::pair<VkDescriptorType, uint32_t>>& descriptor_types, uint32_t max_sets, VkDescriptorPoolCreateFlags flags = 0);
        /* Copy constructor for the DescriptorPool. */
        DescriptorPool(const DescriptorPool& other);
        /* Move constructor for the DescriptorPool. */
        DescriptorPool(DescriptorPool&& other);
        /* Destructor for the DescriptorPool. */
        ~DescriptorPool();

        /* Returns a DescriptorSet from the given handle, which can be used as a CommandBuffer. Does not perform any checks on the handle validity. */
        inline DescriptorSet deref(descriptor_set_h handle) const { return DescriptorSet(handle, this->vk_descriptor_sets.at(handle)); }

        /* Allocates a single descriptor set with the given layout. Will fail with errors if there's no more space. */
        inline DescriptorSet allocate(const Rendering::DescriptorSetLayout& descriptor_set_layout) { return this->deref(this->allocate_h(descriptor_set_layout)); }
        /* Allocates a single descriptor set with the given layout and returns it by handle. Will fail with errors if there's no more space. */
        descriptor_set_h allocate_h(const Rendering::DescriptorSetLayout& descriptor_set_layout);
        /* Allocates multiple descriptor sets with the given layout, returning them as an Array. Will fail with errors if there's no more space. */
        Tools::Array<DescriptorSet> nallocate(uint32_t n_sets, const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_set_layouts);
        /* Allocates multiple descriptor sets with the given layout, returning them as an Array of handles. Will fail with errors if there's no more space. */
        Tools::Array<descriptor_set_h> nallocate_h(uint32_t n_sets, const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_set_layouts);

        /* Deallocates the descriptor set with the given handle. */
        void deallocate(descriptor_set_h set);
        /* Deallocates an array of given descriptors sets. */
        void ndeallocate(const Tools::Array<DescriptorSet>& descriptor_sets);
        /* Deallocates an array of given descriptors set handles. */
        void ndeallocate(const Tools::Array<descriptor_set_h>& handles);

        /* Returns the current number of sets allocated in this pool. */
        inline size_t size() const { return this->vk_descriptor_sets.size(); }
        /* Returns the maximum number of sets we can allocate in this pool. */
        inline size_t capacity() const { return static_cast<size_t>(this->vk_max_sets); }

        /* Explicitly returns the internal VkDescriptorPool object. */
        inline const VkDescriptorPool& descriptor_pool() const { return this->vk_descriptor_pool; }
        /* Implicitly returns the internal VkDescriptorPool object. */
        inline operator VkDescriptorPool() const { return this->vk_descriptor_pool; }

        /* Copy assignment operator for the DescriptorPool class. */
        inline DescriptorPool& operator=(const DescriptorPool& other) { return *this = DescriptorPool(other); }
        /* Move assignment operator for the DescriptorPool class. */
        inline DescriptorPool& operator=(DescriptorPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the DescriptorPool class. */
        friend void swap(DescriptorPool& dp1, DescriptorPool& dp2);

    };

    /* Swap operator for the DescriptorPool class. */
    void swap(DescriptorPool& dp1, DescriptorPool& dp2);
}

#endif
