/* NON TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   08/07/2021, 13:55:18
 * Last edited:
 *   08/07/2021, 13:55:18
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the NonTerminal class, which derives from the Symbol class
 *   and is used to represent higher-level structures in the input stream.
**/

#ifndef MODELS_OBJ_NON_TERMINAL_HPP
#define MODELS_OBJ_NON_TERMINAL_HPP

#include "../symbol/Symbol.hpp"

namespace Rasterizer::Models::Obj {
    /* Lists all NonTerminal types. */
    enum class NonTerminalType {
        /* The undefined symbol. */
        undefined = 0,

        /* A NonTerminal for faces of vertices. */
        face = 1,
        /* A NonTerminal for vertices. */
        vertex = 2
    };
    /* Converts the given TerminalType to a string. */
    static std::string non_terminal_type_names[] = {
        "undefined",
        "face",
        "vertex"
    };



    /* The NonTerminal class, which represents higher-level structures in the input stream. */
    class NonTerminal: public Symbol {
    public:
        /* The type of NonTerminal this is. */
        NonTerminalType type;

    protected:
        /* Constructor for the NonTerminal, which takes its own type and a DebugInfo struct. */
        NonTerminal(NonTerminalType type, const DebugInfo& debug_info);

    };

}

#endif
