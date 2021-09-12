/* VERTEX INPUT STATE.hpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:09:34
 * Last edited:
 *   12/09/2021, 14:09:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexInputState class, which describes how the vertices
 *   given to a pipeline look like.
**/

#ifndef RENDERING_VERTEX_INPUT_STATE_HPP
#define RENDERING_VERTEX_INPUT_STATE_HPP

#include <vulkan/vulkan.h>
#include "tools/Array.hpp"

#include "VertexBinding.hpp"
#include "VertexAttribute.hpp"

namespace Makma3D::Rendering {
    /* The VertexInputState class, which is a collection of pipeline properties which states how to read the given vertices. */
    class VertexInputState {
    private:
        /* The list of VertexBindings that describe all bindings. */
        Tools::Array<VertexBinding> vertex_bindings;
        /* The casted list of VkVertexInputBindingDescriptions that decribes all bindings the Vulkan way. */
        Tools::Array<VkVertexInputBindingDescription> vk_vertex_bindings;
        
        /* The list of VertexAttribute that describe all the vertex' attributes. */
        Tools::Array<VertexAttribute> vertex_attributes;
        /* The casted list of VkVertexInputAttributeDescriptions that decribes all the vertex' attributes the Vulkan way. */
        Tools::Array<VkVertexInputAttributeDescription> vk_vertex_attributes;

        /* The VkPipelineVertexInputStateCreateInfo object that we wrap. */
        VkPipelineVertexInputStateCreateInfo vk_vertex_input_state;

    public:
        /* Constructor for the VertexInputState, which takes a list of VertexInputBindings and a list of VertexAttributes that describe how the vertices look like. */
        VertexInputState(const Tools::Array<VertexBinding>& vertex_bindings, const Tools::Array<VertexAttribute>& vertex_attributes);
        /* Copy constructor for the VertexInputState class. */
        VertexInputState(const VertexInputState& other);
        /* Move constructor for the VertexInputState class. */
        VertexInputState(VertexInputState&& other);
        /* Destructor for the VertexInputState class. */
        ~VertexInputState();

        /* Explicitly returns the internal VkPipelineVertexInputStateCreateInfo object. */
        inline const VkPipelineVertexInputStateCreateInfo& info() const { return this->vk_vertex_input_state; }
        /* Implicitly returns the internal VkPipelineVertexInputStateCreateInfo object. */
        inline operator const VkPipelineVertexInputStateCreateInfo&() const { return this->vk_vertex_input_state; }

        /* Copy assignment operator for the VertexInputState class. */
        inline VertexInputState& operator=(const VertexInputState& other) { return *this = VertexInputState(other); }
        /* Move assignment operator for the VertexInputState class. */
        inline VertexInputState& operator=(VertexInputState&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the VertexInputState class. */
        friend void swap(VertexInputState& vis1, VertexInputState& vis2);

    };
    
    /* Swap operator for the VertexInputState class. */
    void swap(VertexInputState& vis1, VertexInputState& vis2);

}

#endif
