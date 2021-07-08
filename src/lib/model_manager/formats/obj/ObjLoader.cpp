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

#include "model_manager/formats/auxillary/SymbolStack.hpp"
#include "tools/CppDebugger.hpp"

#include "ast/NonTerminal.hpp"
#include "tokenizer/Tokenizer.hpp"

#include "ObjLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace Rasterizer::Models::Obj;
using namespace CppDebugger::SeverityValues;


/***** MACROS *****/
/* Looks at the next symbol of the stack, returning a pointer to it. */
#define PEEK(SYMBOL, STACK_ITER, N_SYMBOLS) \
    (SYMBOL) = *((STACK_ITER)++); \
    ++(N_SYMBOLS);





/***** HELPER FUNCTIONS *****/
/* Given a symbol stack, tries to reduce it to a higher-level structure. Returns a description of the rule applied, or an empty string if no rule was applied. */
static std::string reduce(SymbolStack<Symbol>& stack) {
    DENTER("reduce");

    // Get the iterator for the SymbolStack
    SymbolStack<Symbol>::const_iterator iter = stack.begin();
    // The symbol we examine at each timestep
    Symbol* symbol;
    // Keeps track of how many symbols we're using for this rule
    size_t n_symbols = 0;



    // Start
    {
        // Look at the symbol at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type) {
                case TerminalType::uint:
                    // We might be looking at three uints, then a face token
                    goto face_start;
                
                case TerminalType::decimal:
                    // We might be looking at three floats, then a vertex token
                    goto vertex_start;

                case TerminalType::vertex:
                case TerminalType::face:
                    // We use this opportunity to check if the previous one was completely successfully
                    goto sanity_check_start;

                default:
                    // Nothing matches our rules
                    DRETURN "";

            }

        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            // We never parse on nonterminal
            DRETURN "";

        }
    }



face_start:
    {
        // Look at the symbol at the top of the stack
        // Look at the symbol at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type) {
                case TerminalType::uint:
                    // If this is actually the fourth uint we see, we throw a tantrum
                    if (n_symbols > 3) {
                        stack[0]->debug_info.print_error(cerr, "Encountered more than three vertices for a Face.");
                        stack.remove(1);
                        DRETURN "error-more-than-three-vertices";
                    }

                    // Otherwise, continue trying to parse more uints
                    goto face_start;

                case TerminalType::face:
                    // CHeck if we have seen enough vertices
                    if (n_symbols < 4) {
                        // Wait until we do
                        DRETURN "";
                    }

                    // Otherwise, we're done
                    

                case TerminalType::decimal:
                case TerminalType::vertex:
                    stack[0]->debug_info.print_error(cerr, "Cannot use unsigned integers for vertex coordinates.");
                    stack.remove(1);
                    DRETURN "error-uint-for-vertices";

                default:
                    // Nothing matches our rules
                    DRETURN "";

            }

        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            // If we see a non-terminal, it means we are missing a face
            stack[n_symbols - 1]->debug_info.print_error(cerr, "Missing data type indicator ('f' or 'v').");
            stack.remove(n_symbols);
            DRETURN "error-missing-indicator";

        }
    }



    DRETURN "";
}





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and returns a list of vertices from it. */
Tools::Array<Vertex> Models::load_obj_model(const std::string& path) {
    DENTER("Models::load_obj_model");

    // Prepare the Tokenizer
    Obj::Tokenizer tokenizer(path);

    // Initialize the symbol stack
    SymbolStack<Symbol> stack(new Terminal(TerminalType::undefined, DebugInfo("", 0, 0, {})));

    // Next, start alternating shifting by reducing
    bool changed = true;
    while (changed) {
        // Try to reduce the stack we have
        std::string applied_rule = reduce(stack);
        changed = !applied_rule.empty();

        // If we couldn't reduce anything, then try to pop the next terminal
        if (!changed) {
            // Get it
            Terminal* new_term = tokenizer.get();
            if (new_term == nullptr) { DRETURN {}; }

            // If it's EOF, though, don't add it
            if (new_term->type != TerminalType::eof) {
                stack.add_terminal(new_term);
                changed = true;
            }
        }

        // Move to the next attempt
    }

    // Done
    DRETURN {};
}
