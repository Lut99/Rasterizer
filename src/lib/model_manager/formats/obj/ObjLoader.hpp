/* OBJ LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:19
 * Last edited:
 *   03/07/2021, 17:37:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#ifndef MODELS_OBJ_LOADER_HPP
#define MODELS_OBJ_LOADER_HPP

#include "model_manager/Vertex.hpp"
#include "tools/Array.hpp"

namespace Rasterizer::Models {
    /* Loads the file at the given path as a .obj file, and returns a list of vertices from it. */
    Tools::Array<Vertex> load_obj_model(const std::string& path);

};

#endif
