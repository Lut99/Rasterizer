/* VERTEX BINDING.hpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:15:29
 * Last edited:
 *   12/09/2021, 14:15:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexBinding class, which groups together the
 *   information needed to describe a vertex buffer binding.
**/

#ifndef RENDERING_VERTEX_BINDING_HPP
#define RENDERING_VERTEX_BINDING_HPP

#include <cstdint>

namespace Makma3D::Rendering {
    /* The VertexBinding struct, which describes the binding of a single Vertex buffer. */
    struct VertexBinding {
        /* The binding index of the vertex buffer. */
        uint32_t bind_index;
        /* The size (in bytes) of a single vertex. */
        uint32_t vertex_size;

        /* Constructor for the VertexBinding class, which takes the binding index and the size of each Vertex object in this buffer. */
        VertexBinding(uint32_t bind_index, uint32_t vertex_size): bind_index(bind_index), vertex_size(vertex_size) {}

    };

}

#endif
