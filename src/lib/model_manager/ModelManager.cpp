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

#include "tools/CppDebugger.hpp"

#include "ModelManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;





/***** MODELMANAGER CLASS *****/
/* Constructor for the ModelManager class, which takes a memory pool to allocate a new vertex buffer from, optionally the size of the buffer and optionally the index for the array in the shaders. */
ModelManager::ModelManager(Rendering::MemoryPool& memory_pool, VkDeviceSize max_vertices, uint32_t binding_index) :
    memory_pool(memory_pool),
    vertex_buffer_h(Rendering::MemoryPool::NullHandle),
    n_vertices(0),
    max_vertices(max_vertices),

    vk_input_binding_description(Vertex::input_binding_description()),
    vk_input_attribute_descriptions(Vertex::input_attribute_descriptions())
{
    DENTER("Models::ModelManager::ModelManager");
    DLOG(info, "Initializing ModelManager...");
    DINDENT;

    // Allocate the vertex buffer
    DLOG(info, "Allocating vertex buffer...");
    this->vertex_buffer_h = this->memory_pool.allocate_buffer_h(max_vertices * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

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
    memory_pool(other.memory_pool),
    vertex_buffer_h(this->memory_pool.allocate_buffer_h(other.vertex_buffer_h)),

    vk_input_binding_description(other.vk_input_binding_description),
    vk_input_attribute_descriptions(other.vk_input_attribute_descriptions)
{}

/* Move constructor for the ModelManager class. */
ModelManager::ModelManager(ModelManager&& other) :
    memory_pool(other.memory_pool),
    vertex_buffer_h(other.vertex_buffer_h),

    vk_input_binding_description(other.vk_input_binding_description),
    vk_input_attribute_descriptions(other.vk_input_attribute_descriptions)
{
    // Tell the other not to deallocate
    other.vertex_buffer_h = Rendering::MemoryPool::NullHandle;
}

/* Destructor for the ModelManager class. */
ModelManager::~ModelManager() {
    DENTER("Models::ModelManager::~ModelManager");
    DLOG(info, "Cleaning ModelManager...");
    DINDENT;

    if (this->vertex_buffer_h != Rendering::MemoryPool::NullHandle) {
        DLOG(info, "Deallocating vertex buffer...");
        this->memory_pool.deallocate(this->vertex_buffer_h);
    }

    DDEDENT;
    DLEAVE;
}



/* Loads a new model from the given file and with the given format. This overload uses a custom UID for the model instead of the file path. */
void ModelManager::load_model(const std::string& uid, const std::string& path, ModelFormat format) {
    DENTER("Models::ModelManager::load_model");

    // Load the model according to the given format
    Tools::Array<Vertex> new_vertices;
    switch (format) {
        case ModelFormat::obj:
            DLOG(fatal, "Not yet implemented.");
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

}

/* Unloads the model given by the uid. */
void ModelManager::unload_model(const std::string& uid) {

}



/* Binds the vertex buffer to the given command buffer. */
void ModelManager::schedule(const Rendering::CommandBuffer& draw_cmd) {
    DENTER("Models::ModelManager::schedule");

    // Schedule the vertex buffer to be drawn
    Rendering::Buffer vertex_buffer = this->memory_pool.deref_buffer(this->vertex_buffer_h);
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(draw_cmd, this->vk_input_binding_description.binding, 1, &vertex_buffer.buffer(), offsets);

    // Done
    DRETURN;
}



/* Swap operator for the ModelManager class. */
void Models::swap(ModelManager& mm1, ModelManager& mm2) {
    DENTER("Models::swap(ModelManager)");

    #ifndef NDEBUG
    // Make sure the two memory pools overlap
    if (&mm1.memory_pool != &mm2.memory_pool) {
        DLOG(fatal, "Cannot swap model managers with different memory pools");
    }
    #endif

    // Otherwise, swap all elements
    using std::swap;
    swap(mm1.vertex_buffer_h, mm2.vertex_buffer_h);
    swap(mm1.vk_input_binding_description, mm2.vk_input_binding_description);
    swap(mm1.vk_input_attribute_descriptions, mm2.vk_input_attribute_descriptions);

    // Done
    DRETURN;
}
