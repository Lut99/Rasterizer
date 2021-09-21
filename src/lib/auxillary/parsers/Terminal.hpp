/* TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:08:02
 * Last edited:
 *   21/09/2021, 17:08:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Terminal class, which forms the basis for all terminals in
 *   the tokenizer.
**/

#ifndef MODELS_OBJ_TERMINAL_HPP
#define MODELS_OBJ_TERMINAL_HPP

#include <type_traits>

#include "Symbol.hpp"

namespace Makma3D::Auxillary {
    /* The Terminal class, which represents a token in the input stream. Takes the enum that determines what type the Terminal is. */
    template <class TYPE, typename = std::enable_if_t<std::is_enum<TYPE>::value>>
    class Terminal: public Symbol {
    public:
        /* The type type of this Terminal. */
        using type_t = TYPE;

    protected:
        /* The type of this Terminal. */
        type_t _type;
    
    public:
        /* Constructor for the Terminal, which takes what type it is and where it came from in the form of a DebugInfo. */
        Terminal(type_t type, const DebugInfo& debug_info): Symbol(true, debug_info), _type(type) {}

        /* Returns the type of this Terminal. */
        inline type_t type() const { return this->_type; }

        /* Allows the Terminal to be copied polymorphically. */
        virtual Terminal* copy() const { return new Terminal(*this); };

    };

}

#endif
