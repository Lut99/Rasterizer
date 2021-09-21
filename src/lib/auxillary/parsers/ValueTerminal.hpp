/* VALUE TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:09:32
 * Last edited:
 *   21/09/2021, 17:09:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ValueTerminal class, which is a terminal except that it
 *   also carries a generic value.
**/

#ifndef MODELS_OBJ_VALUE_TERMINAL_HPP
#define MODELS_OBJ_VALUE_TERMINAL_HPP

#include <type_traits>

#include "Terminal.hpp"

namespace Makma3D::Auxillary {
    /* The ValueTerminal class, which like a Terminal but with a value of type T. Takes the enum that determines what type the Terminal is. */
    template <class TYPE, class T, typename = std::enable_if_t<std::is_enum<TYPE>::value>>
    class ValueTerminal: public Terminal<TYPE> {
    public:
        /* The value type of this ValueTerminal. */
        using value_type_t = T;
    
    public:
        /* The value in this Terminal. */
        value_type_t value;

        /* Constructor for the ValueTerminal, which takes what type it is, a DebugInfo for its origins and its value. */
        ValueTerminal(type_t type, value_type_t value, const DebugInfo& debug_info): Terminal(type, debug_info), value(value) {}

        /* Allows the Terminal to be copied polymorphically. */
        virtual ValueTerminal* copy() const { return new ValueTerminal(*this); };

    };

}

#endif
