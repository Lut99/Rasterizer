/* MODEL MANAGER.cpp
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

#include "tools/Common.hpp"
#include "tools/CppDebugger.hpp"

#include "formats/obj/ObjLoader.hpp"

#include "ModelManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;





/***** MODELMANAGER CLASS *****/
/* Constructor for the ModelManager class, which takes a command pool for memory operations, a memory pool to allocate a new vertex buffer from, a another memory pool used to allocate staging buffers, optionally the size of the buffer and optionally the index for the array in the shaders. */
ModelManager::ModelManager(Rendering::CommandPool& cmd_pool, Rendering::MemoryPool& draw_pool, Rendering::MemoryPool& stage_pool, uint32_t max_vertices, uint32_t max_indices, uint32_t binding_index) :
    cmd_pool(cmd_pool),
    draw_pool(draw_pool),
    stage_pool(stage_pool),

    n_vertices(0),
    max_vertices(max_vertices),

    n_indices(0),
    max_indices(max_indices),

    vk_input_binding_description(Vertex::input_binding_description()),
    vk_input_attribute_descriptions(Vertex::input_attribute_descriptions())
{
    DENTER("Models::ModelManager::ModelManager");
    DLOG(info, "Initializing ModelManager...");
    DINDENT;

    // Allocate the command buffer
    DLOG(info, "Allocating command buffer...");
    this->copy_cmd_h = this->cmd_pool.allocate_h();

    // Allocate the vertex buffer
    DLOG(info, "Allocating vertex buffer...");
    this->vertex_buffer_h = this->draw_pool.allocate_buffer_h(max_vertices * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // Allocate the index buffer
    DLOG(info, "Allocating index buffer...");
    this->index_buffer_h = this->draw_pool.allocate_buffer_h(max_indices * sizeof(index_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // Prepare the two vertex descriptions
    DLOG(info, "Preparing Vulkan input descriptions...");
    this->vk_input_binding_description.binding = binding_index;
    for (uint32_t i = 0; i < this->vk_input_attribute_descriptions.size(); i++) {
        this->vk_input_attribute_descriptions[i].binding = binding_index;
    }

    DDEDENT;
    DLEAVE;
}

/* Copy constructor for the ModelManager class. */
ModelManager::ModelManager(const ModelManager& other) :
    cmd_pool(other.cmd_pool),
    draw_pool(other.draw_pool),
    stage_pool(other.stage_pool),

    copy_cmd_h(this->cmd_pool.allocate_h()),

    vertex_buffer_h(this->draw_pool.allocate_buffer_h(other.draw_pool.deref_buffer(other.vertex_buffer_h))),
    n_vertices(other.n_vertices),
    max_vertices(other.max_vertices),

    index_buffer_h(this->draw_pool.allocate_buffer_h(other.draw_pool.deref_buffer(other.index_buffer_h))),
    n_indices(other.n_indices),
    max_indices(other.max_indices),

    vk_input_binding_description(other.vk_input_binding_description),
    vk_input_attribute_descriptions(other.vk_input_attribute_descriptions),

    models(other.models)
{
    DENTER("Models::ModelManager::ModelManager(copy)");

    // Get the command buffer
    Rendering::CommandBuffer copy_cmd = this->cmd_pool.deref(this->copy_cmd_h);

    // Copy the vertex buffers
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    Rendering::Buffer other_vertex_buffer = this->draw_pool.deref_buffer(other.vertex_buffer_h);
    other_vertex_buffer.copyto(vertex_buffer, copy_cmd);

    // Do the same for the index buffer
    Rendering::Buffer index_buffer = this->draw_pool.deref_buffer(this->index_buffer_h);
    Rendering::Buffer other_index_buffer = this->draw_pool.deref_buffer(other.index_buffer_h);
    other_index_buffer.copyto(index_buffer, copy_cmd);

    // Done
    DLEAVE;
}

/* Move constructor for the ModelManager class. */
ModelManager::ModelManager(ModelManager&& other) :
    cmd_pool(other.cmd_pool),
    draw_pool(other.draw_pool),
    stage_pool(other.stage_pool),

    copy_cmd_h(other.copy_cmd_h),

    vertex_buffer_h(other.vertex_buffer_h),
    n_vertices(other.n_vertices),
    max_vertices(other.max_vertices),

    index_buffer_h(other.index_buffer_h),
    n_indices(other.n_indices),
    max_indices(other.max_indices),

    vk_input_binding_description(other.vk_input_binding_description),
    vk_input_attribute_descriptions(other.vk_input_attribute_descriptions),

    models(other.models)
{
    // Tell the other not to deallocate
    other.copy_cmd_h = Rendering::CommandPool::NullHandle;
    other.vertex_buffer_h = Rendering::MemoryPool::NullHandle;
    other.index_buffer_h = Rendering::MemoryPool::NullHandle;
}

/* Destructor for the ModelManager class. */
ModelManager::~ModelManager() {
    DENTER("Models::ModelManager::~ModelManager");
    DLOG(info, "Cleaning ModelManager...");
    DINDENT;
    
    // Deallocate the index buffer if needed
    if (this->index_buffer_h != Rendering::MemoryPool::NullHandle) {
        DLOG(info, "Deallocating index buffer...");
        this->draw_pool.deallocate(this->index_buffer_h);
    }
    // Deallocate the vertex buffer if needed
    if (this->vertex_buffer_h != Rendering::MemoryPool::NullHandle) {
        DLOG(info, "Deallocating vertex buffer...");
        this->draw_pool.deallocate(this->vertex_buffer_h);
    }
    
    // Deallocate the command buffer if needed
    if (this->copy_cmd_h != Rendering::CommandPool::NullHandle) {
        DLOG(info, "Deallocating command buffer...");
        this->cmd_pool.deallocate(this->copy_cmd_h);
    }

    DDEDENT;
    DLEAVE;
}



/* Loads a new model from the given file and with the given format. This overload uses a custom UID for the model instead of the file path. */
void ModelManager::load_model(const std::string& uid, const std::string& path, ModelFormat format) {
    DENTER("Models::ModelManager::load_model");
    DLOG(info, "Loading model...");
    DINDENT;

    // First, check if the uid is unique
    if (this->models.find(uid) != this->models.end()) {
        DLOG(fatal, "Given unique ID '" + uid + "' is not unique.");
    }

    // Load the model according to the given format
    Tools::Array<Vertex> new_vertices;
    Tools::Array<index_t> new_indices;
    switch (format) {
        case ModelFormat::obj:
            // Use the load function from the modelloader
            DLOG(info, "Loading '" + path + "' as .obj file...");
            load_obj_model(new_vertices, new_indices, path);
            printf("Vertex 0: %f %f %f\n", new_vertices[new_indices[0]].pos.x, new_vertices[new_indices[0]].pos.y, new_vertices[new_indices[0]].pos.z);
            printf("Vertex 1: %f %f %f\n", new_vertices[new_indices[1]].pos.x, new_vertices[new_indices[1]].pos.y, new_vertices[new_indices[1]].pos.z);
            printf("Vertex 2: %f %f %f\n", new_vertices[new_indices[2]].pos.x, new_vertices[new_indices[2]].pos.y, new_vertices[new_indices[2]].pos.z);
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

        default:
            DLOG(fatal, "Unsupported model format '" + model_format_names[(int) format] + "'");
            break;

    }
    DINDENT; DLOG(info, "Loaded " + std::to_string(new_vertices.size()) + " vertices and " + std::to_string(new_indices.size()) + " indices."); DDEDENT;



    // Make sure there is enough space in the buffers
    if (this->n_vertices + new_vertices.size() > this->max_vertices) {
        DLOG(fatal, "Could not load model: not enough space left on device for vertices (need " + Tools::bytes_to_string(new_vertices.size() * sizeof(Vertex)) + ", but " + Tools::bytes_to_string((this->max_vertices - this->n_vertices) * sizeof(Vertex)) + " free)");
    }
    if (this->n_indices + new_indices.size() > this->max_indices) {
        DLOG(fatal, "Could not load model: not enough space left on device for indices (need " + Tools::bytes_to_string(new_indices.size() * sizeof(index_t)) + ", but " + Tools::bytes_to_string((this->max_indices - this->n_indices) * sizeof(index_t)) + " free)");
    }



    // With that, stage the memory
    DLOG(info, "Moving vertices to GPU...");
    Rendering::Buffer stage_buffer = this->stage_pool.allocate_buffer(std::max(new_vertices.size() * sizeof(Vertex), new_indices.size() * sizeof(index_t)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_memory;
    stage_buffer.map(&stage_memory);

    // Copy the new vertices to it
    memcpy(stage_memory, (void*) new_vertices.rdata(), new_vertices.size() * sizeof(Vertex));
    for (uint32_t i = 0; i < new_vertices.size(); i++) {
        printf("%u) Vertex (%f,%f,%f)\n", this->n_vertices + i, ((Vertex*) stage_memory)[i].pos.x, ((Vertex*) stage_memory)[i].pos.y, ((Vertex*) stage_memory)[i].pos.z);
    }
    stage_buffer.flush();

    // With it containing the new vertices, copy the data to the large buffer
    Rendering::CommandBuffer copy_cmd = this->cmd_pool.deref(this->copy_cmd_h);
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    stage_buffer.copyto(vertex_buffer, VK_WHOLE_SIZE, 0, this->n_vertices * sizeof(Vertex), copy_cmd);



    // Next, populate the staging buffer with the index data
    DLOG(info, "Moving indices to GPU...");
    memcpy(stage_memory, (void*) new_indices.rdata(), new_indices.size() * sizeof(index_t));
    for (uint32_t i = 0; i < new_indices.size(); i++) {
        printf("%u) Index %u\n", this->n_indices + i, ((index_t*) stage_memory)[i]);
    }
    stage_buffer.flush();

    // With it containing the new indices, copy the data to the large buffer
    Rendering::Buffer index_buffer = this->draw_pool.deref_buffer(this->index_buffer_h);
    stage_buffer.copyto(index_buffer, VK_WHOLE_SIZE, 0, this->n_indices * sizeof(index_t), copy_cmd);



    // We clean the staging buffer
    stage_buffer.unmap();
    this->stage_pool.deallocate(stage_buffer);

    // With the vertices in place, update the internal trackers of the data
    this->models.insert(
        std::make_pair(uid, Model({ this->n_vertices, new_vertices.size(), this->n_indices, new_indices.size() }))
    );
    this->n_vertices += new_vertices.size();
    this->n_indices  += new_indices.size();

    // Done
    DDEDENT;
    DRETURN;
}

/* Unloads the model given by the uid. */
void ModelManager::unload_model(const std::string& uid) {
    DENTER("Models::ModelManager::unload_model");
    DLOG(info, "Unloading model '" + uid + "'...");
    DINDENT;

    // Try to find the referenced model
    std::unordered_map<std::string, Model>::iterator iter = this->models.find(uid);
    if (iter == this->models.end()) {
        DLOG(fatal, "Unknown model ID '" + uid + "'.");
    }
    const Model& model = (*iter).second;

    // Fetch the vertex & index list and the command buffer
    DLOG(info, "Preparing memory structures for transfers...");
    Rendering::CommandBuffer copy_cmd = this->cmd_pool.deref(this->copy_cmd_h);
    Rendering::Buffer vertex_buffer   = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    Rendering::Buffer index_buffer    = this->draw_pool.deref_buffer(this->index_buffer_h);

    // Prepare a staging buffer for use by both the vertex and the index buffer
    uint32_t n_vertices = this->max_vertices - model.voffset;
    uint32_t n_indices  = this->max_indices - model.ioffset;
    Rendering::Buffer stage_buffer = this->stage_pool.allocate_buffer(std::max(n_vertices * sizeof(Vertex), n_indices * sizeof(index_t)), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // Map the staging buffer to CPU memory
    void* stage_memory;
    stage_buffer.map(&stage_memory);



    // Only if we have some moving to do, fetch the list of stuff that needs to be moved
    DLOG(info, "Unloading " + std::to_string(model.vsize) + " model vertices...");
    if (model.voffset + model.vsize < this->n_vertices) {
        // Copy the vertex buffer contents that we need to move + their target space to the staging buffer
        vertex_buffer.copyto(stage_buffer, n_vertices * sizeof(Vertex), model.voffset, 0, copy_cmd);

        // Move all memory following the delete memory forward
        memmove((void*) ((uint8_t*) stage_memory + model.voffset), (void*) ((uint8_t*) stage_memory + model.voffset + model.vsize), (this->n_vertices - model.voffset - model.vsize) * sizeof(Vertex));

        // Copy the staging buffer's contents back to the large vertex buffer
        stage_buffer.flush();
        stage_buffer.copyto(vertex_buffer, (n_vertices - model.vsize) * sizeof(Vertex), 0, model.voffset, copy_cmd);
    }



    // Only if we have some moving to do, fetch the list of stuff that needs to be moved
    DLOG(info, "Unloading " + std::to_string(model.isize) + " model indices...");
    if (model.ioffset + model.isize < this->n_indices) {
        // Copy the index buffer contents that we need to move + their target space to the staging buffer
        index_buffer.copyto(stage_buffer, n_indices * sizeof(index_t), model.ioffset, 0, copy_cmd);

        // Move all the memory following the deleted memory forward
        memmove((void*) ((uint8_t*) stage_memory + model.ioffset), (void*) ((uint8_t*) stage_memory + model.ioffset + model.isize), (this->n_indices - model.ioffset - model.isize) * sizeof(index_t));

        // Copy the staging buffer's contents back to the large index buffer
        stage_buffer.flush();
        stage_buffer.copyto(index_buffer, (n_indices - model.isize) * sizeof(index_t), 0, model.ioffset, copy_cmd);
    }



    // Deallocate the staging buffer
    stage_buffer.unmap();
    this->stage_pool.deallocate(stage_buffer);

    // We're done; shrink the number of vertices and indices and delete the model from the list
    this->n_vertices -= model.vsize;
    this->n_indices  -= model.isize;
    this->models.erase(iter);

    // Done
    DDEDENT;
    DRETURN;
}



/* Binds the vertex buffer to the given command buffer. */
void ModelManager::schedule(const Rendering::CommandBuffer& draw_cmd) const {
    DENTER("Models::ModelManager::schedule");

    // Schedule the vertex buffer to be drawn
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(draw_cmd, this->vk_input_binding_description.binding, 1, &vertex_buffer.buffer(), offsets);

    // Schedule the index buffer to be drawn
    Rendering::Buffer index_buffer = this->draw_pool.deref_buffer(this->index_buffer_h);
    vkCmdBindIndexBuffer(draw_cmd, index_buffer.buffer(), 0, VK_INDEX_TYPE_UINT32);

    // Done
    DRETURN;
}



/* Swap operator for the ModelManager class. */
void Models::swap(ModelManager& mm1, ModelManager& mm2) {
    DENTER("Models::swap(ModelManager)");

    #ifndef NDEBUG
    // Make sure the all pools overlap
    if (&mm1.cmd_pool != &mm2.cmd_pool) {
        DLOG(fatal, "Cannot swap model managers with different command pools");
    }
    if (&mm1.draw_pool != &mm2.draw_pool) {
        DLOG(fatal, "Cannot swap model managers with different draw pools");
    }
    if (&mm1.stage_pool != &mm2.stage_pool) {
        DLOG(fatal, "Cannot swap model managers with different stage pools");
    }
    #endif

    // Otherwise, swap all elements
    using std::swap;
    swap(mm1.copy_cmd_h, mm2.copy_cmd_h);

    swap(mm1.vertex_buffer_h, mm2.vertex_buffer_h);
    swap(mm1.n_vertices, mm2.n_vertices);
    swap(mm1.max_vertices, mm2.max_vertices);

    swap(mm1.index_buffer_h, mm2.index_buffer_h);
    swap(mm1.n_indices, mm2.n_indices);
    swap(mm1.max_indices, mm2.max_indices);

    swap(mm1.vk_input_binding_description, mm2.vk_input_binding_description);
    swap(mm1.vk_input_attribute_descriptions, mm2.vk_input_attribute_descriptions);
    
    swap(mm1.models, mm2.models);

    // Done
    DRETURN;
}
