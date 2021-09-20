/* MATERIAL FORMAT.hpp
 *   by Lut99
 *
 * Created:
 *   20/09/2021, 16:15:48
 * Last edited:
 *   20/09/2021, 16:15:48
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Lists the supported formats for the MaterialManager.
**/

#ifndef MATERIALS_MATERIAL_FORMAT_HPP
#define MATERIALS_MATERIAL_FORMAT_HPP

#include <string>

namespace Makma3D::Materials {
    /* Describes the Material formats supported by the ModelManager. */
    enum class MaterialFormat {
        /* Meta enum value for unsupported formats. */
        unsupported = 0,
        /* The standard and very simple .mtl format. */
        mtl = 1
    };

    /* Maps enums to readable names. */
    static const std::string material_format_names[] = {
        "unsupported",
        "mtl"
    };

}

#endif
