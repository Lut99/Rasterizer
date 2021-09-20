/* MATERIAL.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:46:02
 * Last edited:
 *   09/09/2021, 16:46:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the definition for a Material, which is simply a numeric
 *   identifier.
**/

#ifndef MATERIALS_MATERIAL_HPP
#define MATERIALS_MATERIAL_HPP

#include <cstdint>

namespace Makma3D::Materials {
    /* A Material, which is an ID used by the MaterialSystem to recognize it. */
    using material_t = uint32_t;

    /* The null value of the material_t. */
    static constexpr const material_t NullMaterial = 0;
    /* The default material value, which is a SimpleColoured red one. */
    static constexpr const material_t DefaultMaterial = 1;
}

#endif
