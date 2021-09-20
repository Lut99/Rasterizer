/* VALUE TERMINAL.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:53:59
 * Last edited:
 *   07/08/2021, 18:39:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ValueTerminal class, which derives from a standard
 *   Terminal and carries a certain value.
**/

#include "ValueTerminal.hpp"

using namespace std;
using namespace Makma3D::Models::Obj;


/***** VALUETERMINAL CLASS *****/
/* Constructor for the ValueTerminal class, which takes its type, its value and the DebugInfo. */
template <class T>
ValueTerminal<T>::ValueTerminal(TerminalType type, const T& value, const Auxillary::DebugInfo& debug_info) :
    Terminal(type, debug_info),
    value(value)
{}
