/* FORMATS.hpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 16:24:03
 * Last edited:
 *   29/06/2021, 16:24:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that defines the different model formats that the Rasterizer
 *   supports. Also contains a function to automatically select the proper
 *   loader.
**/

#ifndef LOADERS_FORMATS_HPP
#define LOADERS_FORMATS_HPP

#include <string>

#include "loaders/auxillary/IndexedVertexArray.hpp"

namespace Rasterizer::Loaders {
    /* The different formats that are supported by the Rasterizer. */
    enum class ModelFormat {
        obj = 0,

        none = 1
    };
    /* Maps each format to a readable name. */
    const static std::string model_format_names[] = {
        "obj",
        "none"
    };



    /* Given a path and a model format, returns an IndexVertexList of vertices belonging to the given model. */
    Loaders::IndexedVertexArray load_model(const std::string& path, ModelFormat format);

}

#endif
