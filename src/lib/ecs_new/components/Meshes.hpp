/* MESHES.hpp
 *   by Lut99
 *
 * Created:
 *   29/07/2021, 21:16:53
 * Last edited:
 *   29/07/2021, 21:16:53
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the Meshes component, which is used to load and keep track of
 *   multiple model meshes per entity/model.
**/

#ifndef ECS_MESHES_HPP
#define ECS_MESHES_HPP

#include "glm/glm.hpp"

#include "../auxillary/ComponentHash.hpp"
#include "rendering/memory/Buffer.hpp"
#include "rendering/data/MeshData.hpp"
#include "tools/Array.hpp"
#include "tools/Typenames.hpp"

namespace Makma3D::ECS {
    /* The Mesh component, which is used to add several model meshes to an entity. */
    struct Mesh {
        /* The buffers containing the vertices. */
        Rendering::Buffer* vertices;
        /* The buffers containing the indices. */
        Rendering::Buffer* indices;
        /* The number of indices used in this mesh. */
        uint32_t n_indices;
        
        /* Optional name for this mesh, not unique. Only for debugging. */
        std::string name;
        /* Name (ID) of the material used in this mesh. Not used except for debugging. */
        std::string mtl;
        /* The colour associated with the selected material. */
        glm::vec4 mtl_col;
    };
    /* Shortcut for the list of Meshes. */
    using Meshes = Tools::Array<Mesh>;

    /* Hash function for the Model struct, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Meshes>() { return 1; }

}



namespace Tools {
    /* The string name of the Mesh component. */
    template <> inline constexpr const char* type_name<Makma3D::ECS::Mesh>() { return "ECS::Mesh"; }
    /* The string name of the Meshes list. */
    template <> inline constexpr const char* type_name<Makma3D::ECS::Meshes>() { return "ECS::Meshes"; }
}

#endif
