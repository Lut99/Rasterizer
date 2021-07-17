/* OBJECT TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   17/07/2021, 14:51:01
 * Last edited:
 *   17/07/2021, 14:51:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains an enum (and related structures) that lists all the types an
 *   object can be.
**/

#ifndef OBJECTS_OBJECT_TYPE_HPP
#define OBJECTS_OBJECT_TYPE_HPP

#include <string>

namespace Rasterizer::Objects::ECS {
    /* Lists the types that an object can be. */
    enum class ObjectType {
        // Undefined type
        undefined = 0,

        // A player object
        player = 1

    };

    /* Maps the ObjectType to a readable string name. */
    static const std::string object_type_names[] = {
        "undefined",

        "player"
    };
    
}

#endif
