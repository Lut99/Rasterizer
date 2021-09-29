/* MATERIAL.cpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 12:52:49
 * Last edited:
 *   29/09/2021, 12:52:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "Material.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** MATERIAL CLASS *****/
/* Constructor for the Material class, which takes its type and its name. */
Material::Material(MaterialType type, const std::string& name) :
    _type(type),
    _name(name)
{}

/* Destructor for the Material class, which is virtual but private. */
Material::~Material() {}
