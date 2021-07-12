/* TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:43:51
 * Last edited:
 *   04/07/2021, 16:43:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Terminal class, which is used to describe irreducible
 *   tokens, read directly from the file.
**/

#ifndef MODELS_OBJ_TERMINAL_HPP
#define MODELS_OBJ_TERMINAL_HPP

#include <string>

#include "model_manager/formats/auxillary/DebugInfo.hpp"

namespace Rasterizer::Models::Obj {
    /* Lists all Terminal types. */
    enum class TerminalType {
        /* The undefined symbol. */
        undefined = 0,
        /* The EOF-symbol. */
        eof = 1,

        /* The f, indicating we're talking about a Face. */
        face = 2,
        /* The v, indicating we're talking about a Vertex. */
        vertex = 3,

        /* An unsigned integer value. */
        uint = 4,
        /* A floating-point value. */
        decimal = 5
    };
    /* Converts the given TerminalType to a string. */
    static std::string terminal_type_names[] = {
        "undefined",
        "end-of-file",
        "face",
        "vertex",
        "uint",
        "decimal"
    };



    /* The Terminal class, which represents a non-reducible symbol. */
    class Terminal {
    public:
        /* The type of the terminal. */
        TerminalType type;
        /* The DebugInfo describing the terminal's location. */
        DebugInfo debug_info;


        /* Constructor for the Terminal class, which takes its type and the DebugInfo describing its origin. */
        Terminal(TerminalType type, const DebugInfo& debug_info);

    };

}

#endif
