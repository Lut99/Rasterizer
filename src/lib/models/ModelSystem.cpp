/* MODEL SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:09:32
 * Last edited:
 *   01/07/2021, 14:09:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ModelManager class, which is in charge of managing all
 *   models in the rasterizer.
**/

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

    // Load the model according to the given format
    Tools::Array<Rendering::Vertex> new_vertices;
    Tools::Array<Rendering::index_t> new_indices;
    switch (format) {
        case ModelFormat::obj:
            // Use the load function from the modelloader
            DLOG(info, "Loading '" + path + "' as .obj file...");
            load_obj_model(new_vertices, new_indices, path);
            break;

        case ModelFormat::triangle:
            // Simply append the hardcoded list
            DLOG(info, "Loading static triangle...");
            new_vertices = {
                { { 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
                { { 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} }
            };
            new_indices = {
                0, 1, 2
            };
            break;
        
        case ModelFormat::square:
            // Simply append the hardcoded list
            DLOG(info, "Loading static square...");
            new_vertices = {
                { {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
                { { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} },
                { {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} }
            };
            new_indices = {
                0, 1, 2, 2, 3, 0
            };
            break;
        
        case ModelFormat::squares:
            // Simply append the hardcoded list
            DLOG(info, "Loading two static squares positioned in a stacked manner...");
            new_vertices = {
                { {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
                { { 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
                { { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} },
                { {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} },

                { {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} },
                { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} },
                { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f} },
                { {-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f} }
            };
            new_indices = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4
            };
            break;

        default:
            DLOG(fatal, "Unsupported model format '" + model_format_names[(int) format] + "'");
            break;

    }
    DINDENT; DLOG(info, "Loaded " + std::to_string(new_vertices.size()) + " vertices and " + std::to_string(new_indices.size()) + " indices."); DDEDENT;



    DLOG(info, "Preparing to copy to the GPU...");
    DINDENT;
    // Allocate the staging buffer
    Rendering::Buffer stage_buffer = this->memory_manager.stage_pool.allocate_buffer(std::max(new_vertices.size() * sizeof(Rendering::Vertex), new_indices.size() * sizeof(Rendering::index_t)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_memory;
    stage_buffer.map(&stage_memory);

    // Get the mesh
    ECS::Mesh& mesh = entity_manager.get_component<ECS::Mesh>(entity);

    // Get the command buffer
    Rendering::CommandBuffer copy_cmd = this->memory_manager.mem_cmd_pool.deref(this->copy_cmd_h);



    // Prepare a staging buffer with already mapped memory
    DLOG(info, "Moving vertices to GPU...");
    memcpy(stage_memory, (void*) new_vertices.rdata(), new_vertices.size() * sizeof(Rendering::Vertex));
    stage_buffer.flush();

    // Allocate the GPU-local buffer
    mesh.vertices_h = this->memory_manager.draw_pool.allocate_buffer_h(new_vertices.size() * sizeof(Rendering::Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    Rendering::Buffer vertices = this->memory_manager.draw_pool.deref_buffer(mesh.vertices_h);

    // With it containing the new vertices, copy the data to the large buffer
    stage_buffer.copyto(vertices, new_vertices.size() * sizeof(Rendering::Vertex), 0, 0, copy_cmd);



    // Next, populate the staging buffer with the index data
    DLOG(info, "Moving indices to GPU...");
    memcpy(stage_memory, (void*) new_indices.rdata(), new_indices.size() * sizeof(Rendering::index_t));
    stage_buffer.flush();

    // Allocate the GPU-local buffer
    mesh.indices_h = this->memory_manager.draw_pool.allocate_buffer_h(new_indices.size() * sizeof(Rendering::index_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    Rendering::Buffer indices = this->memory_manager.draw_pool.deref_buffer(mesh.indices_h);

    // With it containing the new indices, copy the data to the large buffer
    stage_buffer.copyto(indices, new_indices.size() * sizeof(Rendering::index_t), 0, 0, copy_cmd);
    mesh.n_instances = new_indices.size();



    // We clean the staging buffer
    stage_buffer.unmap();
    this->memory_manager.stage_pool.deallocate(stage_buffer);

    // Done
    DDEDENT; DDEDENT;
    DRETURN;
}

/* Unloads the model belonging to the given entity in the given entity manager. */
void ModelSystem::unload_model(ECS::EntityManager& entity_manager, entity_t entity) {
    DENTER("Models::ModelManager::unload_model");
    DLOG(info, "Deallocating model for entity " + std::to_string(entity) + "...");
    DINDENT;

    // Try to get the entity's mesh
    ECS::Mesh& mesh = entity_manager.get_component<ECS::Mesh>(entity);

    // Simply deallocate the two arrays
    this->memory_manager.draw_pool.deallocate(mesh.vertices_h);
    this->memory_manager.draw_pool.deallocate(mesh.indices_h);
    mesh.n_instances = 0;

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
