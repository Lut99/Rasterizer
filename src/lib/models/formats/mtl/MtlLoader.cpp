/* MTL LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:31:35
 * Last edited:
 *   07/08/2021, 18:34:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MtlLoader, which is an expert on loading the .mtl files
 *   associated with .obj files.
**/

#include "tools/CppDebugger.hpp"

#include "MtlLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Mtl;


/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .mtl file, and returns a map of material name: color schemes for it. */
void Models::load_mtl_lib(std::unordered_map<std::string, glm::vec3>& new_materials, const std::string& path) {
    DENTER("Models::load_mtl_lib");

    

    DRETURN;
}

