/* SYMBOL.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 16:42:25
 * Last edited:
 *   04/07/2021, 16:42:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Symbol class for the .obj parser, which is the baseclass
 *   for both terminals and non-terminals.
**/

#include "Symbol.hpp"

using namespace std;
using namespace Rasterizer::Models::Obj;


/***** OBJSYMBOL CLASS *****/
/* Constructor for the Symbol class, which takes whether or not it is a terminal symbol. */
Symbol::Symbol(bool is_terminal) :
    is_terminal(is_terminal)
{}
