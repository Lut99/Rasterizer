/* MODEL.hpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 16:59:44
 * Last edited:
 *   10/09/2021, 17:06:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Model component, which contains everything needed to
 *   render the meshes of an entity.
**/

#ifndef ECS_MODEL_HPP
#define ECS_MODEL_HPP

#include <string>

#include "tools/Typenames.hpp"
#include "tools/Array.hpp"
#include "rendering/memory/Buffer.hpp"
#include "materials/Material.hpp"

namespace Makma3D::ECS {
    /* Helper struct for the Model component, which contains data about a single mesh for the Model. */
    struct Mesh {
        /* List of indices to render for this mesh. */
        const Rendering::Buffer* indices;
        /* The number of indices to render. */
        uint32_t n_indices;
        /* The material for this mesh. */
        const Materials::Material* material;

        /* Name for this Mesh (only used for debugging). */
        std::string name;

    };



    /* The Model component, which contains everything needed to render all meshes of an entity. */
    struct Model {
        /* All vertices belonging to this Model. Because we sort everything by Vertex, also includes normal and texel coordinates. */
        Rendering::Buffer* vertices;
        /* The number of vertices in this Model. */
        uint32_t n_vertices;

        /* The list of meshes for this entity. */
        Tools::Array<Mesh> meshes;

        /* Name for this Model (only used for debugging). */
        std::string name;

    };

}



namespace Tools {
    /* The string name of the Mesh struct. */
    template <> inline constexpr const char* type_name<Makma3D::ECS::Mesh>() { return "ECS::Mesh"; }
    /* The string name of the Model component. */
    template <> inline constexpr const char* type_name<Makma3D::ECS::Model>() { return "ECS::Model"; }
}

#endif
