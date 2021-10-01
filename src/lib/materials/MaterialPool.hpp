/* MATERIAL POOL.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:28:57
 * Last edited:
 *   9/20/2021, 10:52:39 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MaterialPool, which is in charge of loading and managing
 *   materials. Also prepares descriptor set layouts and pipeline constructors
 *   for the different material types supported.
**/

#ifndef MATERIALS_MATERIAL_POOL_HPP
#define MATERIALS_MATERIAL_POOL_HPP

#include <unordered_set>
#include "glm/glm.hpp"

#include "tools/Array.hpp"
#include "rendering/descriptors/DescriptorSetLayout.hpp"
#include "rendering/shaders/ShaderPool.hpp"
#include "rendering/pipeline/PipelineConstructor.hpp"

#include "textures/TexturePool.hpp"
#include "variants/simple/Simple.hpp"
#include "variants/simple_coloured/SimpleColoured.hpp"
#include "variants/simple_textured/SimpleTextured.hpp"
#include "MaterialType.hpp"
#include "Material.hpp"

namespace Makma3D::Materials {
    /* The MaterialPool class, which is in charge of loading and managing materials. Additionally, contains functions to prepare pipelines for rendering. */
    class MaterialPool {
    public:
        /* The logging channel for the MaterialPool class. */
        static constexpr const char* channel = "MaterialPool";
        /* The number of material types in the MaterialPool. */
        static constexpr const uint32_t n_types = 3;
        /* A static list of materials in the MaterialPool. */
        static constexpr const MaterialType types[n_types] = { MaterialType::simple, MaterialType::simple_coloured, MaterialType::simple_textured };

        /* Reference to the TexturePool that we use to allocate textures from. */
        Materials::TexturePool& texture_pool;

    private:
        #ifndef NDEBUG
        /* List of all names in the pool, which speeds up looking for them. */
        std::unordered_set<std::string> names;
        #endif

        /* The default material for this pool. */
        const Materials::Material* _default;
        /* The list of Simple materials allocated in the pool. */
        std::unordered_set<Materials::Simple*> simple;
        /* The list of SimpleColoured materials allocated in the pool. */
        std::unordered_set<Materials::SimpleColoured*> simple_coloured;
        /* The list of SimpleTextured materials allocated in the pool. */
        std::unordered_set<Materials::SimpleTextured*> simple_textured;

    public:
        /* Constructor for the MaterialPool class, which takes a TexturePool to allocate textures with. */
        MaterialPool(Materials::TexturePool& texture_pool);
        /* Copy constructor for the MaterialPool, which is deleted. */
        MaterialPool(const MaterialPool& other) = delete;
        /* Move constructor for the MaterialPool. */
        MaterialPool(MaterialPool&& other);
        /* Destructor for the MaterialPool. */
        ~MaterialPool();

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

        /* Adds a new material to the pool that simply takes the vertex colours, no lighting applied. Only takes the name of that material. */
        Materials::Simple* allocate_simple(const std::string& name);
        /* Adds a new material to the pool that simply takes a uniform colour, no lighting applied. Takes the name of that material and the colour to apply. */
        Materials::SimpleColoured* allocate_simple_coloured(const std::string& name, const glm::vec3& colour);
        /* Adds a new material to the pool that simply takes a texture, no lighting applied. Takes the name of that material, its path, the filter to apply in its Sampler, whether or not to apply anisotropy, by how much to apply anisotropy at most and the format of the file. When set to automatic, automatically deduces the file's format from its extension. */
        Materials::SimpleTextured* allocate_simple_textured(const std::string& name, const std::string& path, VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy_level = 16.0f, TextureFormat format = TextureFormat::automatic);
        /* Frees the given Material again. */
        void free(const Materials::Material* material);

        /* Sets the default material for this pool. */
        inline void set_default(const Materials::Material* material) { this->_default = material; }
        /* Gets the default material for this pool. */
        inline const Materials::Material* default() const { return this->_default; }

        /* Returns a constant reference to the internal set of Simple materials so they can be iterated over. */
        inline const std::unordered_set<Materials::Simple*>& get_simple() const { return this->simple; }
        /* Returns a constant reference to the internal set of SimpleColoured materials so they can be iterated over. */
        inline const std::unordered_set<Materials::SimpleColoured*>& get_simple_coloured() const { return this->simple_coloured; }
        /* Returns a constant reference to the internal set of SimpleTextured materials so they can be iterated over. */
        inline const std::unordered_set<Materials::SimpleTextured*>& get_simple_textured() const { return this->simple_textured; }

        /* Returns the total number of materials registered in the MaterialSystem. */
        inline uint32_t size() const { return static_cast<uint32_t>(this->simple.size()) + static_cast<uint32_t>(this->simple_coloured.size()) + static_cast<uint32_t>(this->simple_textured.size()); } 

        /* Copy assignment operator for the MaterialPool class, which is deleted. */
        MaterialPool& operator=(const MaterialPool& other) = delete;
        /* Move assignment operator for the MaterialPool class. */
        inline MaterialPool& operator=(MaterialPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the MaterialPool class. */
        friend void swap(MaterialPool& ms1, MaterialPool& ms2);

    };
    
    /* Swap operator for the MaterialPool class. */
    void swap(MaterialPool& ms1, MaterialPool& ms2);

}

#endif
