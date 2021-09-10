/* SIMPLE COLOURED.hpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 16:43:35
 * Last edited:
 *   10/09/2021, 16:45:39
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SimpleColour struct, which contains data for a material
 *   which is not textured but rather has a plain colour all over.
**/

#ifndef MATERIALS_SIMPLE_COLOUR_HPP
#define MATERIALS_SIMPLE_COLOUR_HPP

#include "glm/glm.hpp"

namespace Makma3D::Materials {
    /* The SimpleColoured struct, which contains the data needed to render a material with no lighting and no texture. */
    struct SimpleColoured {
        /* The colour of the material. */
        glm::vec3 colour;
    };

}

#endif
