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
#include "materials/Material.hpp"
#include "rendering/memory/Buffer.hpp"

namespace Makma3D::ECS {
    /* Helper struct for the Model component, which contains data about a single mesh for the Model. */
    struct Mesh {
        /* The buffer containing the vertex data for this Mesh. */
        Rendering::Buffer* vertices;
        /* The number of vertices stored in the buffer. */
        uint32_t n_vertices;

        /* The buffer containing the index data for this Mesh. */
        Rendering::Buffer* indices;
        /* The number of indices stored in the buffer. */
        uint32_t n_indices;

        /* Link to the material with which the mesh should be rendered. */
        Materials::material_t material;

        /* Name for this Mesh (only used for debugging). */
        std::string name;

    };



    /* The Model component, which contains everything needed to render all meshes of an entity. */
    struct Model {
        /* The list of meshes for this entity. */
        Tools::Array<Mesh> meshes;
    };

}



namespace Tools {
    /* The string name of the Model component. */
    template <> inline constexpr const char* type_name<Makma3D::ECS::Model>() { return "ECS::Model"; }
}

#endif
