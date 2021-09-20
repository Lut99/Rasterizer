/* MATERIAL DATA.hpp
 *   by Lut99
 *
 * Created:
 *   20/09/2021, 15:14:42
 * Last edited:
 *   20/09/2021, 15:14:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a struct for carrying material-related data to and from the
 *   GPU.
**/

#ifndef RENDERING_MATERIAL_DATA_HPP
#define RENDERING_MATERIAL_DATA_HPP

#include "glm/glm.hpp"

namespace Makma3D::Rendering {
    /* The MaterialData struct, which is used to carry material-specific data to the GPU. */
    struct MaterialData {
        /* The colour of this Material. */
        glm::vec3 colour;
    };
}

#endif
