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
ModelManager::ModelManager(Rendering::CommandPool& cmd_pool, Rendering::MemoryPool& draw_pool, Rendering::MemoryPool& stage_pool, uint32_t max_vertices, uint32_t binding_index) :
    cmd_pool(cmd_pool),
    draw_pool(draw_pool),
    stage_pool(stage_pool),

    n_vertices(0),
    max_vertices(max_vertices),

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

    // Allocate the staging buffer
    DLOG(info, "Allocating staging buffer...");
    this->stage_buffer_h = this->stage_pool.allocate_buffer_h(max_vertices * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

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

    stage_buffer_h(this->stage_pool.allocate_buffer_h(other.stage_pool.deref_buffer(other.stage_buffer_h))),

    vk_input_binding_description(other.vk_input_binding_description),
    vk_input_attribute_descriptions(other.vk_input_attribute_descriptions),

    models(other.models)
{
    DENTER("Models::ModelManager::ModelManager(copy)");

    // Get the command buffer
    Rendering::CommandBuffer copy_cmd = this->cmd_pool.deref(this->copy_cmd_h);

    // Get the vertex buffers
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    Rendering::Buffer other_vertex_buffer = this->draw_pool.deref_buffer(other.vertex_buffer_h);

    // Schedule the copy
    other_vertex_buffer.copyto(vertex_buffer, copy_cmd);

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

    stage_buffer_h(other.stage_buffer_h),

    vk_input_binding_description(other.vk_input_binding_description),
    vk_input_attribute_descriptions(other.vk_input_attribute_descriptions),

    models(other.models)
{
    // Tell the other not to deallocate
    other.copy_cmd_h = Rendering::CommandPool::NullHandle;
    other.vertex_buffer_h = Rendering::MemoryPool::NullHandle;
    other.stage_buffer_h = Rendering::MemoryPool::NullHandle;
}

/* Destructor for the ModelManager class. */
ModelManager::~ModelManager() {
    DENTER("Models::ModelManager::~ModelManager");
    DLOG(info, "Cleaning ModelManager...");
    DINDENT;

    // Deallocate the staging buffer if needed
    if (this->stage_buffer_h != Rendering::MemoryPool::NullHandle) {
        DLOG(info, "Deallocating staging buffer...");
        this->stage_pool.deallocate(this->stage_buffer_h);
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

    // First, check if the uid is unique
    if (this->models.find(uid) != this->models.end()) {
        DLOG(fatal, "Given unique ID '" + uid + "' is not unique.");
    }

    // Load the model according to the given format
    Tools::Array<Vertex> new_vertices;
    switch (format) {
        case ModelFormat::obj:
            // Use the load function from the modelloader
            new_vertices = load_obj_model(path);
            break;

        case ModelFormat::triangle:
            // Simply append the hardcoded list
            new_vertices = {
                { glm::vec3( 0.0, -0.5, 0.0), glm::vec3(1.0, 0.0, 0.0) },
                { glm::vec3( 0.5,  0.5, 0.0), glm::vec3(0.0, 1.0, 0.0) },
                { glm::vec3(-0.5,  0.5, 0.0), glm::vec3(0.0, 0.0, 1.0) }
            };
            break;

        default:
            DLOG(fatal, "Unsupported model format '" + std::to_string((int) format) + "'");
            break;

    }

    // Make sure there is enough space in the buffer
    if (this->n_vertices + new_vertices.size() > this->max_vertices) {
        DLOG(fatal, "Could not load model: not enough space left on device (need " + Tools::bytes_to_string(new_vertices.size() * sizeof(Vertex)) + ", but " + Tools::bytes_to_string((this->max_vertices - this->n_vertices) * sizeof(Vertex)) + " free)");
    }

    // With that, stage the memory
    Rendering::Buffer stage_buffer = this->stage_pool.allocate_buffer(new_vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_memory;
    stage_buffer.map(&stage_memory);

    // Copy the new vertices to it
    memcpy(stage_memory, (void*) new_vertices.rdata(), new_vertices.size() * sizeof(Vertex));

    // Then, unmap and flush the buffer
    stage_buffer.unmap();
    stage_buffer.flush();

    // With it containing the new vertices, copy the data there too
    Rendering::CommandBuffer copy_cmd = this->cmd_pool.deref(this->copy_cmd_h);
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    stage_buffer.copyto(vertex_buffer, VK_WHOLE_SIZE, 0, this->n_vertices, copy_cmd);

    // We're done, so deallocate the staging buffer
    this->stage_pool.deallocate(stage_buffer);

    // With the vertices in place, update the internal trackers of the data
    this->models.insert(
        std::make_pair(uid, Model({ this->n_vertices, new_vertices.size() }))
    );
    this->n_vertices += new_vertices.size();

    // Done
    DRETURN;
}

/* Unloads the model given by the uid. */
void ModelManager::unload_model(const std::string& uid) {
    DENTER("Models::ModelManager::unload_model");

    // Try to find the referenced model
    std::unordered_map<std::string, Model>::iterator iter = this->models.find(uid);
    if (iter == this->models.end()) {
        DLOG(fatal, "Unknown model ID '" + uid + "'.");
    }
    const Model& model = (*iter).second;

    // Fetch the vertex list and the command buffer
    Rendering::CommandBuffer copy_cmd = this->cmd_pool.deref(this->copy_cmd_h);
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);

    // Fetch the list of vertices in a staging buffer, but only from it and up
    uint32_t n_vertices = this->max_vertices - model.offset;
    Rendering::Buffer stage_buffer = this->stage_pool.allocate_buffer(n_vertices * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    vertex_buffer.copyto(stage_buffer, n_vertices * sizeof(Vertex), model.offset, 0, copy_cmd);

    // Map the staging buffer to CPU memory
    void* stage_memory;
    stage_buffer.map(&stage_memory);

    // Set the deleted vertices to 0.0
    memset((void*) ((uint8_t*) stage_memory + model.offset), 0, model.size * sizeof(Vertex));
    // If the model is not at the end of the list, get the list from the GPU and move the rest of the vertices forward
    if (model.offset + model.size < this->n_vertices) {
        // Move all memory following the delete memory forward
        memmove((void*) ((uint8_t*) stage_memory + model.offset), (void*) ((uint8_t*) stage_memory + model.offset + model.size), (this->n_vertices - model.offset - model.size) * sizeof(Vertex));
    }

    // With that done, unmap and flush the stage buffer
    stage_buffer.unmap();
    stage_buffer.flush();

    // Copy it back to the vertex buffer
    stage_buffer.copyto(vertex_buffer, (n_vertices - model.size) * sizeof(Vertex), 0, model.offset, copy_cmd);

    // Deallocate the staging buffer
    this->stage_pool.deallocate(stage_buffer);

    // We're done; shrink the number of vertices and delete the model from the list
    this->n_vertices -= model.size;
    this->models.erase(iter);

    // Done
    DRETURN;
}



/* Binds the vertex buffer to the given command buffer. */
void ModelManager::schedule(const Rendering::CommandBuffer& draw_cmd) const {
    DENTER("Models::ModelManager::schedule");

    // Schedule the vertex buffer to be drawn
    Rendering::Buffer vertex_buffer = this->draw_pool.deref_buffer(this->vertex_buffer_h);
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(draw_cmd, this->vk_input_binding_description.binding, 1, &vertex_buffer.buffer(), offsets);

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

    swap(mm1.stage_buffer_h, mm2.stage_buffer_h);

    swap(mm1.vk_input_binding_description, mm2.vk_input_binding_description);
    swap(mm1.vk_input_attribute_descriptions, mm2.vk_input_attribute_descriptions);
    
    swap(mm1.models, mm2.models);

    // Done
    DRETURN;
}
