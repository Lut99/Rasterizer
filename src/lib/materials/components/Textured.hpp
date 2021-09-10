/* TEXTURED.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:42:35
 * Last edited:
 *   09/09/2021, 16:42:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Material Component for Textured components, i.e.,
 *   materials that have a texture.
**/

#ifndef MATERIALS_TEXTURED_HPP
#define MATERIALS_TEXTURED_HPP

#include "tools/Typenames.hpp"

namespace Makma3D::Materials {
    /* The Textured material component, which means the material has a texture to load and sample. */
    struct Textured {

    };

}



namespace Tools {
    /* The string name of the Mesh component. */
    template <> inline constexpr const char* type_name<Makma3D::Materials::Textured>() { return "Materials::Textured"; }
}

#endif
