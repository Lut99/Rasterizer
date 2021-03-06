/* COMPONENT FLAGS.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 15:32:11
 * Last edited:
 *   10/09/2021, 17:18:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Lists the components as flags, using an enum that supports the
 *   necessary operations.
**/

#ifndef ECS_COMPONENT_FLAGS_HPP
#define ECS_COMPONENT_FLAGS_HPP

#include <cstdint>
#include <unordered_map>
#include <string>

namespace Makma3D::ECS {
    /* Values for the ComponentType enum. */
    namespace ComponentFlagsValues {
        enum component {
            /* Shortcut for no component given. */
            none = 0,
            /* Shortcut for all components. */
            all = ~0,

            /* The Transform component, which means the entity has a location in world space. */
            transform = 0x1,
            /* The Model component, which means the entity has one or more meshes to render. */
            model = 0x2,
            /* The Camera component, which means the entity defines some camera through which we can render the scene. */
            camera = 0x4,
            /* The Controllable component, which means the entity can listen to mouse/keyboard input. */
            controllable = 0x8

        };
    };
    /* Lists the possible components, which behave as flags. */
    using ComponentFlags = ComponentFlagsValues::component;

    /* Names for the Component enum. */
    static const std::unordered_map<ComponentFlags, std::string> component_flags_names = {
        { ComponentFlags::none, "none" },
        { ComponentFlags::all,  "all" },

        { ComponentFlags::transform,    "transform" },
        { ComponentFlags::model,        "model" },
        { ComponentFlags::camera,       "camera" },
        { ComponentFlags::controllable, "controllable" }
    };

}

#endif
