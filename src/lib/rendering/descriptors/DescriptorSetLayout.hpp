/* DESCRIPTOR SET LAYOUT.hpp
 *   by Lut99
 *
 * Created:
 *   26/04/2021, 15:33:48
 * Last edited:
 *   9/20/2021, 10:49:35 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorSetLayout class, which describes the format for 
 *   a single type of buffer.
**/

#ifndef COMPUTE_DESCRIPTOR_SET_LAYOUT_HPP
#define COMPUTE_DESCRIPTOR_SET_LAYOUT_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "../gpu/GPU.hpp"

namespace Makma3D::Rendering {
    /* The DescriptorSetLayout class, which describes the layout for a single type of buffer. */
    class DescriptorSetLayout {
    public:
        /* Channel name for the DescriptorSetLayout class. */
        static constexpr const char* channel = "DescriptorSetLayout";

        /* Constant reference to the device for which we defined this DescriptorSetLayout. */
        const Rendering::GPU& gpu;

    private:
        /* The actual VkDescriptorSetLayout we wrap. */
        VkDescriptorSetLayout vk_descriptor_set_layout;
        /* Keeps track of all bindings defined. */
        Tools::Array<VkDescriptorSetLayoutBinding> vk_bindings;

    public:
        /* Constructor for the DescriptorSetLayout class, which takes a gpu to bind the buffer to, the index of this bind as seen on the shader, the type of the buffer we describe for and the shader stage where the uniform buffer will eventually be bound to. */
        DescriptorSetLayout(const Rendering::GPU& gpu);
        /* Copy constructor for the DescriptorSetLayout class, which is deleted. */
        DescriptorSetLayout(const DescriptorSetLayout& other);
        /* Move constructor for the DescriptorSetLayout class. */
        DescriptorSetLayout(DescriptorSetLayout&& other);
        /* Destructor for the DescriptorSetLayout class. */
        ~DescriptorSetLayout();

        /* Adds a binding to the DescriptorSetLayout; i.e., one type of resource that a single descriptorset will bind. Returns the binding index of this binding. */
        uint32_t add_binding(VkDescriptorType vk_descriptor_type, uint32_t n_descriptors, VkShaderStageFlags vk_shader_stage);
        /* Finalizes the descriptor layout. Note that no more bindings can be added after this point. */
        void finalize();

        /* Expliticly returns the internal VkDescriptorSetLayout object. */
        inline const VkDescriptorSetLayout& vulkan() const { return this->vk_descriptor_set_layout; }
        /* Implicitly casts this class to a VkDescriptorSetLayout by returning the internal object. */
        inline operator VkDescriptorSetLayout() const { return this->vk_descriptor_set_layout; }

        /* Copy assignment operator for the DescriptorSetLayout class, which is deleted. */
        DescriptorSetLayout& operator=(const DescriptorSetLayout& other) { return *this = DescriptorSetLayout(other); };
        /* Move assignment operator for the DescriptorSetLayout class. */
        DescriptorSetLayout& operator=(DescriptorSetLayout&& other);
        /* Swap operator for the DescriptorSetLayout class. */
        friend void swap(DescriptorSetLayout& dsl1, DescriptorSetLayout& dsl2);
    
    };

    /* Swap operator for the DescriptorSetLayout class. */
    void swap(DescriptorSetLayout& dsl1, DescriptorSetLayout& dsl2);
}

#endif
