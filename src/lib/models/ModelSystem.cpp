/* MODEL SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:09:32
 * Last edited:
 *   8/1/2021, 3:41:39 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ModelManager class, which is in charge of managing all
 *   models in the rasterizer.
**/

#include <cstring>
#include <algorithm>

#include "tools/Common.hpp"
#include "tools/CppDebugger.hpp"

#include "formats/obj/ObjLoader.hpp"

#include "ModelSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;





/***** MODELMANAGER CLASS *****/
/* Constructor for the ModelSystem class, which takes a MemoryManager struct for the required memory pools. */
ModelSystem::ModelSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager)
{
    DENTER("Models::ModelManger::ModelManager");

    // Also allocate the command buffer
    this->copy_cmd_h = this->memory_manager.mem_cmd_pool.allocate_h();

    // Done
    DLOG(info, "Initialized ModelManager.");
    DLEAVE;
}

/* Copy constructor for the ModelSystem class. */
ModelSystem::ModelSystem(const ModelSystem& other) :
    memory_manager(other.memory_manager)
{
    DENTER("Models::ModelManger::ModelManager(copy)");

    // "Copy" the buffer
    this->copy_cmd_h = this->memory_manager.mem_cmd_pool.allocate_h();

    DLEAVE;
}

/* Move constructor for the ModelSystem class. */
ModelSystem::ModelSystem(ModelSystem&& other) :
    memory_manager(other.memory_manager),
    copy_cmd_h(other.copy_cmd_h)
{
    other.copy_cmd_h = Rendering::CommandPool::NullHandle;
}

/* Destructor for the ModelSystem class. */
ModelSystem::~ModelSystem() {
    DENTER("Models::ModelManager::~ModelManager");

    if (this->copy_cmd_h != Rendering::CommandPool::NullHandle) {
        this->memory_manager.mem_cmd_pool.deallocate(this->copy_cmd_h);
    }

    DLEAVE;
}



/* Loads a model at the given path and with the given format and adds it to the given entity in the given entity manager. */
void ModelSystem::load_model(ECS::EntityManager& entity_manager, entity_t entity, const std::string& path, ModelFormat format) {
    DENTER("Models::ModelManager::load_model");
    DLOG(info, "Loading model for entity " + std::to_string(entity) + "...");
    DINDENT;

    // Get the entity's component
    ECS::Meshes& meshes = entity_manager.get_component<ECS::Meshes>(entity);

    // Load the model according to the given format
    uint32_t n_vertices = 0;
    uint32_t n_indices = 0;
    switch (format) {
        case ModelFormat::obj:
            // Use the load function from the modelloader
            DLOG(info, "Loading '" + path + "' as .obj file...");
            // load_obj_model(new_vertices, new_indices, path);
            DLOG(fatal, "Not implemented.");
            break;

        case ModelFormat::triangle: {
            // Simply set the hardcoded list
            DLOG(info, "Loading static triangle...");

            // Prepare the mesh struct
            Mesh mesh = {};
            mesh.vertices_h = this->memory_manager.draw_pool.allocate_buffer_h(3 * sizeof(Rendering::Vertex ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.indices_h  = this->memory_manager.draw_pool.allocate_buffer_h(3 * sizeof(Rendering::index_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT  | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.n_indices = 3;
            mesh.name = "triangle";
            mesh.material = "rainbow";

            // Prepare a staging buffer
            Rendering::Buffer stage_buffer = this->memory_manager.stage_pool.allocate_buffer(std::max(3 * sizeof(Rendering::Vertex), 3 * sizeof(Rendering::index_t)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            void* stage_memory;
            stage_buffer.map(&stage_memory);

            // Populate it with vertices, then copy to the mesh
            Rendering::CommandBuffer copy_cmd = this->memory_manager.draw_cmd_pool.deref(this->copy_cmd_h);
            Rendering::Vertex* vstage_memory = (Rendering::Vertex*) stage_memory;
            vstage_memory[0] = Rendering::Vertex({ 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f});
            vstage_memory[1] = Rendering::Vertex({ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f});
            vstage_memory[2] = Rendering::Vertex({-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f});
            stage_buffer.flush();
            stage_buffer.copyto(this->memory_manager.draw_pool.deref_buffer(mesh.vertices_h), 3 * sizeof(Rendering::Vertex), 0, 0, copy_cmd);

            // Next, populate the indices
            Rendering::index_t* istage_memory = (Rendering::index_t*) stage_memory;
            istage_memory[0] = 0;
            istage_memory[1] = 1;
            istage_memory[2] = 2;
            stage_buffer.flush();
            stage_buffer.copyto(this->memory_manager.draw_pool.deref_buffer(mesh.indices_h), 3 * sizeof(Rendering::index_t), 0, 0, copy_cmd);

            // Deallocate the stage buffer and update the index count
            stage_buffer.unmap();
            this->memory_manager.stage_pool.deallocate(stage_buffer);
            n_vertices += 3;
            n_indices  += 3;

            // Add the mesh to the list
            meshes.push_back(mesh);
            break;
        }

        // case ModelFormat::square:
        //     // Simply append the hardcoded list
        //     DLOG(info, "Loading static square...");
        //     new_vertices = {
        //         { {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
        //         { { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
        //         { { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} },
        //         { {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} }
        //     };
        //     new_indices = {
        //         0, 1, 2, 2, 3, 0
        //     };
        //     break;
        
        // case ModelFormat::squares:
        //     // Simply append the hardcoded list
        //     DLOG(info, "Loading two static squares positioned in a stacked manner...");
        //     new_vertices = {
        //         { {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
        //         { { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
        //         { { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} },
        //         { {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} },

        //         { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} },
        //         { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} },
        //         { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f} },
        //         { {-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f} }
        //     };
        //     new_indices = {
        //         0, 1, 2, 2, 3, 0,
        //         4, 5, 6, 6, 7, 4
        //     };
        //     break;

        default:
            DLOG(fatal, "Unsupported model format '" + model_format_names[(int) format] + "'");
            break;

    }

    // Do some debug print
    DINDENT;
    DLOG(info, "Loaded " + std::to_string(meshes.size()) + " new meshes, totalling " + std::to_string(n_vertices) + " vertices and " + std::to_string(n_indices) + " indices.");
    DDEDENT;

    // Done
    DDEDENT;
    DRETURN;
}

/* Unloads the model belonging to the given entity in the given entity manager. */
void ModelSystem::unload_model(ECS::EntityManager& entity_manager, entity_t entity) {
    DENTER("Models::ModelManager::unload_model");
    DLOG(info, "Deallocating model for entity " + std::to_string(entity) + "...");
    DINDENT;

    // Try to get the entity's meshes and loop through them
    ECS::Meshes& meshes = entity_manager.get_component<ECS::Meshes>(entity);
    for (uint32_t i = 0; i < meshes.size(); i++) {
        // Simply deallocate the two arrays
        this->memory_manager.draw_pool.deallocate(meshes[i].vertices_h);
        this->memory_manager.draw_pool.deallocate(meshes[i].indices_h);
    }

    // Clear the list
    meshes.clear();

    // Done
    DDEDENT;
    DRETURN;
}

/* Binds the model-related buffers for the given mesh component to the given command buffer. */
void ModelSystem::schedule(const ECS::Mesh& entity_mesh, const Rendering::CommandBuffer& draw_cmd) const {
    DENTER("Models::ModelManager::unload_model");

    // Dereference the buffers
    Rendering::Buffer vertices = this->memory_manager.draw_pool.deref_buffer(entity_mesh.vertices_h);
    Rendering::Buffer indices = this->memory_manager.draw_pool.deref_buffer(entity_mesh.indices_h);

    // Bind the buffers
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(draw_cmd, 0, 1, &vertices.buffer(), offsets);
    vkCmdBindIndexBuffer(draw_cmd, indices.buffer(), 0, VK_INDEX_TYPE_UINT32);

    // Done
    DRETURN;
}



/* Swap operator for the ModelSystem class. */
void Models::swap(ModelSystem& mm1, ModelSystem& mm2) {
    DENTER("Models::swap(ModelManager)");

    #ifndef NDEBUG
    // Make sure the all pools overlap
    if (&mm1.memory_manager != &mm2.memory_manager) {
        DLOG(fatal, "Cannot swap model managers with different memory managers");
    }
    #endif

    // Otherwise, swap all elements
    using std::swap;
    swap(mm1.copy_cmd_h, mm2.copy_cmd_h);

    // Done
    DRETURN;
}
