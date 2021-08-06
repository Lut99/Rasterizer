/* CAMERA.hpp
 *   by Lut99
 *
 * Created:
 *   06/08/2021, 13:17:30
 * Last edited:
 *   06/08/2021, 13:20:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the definition for the Camera component, which contains the
 *   camera matrices that should be send to the GPU.
**/

#ifndef ECS_CAMERA_HPP
#define ECS_CAMERA_HPP

#include "glm/glm.hpp"

#include "../auxillary/ComponentHash.hpp"
#include "tools/Typenames.hpp"

namespace Rasterizer::ECS {
    /* The Camera component, which carries the camera matrices around for that camera. Its position and rotation are defined by the Transform component. Also thus determines how many times we have to render a scene. */
    struct Camera {
        /* The projection matrix for the camera. */
        glm::mat4 proj;
        /* The view matrix for the camera. */
        glm::mat4 view;
    };

    /* Hash function for the Camera struct, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Camera>() { return 2; }

}



namespace Tools {
    /* The string name of the Camera component. */
    template <> inline constexpr const char* type_name<Rasterizer::ECS::Camera>() { return "ECS::Camera"; }
}

#endif
