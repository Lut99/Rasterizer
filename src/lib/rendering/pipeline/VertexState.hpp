/* VERTEX STATE.hpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 11:53:44
 * Last edited:
 *   30/07/2021, 11:53:44
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexState class, which manages an
 *   VkPipelineVertexInputStateCreateInfo struct and associated pointers.
**/

#ifndef RENDERING_VERTEX_STATE_HPP
#define RENDERING_VERTEX_STATE_HPP

#include <vulkan/vulkan.h>
#include "tools/Array.hpp"

namespace Makma3D::Rendering {
    /* The VertexState class, which manages associated memory for a VkPipelineVertexInputStateCreateInfo struct. */
    class VertexState {
    private:
        /* The VkPipelineVertexInputStateCreateInfo struct we wrap. */
        VkPipelineVertexInputStateCreateInfo vk_vertex_state_info;

        /* Pointer to the heap-allocated VkVertexInputBindingDescription we keep track of. */
        VkVertexInputBindingDescription* vk_vertex_binding;
        /* Pointer to the heap-allocated VkVertexInputBindingDescription we keep track of. */
        Tools::Array<VkVertexInputAttributeDescription> vk_vertex_attributes;

    public:
        /* Constructor for the VertexState class, which takes a VkVertexInputBindingDescription and a list of VkVertexInputAttributeDescriptions. */
        VertexState(const VkVertexInputBindingDescription& vk_vertex_binding, const Tools::Array<VkVertexInputAttributeDescription>& vk_vertex_attributes);
        /* Copy constructor for the VertexState class. */
        VertexState(const VertexState& other);
        /* Move constructor for the VertexState class. */
        VertexState(VertexState&& other);
        /* Destructor for the VertexState class. */
        ~VertexState();

        /* Explicitly returns the internal VkPipelineVertexInputStateCreateInfo struct. */
        inline const VkPipelineVertexInputStateCreateInfo& vertex_state_info() const { return this->vk_vertex_state_info; }
        /* Implicitly returns the internal VkPipelineVertexInputStateCreateInfo struct. */
        inline operator const VkPipelineVertexInputStateCreateInfo&() const { return this->vk_vertex_state_info; }

        /* Copy assignment operator for the VertexState class. */
        inline VertexState& operator=(const VertexState& other) { return *this = VertexState(other); }
        /* Move assignment operator for the VertexState class. */
        inline VertexState& operator=(VertexState&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the VertexState class. */
        friend void swap(VertexState& vs1, VertexState& vs2);

    };
    
    /* Swap operator for the VertexState class. */
    void swap(VertexState& vs1, VertexState& vs2);

}

#endif
