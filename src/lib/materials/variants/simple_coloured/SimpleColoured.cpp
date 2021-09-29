/* SIMPLE COLOURED.cpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 12:57:34
 * Last edited:
 *   29/09/2021, 12:57:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SimpleColoured class, which carries data needed for a
 *   material with a uniform colour and no lighting. Extends from the
 *   base Material class, and is thus managed by the MaterialPool.
**/

#include "SimpleColoured.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** SIMPLECOLOURED CLASS *****/
/* Constructor for the SimpleColoured class, which takes its name and the colour it is supposed to have. */
SimpleColoured::SimpleColoured(const std::string& name, const glm::vec3& colour) :
    Material(MaterialType::simple_coloured, name),
    colour(colour)
{}

/* Destructor for the SimpleColoured class, which is virtual and private. */
SimpleColoured::~SimpleColoured() {}
