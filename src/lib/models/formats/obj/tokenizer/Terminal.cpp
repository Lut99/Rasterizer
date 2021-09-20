/* TERMINAL.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:50:32
 * Last edited:
 *   04/07/2021, 16:50:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Terminal class, which is used to describe irreducible
 *   tokens, read directly from the file.
**/

#include "Terminal.hpp"

using namespace std;
using namespace Makma3D::Models::Obj;


/***** TERMINAL CLASS *****/
/* Constructor for the Terminal class, which takes its type and the DebugInfo describing its origin. */
Terminal::Terminal(TerminalType type, const Auxillary::DebugInfo& debug_info) :
    type(type),
    debug_info(debug_info)
{}
