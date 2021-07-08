/* NON TERMINAL.cpp
 *   by Lut99
 *
 * Created:
 *   08/07/2021, 13:58:10
 * Last edited:
 *   08/07/2021, 13:58:10
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the NonTerminal class, which derives from the Symbol class
 *   and is used to represent higher-level structures in the input stream.
**/

#include "NonTerminal.hpp"

using namespace std;
using namespace Rasterizer::Models::Obj;


/***** NONTERMINAL CLASS *****/
/* Constructor for the NonTerminal, which takes its own type and a DebugInfo struct. */
NonTerminal::NonTerminal(NonTerminalType type, const DebugInfo& debug_info) :
    Symbol(false, debug_info),
    type(type)
{}
