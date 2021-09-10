/* CONTROLLABLE.hpp
 *   by Lut99
 *
 * Created:
 *   06/08/2021, 13:22:44
 * Last edited:
 *   06/08/2021, 13:24:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the Controllable component, which allows an entity's rotation
 *   and position to be controlled by the mouse and keyboard, respectively.
**/

#ifndef ECS_CONTROLLABLE_HPP
#define ECS_CONTROLLABLE_HPP

#include "../auxillary/ComponentHash.hpp"
#include "tools/Typenames.hpp"

namespace Makma3D::ECS {
    /* The Controllable component, which allows an entity to be controlled by a keyboard and mouse (or more accurately, which allows its transform position and rotation to be controlled by keyboard and mouse). */
    struct Controllable {
        /* The entity's movement speed. */
        float mov_speed;
        /* The entity's rotation speed. */
        float rot_speed;
    };

    /* Hash function for the Controllable struct, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Controllable>() { return 3; }

}



namespace Tools {
    /* The string name of the Controllable component. */
    template <> inline constexpr const char* type_name<Makma3D::ECS::Controllable>() { return "ECS::Controllable"; }
}

#endif
