/* MATERIAL TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 13:56:27
 * Last edited:
 *   11/09/2021, 13:56:27
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Enum that lists the possible material types supported by the Makma3D
 *   engine.
**/

#ifndef MATERIALS_MATERIAL_TYPE_HPP
#define MATERIALS_MATERIAL_TYPE_HPP

#include <string>

namespace Makma3D::Materials {
    /* The MaterialType enum, which lists the types of materials available. */
    enum class MaterialType {
        /* Meta enum that denotes a default, unknown material. */
        unknown = 0,

        /* Material type for a material with no lighting model and no texture. */
        simple_coloured = 1,
        /* Material type for a material with no lighting model, but with a texture. */
        simple_textured = 2
    };

    /* Maps MaterialType enum values to readable strings. */
    static const std::string material_type_names[] = {
        "unknown",

        "simple_coloured",
        "simple_textured"
    };

}

#endif
