/* OBJECT.hpp
 *   by Lut99
 *
 * Created:
 *   17/07/2021, 14:32:36
 * Last edited:
 *   17/07/2021, 14:32:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Object struct, which forms the basis of all objects
 *   managed by the ObjectManager.
**/

#ifndef OBJECTS_ECS_OBJECT_HPP
#define OBJECTS_ECS_OBJECT_HPP

#include <string>
#include "glm/glm.hpp"

namespace Rasterizer::Objects::ECS {
    /* The Object struct, which forms the basis of all objects in the Rasterizer. */
    struct Object {
        /* The position of the object. */
        glm::vec3 pos;

        /* The model associated with the object. */
        std::string model_id;

    };

}

#endif
