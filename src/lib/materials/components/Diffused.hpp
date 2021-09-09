/* DIFFUSED.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:42:29
 * Last edited:
 *   09/09/2021, 16:42:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Material Component for Diffused components, i.e.,
 *   materials that have simple diffuse light patterns.
**/

#ifndef MATERIALS_DIFFUSED_HPP
#define MATERIALS_DIFFUSED_HPP

#include "tools/Typenames.hpp"

namespace Rasterizer::Materials {
    /* The Diffused material component, which means the material should be rendered using simple diffuse lighting. */
    struct Diffused {

    };

}



namespace Tools {
    /* The string name of the Mesh component. */
    template <> inline constexpr const char* type_name<Rasterizer::Materials::Diffused>() { return "Materials::Diffused"; }
}

#endif
