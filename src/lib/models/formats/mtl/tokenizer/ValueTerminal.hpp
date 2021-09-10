/* VALUE TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:38:35
 * Last edited:
 *   07/08/2021, 18:39:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ValueTerminal class, which is a Terminal - but then with
 *   a value of some type T.
**/

#ifndef MODELS_MTL_VALUE_TERMINAL_HPP
#define MODELS_MTL_VALUE_TERMINAL_HPP

#include "Terminal.hpp"

namespace Makma3D::Models::Mtl {
    /* The ValueTerminal class, which is a standard Terminal but with some value T. */
    template <class T>
    class ValueTerminal: public Terminal {
    public:
        /* The value carried by the ValueTerminal. */
        T value;


        /* Constructor for the ValueTerminal class, which takes its type, its value and the DebugInfo. */
        ValueTerminal(TerminalType type, const T& value, const DebugInfo& debug_info);

    };
}

// Include the .cpp since it's a template
#include "ValueTerminal.cpp"

#endif
