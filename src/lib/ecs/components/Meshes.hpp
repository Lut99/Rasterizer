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

namespace Rasterizer::ECS {
    /* The Mesh component, which is used to add several model meshes to an entity. */
    struct Mesh {
        /* The buffers containing the vertices. */
        Rendering::buffer_h vertices_h;
        /* The buffers containing the indices. */
        Rendering::buffer_h indices_h;
        /* The number of indices used in this mesh. */
        uint32_t n_indices;

        /* Auxillary data about the Mesh that will be send to the GPU (CPU-side). */
        Rendering::MeshData data;
        /* Auxillary data about the Mesh that will be send to the GPU (GPU-side). */
        Rendering::buffer_h data_h;
        
        /* Optional name for this mesh, not unique. Only for debugging. */
        std::string name;
        /* Name (ID) of the material used in this mesh. Not used except for debugging. */
        std::string mtl;
    };
    /* Shortcut for the list of Meshes. */
    using Meshes = Tools::Array<Mesh>;

    /* Hash function for the Model struct, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Meshes>() { return 1; }

}



namespace Tools {
    /* The string name of the Model component. */
    template <> inline constexpr const char* type_name<Rasterizer::ECS::Meshes>() { return "ECS::Meshes"; }
}

#endif
