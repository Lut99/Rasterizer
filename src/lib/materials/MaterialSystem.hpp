/* MATERIAL SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:28:57
 * Last edited:
 *   10/09/2021, 16:56:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MaterialSystem, which is in charge of loading, managing
 *   and then rendering materials. Models will be able to load materials
 *   for their meshes.
**/

#ifndef MATERIALS_MATERIAL_SYSTEM_HPP
#define MATERIALS_MATERIAL_SYSTEM_HPP

#include <unordered_map>
#include "glm/glm.hpp"

#include "tools/Array.hpp"
#include "ecs/EntityManager.hpp"
#include "textures/Texture.hpp"
#include "rendering/gpu/GPU.hpp"
#include "rendering/pipeline/PipelineProperties.hpp"

#include "Material.hpp"
#include "variants/MaterialType.hpp"
#include "variants/SimpleColoured.hpp"
#include "variants/SimpleTextured.hpp"

namespace Makma3D::Materials {
    /* The MaterialSystem class, which is in charge of loading, managing and rendering materials. */
    class MaterialSystem {
    public:
        /* The logging channel for the MaterialSystem class. */
        static constexpr const char* channel = "MaterialSystem";

        /* Reference to the GPU used to create the pipeline properties for materials. */
        const Rendering::GPU& gpu;

    private:
        /* The DescriptorSetLayout for the SimpleColoured material. */
        Rendering::DescriptorSetLayout simple_coloured_layout;

        /* List of all material IDs in use. */
        std::unordered_map<material_t, MaterialType> material_ids;
        /* List of materials that use the Simple lighting model and that are not textured. */
        std::unordered_map<material_t, SimpleColoured> simple_coloured;
        /* List of materials that use the Simple lighting model and that are yes textured. */
        std::unordered_map<material_t, SimpleTextured> simple_textured;

        /* Simple helper function that returns the first available material ID in the system. Takes some name to show as the material type that failed to add. */
        material_t get_available_id(const char* material_type) const;

    public:
        /* Constructor for the MaterialSystem class, which takes a GPU where the pipelines referencing materials created here will live. */
        MaterialSystem(const Rendering::GPU& gpu);
        /* Copy constructor for the MaterialSystem, which is deleted. */
        MaterialSystem(const MaterialSystem& other) = delete;
        /* Move constructor for the MaterialSystem. */
        MaterialSystem(MaterialSystem&& other);
        /* Destructor for the MaterialSystem. */
        ~MaterialSystem();

        /* Returns the pipeline properties for the SimpleColoured material. The properties for the ViewportTransformation, Rasterization and ColorLogic, as well as the descriptor sets for the global and object's descriptors, have to be given. */
        Rendering::PipelineProperties props_simple_coloured(Rendering::ViewportTransformation&& viewport_transformation, Rendering::Rasterization&& rasterization, Rendering::ColorLogic&& color_logic, Rendering::DescriptorSetLayout&& global_descriptor_layout, Rendering::DescriptorSetLayout&& object_descriptor_layout) const;
        /* Returns the pipeline properties for the SimpleTextured material. */
        Rendering::PipelineProperties props_simple_textured() const;

        /* Adds a new material that uses the simple lighting model and no textures. The colour given is the colour for the entire object. Returns the ID of the new material. */
        material_t create_simple_coloured(const glm::vec3& colour);
        /* Adds a new material that uses the simple lighting model with a texture. The texture used is the given one. */
        material_t create_simple_textured(const Textures::Texture& texture);
        /* Removes the material with the given ID from the system. Throws errors if no such material exists. */
        void remove(material_t material);

        /* Copy assignment operator for the MaterialSystem class, which is deleted. */
        MaterialSystem& operator=(const MaterialSystem& other) = delete;
        /* Move assignment operator for the MaterialSystem class. */
        inline MaterialSystem& operator=(MaterialSystem&& other);
        /* Swap operator for the MaterialSystem class. */
        friend void swap(MaterialSystem& ms1, MaterialSystem& ms2);

    };
    
    /* Swap operator for the MaterialSystem class. */
    void swap(MaterialSystem& ms1, MaterialSystem& ms2);

}

#endif
