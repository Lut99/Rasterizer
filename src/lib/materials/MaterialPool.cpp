/* MATERIAL POOL.cpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:32:42
 * Last edited:
 *   9/20/2021, 10:52:48 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MaterialPool, which is in charge of loading and managing
 *   materials. Also prepares descriptor set layouts and pipeline constructors
 *   for the different material types supported.
**/

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "tools/Logger.hpp"
#include "rendering/auxillary/Vertex.hpp"

#include "MaterialPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;
using namespace Makma3D::Rendering;


/***** MATERIALPOOL CLASS *****/
/* Constructor for the MaterialPool class, which takes a TexturePool to allocate textures with. */
MaterialPool::MaterialPool(Materials::TexturePool& texture_pool) :
    texture_pool(texture_pool)
{
    logger.logc(Verbosity::important, MaterialPool::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, MaterialPool::channel, "Init success.");
}

/* Move constructor for the MaterialPool. */
MaterialPool::MaterialPool(MaterialPool&& other) :
    texture_pool(other.texture_pool),

    #ifndef NDEBUG
    names(std::move(other.names)),
    #endif

    _default(std::move(other._default)),
    simple(std::move(other.simple)),
    simple_coloured(std::move(other.simple_coloured)),
    simple_textured(std::move(other.simple_textured))
{
    other.simple.clear();
    other.simple_coloured.clear();
    other.simple_textured.clear();
}

/* Destructor for the MaterialPool. */
MaterialPool::~MaterialPool() {
    logger.logc(Verbosity::important, MaterialPool::channel, "Cleaning...");

    // Destroy the simple textured
    if (!this->simple_textured.empty()) {
        logger.logc(Verbosity::details, MaterialPool::channel, "Freeing ", this->simple_textured.size(), " SimpleTexture materials...");
        for (const SimpleTextured* m : this->simple_textured) {
            this->texture_pool.free(m->texture);
            delete m;
        }
    }
    // Destroy the simple coloured
    if (!this->simple_coloured.empty()) {
        logger.logc(Verbosity::details, MaterialPool::channel, "Freeing ", this->simple_coloured.size(), " SimpleColoured materials...");
        for (const SimpleColoured* m : this->simple_coloured) {
            delete m;
        }
    }
    // Destroy the simple
    if (!this->simple.empty()) {
        logger.logc(Verbosity::details, MaterialPool::channel, "Freeing ", this->simple.size(), " Simple materials...");
        for (const Simple* m : this->simple) {
            delete m;
        }
    }

    logger.logc(Verbosity::important, MaterialPool::channel, "Cleaned.");
}



/* Initializes given DescriptorSetLayout with everything needed for materials. */
void MaterialPool::init_layout(Rendering::DescriptorSetLayout& descriptor_set_layout) {
    descriptor_set_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    descriptor_set_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptor_set_layout.add_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptor_set_layout.finalize();
}



/* Modifies the pipeline constructor based on the given MaterialType. The shader pool is used to allocate new shaders, unless those shaders are already allocated. As little properties as possible are changed. */
void MaterialPool::init_props(MaterialType material_type, Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
    switch (material_type) {
        case MaterialType::simple:
            return MaterialPool::init_props_simple(shader_pool, pipeline_constructor);

        case MaterialType::simple_coloured:
            return MaterialPool::init_props_simple_coloured(shader_pool, pipeline_constructor);
        
        case MaterialType::simple_textured:
            return MaterialPool::init_props_simple_textured(shader_pool, pipeline_constructor);
        
        default:
            logger.fatalc(MaterialPool::channel, "Cannot return properties of unknown material type '", material_type_names[(int) material_type], '\'');

    }
}

/* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the Simple material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
void MaterialPool::init_props_simple(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
    // Load the shaders to use
    Tools::Array<ShaderStage> shaders(2);
    shaders.push_back(ShaderStage(
        shader_pool.allocate("shaders/materials/simple_vert.spv"),
        VK_SHADER_STAGE_VERTEX_BIT,
        {}
    ));
    shaders.push_back(ShaderStage(
        shader_pool.allocate("shaders/materials/simple_frag.spv"),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        {}
    ));

    // Set the pipeline shaders
    pipeline_constructor.shaders = shaders;
    // Set the pipeline input vertex attributes
    pipeline_constructor.vertex_input_state = VertexInputState(
        { VertexBinding(0, sizeof(Vertex)) },
        { VertexAttribute(0, 0, offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT), VertexAttribute(0, 1, offsetof(Vertex, colour), VK_FORMAT_R32G32B32_SFLOAT) }
    );
    // Set the input assembly
    pipeline_constructor.input_assembly_state = InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

    // Done
}

/* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the SimpleColoured material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
void MaterialPool::init_props_simple_coloured(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
    // Load the shaders to use
    Tools::Array<ShaderStage> shaders(2);
    shaders.push_back(ShaderStage(
        shader_pool.allocate("shaders/materials/simple_coloured_vert.spv"),
        VK_SHADER_STAGE_VERTEX_BIT,
        {}
    ));
    shaders.push_back(ShaderStage(
        shader_pool.allocate("shaders/materials/simple_coloured_frag.spv"),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        {}
    ));

    // Set the pipeline shaders
    pipeline_constructor.shaders = shaders;
    // Set the pipeline input vertex attributes
    pipeline_constructor.vertex_input_state.vertex_attributes = {
        VertexAttribute(0, 0, offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT)
    };
    // Set the input assembly
    pipeline_constructor.input_assembly_state = InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

    // Done
}

/* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the SimpleTextured material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
void MaterialPool::init_props_simple_textured(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
    // Load the shaders to use
    Tools::Array<ShaderStage> shaders(2);
    shaders.push_back(ShaderStage(
        shader_pool.allocate("shaders/materials/simple_textured_vert.spv"),
        VK_SHADER_STAGE_VERTEX_BIT,
        {}
    ));
    shaders.push_back(ShaderStage(
        shader_pool.allocate("shaders/materials/simple_textured_frag.spv"),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        {}
    ));

    // Set the pipeline shaders
    pipeline_constructor.shaders = shaders;
    // Set the pipeline input vertex attributes
    pipeline_constructor.vertex_input_state.vertex_attributes = {
        VertexAttribute(0, 0, offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT),
        VertexAttribute(0, 1, offsetof(Vertex, texel), VK_FORMAT_R32G32_SFLOAT)
    };
    // Set the input assembly
    pipeline_constructor.input_assembly_state = InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

    // Done
}



/* Adds a new material to the pool that simply takes the vertex colours, no lighting applied. Only takes the name of that material. */
Materials::Simple* MaterialPool::allocate_simple(const std::string& name) {
    #ifndef NDEBUG
    // Try to find if this is a duplicate
    std::unordered_set<std::string>::iterator iter = this->names.find(name);
    if (iter != this->names.end()) {
        logger.fatalc(MaterialPool::channel, "Duplicate material name '", name, "'");
    }
    #endif
    
    // Create the Simple material and add it internally
    Simple* result = new Simple(name);
    this->simple.insert(result);

    #ifndef NDEBUG
    // Insert its name
    this->names.insert(result->_name);
    #endif

    // Done, return the new material
    return result;
}

/* Adds a new material to the pool that simply takes a uniform colour, no lighting applied. Takes the name of that material and the colour to apply. */
Materials::SimpleColoured* MaterialPool::allocate_simple_coloured(const std::string& name, const glm::vec3& colour) {
    #ifndef NDEBUG
    // Try to find if this is a duplicate
    std::unordered_set<std::string>::iterator iter = this->names.find(name);
    if (iter != this->names.end()) {
        logger.fatalc(MaterialPool::channel, "Duplicate material name '", name, "'");
    }
    #endif
    
    // Create the SimpleColoured material
    SimpleColoured* result = new SimpleColoured(name, colour);
    this->simple_coloured.insert(result);

    #ifndef NDEBUG
    // Insert its name
    this->names.insert(result->_name);
    #endif

    // Done, return the new material
    return result;
}

/* Adds a new material to the pool that simply takes a texture, no lighting applied. Takes the name of that material, its path, the filter to apply in its Sampler, whether or not to apply anisotropy, by how much to apply anisotropy at most and the format of the file. When set to automatic, automatically deduces the file's format from its extension. */
Materials::SimpleTextured* MaterialPool::allocate_simple_textured(const std::string& name, const std::string& path, VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy_level, TextureFormat format) {
    #ifndef NDEBUG
    // Try to find if this is a duplicate
    std::unordered_set<std::string>::iterator iter = this->names.find(name);
    if (iter != this->names.end()) {
        logger.fatalc(MaterialPool::channel, "Duplicate material name '", name, "'");
    }
    #endif
    
    // Load the requested texture
    Texture* texture = this->texture_pool.allocate(path, filter, enable_anisotropy, max_anisotropy_level, format);
    
    // Use that to create the SimpleTextured material
    SimpleTextured* result = new SimpleTextured(name, texture);
    this->simple_textured.insert(result);

    #ifndef NDEBUG
    // Insert its name
    this->names.insert(result->_name);
    #endif

    // Done, return the new material
    return result;
}

/* Frees the given Material again. */
void MaterialPool::free(const Materials::Material* material) {
    // Find based on the material's type
    switch(material->_type) {
        case MaterialType::simple: {
            const Materials::Simple* simple = (const Materials::Simple*) material;

            // Try to find in the map if we have it allocated
            std::unordered_set<Materials::Simple*>::iterator iter = this->simple.find(const_cast<Materials::Simple*>(simple));
            if (iter == this->simple.end()) {
                logger.fatalc(MaterialPool::channel, "Cannot free material that isn't allocated with this pool.");
            }
            
            #ifndef NDEBUG
            // Remove the name from the list
            this->names.erase(material->_name);
            #endif

            // Destroy the material
            delete simple;

            // Remove it from the list, done
            this->simple.erase(iter);
            return;
        }

        case MaterialType::simple_coloured: {
            const Materials::SimpleColoured* simple_coloured = (const Materials::SimpleColoured*) material;

            // Try to find in the map if we have it allocated
            std::unordered_set<Materials::SimpleColoured*>::iterator iter = this->simple_coloured.find(const_cast<Materials::SimpleColoured*>(simple_coloured));
            if (iter == this->simple_coloured.end()) {
                logger.fatalc(MaterialPool::channel, "Cannot free material that isn't allocated with this pool.");
            }

            #ifndef NDEBUG
            // Remove the name from the list
            this->names.erase(material->_name);
            #endif

            // Destroy the material
            delete simple_coloured;

            // Remove it from the list, done
            this->simple_coloured.erase(iter);
            return;
        }

        case MaterialType::simple_textured: {
            const Materials::SimpleTextured* simple_textured = (const Materials::SimpleTextured*) material;

            // Try to find in the map if we have it allocated
            std::unordered_set<Materials::SimpleTextured*>::iterator iter = this->simple_textured.find(const_cast<Materials::SimpleTextured*>(simple_textured));
            if (iter == this->simple_textured.end()) {
                logger.fatalc(MaterialPool::channel, "Cannot free material that isn't allocated with this pool.");
            }
            
            #ifndef NDEBUG
            // Remove the name from the list
            this->names.erase(material->_name);
            #endif

            // Destroy the material
            this->texture_pool.free(simple_textured->texture);
            delete simple_textured;

            // Remove it from the list, done
            this->simple_textured.erase(iter);
            return;
        }

        default:
            logger.fatalc(MaterialPool::channel, "Unknown material type '", material_type_names[(int) material->_type], '\'');

    }

}



/* Swap operator for the MaterialPool class. */
void Materials::swap(MaterialPool& ms1, MaterialPool& ms2) {
    #ifndef NDEBUG
    if (&ms1.texture_pool != &ms2.texture_pool) { logger.fatalc(MaterialPool::channel, "Cannot swap material pools with different texture pools."); }
    #endif

    using std::swap;

    #ifndef NDEBUG
    swap(ms1.names, ms2.names);
    #endif
    
    swap(ms1._default, ms2._default);
    swap(ms1.simple, ms2.simple);
    swap(ms1.simple_coloured, ms2.simple_coloured);
    swap(ms1.simple_textured, ms2.simple_textured);
}
