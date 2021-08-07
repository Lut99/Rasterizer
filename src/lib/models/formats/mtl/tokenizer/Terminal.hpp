/* TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:36:59
 * Last edited:
 *   07/08/2021, 18:37:39
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Terminal class, which is used to represent tokens
 *   streamed from the input file.
**/

#ifndef MODELS_MTL_TERMINAL_HPP
#define MODELS_MTL_TERMINAL_HPP

#include <string>

#include "../../auxillary/DebugInfo.hpp"

namespace Rasterizer::Models::Mtl {
    /* Lists all Terminal types. */
    enum class TerminalType {
        /* The undefined symbol. */
        undefined = 0,
        /* The EOF-symbol. */
        eof = 1

        
    };
    /* Converts the given TerminalType to a string. */
    static std::string terminal_type_names[] = {
        "undefined",
        "end-of-file"

        
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
