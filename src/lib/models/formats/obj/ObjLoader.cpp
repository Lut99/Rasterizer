/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:15
 * Last edited:
 *   07/08/2021, 23:11:09
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
#include <sstream>

#include "tools/CppDebugger.hpp"
#include "tools/Common.hpp"
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





/***** HELPER STRUCTS *****/
/* Keeps track of the parser's state in between reduce() calls. */
struct ParserState {
    /* The symbol stack we're operating on. */
    Tools::LinkedArray<Terminal*> symbol_stack;

    /* The current Mesh we're working on. */
    ECS::Mesh mesh;

    /* Temporary list of vertices that we collect in main memory before we move it to GPU memory. */
    Tools::Array<Rendering::Vertex> temp_vertices;
    /* Temporary list of indices that we collect in main memory before we move it to GPU memory. */
    Tools::Array<Rendering::index_t> temp_indices;

    /* Library of materials, as defined in .mtl files. */
    std::unordered_map<std::string, glm::vec3> mtl_lib;

};





/***** HELPER FUNCTIONS *****/
/* Transfers the given vertex and index lists to the given Mesh component. */
static void transfer_to_mesh(Rendering::MemoryManager& memory_manager, ECS::Mesh& mesh, const Tools::Array<Rendering::Vertex>& cpu_vertices, const Tools::Array<Rendering::index_t>& cpu_indices) {
    DENTER("transfer_to_mesh");
    DINDENT;
    DLOG(info, "Transferring mesh '" + mesh.name + "' (" + std::to_string(cpu_vertices.size()) + " vertices & " + std::to_string(cpu_indices.size()) + " indices) to GPU (" + Tools::bytes_to_string(cpu_vertices.size() * sizeof(Rendering::Vertex) + cpu_indices.size() * sizeof(Rendering::index_t) + sizeof(Rendering::MeshData)) + ")...");

    // Allocate memory in the mesh
    VkDeviceSize vertices_size = cpu_vertices.size() * sizeof(Rendering::Vertex);
    VkDeviceSize indices_size  = cpu_indices.size() * sizeof(Rendering::index_t);
    VkDeviceSize data_size     = sizeof(Rendering::MeshData);
    mesh.vertices_h = memory_manager.draw_pool.allocate_buffer_h(vertices_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    mesh.indices_h  = memory_manager.draw_pool.allocate_buffer_h(indices_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    mesh.data_h     = memory_manager.draw_pool.allocate_buffer_h(data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    Rendering::Buffer vertices = memory_manager.draw_pool.deref_buffer(mesh.vertices_h);
    Rendering::Buffer indices  = memory_manager.draw_pool.deref_buffer(mesh.indices_h);
    Rendering::Buffer data     = memory_manager.draw_pool.deref_buffer(mesh.data_h);

    // Allocate a stage memory
    Rendering::Buffer stage = memory_manager.stage_pool.allocate_buffer(std::max(vertices_size, indices_size, data_size), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_map;
    stage.map(&stage_map);

    // Fetch the command buffer
    Rendering::CommandBuffer copy_cmd = memory_manager.copy_cmd();

    // First copy the vertices to the mesh via the stage buffer
    memcpy(stage_map, cpu_vertices.rdata(), vertices_size);
    stage.flush();
    stage.copyto(vertices, vertices_size, 0, 0, copy_cmd);
    
    // Then copy the indices the same way
    memcpy(stage_map, cpu_indices.rdata(), indices_size);
    stage.flush();
    stage.copyto(indices, indices_size, 0, 0, copy_cmd);

    // Finally, copy the auxillary mesh data
    memcpy(stage_map, &mesh.data, sizeof(Rendering::MeshData));
    stage.flush();
    stage.copyto(data, data_size, 0, 0, copy_cmd);

    // We're done, deallocate the stage buffer
    stage.unmap();
    memory_manager.stage_pool.deallocate(stage);

    // Done
    DDEDENT;
    DRETURN;
}

/* Given a symbol stack, tries to reduce it according to the rules to parse new vertices and indices. Returns the rule applied. */
static std::string reduce(ParserState& state, Rendering::MemoryManager& memory_manager, ECS::Meshes& meshes, const std::string& path) {
    DENTER("reduce");

    // Prepare the iterator over the linked array
    Tools::LinkedArray<Terminal*>::iterator iter = state.symbol_stack.begin();
    Tools::linked_array_size_t i = 0;



/* start */ {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (iter == state.symbol_stack.end()) { DRETURN ""; }
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
        
        case TerminalType::eof:
            // Nothing to be done anymore
            DRETURN "";

        default:
            // Unexpected token
            term->debug_info.print_error(cerr, "Unexpected token '" + terminal_type_names[(int) term->type] + "'.");
            remove_stack_bottom(state.symbol_stack, iter);;
            DRETURN "error";

    }

}



vertex_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
                DRETURN "error";
            } else if (i - 2 > 4) {
                term->debug_info.print_error(cerr, "Too many coordinates given for vector (got " + std::to_string(i - 2) + ", expected 4)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the vector; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            if (i - 2 == 4) { --value_iter; }
            float z = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float y = ((ValueTerminal<float>*) (*(--value_iter)))->value;
            float x = ((ValueTerminal<float>*) (*(--value_iter)))->value;

            // Store the vertex
            state.temp_vertices.push_back(Rendering::Vertex({ x, y, z }, { 0.5f + ((0.5 * rand()) / RAND_MAX), 0.0f, 0.0f }));

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "vertex";

    }
}



normal_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for normal (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
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
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "normal";

    }
}



texture_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for texture coordinate (got " + std::to_string(i - 2) + ", expected 2)");
                remove_stack_bottom(state.symbol_stack, --iter);
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
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "texture";

    }
}



face_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "error";

    }
}

face_v: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
                DRETURN "not-yet-implemented";
            }

        case TerminalType::v_vt:
        case TerminalType::v_vn:
        case TerminalType::v_vt_vn:
            // Invalid argument type
            term->debug_info.print_error(cerr, "Cannot mix vertex-only indices with other indices.");
            (*(iter - (i - 2)))->debug_info.print_note(cerr, "Face type determined by first argument.");
            delete *iter;
            state.symbol_stack.erase(iter);
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the coordinates
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            uint32_t v3 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;
            uint32_t v2 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;
            uint32_t v1 = ((ValueTerminal<uint32_t>*) (*(--value_iter)))->value;

            // Store the indices
            state.temp_indices   += { v1 - 1, v2 - 1, v3 - 1 };
            state.mesh.n_indices += 3;

            // // If there's a material, update the relevant vertex coordinates
            // if (!current_mtl.empty()) {
            //     const glm::vec3& colour = mtl_map.at(current_mtl);
            //     new_vertices[v1 - 1].colour = colour;
            //     new_vertices[v2 - 1].colour = colour;
            //     new_vertices[v3 - 1].colour = colour;
            // }

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "face(v)";

    }
}

face_v_vt: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Store the indices
            state.temp_indices   += { std::get<0>(v1) - 1, std::get<0>(v2) - 1, std::get<0>(v3) - 1 };
            state.mesh.n_indices += 3;

            // Ignore the texture coordinates for now

            // // If there's a material, update the relevant vertex coordinates
            // if (!current_mtl.empty()) {
            //     const glm::vec3& colour = mtl_map.at(current_mtl);
            //     new_vertices[std::get<0>(v1) - 1].colour = colour;
            //     new_vertices[std::get<0>(v2) - 1].colour = colour;
            //     new_vertices[std::get<0>(v3) - 1].colour = colour;
            // }

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "face(v_vt)";

    }
}

face_v_vn: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
            DRETURN "error";
        
        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Store the indices
            state.temp_indices   += { std::get<0>(v1) - 1, std::get<0>(v2) - 1, std::get<0>(v3) - 1 };
            state.mesh.n_indices += 3;

            // Ignore the normal coordinates for now

            // // If there's a material, update the relevant vertex coordinates
            // if (!current_mtl.empty()) {
            //     const glm::vec3& colour = mtl_map.at(current_mtl);
            //     new_vertices[std::get<0>(v1) - 1].colour = colour;
            //     new_vertices[std::get<0>(v2) - 1].colour = colour;
            //     new_vertices[std::get<0>(v3) - 1].colour = colour;
            // }


            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "face(v_vn)";

    }
}

face_v_vt_vn: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
            DRETURN "error";

        default:
            // We're done parsing the face; determine if we seen enough indices
            if (i - 2 < 3) {
                term->debug_info.print_error(cerr, "Too few coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            } else if (i - 2 > 3) {
                term->debug_info.print_error(cerr, "Too many coordinates given for face (got " + std::to_string(i - 2) + ", expected 3)");
                remove_stack_bottom(state.symbol_stack, --iter);
                DRETURN "error";
            }

            // Otherwise, we can parse the face; get the indices
            Tools::LinkedArray<Terminal*>::iterator value_iter = iter;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v3 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v2 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;
            const std::tuple<uint32_t, uint32_t, uint32_t>& v1 = ((ValueTerminal<std::tuple<uint32_t, uint32_t, uint32_t>>*) (*(--value_iter)))->value;

            // Store the indices
            state.temp_indices   += { std::get<0>(v1) - 1, std::get<0>(v2) - 1, std::get<0>(v3) - 1 };
            state.mesh.n_indices += 3;

            // Ignore the texture & normal coordinates for now

            // // If there's a material, update the relevant vertex coordinates
            // if (!current_mtl.empty()) {
            //     const glm::vec3& colour = mtl_map.at(current_mtl);
            //     new_vertices[std::get<0>(v1) - 1].colour = colour;
            //     new_vertices[std::get<0>(v2) - 1].colour = colour;
            //     new_vertices[std::get<0>(v3) - 1].colour = colour;
            // }

            // Remove the used symbols off the top of the stack (except the next one), then return
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "face(v_vt_vn)";

    }
}



group_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name:
            // If the previous mesh needs copying, do so
            if (state.mesh.n_indices > 0) {
                transfer_to_mesh(memory_manager, state.mesh, state.temp_vertices, state.temp_indices);
                meshes.push_back(state.mesh);
            }

            // Reset the mesh, then set its name
            state.mesh = {};
            state.mesh.name = ((ValueTerminal<std::string>*) term)->value;

            // Remove the token, then we're done
            remove_stack_bottom(state.symbol_stack, iter);
            DRETURN "group";
        
        default:
            // Missing name
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after group definition.");
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "error";

    }
}



mtllib_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
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
            DINDENT;
            DLOG(info, "Loading '" + filepath + "' as .mtl file...");
            load_mtl_lib(state.mtl_lib, filepath);
            stringstream sstr;
            bool first = true;
            for (const std::pair<std::string, glm::vec3>& p : state.mtl_lib) {
                if (first) { first = false; }
                else { sstr << ", "; }
                sstr << '\'' << p.first << '\'';
            }
            DLOG(info, "Loaded materials: " + sstr.str());
            DDEDENT;

            // Done with this one
            remove_stack_bottom(state.symbol_stack, iter);
            DRETURN "mtllib";
        }
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing filename after material definition.");
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "error";

    }
}

usemtl_start: {
    // If we're at the end of the symbol stack, then assume we just have to wait for more
    if (++iter == state.symbol_stack.end()) { DRETURN ""; }
    ++i;

    // Get the next symbol off the stack
    Terminal* term = *iter;
    switch(term->type) {
        case TerminalType::name: {
            // Set this material as current, but only if we know it
            std::string material = ((ValueTerminal<std::string>*) term)->value;
            std::unordered_map<std::string, glm::vec3>::iterator mtl_iter = state.mtl_lib.find(material);
            if (mtl_iter == state.mtl_lib.end()) {
                term->debug_info.print_error(cerr, "Unknown material name '" + material + "'.");
                remove_stack_bottom(state.symbol_stack, iter);
                DRETURN "error";
            }

            // Else, set as current and return
            state.mesh.mtl     = material;
            state.mesh.data.mtl_col = glm::vec4((*mtl_iter).second, 1.0f);
            remove_stack_bottom(state.symbol_stack, iter);
            DRETURN "usemtl";
        }
        
        default:
            // Missing filename
            (*(iter - 1))->debug_info.print_error(cerr, "Missing name after material usage.");
            remove_stack_bottom(state.symbol_stack, --iter);
            DRETURN "error";

    }
}



    // Nothing applied
    DLOG(fatal, "Hole in jump logic encountered.");
    DRETURN "fatal";
} 





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and populates the given list of meshes from it. The n_vertices and n_indices are debug counters, to keep track of the total number of vertices and indices loaded. */
void Models::load_obj_model(Rendering::MemoryManager& memory_manager, ECS::Meshes& meshes, const std::string& path) {
    DENTER("Models::load_obj_model");

    // Prepare the Tokenizer
    Obj::Tokenizer tokenizer(path);

    // Prepare the parser state
    ParserState state{};

    // Start looping to parse stuff off the stack
    bool changed = true;
    #ifdef EXTRA_DEBUG
    printf("[objloader] Starting parsing.\n");
    #endif
    while (changed) {
        // Run the parser
        std::string rule = reduce(state, memory_manager, meshes, path);
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
                state.symbol_stack.push_back(term);
                changed = true;
                #ifdef EXTRA_DEBUG
                printf("[objloader] Shifted new token: %s\n", terminal_type_names[(int) term->type].c_str());
                printf("            Total progress: %.2f%%\n", (float) tokenizer.bytes() / (float) tokenizer.size() * 100.0f);
                #else
                printf("Progress: %.2f%%\r", (float) tokenizer.bytes() / (float) tokenizer.size() * 100.0f);
                #endif
            } else {
                // Add the EOF token at the end if there isn't one already
                if (state.symbol_stack.size() == 0 || state.symbol_stack.last()->type != TerminalType::eof) {
                    state.symbol_stack.push_back(term);
                    changed = true;
                } else {
                    delete term;
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
        // Set a name & material if it doesn't have one
        if (state.mesh.name.empty()) {
            state.mesh.name = "-";
        }
        if (state.mesh.mtl.empty()) {
            state.mesh.mtl = "-";
        }

        // Do the transfer
        transfer_to_mesh(memory_manager, state.mesh, state.temp_vertices, state.temp_indices);
        meshes.push_back(state.mesh);
    }

    // When done, delete everything left on the symbol stack (we assume all errors have been processed)
    if (state.symbol_stack.size() > 1 || (state.symbol_stack.size() == 1 && state.symbol_stack.first()->type != TerminalType::eof)) {
        DLOG(warning, "Symbol stack hasn't been completely emptied by parser");
        std::stringstream sstr;
        bool first = true;
        for (Terminal* term : state.symbol_stack) {
            if (first) { first = false; }
            else { sstr << " "; }
            sstr << terminal_type_names[(int) term->type];
            delete term;
        }
        DLOG(warning, "Current stack: [" + sstr.str() + "]");
    }

    // Done
    DRETURN;
}
