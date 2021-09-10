/* SIMPLE TEXTURED.hpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 16:37:52
 * Last edited:
 *   10/09/2021, 16:43:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SimpleTextured struct, which contains data for a material
 *   which is textured and has no other lighting effects.
**/

#ifndef MATERIALS_SIMPLE_TEXTURED_HPP
#define MATERIALS_SIMPLE_TEXTURED_HPP

#include "textures/Texture.hpp"

namespace Makma3D::Materials {
    /* The SimpleTextured struct, which contains the data needed to render a material with no lighting but with a texture. */
    struct SimpleTextured {
        /* The texture associated with the material. */
        Textures::Texture texture;
    };

}

#endif
