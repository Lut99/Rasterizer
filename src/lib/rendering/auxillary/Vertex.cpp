/* VERTEX.cpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:37:11
 * Last edited:
 *   8/1/2021, 3:48:20 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Describes how a single Vertex looks like. Also contains code to create
 *   Vulkan descriptions for itself.
**/

#include "Vertex.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** VERTEX STRUCT *****/
/* Default constructor for the Vertex struct. */
Vertex::Vertex() :
    pos(0.0, 0.0, 0.0),
    colour(0.0, 0.0, 0.0)
{}

/* Constructor for the Vertex struct, which takes the position and colour. */
Vertex::Vertex(const glm::vec3& pos, const glm::vec3& colour) :
    pos(pos),
    colour(colour),
    texel(0.0f, 0.0f)
{}

/* Constructor for the Vertex struct, which takes the position, colour and texel coordinate. */
Vertex::Vertex(const glm::vec3& pos, const glm::vec3& colour, const glm::vec2& texel) :
    pos(pos),
    colour(colour),
    texel(texel)
{}
