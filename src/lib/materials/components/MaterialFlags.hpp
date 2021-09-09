/* MATERIAL FLAGS.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:38:13
 * Last edited:
 *   09/09/2021, 16:38:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the flags that enable specific material properties on a kind
 *   of Material.
**/

#ifndef MATERIALS_MATERIAL_FLAGS_HPP
#define MATERIALS_MATERIAL_FLAGS_HPP

#include <cstdint>
#include <unordered_map>
#include <string>

namespace Rasterizer::Materials {
    /* Values for the MaterialFlags enum. */
    namespace MaterialFlagsValues {
        enum component {
            /* Shortcut for no component given. */
            none = 0,
            /* Shortcut for all components. */
            all = ~0,

            /* The Diffused component, which means the material is rendered using diffuse lighting. */
            diffused = 0x1,
            /* The Textured component, which means the material has a texture. */
            textured = 0x2

        };
    };
    /* Lists the possible components, which behave as flags. */
    using MaterialFlags = MaterialFlagsValues::component;

    /* Names for the Component enum. */
    static const std::unordered_map<MaterialFlags, std::string> material_flags_names = {
        { MaterialFlags::none, "none" },
        { MaterialFlags::all,  "all" },

        { MaterialFlags::diffused, "diffused" },
        { MaterialFlags::textured, "textured" }
    };
}

#endif
