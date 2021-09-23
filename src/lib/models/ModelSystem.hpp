/* MODEL SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:09:53
 * Last edited:
 *   9/19/2021, 5:56:30 PM
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
#include "ecs/components/Model.hpp"
#include "materials/MaterialSystem.hpp"
#include "rendering/memory_manager/MemoryManager.hpp"
#include "rendering/commandbuffers/CommandBuffer.hpp"
#include "ModelFormat.hpp"

namespace Makma3D::Models {
    /* The ModelSystem class, which is in charge of handling all models in the rasterizer. */
    class ModelSystem {
    public:
        /* Channel name for the ModelSystem class. */
        static constexpr const char* channel = "ModelSystem";

        /* Reference to the memory manager that contains the pools we need. */
        Rendering::MemoryManager& memory_manager;
        /* Reference to the MaterialSystem which we use to load new materials with. */
        Materials::MaterialSystem& material_system;

    public:
        /* Constructor for the ModelSystem class, which takes a MemoryManager struct for the required memory pools and a material system to possibly define new materials found in, for example, .obj files. */
        ModelSystem(Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system);
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

        /* Binds the global vertex buffer part of the given Model to the given draw command queue. */
        void schedule(const Rendering::CommandBuffer* draw_cmd, const ECS::Model& model) const;
        /* Binds the given per-mesh index buffer part of the given Mesh to the given draw command queue. */
        void schedule(const Rendering::CommandBuffer* draw_cmd, const ECS::Mesh& mesh, uint32_t index_buffer_index) const;

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
