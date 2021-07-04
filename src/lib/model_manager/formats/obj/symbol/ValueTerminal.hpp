/* VALUE TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:52:15
 * Last edited:
 *   04/07/2021, 16:52:15
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
    /* The ValueTerminal class, which is like a Terminal but with some value. */
    template <class T>
    class ValueTerminal: public Terminal {
    public:
        /* The value carried by the ValueTerminal. */
        T value;

        
        /* Constructor for the ValueTerminal, which takes the type of the ValueTerminal. */
        ValueTerminal(TerminalType type);

    };

}

// Include the .cpp as well to still have the separation
#include "ValueTerminal.cpp"

#endif
