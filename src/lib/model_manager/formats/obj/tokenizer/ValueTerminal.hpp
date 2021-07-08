/* VALUE TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   08/07/2021, 11:34:45
 * Last edited:
 *   08/07/2021, 11:34:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ValueTerminal class, which derives from a standard
 *   Terminal and carries a certain value.
**/

#ifndef MODELS_OBJ_VALUE_TERMINAL_HPP
#define MODELS_OBJ_VALUE_TERMINAL_HPP

#include "Terminal.hpp"

namespace Rasterizer::Models::Obj {
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
