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
 *   Contains structs for carrying material-related data to and from the
 *   GPU.
**/

#ifndef RENDERING_MATERIAL_DATA_HPP
#define RENDERING_MATERIAL_DATA_HPP

#include <vulkan/vulkan.h>
#include "glm/glm.hpp"

namespace Makma3D::Rendering {
    /* GPU data carrier for the SimpleColoured class. */
    struct SimpleColouredData {
        /* The colour of the material. */
        glm::vec3 colour;
    };

}

#endif
