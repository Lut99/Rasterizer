/* MTL LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 18:31:35
 * Last edited:
 *   07/08/2021, 23:13:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MtlLoader, which is an expert on loading the .mtl files
 *   associated with .obj files.
**/


/***** MACROS *****/
/* If defined, enables extra debug prints tracing the tokenizer's steps. */
// #define EXTRA_DEBUG





/***** INCLUDES *****/
#include "tools/CppDebugger.hpp"
#include "tools/LinkedArray.hpp"

#include "tokenizer/ValueTerminal.hpp"
#include "tokenizer/Tokenizer.hpp"

#include "../auxillary/ParserTools.hpp"
#include "MtlLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Mtl;
using namespace CppDebugger::SeverityValues;





/***** HELPER FUNCTIONS *****/
/* Given a symbol stack, tries to reduce it according to the rules to parse new vertices and indices. Returns the rule applied. */
static std::string reduce(std::string& current_mtl, std::unordered_map<std::string, glm::vec3>& new_materials, Tools::LinkedArray<Terminal*>& symbol_stack) {
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
        case TerminalType::Kd:
            // Start of a colour definition, but only continue if we're in a material
            if (current_mtl.empty()) {
                term->debug_info.print_error(cerr, "Encountered colour definition without newmtl.");
                remove_stack_bottom(symbol_stack, iter);
                DRETURN "error";
            }
            goto Kd_start;

        case TerminalType::decimal:
            // Looking at a decimal without a colour definition; stop
            term->debug_info.print_error(cerr, "Encountered stray colour value.");
            remove_stack_bottom(symbol_stack, iter);
            DRETURN "error";
        
        case TerminalType::newmtl:
            // Start of a new material
            goto newmtl_start;
        
        case TerminalType::eof:
            // Nothing to be done anymore
            DRETURN "";

        default:
            // Unexpected token
            term->debug_info.print_error(cerr, "Unexpected token '" + terminal_type_names[(int) term->type] + "'.");
            remove_stack_bottom(symbol_stack, iter);;
            DRETURN "error";

    }

}



Kd_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::decimal:
            // Simply keep trying to grab more, since too many is good for error handling
            goto Kd_start;

        default:
            // Check if the color is too small or large
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few values given for diffuse colour (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many values given for diffuse colour (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the vector; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            float b = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float g = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float r = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // Store the colour for this material
            new_materials.at(current_mtl) = glm::vec3(r, g, b);

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "Kd";

    }
}



newmtl_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name:
            // Parse the material and set it as current
            current_mtl = ((ValueTerminal<std::string>*) term)->value;
            new_materials.insert(make_pair(current_mtl, glm::vec3(-1.0, -1.0, -1.0)));
            remove_stack_bottom(symbol_stack, iter);
            DRETURN "newmtl";
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after new material definition.");
            remove_stack_bottom(symbol_stack, --iter);
            DRETURN "error";

    }
}



    // Nothing applied
    DLOG(fatal, "Hole in jump logic encountered.");
    DRETURN "fatal";
}




/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .mtl file, and returns a map of material name: color schemes for it. */
void Models::load_mtl_lib(std::unordered_map<std::string, glm::vec3>& new_materials, const std::string& path) {
    DENTER("Models::load_mtl_lib");

    // Prepare the Tokenizer
    Tokenizer tokenizer(path);
    // Prepare the 'symbol stack'
    Tools::LinkedArray<Terminal*> symbol_stack;

    // Start looping to parse stuff off the stack
    std::string current_mtl;
    bool changed = true;
    while (changed) {
        // Run the parser
        std::string rule = reduce(current_mtl, new_materials, symbol_stack);
        changed = !rule.empty() && rule != "error";

        // If there's no change and we're not at the end, pop a new terminal
        if (!changed && !tokenizer.eof()) {
            Terminal* term = tokenizer.get();
            if (term == nullptr) {
                // Early quit
                #ifdef EXTRA_DEBUG
                printf("[mtlloader] Tokenizer failed.\n");
                #endif
                exit(EXIT_FAILURE);
            } else if (term->type != TerminalType::eof) {
                symbol_stack.push_back(term);
                changed = true;
               #ifdef EXTRA_DEBUG
                printf("[mtlloader] Shifted new token: %s\n", terminal_type_names[(int) term->type].c_str());
                printf("            Total progress: %.2f%%\n", (float) tokenizer.bytes() / (float) tokenizer.size() * 100.0f);
                #else
                printf("Progress: %.2f%%\r", (float) tokenizer.bytes() / (float) tokenizer.size() * 100.0f);
                #endif
            } else {
                // Add the EOF token at the end if there isn't one already
                if (symbol_stack.size() == 0 || symbol_stack.last()->type != TerminalType::eof) {
                    symbol_stack.push_back(term);
                    changed = true;
                } else {
                    delete term;
                }
                #ifdef EXTRA_DEBUG
                printf("[mtlloader] No tokens to shift anymore.\n");
                #endif
            }
        } else if (rule == "error") {
            // Stop
            exit(EXIT_FAILURE);
        } else if (changed) {
            // Print that it did
            #ifdef EXTRA_DEBUG
            printf("[mtlloader] Applied rule '%s'\n", rule.c_str());
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

    // D0ne
    DRETURN;
}

