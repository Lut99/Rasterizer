/* DEBUG INFO.cpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 17:30:01
 * Last edited:
 *   04/07/2021, 17:30:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DebugInfo class, which keeps track of a terminal's origin
 *   in a file to use in (error) messages.
**/

#include "tools/CppDebugger.hpp"

#include "DebugInfo.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;


/***** DEBUGINFO CLASS *****/
/* Constructor for the DebugInfo class, which takes a file handle to extract the line from, a start line, a start column and the length of the symbol (in characters). */
DebugInfo::DebugInfo(std::ifstream& file, size_t line, size_t col, size_t length) {

}

/* Constructor for the DebugInfo class, which takes a file handle to extract the line from, a start line, a start column, an end line and an end column. */
DebugInfo::DebugInfo(std::ifstream& file, size_t line1, size_t col1, size_t line2, size_t col2) {

}

/* Constructor for the DebugInfo class, which takes lines to show during debugging, a start line, a start column and the length of the symbol (in characters). The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
DebugInfo::DebugInfo(const Tools::Array<std::string>& raw_lines, size_t line, size_t col, size_t length) {

}

/* Constructor for the DebugInfo class, which takes lines to show during debugging, a start line, a start column, an end line and an end column. The array of lines must contain one entry for each line spanned by the symbol. By convention, we can interpret empty strings as irrelevant lines (since empty lines are usually irrelevant). */
DebugInfo::DebugInfo(const Tools::Array<std::string>& raw_lines, size_t line1, size_t col1, size_t line2, size_t col2) {

}



/* Adds another DebugInfo, and returns a new one that spans from the start of this one to the end of the other one. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
DebugInfo DebugInfo::operator+(const DebugInfo& other) {

}

/* Adds another DebugInfo to this one s.t. we end at the given DebugInfo's end line and end column. Note that if the DebugInfo's aren't (line) adjacent, the unknown lines will be interpreted as empty. */
DebugInfo& DebugInfo::operator+=(const DebugInfo& other) {

}

