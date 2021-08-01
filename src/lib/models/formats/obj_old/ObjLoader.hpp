/* OBJ LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:19
 * Last edited:
 *   8/1/2021, 3:48:11 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#ifndef MODELS_OBJ_LOADER_HPP
#define MODELS_OBJ_LOADER_HPP

#include "rendering/auxillary/Vertex.hpp"
#include "rendering/auxillary/Index.hpp"
#include "tools/Array.hpp"

namespace Rasterizer::Models {
    /* Loads the file at the given path as a .obj file, and returns a list of vertices and list of indices in that list of vertices from it. */
    void load_obj_model(Tools::Array<Rendering::Vertex>& new_vertices, Tools::Array<Rendering::index_t>& new_indices, const std::string& path);

};

#endif