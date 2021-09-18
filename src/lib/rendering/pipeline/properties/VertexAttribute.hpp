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

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The VertexAttribute class, which describes the binding of a single Vertex buffer. */
    struct VertexAttribute {
        /* The bind index of the array for which we define the attribute. */
        uint32_t bind_index;
        /* The location of the vertex attribute in the shader. */
        uint32_t location;
        /* The offset of the attribute within a single shader object. */
        uint32_t offset;
        /* The format of the attribute as a VkFormat. */
        VkFormat format;

        /* Constructor for the VertexAttribute class, which takes the binding index for the buffer, the location index of this attribute, the offset of the attribute in the Vertex and the VkFormat describing the size of the attribute. */
        VertexAttribute(uint32_t bind_index, uint32_t location, uint32_t offset, VkFormat format):
            bind_index(bind_index), location(location),
            offset(offset), format(format)
        {}

    };

}

#endif

