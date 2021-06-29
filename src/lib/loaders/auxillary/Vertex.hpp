/* VERTEX.hpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 16:27:03
 * Last edited:
 *   29/06/2021, 16:27:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines how a default vertex looks like.
**/

#ifndef LOADERS_VERTEX_HPP
#define LOADERS_VERTEX_HPP

#include "glm/glm.hpp"

namespace Rasterizer::Loaders {
    /* The Vertex class, which is used to represent a vertex on the CPU-side. */
    struct Vertex {
        /* The position of the vertex in 2D. */
        glm::vec2 pos;
        /* The color of the vertex as RGB. */
        glm::vec3 color;
    };

}

#endif
