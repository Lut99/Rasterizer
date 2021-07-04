/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:15
 * Last edited:
 *   03/07/2021, 17:37:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#include "tools/CppDebugger.hpp"

#include "ObjLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;


/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and returns a list of vertices from it. */
Tools::Array<Vertex> Models::load_obj_model(const std::string& path) {
    DENTER("Models::load_obj_model");

    /* First, setup the general Tokenizer class to get access to the input stream of tokens. */
    

    DRETURN {};
}
