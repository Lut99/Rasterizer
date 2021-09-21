/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:15
 * Last edited:
 *   9/20/2021, 8:05:50 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/


/***** MACROS *****/
/* If defined, enables extra debug prints tracing the tokenizer's steps. */
// #define EXTRA_DEBUG





/***** INCLUDES *****/
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "tools/LinkedArray.hpp"

#include "auxillary/parsers/Symbol.hpp"
#include "auxillary/parsers/ParserTools.hpp"

#include "tokenizer/ValueToken.hpp"
#include "tokenizer/Tokenizer.hpp"
#include "ast/Nonterm.hpp"
#include "ast/Vertex.hpp"
#include "ObjLoader.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Models;
using namespace Makma3D::Models::Obj;
using namespace Makma3D::Auxillary;


/***** CONSTANTS *****/
/* Channel for the ObjLoader. */
static constexpr const char* channel = "ObjLoader";





/***** HELPER STRUCTS *****/
/* Keeps track of the parser's state in between reduce() calls. */
struct ParserState {
    /* The symbol stack we're operating on. */
    Tools::LinkedArray<Symbol*> symbol_stack;

    /* 'Global' (i.e., file-wide) list of vertex points. */
    Tools::Array<glm::vec4> vertices;
    /* 'Global" (i.e., file-wide) list of vertex normals. */
    Tools::Array<glm::vec3> normals;
    /* 'Global' (i.e., file-wide) list of texture coordinates. */
    Tools::Array<glm::vec2> texels;

    /* 'Global' (i.e., file-wide) collection of vertex/normal/texel pairs, as used in our renderer. Each is identifyable by their unique index set. */
    std::unordered_map<glm::uvec3, std::pair<uint32_t, Rendering::Vertex>> model_vertices;
    /* List of index lists, each of which uses a separate material. */
    Tools::Array<Tools::Array<Rendering::index_t>> model_indices;
    /* For each list of indices, defines the material with which it should be rendered. */
    Tools::Array<Materials::material_t> model_materials;

};





/***** HELPER FUNCTIONS *****/
/* Transfers the given vertex and index lists to the given Mesh component. */
static void transfer_to_mesh(Rendering::MemoryManager& memory_manager, ECS::Mesh& mesh, const std::unordered_map<glm::uvec3, std::pair<uint32_t, Rendering::Vertex>>& unordered_vertices, const Tools::Array<Rendering::index_t>& cpu_indices) {
    logger.logc(Verbosity::debug, channel, "Arranging vertices for copy...");

    // Flatten the unordered vertices to a list
    Tools::Array<Rendering::Vertex> cpu_vertices;
    cpu_vertices.resize(static_cast<uint32_t>(unordered_vertices.size()));
    for (const std::pair<glm::uvec3, std::pair<uint32_t, Rendering::Vertex>>& p : unordered_vertices) {
        cpu_vertices[p.second.first] = p.second.second;
    }



    logger.logc(Verbosity::debug, channel, "Transferring mesh '", mesh.name, "' (", cpu_vertices.size(), " vertices & ", cpu_indices.size(), " indices) to GPU (", Tools::bytes_to_string(cpu_vertices.size() * sizeof(Rendering::Vertex) + cpu_indices.size() * sizeof(Rendering::index_t)), ")...");

    // Allocate memory in the mesh
    VkDeviceSize vertices_size = cpu_vertices.size() * sizeof(Rendering::Vertex);
    VkDeviceSize indices_size  = cpu_indices.size() * sizeof(Rendering::index_t);
    mesh.vertices = memory_manager.draw_pool.allocate(vertices_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    mesh.indices  = memory_manager.draw_pool.allocate(indices_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    // Allocate a stage memory
    Rendering::Buffer* stage = memory_manager.stage_pool.allocate(std::max({vertices_size, indices_size}), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_map;
    stage->map(&stage_map);

    // First copy the vertices to the mesh via the stage buffer
    memcpy(stage_map, cpu_vertices.rdata(), vertices_size);
    stage->flush();
    stage->copyto(mesh.vertices, vertices_size, 0, 0, memory_manager.copy_cmd);
    
    // Then copy the indices the same way
    memcpy(stage_map, cpu_indices.rdata(), indices_size);
    stage->flush();
    stage->copyto(mesh.indices, indices_size, 0, 0, memory_manager.copy_cmd);

    // We're done, deallocate the stage buffer
    stage->unmap();
    memory_manager.stage_pool.free(stage);
}

/* Stores a given vertex/texture/normal index in the given state struct. The final (returned) index will be different to the given one, as the result indexes into our large array of unique vertices, where uniqueness is also determined by texture and normal coordinates. */
static uint32_t store_vertex(ParserState& state, uint32_t vertex_index, uint32_t texture_index = std::numeric_limits<uint32_t>::max(), uint32_t normal_index = std::numeric_limits<uint32_t>::max()) {
    // First, check if the pair already exists
    glm::uvec3 index = {vertex_index, normal_index, texture_index};
    std::unordered_map<glm::uvec3, std::pair<uint32_t, Rendering::Vertex>>::iterator iter = state.model_vertices.find(index);
    if (iter != state.model_vertices.end()) {
        // Simply return the existing index
        return (*iter).second.first;
    } else {
        // Generate the new index
        uint32_t to_return = static_cast<uint32_t>(state.model_vertices.size());
        // Insert the new vertex, with flipped texture y-coordinates cuz of Vulkan
        state.model_vertices.insert(make_pair(
            index,
            make_pair(
                to_return,
                Rendering::Vertex(
                    glm::vec3(state.vertices[vertex_index - 1]),
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    { state.texels[texture_index - 1].x, -state.texels[texture_index - 1].y }
                )
            )
        ));
        // Return the index
        return to_return;
    }
}

/* Given a symbol stack, tries to reduce it according to the rules to parse new vertices and indices. Returns the rule applied. */
static std::string reduce(ParserState& state, Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system, ECS::Model& model, const std::string& path) {
    // Prepare the iterator over the linked array
    Tools::LinkedArray<Symbol*>::reverse_iterator iter = state.symbol_stack.rbegin();
    Tools::linked_array_size_t i = 0;



/* start */ {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (iter == state.symbol_stack.rend()) { return ""; }
    ++i;

    // Do different things depending on if we're looking at a terminal or a nonterminal
    Symbol* sym = *iter;
    if (sym->is_term()) {
        // It's a terminal
        Token* token = sym->as<Token>();
        switch(token->type()) {
            case TerminalType::vertex:
                // Wait, as more interesting things are bound to come
                return "";
            
            case TerminalType::decimal:
                // Could be the end of a vertex
                goto decimal_start;

            default:
                // Unexpected token
                token->debug_info().print_error(cerr, "Unexpected token '" + terminal_type_names[(int) token->type()] + "'.");
                remove_stack_bottom(state.symbol_stack, iter);;
                return "error";

        }
    } else {
        // It's a nonterminal
        Nonterm* nterm = sym->as<Nonterm>();
        switch(nterm->type()) {
            case NonterminalType::vertex:
                // Wait, as more interesting things are bound to come
                return "";

            default:
                // Unexpected nonterminal
                nterm->debug_info().print_error(cerr, "Unexpected nonterminal '" + nonterminal_type_names[(int) nterm->type()] + "'.");
                remove_stack_bottom(state.symbol_stack, iter);;
                return "error";

        }
    }
}



decimal_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (iter == state.symbol_stack.rend()) { return ""; }
    ++i;

    // Do different things depending on if we're looking at a terminal or a nonterminal
    Symbol* sym = *iter;
    if (sym->is_term()) {
        // It's a terminal
        Token* token = sym->as<Token>();
        switch(token->type()) {
            case TerminalType::vertex:
                // Make sure there are enough coordinates
                if (i - 1 < 3) {
                    // Not enough coordinates; we probably have to wait
                    return "";

                } else if (i - 1 > 4) {
                    token->debug_info().print_error(cerr, "Too many coordinates given for vector (got " + std::to_string(i - 1) + ", expected 3-4)");
                    remove_stack_bottom(state.symbol_stack, iter);
                    return "error";

                }

                // Parse the values
                AST::Vertex* new_nterm;
                if (i - 1 == 3) {
                    // Retrieve them from the stack, walking back
                    Tools::LinkedArray<Symbol*>::reverse_iterator value_iter = iter;
                    float x = (*(--value_iter))->as<ValueToken<float>>()->value;
                    float y = (*(--value_iter))->as<ValueToken<float>>()->value;
                    float z = (*(--value_iter))->as<ValueToken<float>>()->value;

                    // Create the Vertex nonterminal
                    new_nterm = new AST::Vertex(x, y, z, (*iter)->debug_info() + (*value_iter)->debug_info());
                } else if (i - 1 == 4) {
                    // Retrieve them from the stack, walking back
                    Tools::LinkedArray<Symbol*>::reverse_iterator value_iter = iter;
                    float x = (*(--value_iter))->as<ValueToken<float>>()->value;
                    float y = (*(--value_iter))->as<ValueToken<float>>()->value;
                    float z = (*(--value_iter))->as<ValueToken<float>>()->value;
                    float w = (*(--value_iter))->as<ValueToken<float>>()->value;

                    // Create the Vertex nonterminal
                    new_nterm = new AST::Vertex(x, y, z, w, (*iter)->debug_info() + (*value_iter)->debug_info());
                }

                // Replace the new vertex nonterminal on the stack
                /* TBD */

                return "";
            
            case TerminalType::decimal:
                // Try to find more decimals
                goto decimal_start;

            default:
                // Unexpected token
                token->debug_info().print_error(cerr, "Unexpected token '" + terminal_type_names[(int) token->type()] + "'.");
                remove_stack_bottom(state.symbol_stack, iter);;
                return "error";

        }
    } else {
        // Any nonterminal counts as a stray decimal
        sym->debug_info().print_error(cerr, "Encountered stray coordinate.");
        remove_stack_bottom(state.symbol_stack, --iter);;
        return "error";
    }
}

}

/* Given a symbol stack, tries to reduce it according to the rules to parse new vertices and indices. Returns the rule applied. */
static std::string reduce_old(ParserState& state, Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system, ECS::Model& model, const std::string& path) {
    // Prepare the iterator over the linked array
    Tools::LinkedArray<Symbol*>::reverse_iterator iter = state.symbol_stack.rbegin();
    Tools::linked_array_size_t i = 0;



/* start */ {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (iter == state.symbol_stack.rend()) { return ""; }
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
            remove_stack_bottom(state.symbol_stack, iter);;
            return "error";

        case TerminalType::group:
            // Start of a new group definition
            goto group_start;

        case TerminalType::object:
            // Start of a new object definition
            goto object_start;

        case TerminalType::mtllib:
            // Start of a new material definition
            goto mtllib_start;

        case TerminalType::usemtl:
            // Start of a material usage
            goto usemtl_start;

        case TerminalType::smooth:
            // Start of a smooth shading command
            goto smooth_start;

        case TerminalType::eof:
            // Nothing to be done anymore
            return "";

        default:
            // Unexpected token
            term->debug_info.print_error(cerr, "Unexpected token '" + terminal_type_names[(int) term->type] + "'.");
            remove_stack_bottom(state.symbol_stack, iter);;
            return "error";

    }

}



vertex_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 4) {
                term->debug_info.print_error(cerr, "Too many coordinates given for vector (got " + std::to_string(i - 2) + ", expected 4)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the vector; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            float w = 0; if (i - 2 == 4) { w = ((ValueTerminal<float>*) (*(--value_iter)))->value; }
            float z = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // /* DEBUG */: Add extra padding to separate the individual groups
            x += model.meshes.size();

            // Store the vertex
            state.vertices.push_back({ x, y, z, w });

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "vertex";

    }
}



normal_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for normal (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the vector normal; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            float z = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // Store the normal
            state.normals.push_back({ x, y, z });

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "normal";

    }
}



texture_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for texture coordinate (got " + std::to_string(i - 2) + ", expected 2)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the vector normal; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // Store the normal
            state.texels.push_back({ x, y });

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "texture";

    }
}



face_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
                state.symbol_stack.erase(iter);
                return "not-yet-implemented";
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
            remove_stack_bottom(state.symbol_stack, --iter);
            return "error";

    }
}

face_v: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
                state.symbol_stack.erase(iter);
                return "not-yet-implemented";
            }

        case TerminalType::v_vt:
        case TerminalType::v_vn:
        case TerminalType::v_vt_vn:
            // Invalid argument type
            term->debug_info.print_error(cerr, "Cannot mix vertex-only indices with other indices.");
            (*(iter - (i - 2)))->debug_info.print_note(cerr, "Face type determined by first argument.");
            delete *iter;
            state.symbol_stack.erase(iter);
            return "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the face; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            uint32_t v3 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;
            uint32_t v2 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;
            uint32_t v1 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;

            // Set the vertices
            uint32_t i1 = store_vertex(state, v1);
            uint32_t i2 = store_vertex(state, v2);
            uint32_t i3 = store_vertex(state, v3);

            // Store the indices
            state.temp_indices   += { i1, i2, i3 };
            state.mesh.n_indices += 3;

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "face(v)";

    }
}

face_v_vt: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
            state.symbol_stack.erase(iter);
            return "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Set the vertices
            uint32_t i1 = store_vertex(state, std::get<0>(v1), std::get<1>(v1));
            uint32_t i2 = store_vertex(state, std::get<0>(v2), std::get<1>(v2));
            uint32_t i3 = store_vertex(state, std::get<0>(v3), std::get<1>(v3));

            // Store the indices
            state.temp_indices   += { i1, i2, i3 };
            state.mesh.n_indices += 3;

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "face(v_vt)";

    }
}

face_v_vn: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
            state.symbol_stack.erase(iter);
            return "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Set the vertices
            uint32_t i1 = store_vertex(state, std::get<0>(v1), std::numeric_limits<uint32_t>::max(), std::get<1>(v1));
            uint32_t i2 = store_vertex(state, std::get<0>(v2), std::numeric_limits<uint32_t>::max(), std::get<1>(v2));
            uint32_t i3 = store_vertex(state, std::get<0>(v3), std::numeric_limits<uint32_t>::max(), std::get<1>(v3));

            // Store the indices
            state.temp_indices   += { i1, i2, i3 };
            state.mesh.n_indices += 3;

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "face(v_vn)";

    }
}

face_v_vt_vn: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
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
            state.symbol_stack.erase(iter);
            return "error";

        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                return "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Set the vertices
            uint32_t i1 = store_vertex(state, std::get<0>(v1), std::get<1>(v1), std::get<2>(v1));
            uint32_t i2 = store_vertex(state, std::get<0>(v2), std::get<1>(v2), std::get<2>(v2));
            uint32_t i3 = store_vertex(state, std::get<0>(v3), std::get<1>(v3), std::get<2>(v3));

            // Store the indices
            state.temp_indices   += { i1, i2, i3 };
            state.mesh.n_indices += 3;

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            return "face(v_vt_vn)";

    }
}



group_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name:
            // If the previous mesh needs copying, do so
            if (state.mesh.n_indices > 0) {
                transfer_to_mesh(memory_manager, state.mesh, state.temp_vertices, state.temp_indices);
                model.meshes.push_back(state.mesh);
            }

            // Reset the mesh, then set its name
            state.mesh = {};
            state.mesh.name = ((ValueTerminal<std::string>*) term)->value;
            state.mesh.material = Materials::DefaultMaterial;

            // Remove the token, then we're done
            remove_stack_bottom(state.symbol_stack, iter);
            return "group";
        
        default:
            // Missing name
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after group definition.");
            remove_stack_bottom(state.symbol_stack, --iter);
            return "error";

    }
}



object_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name:
            // Set the name, if any
            state.mesh.name = ((ValueTerminal<std::string>*) term)->value;

            // Remove the token, then we're done
            remove_stack_bottom(state.symbol_stack, iter);
            return "object";
        
        default:
            // Missing name
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after object definition.");
            remove_stack_bottom(state.symbol_stack, --iter);
            return "error";

    }
}



mtllib_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::filename: {
            // Fetch a material file relative to the obj file
            std::string filepath = path;
            size_t slash_pos = filepath.find_last_of('/');
            if (slash_pos == std::string::npos) {
                slash_pos = filepath.find_last_of('\\');
            }
            if (slash_pos != std::string::npos) {
                filepath = filepath.substr(0, slash_pos + 1);
            } else {
                filepath += "/";
            }
            filepath += ((ValueTerminal<std::string>*) term)->value;

            // Parse the material file
            if (FILE* test = fopen(filepath.c_str(), "r")) {
                fclose(test);

                // Load the referenced material file
                logger.logc(Verbosity::details, channel, "Loading '", filepath, "' as .mtl file...");
                Tools::Array<std::pair<std::string, Materials::material_t>> new_materials = material_system.load_simple_coloured(filepath);

                // Add the materials to the internal library
                stringstream sstr;
                for (uint32_t i = 0; i < new_materials.size(); i++) {
                    // Add the material, possibly with a warning if it's a duplicate
                    if (state.mtl_lib.find(new_materials[i].first) != state.mtl_lib.end()) { logger.warningc(channel, "Overwriting material '", new_materials[i].first, "' with new value."); }
                    state.mtl_lib.insert(new_materials[i]);
                    
                    // Construct the new material string
                    if (i > 0) { sstr << ", "; }
                    sstr << '\'' << new_materials[i].first << '\'';
                }

                // Show what we loaded and done
                logger.logc(Verbosity::debug, channel, "Loaded materials: ", sstr.str());
            } else {
                (*iter)->debug_info.print_warning(cerr, "Material library '" + filepath + "' not found, skipping.");
            }

            // Done with this one
            remove_stack_bottom(state.symbol_stack, iter);
            return "mtllib";
        }
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing filename after material definition.");
            remove_stack_bottom(state.symbol_stack, --iter);
            return "error";

    }
}

usemtl_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name: {
            // Set this material as current, but only if we know it
            std::string material = ((ValueTerminal<std::string>*) term)->value;
            std::unordered_map<std::string, Materials::material_t>::iterator mtl_iter = state.mtl_lib.find(material);
            if (mtl_iter == state.mtl_lib.end()) {
                term->debug_info.print_warning(cerr, "Unknown material name '" + material + "'.");
                Auxillary::remove_stack_bottom(state.symbol_stack, iter);
                return "warning";
            }

            // Else, set as current and return
            state.mesh.material = (*mtl_iter).second;
            Auxillary::remove_stack_bottom(state.symbol_stack, iter);
            return "usemtl";
        }

        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after material usage.");
            Auxillary::remove_stack_bottom(state.symbol_stack, --iter);
            return "error";

    }
}



smooth_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { return ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name: {
            // Check if the name is what we want
            std::string value = ((ValueTerminal<std::string>*) term)->value;
            if (value == "1") {
                // Do nothing

            } else if (value == "off") {
                // Do nothing

            } else {
                term->debug_info.print_error(cerr, "Unknown smooth shading value '" + value + "'");
                Auxillary::remove_stack_bottom(state.symbol_stack, iter);
                return "error";
            }

            // Pop from the stack and return
            Auxillary::remove_stack_bottom(state.symbol_stack, iter);
            return "smooth";
        }
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after material usage.");
            Auxillary::remove_stack_bottom(state.symbol_stack, --iter);
            return "error";

    }
}



    // Nothing applied
    logger.fatal(channel, "Hole in jump logic encountered.");
    return "fatal";
} 





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and populates the given model from it. The n_vertices and n_indices are debug counters, to keep track of the total number of vertices and indices loaded. */
void Models::load_obj_model(Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system, ECS::Model& model, const std::string& path) {
    // Prepare the Tokenizer
    Obj::Tokenizer tokenizer(path);

    // Prepare the parser state
    ParserState state{};
    state.mesh.name = "-";
    state.mesh.material = Materials::DefaultMaterial;

    // Start looping to parse stuff off the stack
    bool changed = true;
    #ifdef EXTRA_DEBUG
    printf("[objloader] Starting parsing.\n");
    #endif
    while (changed) {
        // Run the parser
        std::string rule = reduce(state, memory_manager, material_system, model, path);
        changed = !rule.empty() && rule != "error";

        // If there's no change and we're not at the end, pop a new terminal
        if (!changed && !tokenizer.eof()) {
            Token* token = tokenizer.get();
            if (token == nullptr) {
                // Early quit
                #ifdef EXTRA_DEBUG
                printf("[objloader] Tokenizer failed.\n");
                #endif
                exit(EXIT_FAILURE);
            } else if (token->type() != TerminalType::eof) {
                state.symbol_stack.push_back((Symbol*) token);
                changed = true;
                #ifdef EXTRA_DEBUG
                printf("[objloader] Shifted new token: %s\n", terminal_type_names[(int) term->type].c_str());
                printf("            Total progress: %.2f%%\n", (float) tokenizer.bytes() / (float) tokenizer.size() * 100.0f);
                #else
                printf("Progress: %.2f%%\r", (float) tokenizer.bytes() / (float) tokenizer.size() * 100.0f);
                #endif
            } else {
                // Add the EOF token at the end if there isn't one already
                if (state.symbol_stack.size() == 0 || state.symbol_stack.last()->as<Token>()->type() != TerminalType::eof) {
                    state.symbol_stack.push_back((Symbol*) token);
                    changed = true;
                } else {
                    delete token;
                }
                #ifdef EXTRA_DEBUG
                printf("[objloader] No tokens to shift anymore.\n");
                #endif
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
        for (Terminal* term : state.symbol_stack) {
            cout << ' ' << terminal_type_names[(int) term->type];
        }
        cout << endl;
        #endif
    }

    // If there are indices left, transfer them to the meshes list too
    if (state.mesh.n_indices > 0) {
        // Do the transfer
        transfer_to_mesh(memory_manager, state.mesh, state.temp_vertices, state.temp_indices);
        model.meshes.push_back(state.mesh);
    }

    // When done, delete everything left on the symbol stack (we assume all errors have been processed)
    if (state.symbol_stack.size() > 1 || (state.symbol_stack.size() == 1 && state.symbol_stack.first()->as<Token>()->type() != TerminalType::eof)) {
        logger.warningc(channel, "Symbol stack hasn't been completely emptied by parser");
        std::stringstream sstr;
        bool first = true;
        for (Symbol* term : state.symbol_stack) {
            if (first) { first = false; }
            else { sstr << " "; }
            sstr << terminal_type_names[(int) term->type];
            delete term;
        }
        logger.warningc(channel, "Current stack: [" + sstr.str() + "]");
    }

    // Done
    return;
}
