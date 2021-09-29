/* SIMPLE.cpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 14:27:06
 * Last edited:
 *   29/09/2021, 14:27:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Simple class, which "carries" data for a material that
 *   just takes vertex colours and no lighting. Since it doesn't have any
 *   parameters, doesn't carry anything either; but is there to represent
 *   it.
**/

#include "Simple.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** SIMPLE CLASS *****/
/* Constructor for the Simple class, which takes its name only. */
Simple::Simple(const std::string& name) :
    Material(MaterialType::simple, name)
{}

/* Destructor for the Simple class, which is virtual and private. */
Simple::~Simple() {}
