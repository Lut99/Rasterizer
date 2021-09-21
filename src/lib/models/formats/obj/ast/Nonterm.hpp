/* NONTERM.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:45:50
 * Last edited:
 *   21/09/2021, 17:45:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a shortcut for the general-purpose Nonterminal class.
**/

#ifndef MODELS_OBJ_NONTERM_HPP
#define MODELS_OBJ_NONTERM_HPP

#include "auxillary/parsers/Nonterminal.hpp"

#include "NonterminalType.hpp"

namespace Makma3D::Models::Obj {
    /* A shortcut for the general-purpose Nonterminal class. */
    using Nonterm = Makma3D::Auxillary::Nonterminal<Makma3D::Models::Obj::NonterminalType>;

}

#endif
