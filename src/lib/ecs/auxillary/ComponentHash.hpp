/* COMPONENT HASH.hpp
 *   by Lut99
 *
 * Created:
 *   20/07/2021, 14:27:18
 * Last edited:
 *   8/1/2021, 3:32:24 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains functions to hash our component types. More specifically,
 *   contains a templated hash function that the components are supposed to
 *   override.
**/

#ifndef ECS_COMPONENT_HASH_HPP
#define ECS_COMPONENT_HASH_HPP

#include <cstdint>

#include "tools/Typenames.hpp"
#include "tools/CppDebugger.hpp"

namespace Rasterizer::ECS {
    /* Function that returns the hash of the given type. Note that the general case throws errors; it relies on specializations alone. */
    template <class T>
    inline constexpr uint32_t hash_component() { DENTER("ECS::hash_component"); DLOG(CppDebugger::Severity::fatal, "Hash component is not properly specialized for type " + Tools::type_sname<T>() + "."); DRETURN ~0;  }

}

#endif
