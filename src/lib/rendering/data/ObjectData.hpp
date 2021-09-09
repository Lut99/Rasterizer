/* OBJECT DATA.hpp
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
 *   object.
**/

#ifndef RENDERING_OBJECT_DATA_HPP
#define RENDERING_OBJECT_DATA_HPP

#include "glm/glm.hpp"

namespace Rasterizer::Rendering {
    /* The ObjectData struct, which is used to carry object-specific data to the GPU. */
    struct ObjectData {
        /* The translation matrix of the Object. */
        glm::mat4 translation;
    };
}

#endif
