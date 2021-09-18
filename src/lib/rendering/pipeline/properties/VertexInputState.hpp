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
    public:
        /* The list of VertexBindings that describe all bindings. */
        Tools::Array<VertexBinding> vertex_bindings;
        /* The list of VertexAttribute that describe all the vertex' attributes. */
        Tools::Array<VertexAttribute> vertex_attributes;

    public:
        /* Default constructor for the VertexInputState. */
        VertexInputState();
        /* Constructor for the VertexInputState, which takes a list of VertexInputBindings and a list of VertexAttributes that describe how the vertices look like. */
        VertexInputState(const Tools::Array<VertexBinding>& vertex_bindings, const Tools::Array<VertexAttribute>& vertex_attributes);

        /* Casts the internal list of VertexBindings to VkVertexInputBindingDescription structs. */
        Tools::Array<VkVertexInputBindingDescription> get_bindings() const;
        /* Casts the internal list of VertexAttributes to VkVertexInputAttributeDescription structs. */
        Tools::Array<VkVertexInputAttributeDescription> get_attributes() const;
        /* Given a list of vertex input binding descriptions and input attribute descriptions, creates a new VkPipelineVertexInputStateCreateInfo struct. */
        VkPipelineVertexInputStateCreateInfo get_info(const Tools::Array<VkVertexInputBindingDescription>& vk_bindings, const Tools::Array<VkVertexInputAttributeDescription>& vk_attributes) const;

    };

}

#endif
