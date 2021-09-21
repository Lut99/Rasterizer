/* VALUE TOKEN.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:36:33
 * Last edited:
 *   21/09/2021, 17:36:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a shortcut for the general-purpose ValueTerminal class.
**/

#ifndef MODELS_OBJ_VALUE_TOKEN_HPP
#define MODELS_OBJ_VALUE_TOKEN_HPP

#include "auxillary/parsers/ValueTerminal.hpp"
#include "TerminalType.hpp"

namespace Makma3D::Models::Obj {
    /* A shortcut for the general-purpose ValueTerminal class. */
    template <class T>
    using ValueToken = Makma3D::Auxillary::ValueTerminal<Makma3D::Models::Obj::TerminalType, T>;

}

#endif
