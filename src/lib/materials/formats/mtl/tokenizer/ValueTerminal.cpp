/* VALUE TERMINAL.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:38:40
 * Last edited:
 *   07/08/2021, 18:39:31
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ValueTerminal class, which is a Terminal - but then with
 *   a value of some type T.
**/

#include "ValueTerminal.hpp"

using namespace std;
using namespace Makma3D::Materials::Mtl;


/***** VALUETERMINAL CLASS *****/
/* Constructor for the ValueTerminal class, which takes its type, its value and the DebugInfo. */
template <class T>
ValueTerminal<T>::ValueTerminal(TerminalType type, const T& value, const Auxillary::DebugInfo& debug_info) :
    Terminal(type, debug_info),
    value(value)
{}
