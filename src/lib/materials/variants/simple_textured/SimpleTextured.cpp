/* SIMPLE TEXTURED.cpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 14:07:45
 * Last edited:
 *   29/09/2021, 14:07:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SimpleTextured class, which carries data needed for a
 *   material with a texture and no lighting. Extends from the base
 *   Material class, and is thus managed by the MaterialPool.
**/

#include "SimpleTextured.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** SIMPLETEXTURE CLASS *****/
/* Constructor for the SimpleTextured class, which takes its name and the texture it is supposed to have. */
SimpleTextured::SimpleTextured(const std::string& name, const Materials::Texture* texture) :
    Material(MaterialType::simple_textured, name),
    texture(texture)
{}

/* Destructor for the SimpleTextured class, which is virtual and private. */
SimpleTextured::~SimpleTextured() {}
