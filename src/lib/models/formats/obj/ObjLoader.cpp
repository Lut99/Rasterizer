/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   01/08/2021, 13:28:22
 * Last edited:
 *   01/08/2021, 13:28:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the loader for .obj model files.
**/

#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include "tools/CppDebugger.hpp"

extern "C" {
#include "pegleg/test.c"
}

#include "ObjLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;


/***** GLOBALS *****/
/* The input file stream. */
static std::istream* file;





/***** MACROS *****/
/* Undefine the pre-defined YYINPUT. */
#ifdef YY_INPUT
#undef YY_INPUT
#endif
#ifdef _WIN32

/* Overwrites the default YY_INPUT to load from the text file, using Windows-style error handling. */
#define YY_INPUT(BUF, RESULT, MAX_SIZE) \
    file->read((BUF), (MAX_SIZE)); \
    if (file->fail() && !file->eof()) { \
        char buffer[BUFSIZ]; \
        strerror_s(buffer, errno); \
        std::string err = buffer; \
        DLOG(fatal, "Cannot read from input file: " + err); \
    } \
    (RESULT) = static_cast<int>(file->gcount());

#else
/* Overwrites the default YY_INPUT to load from the text file, using Windows-style error handling. */
#define YY_INPUT(BUF, RESULT, MAX_SIZE) \
    file->read((BUF), (MAX_SIZE)); \
    if (file->fail() && !file->eof()) { \
        std::string err = strerror(errno); \
        DLOG(fatal, "Cannot read from input file: " + err); \
    } \
    (RESULT) = static_cast<int>(file->gcount());

#endif





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and returns a list of vertices and list of indices in that list of vertices from it. */
void Models::load_obj_model(Tools::Array<Rendering::Vertex>& new_vertices, Tools::Array<Rendering::index_t>& new_indices, const std::string& path) {
    DENTER("Models::load_obj_model");

    // Open the file handle
    file = (std::istream*) new std::ifstream(path);
    if (!((std::ifstream*) file)->is_open()) {
        std::string err;
        #ifdef _WIN32
        char buffer[BUFSIZ];
        strerror_s(buffer, errno);
        err = buffer;
        #else
        err = strerror(errno);
        #endif
        DLOG(fatal, "Could not open input file: " + err);
    }

    // Start parsing
    while (yyparse()) {
        // Nothing...
    }

    // Done
    DRETURN;
}
