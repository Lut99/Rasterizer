/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:15
 * Last edited:
 *   03/07/2021, 17:37:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#include "tools/CppDebugger.hpp"

#include "tokenizer/ValueTerminal.hpp"
#include "tokenizer/Tokenizer.hpp"

#include "ObjLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Obj;
using namespace CppDebugger::SeverityValues;


/***** MACROS *****/





/***** HELPER FUNCTIONS *****/





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and returns a list of vertices from it. */
Tools::Array<Vertex> Models::load_obj_model(const std::string& path) {
    DENTER("Models::load_obj_model");

    // Prepare the resulting lists
    Tools::Array<Vertex> vertices;
    Tools::Array<uint32_t> faces;

    // Prepare the Tokenizer
    Obj::Tokenizer tokenizer(path);

    // Next, iterate through the input stream and find lines separated by the face or vertex tokens
    int state = 0;
    uint32_t counter;
    float f1, f2, f3;
    uint32_t u1, u2, u3;
    bool stop = false;
    for (int i = 0; !stop; i++) {
        Terminal* term = tokenizer.get();
        if (state == 0) {
            // We're in idle state
            switch(term->type) {
                case TerminalType::vertex:
                    // We start parsing as vertex
                    state = 1;
                    counter = 0;
                    break;
                
                case TerminalType::face:
                    // We start parsing as face
                    state = 2;
                    counter = 0;
                    break;
                
                case TerminalType::eof:
                    // We accept this one!
                    stop = true;
                    break;
                
                default:
                    // Illegal token
                    term->debug_info.print_error(cerr, "Unexpected token '" + terminal_type_names[(int) term->type] + "': expected a type identifier for the line.");
                    break;

            }
        } else if (state == 1) {
            // We're in parsing vertex state
            switch(term->type) {
                case TerminalType::decimal:
                    // Store the coordinate (or at least, try to)
                    if (counter == 0){
                        f1 = ((ValueTerminal<float>*) term)->value;
                    } else if (counter == 1) {
                        f2 = ((ValueTerminal<float>*) term)->value;
                    } else if (counter == 2) {
                        f3 = ((ValueTerminal<float>*) term)->value;

                        // Now that we have them all anyway, also store it and return to the idle state
                        while (vertices.size() + 1 >= vertices.capacity()) { vertices.reserve(2 * (vertices.capacity() + 1)); }
                        vertices.push_back(Vertex({ f1, f2 - 5.0, f3 }, { 1.0, 0.0, 0.0 }));
                        state = 0;
                    } else {
                        term->debug_info.print_error(cerr, "Too many coordinates for three-dimensional vector.");
                        state = 0;
                        break;
                    }

                    // Done
                    ++counter;
                    break;
                
                case TerminalType::uint:
                    // Incorrect type
                    term->debug_info.print_error(cerr, "Incorrect type used for vertex: can only use decimal values for coordinates.");
                    state = 0;
                    break;
                
                default:
                    // Quit too soon
                    term->debug_info.print_error(cerr, "Missing coordinates for three-dimensional vector.");
                    state = 0;
                    break;

            }
        } else if (state == 2) {
            // We're in parsing face state
            switch(term->type) {
                case TerminalType::uint:
                    // Store the coordinate (or at least, try to)
                    if (counter == 0){
                        u1 = ((ValueTerminal<uint32_t>*) term)->value;
                    } else if (counter == 1) {
                        u2 = ((ValueTerminal<uint32_t>*) term)->value;
                    } else if (counter == 2) {
                        u3 = ((ValueTerminal<uint32_t>*) term)->value;

                        // Now that we have them all anyway, also store it and return to the idle state
                        while (faces.size() + 3 >= faces.capacity()) { faces.reserve(2 * (faces.capacity() + 3)); }
                        faces.push_back(u1);
                        faces.push_back(u2);
                        faces.push_back(u3);
                        state = 0;
                    } else {
                        term->debug_info.print_error(cerr, "Too many indices for triangular face.");
                        state = 0;
                        break;
                    }

                    // Done
                    ++counter;
                    break;
                
                case TerminalType::decimal:
                    // Incorrect type
                    term->debug_info.print_error(cerr, "Incorrect type used for face: can only use unsigned integer values for indices.");
                    state = 0;
                    break;
                
                default:
                    // Quit too soon
                    term->debug_info.print_error(cerr, "Missing indices for triangular face.");
                    state = 0;
                    break;

            }
        }

        // When done with an iteration, clear the term
        delete term;
    }

    // With the lists completed, merge them into one unindexed list of vertices
    Tools::Array<Vertex> result(faces.size());
    for (uint32_t i = 0; i < faces.size(); i++) {
        result.push_back(vertices[faces[i] - 1]);
    }

    // Done
    DRETURN result;
}
