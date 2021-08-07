/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:15
 * Last edited:
 *   07/08/2021, 22:37:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#include "tools/CppDebugger.hpp"
#include "tools/LinkedArray.hpp"

#include "../auxillary/SymbolStack.hpp"

#include "tokenizer/ValueTerminal.hpp"
#include "tokenizer/Tokenizer.hpp"

#include "../mtl/MtlLoader.hpp"

#include "../auxillary/ParserTools.hpp"
#include "ObjLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Obj;
using namespace CppDebugger::SeverityValues;


/***** MACROS *****/
/* If defined, enables extra debug prints tracing the tokenizer's steps. */
// #define EXTRA_DEBUG





/***** HELPER FUNCTIONS *****/
/* Given a symbol stack, tries to reduce it according to the rules to parse new vertices and indices. Returns the rule applied. */
static std::string reduce(Tools::Array<Rendering::Vertex>& new_vertices, Tools::Array<Rendering::index_t>& new_indices, std::string& current_mtl, std::unordered_map<std::string, glm::vec3>& mtl_map, Tools::LinkedArray<Terminal*>& symbol_stack) {
    DENTER("reduce");

    // Prepare the iterator over the linked array
    Tools::LinkedArray<Terminal*>::iterator iter = symbol_stack.begin();
    size_t i = 0;



/* start */ {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the symbol for this iterator
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::vertex:
            // Start of a vertex
            goto vertex_start;
        
        case TerminalType::normal:
            // Start of a normal
            goto normal_start;
        
        case TerminalType::texture:
            // Start of a texture coordinate
            goto texture_start;
        
        case TerminalType::face:
            // Start of a face definition
            goto face_start;

        case TerminalType::decimal:
            // Looking at a decimal without a vector; stop
            term->debug_info.print_error(cerr, "Encountered stray coordinate.");
            remove_stack_bottom(symbol_stack, iter);;
            DRETURN "error";
        
        case TerminalType::group:
            // Start of a new group definition
            goto group_start;

        case TerminalType::mtllib:
            // Start of a new material definition
            goto mtllib_start;

        case TerminalType::usemtl:
            // Start of a material usage
            goto usemtl_start;

        default:
            // Unexpected token
            term->debug_info.print_error(cerr, "Unexpected token '" + terminal_type_names[(int) term->type] + "'.");
            remove_stack_bottom(symbol_stack, iter);;
            DRETURN "error";

    }

}



vertex_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::decimal:
            // Simply keep trying to grab more, since too many is good for error handling
            goto vertex_start;
        
        case TerminalType::uint:
            {
                // Simply keep trying to grab more, except that we first case it to a float
                float val = (float) ((ValueTerminal<uint32_t>*) (*iter))->value;
                DebugInfo debug_info = (*iter)->debug_info;
                delete *iter;
                *iter = (Terminal*) new ValueTerminal<float>(TerminalType::decimal, val, debug_info);
                goto vertex_start;
            }
        
        case TerminalType::sint:
            {
                // Simply keep trying to grab more, except that we first case it to a float
                float val = (float) ((ValueTerminal<int32_t>*) (*iter))->value;
                DebugInfo debug_info = (*iter)->debug_info;
                delete *iter;
                *iter = (Terminal*) new ValueTerminal<float>(TerminalType::decimal, val, debug_info);
                goto vertex_start;
            }

        default:
            // Check if the vertex is too small or large
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for vector (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 4) {
                term->debug_info.print_error(cerr, "Too many coordinates given for vector (got " + std::to_string(i - 2) + ", expected 4)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the vector; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            if (i - 2 == 4) { --value_iter; }
            float z = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // Store the vertex
            new_vertices.push_back(Rendering::Vertex({ x, y, z }, { 0.5f + ((0.5 * rand()) / RAND_MAX), 0.0f, 0.0f }));

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "vertex";

    }
}



normal_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::decimal:
            // Simply keep trying to grab more, since too many is good for error handling
            goto normal_start;
        
        case TerminalType::uint:
            {
                // Simply keep trying to grab more, except that we first case it to a float
                float val = (float) ((ValueTerminal<uint32_t>*) (*iter))->value;
                DebugInfo debug_info = (*iter)->debug_info;
                delete *iter;
                *iter = (Terminal*) new ValueTerminal<float>(TerminalType::decimal, val, debug_info);
                goto normal_start;
            }
        
        case TerminalType::sint:
            {
                // Simply keep trying to grab more, except that we first case it to a float
                float val = (float) ((ValueTerminal<int32_t>*) (*iter))->value;
                DebugInfo debug_info = (*iter)->debug_info;
                delete *iter;
                *iter = (Terminal*) new ValueTerminal<float>(TerminalType::decimal, val, debug_info);
                goto normal_start;
            }

        default:
            // Check if the vertex is too small or large
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for normal (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for normal (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // // Otherwise, we can parse the vector normal; get the coordinates
            // Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            // float z = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            // float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            // float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // // Store the vertex
            // new_vertices.push_back(Rendering::Vertex({ x, y, z }, { 0.5f + (rand() / (2 * RAND_MAX)), 0.0f, 0.0f }));

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "normal";

    }
}



texture_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::decimal:
            // Simply keep trying to grab more, since too many is good for error handling
            goto texture_start;
        
        case TerminalType::uint:
            {
                // Simply keep trying to grab more, except that we first case it to a float
                float val = (float) ((ValueTerminal<uint32_t>*) (*iter))->value;
                DebugInfo debug_info = (*iter)->debug_info;
                delete *iter;
                *iter = (Terminal*) new ValueTerminal<float>(TerminalType::decimal, val, debug_info);
                goto texture_start;
            }

        default:
            // Check if the vertex is too small or large
            if (i - 2 < 1) {
                term->debug_info.print_error(cerr, "Too few coordinates given for texture coordinate (got " + std::to_string(i - 2) + ", expected 2)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for texture coordinate (got " + std::to_string(i - 2) + ", expected 2)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // // Otherwise, we can parse the vector normal; get the coordinates
            // Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            // float z = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            // float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            // float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // // Store the vertex
            // new_vertices.push_back(Rendering::Vertex({ x, y, z }, { 0.5f + (rand() / (2 * RAND_MAX)), 0.0f, 0.0f }));

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "texture";

    }
}



face_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::uint:
            // Simply keep trying to grab more, but except only these
            goto face_v;
        
        case TerminalType::sint:
            {
                // TBD
                delete *iter;
                symbol_stack.erase(iter);
                DRETURN "not-yet-implemented";
            }
        
        case TerminalType::v_vt:
            // Simply keep trying to grab more, but allow only these
            goto face_v_vt;

        case TerminalType::v_vn:
            // Simply keep trying to grab more, but allow only these
            goto face_v_vn;

        case TerminalType::v_vt_vn:
            // Simply keep trying to grab more, but allow only these
            goto face_v_vt_vn;

        default:
            // Definitely too small
            term->debug_info.print_error(cerr, "Too few indices given for face (got 0, expected 3)");
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "error";

    }
}

face_v: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::uint:
            // Parse more
            goto face_v;
        
        case TerminalType::sint:
            {
                // Also parse more but take relative coordinates
                delete *iter;
                symbol_stack.erase(iter);
                DRETURN "not-yet-implemented";
            }

        case TerminalType::v_vt:
        case TerminalType::v_vn:
        case TerminalType::v_vt_vn:
            // Invalid argument type
            term->debug_info.print_error(cerr, "Cannot mix vertex-only indices with other indices.");
            (*(iter - (i - 2)))->debug_info.print_note(cerr, "Face type determined by first argument.");
            delete *iter;
            symbol_stack.erase(iter);
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            uint32_t v3 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;
            uint32_t v2 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;
            uint32_t v1 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;

            // Store the indices
            new_indices += { v1 - 1, v2 - 1, v3 - 1 };

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "face(v)";

    }
}

face_v_vt: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::v_vt:
            // Parse as many as we can
            goto face_v_vt;

        case TerminalType::uint:
        case TerminalType::sint:
        case TerminalType::v_vn:
        case TerminalType::v_vt_vn:
            // Invalid argument type
            term->debug_info.print_error(cerr, "Cannot mix vertex/texture indices with other indices.");
            (*(iter - (i - 2)))->debug_info.print_note(cerr, "Face type determined by first argument.");
            delete *iter;
            symbol_stack.erase(iter);
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Store the indices
            new_indices += { std::get<0>(v1) - 1, std::get<0>(v2) - 1, std::get<0>(v3) - 1 };

            // Ignore the texture coordinates for now

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "face(v_vt)";

    }
}

face_v_vn: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::v_vn:
            // Parse as many as we can
            goto face_v_vn;

        case TerminalType::uint:
        case TerminalType::sint:
        case TerminalType::v_vt:
        case TerminalType::v_vt_vn:
            // Invalid argument type
            term->debug_info.print_error(cerr, "Cannot mix vertex/normal indices with other indices.");
            (*(iter - (i - 2)))->debug_info.print_note(cerr, "Face type determined by first argument.");
            delete *iter;
            symbol_stack.erase(iter);
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Store the indices
            new_indices += { std::get<0>(v1) - 1, std::get<0>(v2) - 1, std::get<0>(v3) - 1 };

            // Ignore the normal coordinates for now

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "face(v_vn)";

    }
}

face_v_vt_vn: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::v_vt_vn:
            // Parse as many as we can
            goto face_v_vt_vn;

        case TerminalType::uint:
        case TerminalType::sint:
        case TerminalType::v_vt:
        case TerminalType::v_vn:
            // Invalid argument type
            term->debug_info.print_error(cerr, "Cannot mix vertex/texture/normal indices with other indices.");
            (*(iter - (i - 2)))->debug_info.print_note(cerr, "Face type determined by first argument.");
            delete *iter;
            symbol_stack.erase(iter);
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Store the indices
            new_indices += { std::get<0>(v1) - 1, std::get<0>(v2) - 1, std::get<0>(v3) - 1 };

            // Ignore the texture & normal coordinates for now

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "face(v_vt_vn)";

    }
}



group_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name:
            // Parse the group (for now, simply remove it)
            remove_stack_bottom(symbol_stack, iter);
            DRETURN "group";
        
        default:
            // Missing name
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after group definition.");
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "error";

    }
}



mtllib_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::filename:
            // Parse the material file
            load_mtl_lib(mtl_map, ((ValueTerminal<std::string>*) term)->value);

            // Done with this one
            remove_stack_bottom(symbol_stack, iter);
            DRETURN "mtllib";
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing filename after material definition.");
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "error";

    }
}


usemtl_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name: {
            // Set this material as current, but only if we know it
            std::string material = ((ValueTerminal<std::string>*) term)->value;
            if (mtl_map.find(material) != mtl_map.end()) {
                term->debug_info.print_error(cerr, "Unknown material name '" + material + "'.");
                remove_stack_bottom(symbol_stack, iter);
                DRETURN "error";
            }

            // Else, set as current and return
            current_mtl = material;
            remove_stack_bottom(symbol_stack, iter);
            DRETURN "usemtl";
        }
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after material usage.");
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "error";

    }
}



    // Nothing applied
    DLOG(fatal, "Hole in jump logic encountered.");
    DRETURN "fatal";
} 





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and returns a list of vertices and list of indices in that list of vertices from it. */
void Models::load_obj_model(Tools::Array<Rendering::Vertex>& new_vertices, Tools::Array<Rendering::index_t>& new_indices, const std::string& path) {
    DENTER("Models::load_obj_model");

    // Prepare the Tokenizer
    Obj::Tokenizer tokenizer(path);
    // Prepare the 'symbol stack'
    Tools::LinkedArray<Terminal*> symbol_stack;

    // Prepare the material map
    std::string current_mtl;
    std::unordered_map<std::string, glm::vec3> mtl_map;

    // Start looping to parse stuff off the stack
    bool changed = true;
    while (changed) {
        // Run the parser
        std::string rule = reduce(new_vertices, new_indices, current_mtl, mtl_map, symbol_stack);
        changed = !rule.empty() && rule != "error";

        // If there's no change and we're not at the end, pop a new terminal
        if (!changed && !tokenizer.eof()) {
            Terminal* term = tokenizer.get();
            if (term == nullptr) {
                // Early quit
                #ifdef EXTRA_DEBUG
                printf("[objloader] Tokenizer failed.\n");
                #endif
                exit(EXIT_FAILURE);
            } else if (term->type != TerminalType::eof) {
                symbol_stack.push_back(term);
                changed = true;
                #ifdef EXTRA_DEBUG
                printf("[objloader] Shifted new token: %s\n", terminal_type_names[(int) term->type].c_str());
                #endif
            } else {
                // Delete the token again
                #ifdef EXTRA_DEBUG
                printf("[objloader] No tokens to shift anymore.\n");
                #endif
                delete term;
            }
        } else if (rule == "error") {
            // Stop
            exit(EXIT_FAILURE);
        } else if (changed) {
            // Print that it did
            #ifdef EXTRA_DEBUG
            printf("[objloader] Applied rule '%s'\n", rule.c_str());
            #endif
        }

        #ifdef EXTRA_DEBUG
        // Print the symbol stack
        cout << "Symbol stack:";
        for (Terminal* term : symbol_stack) {
            cout << ' ' << terminal_type_names[(int) term->type];
        }
        cout << endl;
        #endif
    }

    // When done, delete everything on the symbol stack
    for (Terminal* term : symbol_stack) {
        delete term;
    }

    // Done
    DRETURN;
}
