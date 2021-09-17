/* VERTEX ATTRIBUTE.hpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:21:32
 * Last edited:
 *   12/09/2021, 14:21:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexAttribute class, which describes how a single
 *   attribute (i.e., field) of the vertices given to a certain pipeline
 *   looks like.
**/

#ifndef RENDERING_VERTEX_ATTRIBUTE_HPP
#define RENDERING_VERTEX_ATTRIBUTE_HPP

#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The VertexAttribute class, which describes the binding of a single Vertex buffer. */
    class VertexAttribute {
    private:
        /* The VkVertexInputAttributeDescription struct that we wrap. */
        VkVertexInputAttributeDescription vk_attribute_description;

    public:
        /* Constructor for the VertexAttribute class, which takes the binding index for the buffer, the location index of this attribute, the offset of the attribute in the Vertex and the VkFormat describing the size of the attribute. */
        VertexAttribute(uint32_t bind_index, uint32_t location, uint32_t offset, VkFormat vk_format);

        /* Explicitly returns the internal VkVertexInputAttributeDescription object. */
        inline const VkVertexInputAttributeDescription& description() const { return this->vk_attribute_description; }
        /* Implicitly returns the internal VkVertexInputAttributeDescription object. */
        inline operator const VkVertexInputAttributeDescription&() const { return this->vk_attribute_description; }

    };

}

#endif

