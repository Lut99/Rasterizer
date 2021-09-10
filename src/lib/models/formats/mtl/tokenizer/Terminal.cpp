/* TERMINAL.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:37:56
 * Last edited:
 *   07/08/2021, 18:38:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "Terminal.hpp"

using namespace std;
using namespace Makma3D::Models::Mtl;


/***** TERMINAL CLASS *****/
/* Constructor for the Terminal class, which takes its type and the DebugInfo describing its origin. */
Terminal::Terminal(TerminalType type, const DebugInfo& debug_info) :
    type(type),
    debug_info(debug_info)
{}
