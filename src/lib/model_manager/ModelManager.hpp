/* MODEL MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:09:53
 * Last edited:
 *   01/07/2021, 14:09:53
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ModelManager class, which is in charge of handling all
 *   models in the rasterizer.
**/

#ifndef MODELS_MODEL_MANAGER_HPP
#define MODELS_MODEL_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include "render_engine/memory/MemoryPool.hpp"
#include "render_engine/commandbuffers/CommandPool.hpp"
#include "render_engine/memory/Buffer.hpp"
#include "render_engine/commandbuffers/CommandBuffer.hpp"
#include "Vertex.hpp"
#include "Index.hpp"
#include "ModelFormat.hpp"

namespace Rasterizer::Models {
    /* The ModelManager class, which is in charge of handling all models in the rasterizer. */
    class ModelManager {
    public:
        /* Reference to the CommandPool we use for scheduling memory operations. */
        Rendering::CommandPool& cmd_pool;
        /* Reference to the MemoryPool we use for the vertex buffer. */
        Rendering::MemoryPool& draw_pool;
        /* Reference to the MemoryPool we use for staging buffers. */
        Rendering::MemoryPool& stage_pool;

    private:
        /* Internal struct used to describe a single model. */
        struct Model {
            /* The offset of the model in the list of vertices. */
            uint32_t voffset;
            /* The number of vertices belonging to this model. */
            uint32_t vsize;
            /* The offset of the model in the list of indices. */
            uint32_t ioffset;
            /* The number of indices belonging to this model. */
            uint32_t isize;
        };


        /* The command buffer used to draw to. */
        Rendering::command_buffer_h copy_cmd_h;

        /* The buffer that will be used to store the vertices. */
        Rendering::buffer_h vertex_buffer_h;
        /* Keeps track of how many vertices we loaded. */
        uint32_t n_vertices;
        /* Keeps track of how much space for vertices we have. */
        uint32_t max_vertices;

        /* The buffer that will be used to store the indices. */
        Rendering::buffer_h index_buffer_h;
        /* Keeps track of how many indices we loaded. */
        uint32_t n_indices;
        /* Keeps track of how much space for indices we have. */
        uint32_t max_indices;

        /* Description for the Vulkan input binding. */
        VkVertexInputBindingDescription vk_input_binding_description;
        /* Descriptions for the Vulkan input attributes. */
        Tools::Array<VkVertexInputAttributeDescription> vk_input_attribute_descriptions;

        /* Keeps track of the models allocated in the ModelManager. */
        std::unordered_map<std::string, Model> models;

    public:
        /* Constructor for the ModelManager class, which takes a command pool for memory operations, a memory pool to allocate a new vertex buffer from, a another memory pool used to allocate staging buffers, optionally the size of the buffer and optionally the index for the array in the shaders. */
        ModelManager(Rendering::CommandPool& cmd_pool, Rendering::MemoryPool& draw_pool, Rendering::MemoryPool& stage_pool, uint32_t max_vertices = 500000, uint32_t max_indices = 1000000, uint32_t binding_index = 0);
        /* Copy constructor for the ModelManager class. */
        ModelManager(const ModelManager& other);
        /* Move constructor for the ModelManager class. */
        ModelManager(ModelManager&& other);
        /* Destructor for the ModelManager class. */
        ~ModelManager();

        /* Returns if the ModelManager contains a model with the given uid. */
        inline bool contains(const std::string& uid) const { return this->models.find(uid) != this->models.end(); }

        /* Loads a new model from the given file and with the given format. */
        inline void load_model(const std::string& path, ModelFormat format) { return this->load_model(path, path, format); }
        /* Loads a new model from the given file and with the given format. This overload uses a custom UID for the model instead of the file path. */
        void load_model(const std::string& uid, const std::string& path, ModelFormat format);
        /* Unloads the model given by the uid. */
        void unload_model(const std::string& uid);

        /* Function that returns the binding description for the vertex. */
        inline const VkVertexInputBindingDescription& input_binding_description() const { return this->vk_input_binding_description; }
        /* Function that returns the attribute descriptions for the vertex. */
        inline const Tools::Array<VkVertexInputAttributeDescription>& input_attribute_descriptions() const { return this->vk_input_attribute_descriptions; }
        /* Binds the vertex buffer to the given command buffer. */
        void schedule(const Rendering::CommandBuffer& draw_cmd) const;

        /* Returns the number of vertices currently loaded by models. */
        inline uint32_t vsize() const { return this->n_vertices; }
        /* Returns the maximum number of vertices we can load. */
        inline uint32_t vcapacity() const { return this->max_vertices; }
        /* Returns the number of indices currently loaded by models. */
        inline uint32_t isize() const { return this->n_indices; }
        /* Returns the maximum number of indices we can load. */
        inline uint32_t icapacity() const { return this->max_indices; }

        /* Copy assignment operator for the ModelManager class. */
        inline ModelManager& operator=(const ModelManager& other) { return *this = ModelManager(other); }
        /* Move assignment operator for the ModelManager class. */
        inline ModelManager& operator=(ModelManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ModelManager class. */
        friend void swap(ModelManager& mm1, ModelManager& mm2);

    };

    /* Swap operator for the ModelManager class. */
    void swap(ModelManager& mm1, ModelManager& mm2);

}

#endif
