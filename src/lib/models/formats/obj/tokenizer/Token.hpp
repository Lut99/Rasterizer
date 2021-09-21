/* TOKEN.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:35:15
 * Last edited:
 *   21/09/2021, 17:35:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Provides a shortcut for the general-purpose Terminal class.
**/

#ifndef MODELS_OBJ_TOKEN_HPP
#define MODELS_OBJ_TOKEN_HPP

#include "auxillary/parsers/Terminal.hpp"
#include "TerminalType.hpp"

namespace Makma3D::Models::Obj {
    /* A shortcut for the general-purpose Terminal class. */
    using Token = Makma3D::Auxillary::Terminal<Makma3D::Models::Obj::TerminalType>;

}

#endif
