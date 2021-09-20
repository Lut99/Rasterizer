/* TERMINAL.hpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:36:59
 * Last edited:
 *   07/08/2021, 22:04:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Terminal class, which is used to represent tokens
 *   streamed from the input file.
**/

#ifndef MATERIALS_MTL_TERMINAL_HPP
#define MATERIALS_MTL_TERMINAL_HPP

#include <string>

#include "../../../../auxillary/DebugInfo.hpp"

namespace Makma3D::Materials::Mtl {
    /* Lists all Terminal types. */
    enum class TerminalType {
        /* The undefined symbol. */
        undefined = 0,
        /* The EOF-symbol. */
        eof = 1,

        /* The newmtl keyword, indicating that we define a new material. */
        newmtl = 2,
        /* The Kd-keyword, indicating a diffusing color. */
        Kd = 3,

        /* A floating-point value. */
        decimal = 4,
        /* The name value, carrying some name. */
        name = 5
    };
    /* Converts the given TerminalType to a string. */
    static std::string terminal_type_names[] = {
        "undefined",
        "end-of-file",

        "newmtl",
        "Kd",

        "decimal",
        "name"
    };



    /* The Terminal class, which represents a non-reducible symbol. */
    class Terminal {
    public:
        /* The type of the terminal. */
        TerminalType type;
        /* The DebugInfo describing the terminal's location. */
        Auxillary::DebugInfo debug_info;


        /* Constructor for the Terminal class, which takes its type and the DebugInfo describing its origin. */
        Terminal(TerminalType type, const Auxillary::DebugInfo& debug_info);

    };

}

#endif