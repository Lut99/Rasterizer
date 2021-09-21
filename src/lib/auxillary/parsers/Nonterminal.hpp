/* NONTERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 16:54:47
 * Last edited:
 *   21/09/2021, 16:54:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Nonterminal class, which forms the basis for all
 *   nonterminals in the AST.
**/

#ifndef MODELS_OBJ_NONTERMINAL_HPP
#define MODELS_OBJ_NONTERMINAL_HPP

#include <type_traits>

#include "Symbol.hpp"

namespace Makma3D::Auxillary {
    /* The Nonterminal class, which is the parent for all AST elements. Takes the enum that determines what type the Nonterminal is. */
    template <class TYPE, typename = std::enable_if_t<std::is_enum<TYPE>::value>>
    class Nonterminal: public Symbol {
    public:
        /* The type type of this Nonterminal. */
        using type_t = TYPE;

    protected:
        /* The type of this Nonterminal. */
        type_t _type;

        /* Constructor for the Nonterminal, which takes what type it is and a DebugInfo listing its origins. */
        Nonterminal(type_t type, const DebugInfo& debug_info): Symbol(false, debug_info), _type(type) {}
    
    public:
        /* Returns the type of this Nonterminal. */
        inline type_t type() const { return this->_type; }
        /* Returns the Nonterminal casted to a pointer of the given type. */
        template <class T, typename = std::enable_if_t<std::is_base_of<Nonterminal, T>::value >>
        inline T* as() { return std::reinterpret_pointer_cast<T*>(this); }
        /* Returns the Nonterminal casted to an (immutable) pointer of the given type. */
        template <class T, typename = std::enable_if_t<std::is_base_of<Nonterminal, T>::value >>
        inline const T* as() const { return std::reinterpret_pointer_cast<T*>(this); }

        /* Allows the Nonterminal to be copied polymorphically. */
        virtual Nonterminal* copy() const = 0;

    };

}

#endif
