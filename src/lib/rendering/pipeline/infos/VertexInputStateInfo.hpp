/* VERTEX INPUT STATE INFO.hpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 14:43:02
 * Last edited:
 *   19/09/2021, 14:43:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexInputStateInfo class, which bundles a
 *   VkPipelineVertexInputStateCreateInfo struct with the memory needed for
 *   it to exist.
**/

#ifndef RENDERING_VERTEX_INPUT_STATE_INFO_HPP
#define RENDERING_VERTEX_INPUT_STATE_INFO_HPP

#include <vulkan/vulkan.h>
#include "../properties/VertexInputState.hpp"

namespace Makma3D::Rendering {
    /* The VertexInputStateInfo class, which wraps a VkPipelineVertexInputStateCreateInfo and associated memory. */
    class VertexInputStateInfo {
    private:
        /* The list of bindings that describe how many vertex buffers to add. */
        VkVertexInputBindingDescription* vk_bindings;
        /* The number of bindings in the bindings array. */
        uint32_t vk_bindings_size;
        
        /* The list of attributes of each vertex. */
        VkVertexInputAttributeDescription* vk_attributes;
        /* The number of attributes in the attributes array. */
        uint32_t vk_attributes_size;

        /* The main VkPipelineVertexInputStateCreateInfo struct we wrap. */
        VkPipelineVertexInputStateCreateInfo vk_vertex_input_state_info;

    public:
        /* Constructor for the VertexInputStateInfo class, which takes a normal VertexInputState object to initialize itself with. */
        VertexInputStateInfo(const Rendering::VertexInputState& vertex_input_state);
        /* Copy constructor for the VertexInputStateInfo class. */
        VertexInputStateInfo(const VertexInputStateInfo& other);
        /* Move constructor for the VertexInputStateInfo class. */
        VertexInputStateInfo(VertexInputStateInfo&& other);
        /* Destructor for the VertexInputStateInfo class. */
        ~VertexInputStateInfo();

        /* Expliticly returns the internal VkPipelineVertexInputStateCreateInfo object. */
        inline const VkPipelineVertexInputStateCreateInfo& vulkan() const { return this->vk_vertex_input_state_info; }
        /* Implicitly returns the internal VkPipelineVertexInputStateCreateInfo object. */
        inline operator const VkPipelineVertexInputStateCreateInfo&() const { return this->vk_vertex_input_state_info; }

        /* Copy assignment operator for the VertexInputStateInfo class. */
        inline VertexInputStateInfo& operator=(const VertexInputStateInfo& other) { return *this = VertexInputStateInfo(other); }
        /* Move assignment operator for the VertexInputStateInfo class. */
        inline VertexInputStateInfo& operator=(VertexInputStateInfo&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the VertexInputStateInfo class. */
        friend void swap(VertexInputStateInfo& visi1, VertexInputStateInfo& visi2);

    };
    
    /* Swap operator for the VertexInputStateInfo class. */
    void swap(VertexInputStateInfo& visi1, VertexInputStateInfo& visi2);
}

#endif
