/* MODEL SYSTEM.hpp
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
 *   Contains the ModelSystem class, which is in charge of handling all
 *   models in the rasterizer.
**/

#ifndef MODELS_MODEL_SYSTEM_HPP
#define MODELS_MODEL_SYSTEM_HPP

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include "ecs/EntityManager.hpp"
#include "ecs/components/Meshes.hpp"
#include "rendering/memory/MemoryManager.hpp"
#include "rendering/commandbuffers/CommandBuffer.hpp"
#include "ModelFormat.hpp"

namespace Rasterizer::Models {
    /* The ModelSystem class, which is in charge of handling all models in the rasterizer. */
    class ModelSystem {
    public:
        /* Reference to the memory manager that contains the pools we need. */
        Rendering::MemoryManager& memory_manager;

    private:
        /* The command buffer used to draw to. */
        Rendering::command_buffer_h copy_cmd_h;

    public:
        /* Constructor for the ModelSystem class, which takes a MemoryManager struct for the required memory pools. */
        ModelSystem(Rendering::MemoryManager& memory_manager);
        /* Copy constructor for the ModelSystem class. */
        ModelSystem(const ModelSystem& other);
        /* Move constructor for the ModelSystem class. */
        ModelSystem(ModelSystem&& other);
        /* Destructor for the ModelSystem class. */
        ~ModelSystem();

        /* Loads a model at the given path and with the given format and adds it to the given entity in the given entity manager. */
        void load_model(ECS::EntityManager& entity_manager, entity_t entity, const std::string& path, ModelFormat format = ModelFormat::obj);
        /* Unloads the model belonging to the given entity in the given entity manager. */
        void unload_model(ECS::EntityManager& entity_manager, entity_t entity);

        /* Binds the model-related buffers for the given mesh component to the given command buffer. */
        void schedule(const ECS::Mesh& entity_meshes, const Rendering::CommandBuffer& draw_cmd) const;

        /* Copy assignment operator for the ModelSystem class. */
        inline ModelSystem& operator=(const ModelSystem& other) { return *this = ModelSystem(other); }
        /* Move assignment operator for the ModelSystem class. */
        inline ModelSystem& operator=(ModelSystem&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ModelSystem class. */
        friend void swap(ModelSystem& mm1, ModelSystem& mm2);

    };

    /* Swap operator for the ModelSystem class. */
    void swap(ModelSystem& mm1, ModelSystem& mm2);

}

#endif
