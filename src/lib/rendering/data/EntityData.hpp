/* ENTITY DATA.hpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 19:37:58
 * Last edited:
 *   08/09/2021, 19:37:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Struct that contains data that can be uploaded for each to-be-rendered
 *   entity.
**/

#ifndef RENDERING_ENTITY_DATA_HPP
#define RENDERING_ENTITY_DATA_HPP

#include "glm/glm.hpp"

namespace Makma3D::Rendering {
    /* The EntityData struct, which is used to carry entity-specific data to the GPU. */
    struct EntityData {
        /* The translation matrix of the Entity. */
        glm::mat4 translation;
    };
}

#endif
