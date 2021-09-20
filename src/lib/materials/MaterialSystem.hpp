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
#include "tools/AssociativeArray.hpp"
#include "ecs/EntityManager.hpp"
#include "textures/Texture.hpp"
#include "rendering/data/MaterialData.hpp"
#include "rendering/gpu/GPU.hpp"
#include "rendering/descriptors/DescriptorSetLayout.hpp"
#include "rendering/shaders/ShaderPool.hpp"
#include "rendering/pipeline/PipelineConstructor.hpp"

#include "Material.hpp"
#include "MaterialFormat.hpp"
#include "variants/MaterialType.hpp"

namespace Makma3D::Materials {
    /* The MaterialSystem class, which is in charge of loading, managing and rendering materials. */
    class MaterialSystem {
    public:
        /* The logging channel for the MaterialSystem class. */
        static constexpr const char* channel = "MaterialSystem";
        /* The number of material types in the MaterialSystem. */
        static constexpr const uint32_t n_types = 2/*3*/;
        /* A static list of materials in the MaterialSystem. */
        static constexpr const MaterialType types[n_types] = { MaterialType::simple, MaterialType::simple_coloured/*, MaterialType::simple_textured*/ };

        /* Reference to the GPU used to create the pipeline properties for materials. */
        const Rendering::GPU& gpu;

    private:
        /* List of all material IDs in use. */
        std::unordered_map<material_t, MaterialType> material_ids;
        /* List of materials that use the Simple lighting model, are not textured and have nothing special about them. */
        Tools::AssociativeArray<material_t, Rendering::MaterialData> simple;
        /* List of materials that use the Simple lighting model and that are not textured. */
        Tools::AssociativeArray<material_t, Rendering::MaterialData> simple_coloured;
        /* List of materials that use the Simple lighting model and that are yes textured. */
        Tools::AssociativeArray<material_t, Rendering::MaterialData> simple_textured;

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

        /* Initializes given DescriptorSetLayout with everything needed for materials. */
        static void init_layout(Rendering::DescriptorSetLayout& descriptor_set_layout);

        /* Modifies the pipeline constructor based on the given MaterialType. The shader pool is used to allocate new shaders, unless those shaders are already allocated. As little properties as possible are changed. */
        static void init_props(MaterialType material_type, Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor);
        /* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the Simple material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
        static void init_props_simple(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor);
        /* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the SimpleColoured material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
        static void init_props_simple_coloured(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor);
        /* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the SimpleTextured material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
        static void init_props_simple_textured(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor);

        /* Returns the MaterialType of the given material. */
        inline MaterialType get_type(material_t material) const { return this->material_ids.at(material); }
        /* Returns a muteable reference to the list of all materials of the given type so that it can be iterated over. */
        AssociativeArray<material_t, Rendering::MaterialData>& get_list(MaterialType material_type);
        /* Returns an immuteable reference to the list of all materials of the given type so that it can be iterated over. */
        inline const AssociativeArray<material_t, Rendering::MaterialData>& get_list(MaterialType material_type) const { return const_cast<MaterialSystem*>(this)->get_list(material_type); }

        /* Adds a new material that uses the simple lighting model, no textures and not even anything model-specific. Returns the ID of the new material. */
        inline constexpr material_t create_simple() const { return DefaultMaterial; }
        /* Adds a new material that uses the simple lighting model and no textures. The colour given is the colour for the entire object. Returns the ID of the new material. */
        material_t create_simple_coloured(const glm::vec3& colour);
        /* Adds a new material that uses the simple lighting model with a texture. The texture used is the given one. */
        material_t create_simple_textured(const Textures::Texture& texture);
        /* Loads a new material that uses the simple lighting model and no textures from the given file. Returns the ID of the new material. */
        Tools::Array<std::pair<std::string, material_t>> load_simple_coloured(const std::string& filepath, MaterialFormat format = MaterialFormat::mtl);
        /* Removes the material with the given ID from the system. Throws errors if no such material exists. */
        void remove(material_t material);

        /* Returns the total number of materials registered in the MaterialSystem. */
        inline Tools::array_size_t size() const { return static_cast<Tools::array_size_t>(this->material_ids.size()); } 

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
