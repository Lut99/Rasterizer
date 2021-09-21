/* NONTERMINAL TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 16:57:00
 * Last edited:
 *   21/09/2021, 16:57:00
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains an enum listing all nonterminal types in the AST.
**/

#ifndef MODELS_OBJ_NONTERMINAL_TYPE_HPP
#define MODELS_OBJ_NONTERMINAL_TYPE_HPP

#include <string>

namespace Makma3D::Models::Obj {
    /* Lists all Nonterminal types. */
    enum class NonterminalType {
        /* The undefined symbol. */
        undefined = 0,

        /* A new vertex entry, with either three or four coordinates. */
        vertex = 1
    };

    /* Converts the given NonterminalType to a string. */
    static std::string nonterminal_type_names[] = {
        "undefined",
        "vertex"
    };

}

#endif
