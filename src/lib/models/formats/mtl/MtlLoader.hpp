/* MTL LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:31:46
 * Last edited:
 *   07/08/2021, 18:33:43
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MtlLoader, which is an expert on loading the .mtl files
 *   associated with .obj files.
**/

#ifndef MODELS_MTL_LOADER_HPP
#define MODELS_MTL_LOADER_HPP

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

namespace Rasterizer::Models {
    /* Loads the file at the given path as a .mtl file, and returns a map of material name: color schemes for it. */
    void load_mtl_lib(std::unordered_map<std::string, glm::vec3>& new_materials, const std::string& path);

}

#endif
