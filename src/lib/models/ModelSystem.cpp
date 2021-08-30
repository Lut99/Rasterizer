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

#include "tools/Logger.hpp"
#include "tools/Common.hpp"
#include "formats/obj/ObjLoader.hpp"

#include "ModelSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;





/***** MODELMANAGER CLASS *****/
/* Constructor for the ModelSystem class, which takes a MemoryManager struct for the required memory pools. */
ModelSystem::ModelSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager)
{
    logger.logc(Verbosity::important, ModelSystem::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, ModelSystem::channel, "Init success.");
}

/* Copy constructor for the ModelSystem class. */
ModelSystem::ModelSystem(const ModelSystem& other) :
    memory_manager(other.memory_manager)
{
    logger.logc(Verbosity::debug, ModelSystem::channel, "Copying ModelSystem @ ", &other, "...");

    // Nothing as of yet

    logger.logc(Verbosity::debug, ModelSystem::channel, "Copy success.");
}

/* Move constructor for the ModelSystem class. */
ModelSystem::ModelSystem(ModelSystem&& other) :
    memory_manager(other.memory_manager)
{}

/* Destructor for the ModelSystem class. */
ModelSystem::~ModelSystem() {
    logger.logc(Verbosity::important, ModelSystem::channel, "Cleaning...");

    // Nothing as of yet
    
    logger.logc(Verbosity::important, ModelSystem::channel, "Cleaned.");
}



/* Loads a model at the given path and with the given format and adds it to the given entity in the given entity manager. */
void ModelSystem::load_model(ECS::EntityManager& entity_manager, entity_t entity, const std::string& path, ModelFormat format) {
    logger.logc(Verbosity::important, ModelSystem::channel, "Loading model for entity ", entity, "...");

    // Get the entity's component
    ECS::Meshes& meshes = entity_manager.get_component<ECS::Meshes>(entity);

    // Load the model according to the given format
    switch (format) {
        case ModelFormat::obj:
            // Use the load function from the modelloader
            logger.logc(Verbosity::details, ModelSystem::channel, "Loading '", path, "' as .obj file...");
            load_obj_model(this->memory_manager, meshes, path);
            break;

        case ModelFormat::triangle: {
            // Simply set the hardcoded list
            logger.logc(Verbosity::details, ModelSystem::channel, "Loading static triangle...");
            uint32_t n_vertices = 3, n_indices = 3;

            // Prepare the mesh struct
            Mesh mesh = {};
            mesh.name = "triangle";
            mesh.mtl = "rainbow";
            mesh.mtl_col = { 0.0f, 0.0f, 0.0f, 0.0f };
            mesh.vertices = this->memory_manager.draw_pool.allocate(n_vertices * sizeof(Rendering::Vertex ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.indices = this->memory_manager.draw_pool.allocate(n_indices * sizeof(Rendering::index_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT  | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.n_indices = n_indices;

            // Prepare a staging buffer
            Rendering::Buffer* stage_buffer = this->memory_manager.stage_pool.allocate(std::max(n_vertices * sizeof(Rendering::Vertex), n_indices * sizeof(Rendering::index_t)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            void* stage_memory;
            stage_buffer->map(&stage_memory);

            // Populate it with vertices, then copy to the mesh
            Rendering::Vertex* vstage_memory = (Rendering::Vertex*) stage_memory;
            vstage_memory[0] = Rendering::Vertex({ 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f});
            vstage_memory[1] = Rendering::Vertex({ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f});
            vstage_memory[2] = Rendering::Vertex({-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f});
            stage_buffer->flush(n_vertices * sizeof(Rendering::Vertex));
            stage_buffer->copyto(mesh.vertices, n_vertices * sizeof(Rendering::Vertex), 0, 0, this->memory_manager.copy_cmd);

            // Next, populate the indices
            Rendering::index_t* istage_memory = (Rendering::index_t*) stage_memory;
            istage_memory[0] = 0;
            istage_memory[1] = 1;
            istage_memory[2] = 2;
            stage_buffer->flush(n_indices * sizeof(Rendering::index_t));
            stage_buffer->copyto(mesh.indices, n_indices * sizeof(Rendering::index_t), 0, 0, this->memory_manager.copy_cmd);

            // Deallocate the stage buffer and update the index count
            stage_buffer->unmap();
            this->memory_manager.stage_pool.free(stage_buffer);

            // Add the mesh to the list
            meshes.push_back(mesh);
            break;
        }

        case ModelFormat::square: {
            // Simply set the hardcoded list
            logger.logc(Verbosity::details, ModelSystem::channel, "Loading static square...");
            uint32_t n_vertices = 4, n_indices = 6;

            // Prepare the mesh struct
            Mesh mesh = {};
            mesh.name = "square";
            mesh.mtl = "texture";
            mesh.mtl_col = { 0.0f, 0.0f, 0.0f, 0.0f };
            mesh.vertices = this->memory_manager.draw_pool.allocate(n_vertices * sizeof(Rendering::Vertex ), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.indices = this->memory_manager.draw_pool.allocate(n_indices * sizeof(Rendering::index_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT  | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.n_indices = 6;

            // Prepare a staging buffer
            Rendering::Buffer* stage_buffer = this->memory_manager.stage_pool.allocate(std::max(n_vertices * sizeof(Rendering::Vertex), n_indices * sizeof(Rendering::index_t)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            void* stage_memory;
            stage_buffer->map(&stage_memory);

            // Populate it with vertices, then copy to the mesh
            Rendering::Vertex* vstage_memory = (Rendering::Vertex*) stage_memory;
            vstage_memory[0] = Rendering::Vertex({-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f});
            vstage_memory[1] = Rendering::Vertex({ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f});
            vstage_memory[2] = Rendering::Vertex({ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f});
            vstage_memory[3] = Rendering::Vertex({-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f});
            stage_buffer->flush(n_vertices * sizeof(Rendering::Vertex));
            stage_buffer->copyto(mesh.vertices, n_vertices * sizeof(Rendering::Vertex), 0, 0, this->memory_manager.copy_cmd);

            // Next, populate the indices
            Rendering::index_t* istage_memory = (Rendering::index_t*) stage_memory;
            istage_memory[0] = 0;
            istage_memory[1] = 1;
            istage_memory[2] = 2;
            istage_memory[3] = 2;
            istage_memory[4] = 3;
            istage_memory[5] = 0;
            stage_buffer->flush(n_indices * sizeof(Rendering::index_t));
            stage_buffer->copyto(mesh.indices, n_indices * sizeof(Rendering::index_t), 0, 0, this->memory_manager.copy_cmd);

            // Deallocate the stage buffer and update the index count
            stage_buffer->unmap();
            this->memory_manager.stage_pool.free(stage_buffer);

            // Add the mesh to the list
            meshes.push_back(mesh);
            break;
        }
        
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
            logger.fatal(ModelSystem::channel, "Unsupported model format '", model_format_names[(int) format], "'");
            break;

    }

    // Do some debug print to close off
    logger.logc(Verbosity::debug, ModelSystem::channel, "Loaded ", meshes.size(), " new meshes.");
}

/* Unloads the model belonging to the given entity in the given entity manager. */
void ModelSystem::unload_model(ECS::EntityManager& entity_manager, entity_t entity) {
    logger.logc(Verbosity::important, ModelSystem::channel, "Deallocating model for entity ", entity, "...");

    // Try to get the entity's meshes and loop through them
    ECS::Meshes& meshes = entity_manager.get_component<ECS::Meshes>(entity);
    for (uint32_t i = 0; i < meshes.size(); i++) {
        // Simply deallocate the two arrays
        this->memory_manager.draw_pool.free(meshes[i].vertices);
        this->memory_manager.draw_pool.free(meshes[i].indices);
    }

    // Clear the list
    meshes.clear();
}

/* Binds the model-related buffers for the given mesh component to the given command buffer. */
void ModelSystem::schedule(const Rendering::CommandBuffer* draw_cmd, const ECS::Mesh& entity_mesh) const {
    // Bind the buffers
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(draw_cmd->command_buffer(), 0, 1, &entity_mesh.vertices->buffer(), offsets);
    vkCmdBindIndexBuffer(draw_cmd->command_buffer(), entity_mesh.indices->buffer(), 0, VK_INDEX_TYPE_UINT32);

    // Done
    return;
}



/* Swap operator for the ModelSystem class. */
void Models::swap(ModelSystem& mm1, ModelSystem& mm2) {
    #ifndef NDEBUG
    // Make sure the all pools overlap
    if (&mm1.memory_manager != &mm2.memory_manager) { logger.fatal("Cannot swap model managers with different memory managers"); }
    #endif

    // Otherwise, swap all elements
    using std::swap;
}
