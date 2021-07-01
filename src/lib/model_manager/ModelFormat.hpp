/* MODEL FORMATS.hpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 18:16:34
 * Last edited:
 *   01/07/2021, 18:16:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Lists the supported formats for the ModelManager.
**/

#ifndef MODELS_MODEL_FORMATS_HPP
#define MODELS_MODEL_FORMATS_HPP

#include <string>

namespace Rasterizer::Models {
    /* Describes the Model formats supported by the ModelManager. */
    enum class ModelFormat {
        /* Meta enum value for unsupported formats. */
        unsupported = 0,
        /* The hardcoded triangle 'format', i.e., doesn't touch the disk but simply inserts the debug triangle. */
        triangle = 1,

        /* The standard and very simple .obj format. */
        obj = 2

    };

    /* Maps enums to readable names. */
    static const std::string model_format_names[] = {
        "unsupported",
        "triangle",
        "obj"
    };

}

#endif
