/* TRANSFORM.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:23:32
 * Last edited:
 *   18/07/2021, 12:23:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the definition of a Transform component, which describes how an
 *   entity is rendered (mostly its position-related stuff).
**/

#ifndef ECS_TRANSFORM_HPP
#define ECS_TRANSFORM_HPP

#include "glm/glm.hpp"

#include "../auxillary/ComponentHash.hpp"
#include "tools/Typenames.hpp"

namespace Rasterizer::ECS {
    /* The Transform component, which describes everything needed to position a renderable object in the scene. */
    struct Transform {
        /* The position of the entity in world space. */
        glm::vec3 position;
        /* The rotation of the entity, as radians along each of the three axis. */
        glm::vec3 rotation;
        /* The scale of the entity along each of the three axis. */
        glm::vec3 scale;
        /* The resulting translation matrix, usable in the renderer. */
        glm::mat4 translation;
    };

    /* Hash function for the Transform struct, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Transform>() { return 0; }

}



namespace Tools {
    /* The string name of the Transform component. */
    template <> inline constexpr const char* type_name<Rasterizer::ECS::Transform>() { return "ECS::Transform"; }
}

#endif
