/* SYMBOL.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:40:25
 * Last edited:
 *   04/07/2021, 16:40:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Symbol class for the .obj parser, which is the baseclass
 *   for both terminals and non-terminals.
**/

#ifndef MODELS_OBJ_SYMBOL_HPP
#define MODELS_OBJ_SYMBOL_HPP

#include "model_manager/formats/auxillary/DebugInfo.hpp"

namespace Rasterizer::Models::Obj {
    /* The Symbol class, which forms the baseclass for both the NonTerminal and the Terminal. */
    class Symbol {
    public:
        /* Whether or not the Symbol is secretly a Terminal. */
        bool is_terminal;
        /* The DebugInfo of the symbol, describing its location in the origin file. */
        DebugInfo debug_info;
    
    protected:
        /* Constructor for the Symbol class, which takes whether or not it is a terminal symbol and the symbol's DebugInfo. */
        Symbol(bool is_terminal, const DebugInfo& debug_info);

    };

}

#endif
