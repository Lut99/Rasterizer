/* PARSER TOOLS.hpp
 *   by Lut99
 *
 * Created:
 *   07/08/2021, 22:11:17
 * Last edited:
 *   07/08/2021, 22:21:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains common macro's and tools for Parsers.
**/

#ifndef MODELS_PARSER_TOOLS_HPP
#define MODELS_PARSER_TOOLS_HPP

#include "tools/LinkedArray.hpp"

/***** MACROS *****/
/* If defined, enables extra debug prints tracing the tokenizer's steps. */
// #define EXTRA_DEBUG



namespace Makma3D::Models {
    /* Deletes symbols from the top of the stack until (and including) the given iterator. Also safely deletes the pointers themselves. */
    template <class T>
    void remove_stack_bottom(Tools::LinkedArray<T*>& symbol_stack, typename Tools::LinkedArray<T*>::iterator& iter) {
        // First, delete the tokens
        for (Tools::LinkedArray<Terminal*>::iterator i = symbol_stack.begin(); i != iter; ++i) {
            delete *i;
        }
        delete *iter;

        // Now remove them from the stack itself
        symbol_stack.erase_until(iter);

        return;
    }

}

#endif
