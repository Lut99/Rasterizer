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

#ifndef TEXTURES_TEXTURE_FORMAT_HPP
#define TEXTURES_TEXTURE_FORMAT_HPP

#include <string>

namespace Makma3D::Textures {
    /* Enum that lists all supported texture formats. */
    enum class TextureFormat {
        /* The meta unsupported format. */
        unsupported = 0,

        /* PNG textures. */
        png = 1,
        /* JPG/JPEG textures. */
        jpg = 2
    };

    /* Maps TextureFormat values to readable strings. */
    static const std::string texture_format_names[] = {
        "unsupported",

        "png",
        "jpg"
    };

}

#endif
