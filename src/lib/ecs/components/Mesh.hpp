/* MODEL.hpp
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
 *   Defines the Mesh component, which is used to load and keep track of
 *   model meshes.
**/

#ifndef ECS_MESH_HPP
#define ECS_MESH_HPP

#include "../auxillary/ComponentHash.hpp"
#include "rendering/memory/Buffer.hpp"
#include "tools/Typenames.hpp"

namespace Rasterizer::ECS {
    /* The Mesh component, which is used to add a model mesh to an entity. */
    struct Mesh {
        /* The buffer containing the vertices. */
        Rendering::buffer_h vertices_h;
        /* The buffer containing the indices. */
        Rendering::buffer_h indices_h;
        /* The number of instances used in this mesh. */
        uint32_t n_instances;
    };

    /* Hash function for the Mesh struct, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Mesh>() { return 1; }

}



namespace Tools {
    /* The string name of the Mesh component. */
    template <> inline constexpr const char* type_name<Rasterizer::ECS::Mesh>() { return "ECS::Mesh"; }
}

#endif
