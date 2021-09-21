/* SYMBOL.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:00:46
 * Last edited:
 *   21/09/2021, 17:00:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Symbol class, which is a base class for all terminals and
 *   nonterminals.
**/

#ifndef AUXILLARY_SYMBOL_HPP
#define AUXILLARY_SYMBOL_HPP

#include <type_traits>

#include "DebugInfo.hpp"

namespace Makma3D::Auxillary {
    /* THe Symbol class, which forms the baseclass of all terminals and nonterminals alike. */
    class Symbol {
    protected:
        /* Determines whether or not the Symbol is actually a terminal. */
        bool _is_term;
        /* The DebugInfo where this Symbol originates from. */
        DebugInfo _debug_info;

        /* Constructor for the Symbol class, which takes whether or not the Symbol is a Terminal and where it came from in the input file(s). */
        Symbol(bool is_terminal, const DebugInfo& debug_info): _is_term(is_terminal), _debug_info(debug_info) {}
    
    public:
        /* Returns whether or not this Symbol is a terminal. */
        inline bool is_term() const { return this->_is_term; }
        /* Returns where this Symbol came from in the input file(s). */
        inline const DebugInfo& debug_info() const { return this->_debug_info; }
        /* Returns the Symbol casted to a pointer of the given type. */
        template <class T, typename = std::enable_if_t<std::is_base_of<Symbol, T>::value >>
        inline T* as() { return std::reinterpret_pointer_cast<T*>(this); }
        /* Returns the Symbol casted to an (immutable) pointer of the given type. */
        template <class T, typename = std::enable_if_t<std::is_base_of<Symbol, T>::value >>
        inline const T* as() const { return std::reinterpret_pointer_cast<T*>(this); }

        /* Allows the Symbol to be copied polymorphically. */
        virtual Symbol* copy() const = 0;

    };

}

#endif
