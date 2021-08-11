/* MESH DATA.hpp
 *   by Lut99
 *
 * Created:
 *   11/08/2021, 21:27:24
 * Last edited:
 *   11/08/2021, 21:27:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a struct that is used to transfer mesh data to the GPU.
**/

#ifndef RENDERING_MESH_DATA_HPP
#define RENDERING_MESH_DATA_HPP

#include "glm/glm.hpp"
#include "tools/Typenames.hpp"

namespace Rasterizer::Rendering {
    /* The MeshData struct, which is used to transfer mesh-specific data to the GPU. */
    struct MeshData {
        /* The material colour of the Mesh. */
        glm::vec4 mtl_col;
    };

}



namespace Tools {
    /* The string name of the Model component. */
    template <> inline constexpr const char* type_name<Rasterizer::Rendering::MeshData>() { return "Rendering::MeshData"; }
}

#endif
