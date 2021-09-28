/* TEXTURE FORMAT.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:53:56
 * Last edited:
 *   16/08/2021, 11:53:56
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains an enum listing all supported texture formats.
**/

#ifndef MATERIALS_TEXTURE_FORMAT_HPP
#define MATERIALS_TEXTURE_FORMAT_HPP

#include <string>

namespace Makma3D::Materials {
    /* Enum that lists all supported texture formats. */
    enum class TextureFormat {
        /* The meta unsupported format. */
        unsupported = 0,
        /* The meta automatic format, which tells the TexturePool to find the format itself. */
        automatic = 1,

        /* PNG textures. */
        png = 2,
        /* JPG/JPEG textures. */
        jpg = 3
    };

    /* Maps TextureFormat values to readable strings. */
    static const std::string texture_format_names[] = {
        "unsupported",
        "automatic",

        "png",
        "jpg"
    };

}

#endif
