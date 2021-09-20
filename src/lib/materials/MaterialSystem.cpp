/* MATERIAL SYSTEM.cpp
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
 *   <Todo>
**/

#include "tools/Logger.hpp"
#include "rendering/auxillary/Vertex.hpp"

#include "formats/mtl/MtlLoader.hpp"
#include "MaterialSystem.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;
using namespace Makma3D::Rendering;


/***** MODELSYSTEM CLASS *****/
/* Constructor for the MaterialSystem class, which takes a GPU where the pipelines referencing materials created here will live. */
MaterialSystem::MaterialSystem(const Rendering::GPU& gpu) :
    gpu(gpu)
{
    logger.logc(Verbosity::important, MaterialSystem::channel, "Initializing...");

    // Add in the default material
    this->material_ids.insert({ DefaultMaterial, MaterialType::simple });
    this->simple.add(DefaultMaterial, {});

    logger.logc(Verbosity::important, MaterialSystem::channel, "Init success.");
}

/* Move constructor for the MaterialSystem. */
MaterialSystem::MaterialSystem(MaterialSystem&& other) :
    gpu(other.gpu),

    material_ids(std::move(other.material_ids)),
    simple(std::move(other.simple)),
    simple_coloured(std::move(other.simple_coloured)),
    simple_textured(std::move(other.simple_textured))
{}

/* Destructor for the MaterialSystem. */
MaterialSystem::~MaterialSystem() {
    logger.logc(Verbosity::important, MaterialSystem::channel, "Cleaning...");

    // Nothing as of yet

    logger.logc(Verbosity::important, MaterialSystem::channel, "Cleaned.");
}



/* Simple helper function that returns the first available material ID in the system. Takes some name to show as the material type that failed to add. */
material_t MaterialSystem::get_available_id(const char* material_type) const {
    // Keep incrementing the material, checking to see this first available one
    material_t material = 0;
    while (this->material_ids.find(material) != this->material_ids.end() || material == NullMaterial) {
        if (material == std::numeric_limits<material_t>::max()) {
            logger.fatalc(MaterialSystem::channel, "Cannot add new ", material_type, " material: no material ID available anymore.");
        }
        ++material;
    }

    // Done, return
    return material;
}



/* Initializes given DescriptorSetLayout with everything needed for materials. */
void MaterialSystem::init_layout(Rendering::DescriptorSetLayout& descriptor_set_layout) {
    descriptor_set_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    descriptor_set_layout.finalize();
}



/* Modifies the pipeline constructor based on the given MaterialType. The shader pool is used to allocate new shaders, unless those shaders are already allocated. As little properties as possible are changed. */
void MaterialSystem::init_props(MaterialType material_type, Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
    switch (material_type) {
        case MaterialType::simple:
            return MaterialSystem::init_props_simple(shader_pool, pipeline_constructor);

        case MaterialType::simple_coloured:
            return MaterialSystem::init_props_simple_coloured(shader_pool, pipeline_constructor);
        
        case MaterialType::simple_textured:
            return MaterialSystem::init_props_simple_textured(shader_pool, pipeline_constructor);
        
        default:
            logger.fatalc(MaterialSystem::channel, "Cannot return properties of unknown material type '", material_type_names[(int) material_type], '\'');

    }
}

/* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the Simple material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
void MaterialSystem::init_props_simple(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
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
void MaterialSystem::init_props_simple_coloured(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
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
    pipeline_constructor.vertex_input_state.vertex_attributes = {
        VertexAttribute(0, 0, offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT)
    };
    // Set the input assembly
    pipeline_constructor.input_assembly_state = InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

    // Done
}

/* Takes a PipelineConstructor and modifies the relevant properties so that it's suitable to render the SimpleTextured material. The shaders are allocated with the given ShaderPool. As little properties as possible are changed. */
void MaterialSystem::init_props_simple_textured(Rendering::ShaderPool& shader_pool, Rendering::PipelineConstructor& pipeline_constructor) {
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



/* Sorts given list of 'entities' (list of their Model components) in such a way that they can be rendered material-by-material efficiently. */
std::unordered_map<MaterialType, std::unordered_map<material_t, std::unordered_map<ECS::entity_t, Tools::Array<const ECS::Mesh*>>>> MaterialSystem::sort_entities(const ECS::ComponentList<ECS::Model>& entities) const {
    // Start by looping through the possible entities
    std::unordered_map<MaterialType, std::unordered_map<material_t, std::unordered_map<ECS::entity_t, Tools::Array<const ECS::Mesh*>>>> result;
    for (uint32_t i = 0; i < entities.size(); i++) {
        // Get the entity and its component
        ECS::entity_t entity = entities.get_entity(i);
        const ECS::Model& model = entities[i];

        // Next, loop through all the entity's meshes
        for (uint32_t j = 0; j < model.meshes.size(); j++) {
            // Get a shortcut to the mesh & material in question
            const ECS::Mesh& mesh = model.meshes[j];
            material_t material = mesh.material;
            // Also lookup the meshes MaterialType based on its material index
            MaterialType material_type = this->material_ids.at(material);

            // Next, add a new unordered map to the main map if this is the first time we see this type
            std::unordered_map<MaterialType, std::unordered_map<material_t, std::unordered_map<ECS::entity_t, Tools::Array<const ECS::Mesh*>>>>::iterator type_iter = result.find(material_type);
            if (type_iter == result.end()) {
                // Insert a new map
                type_iter = result.insert({ material_type, {} }).first;
            }

            // Next, check if we have seen this specific material before
            std::unordered_map<material_t, std::unordered_map<ECS::entity_t, Tools::Array<const ECS::Mesh*>>>::iterator material_iter = (*type_iter).second.find(material);
            if (material_iter == (*type_iter).second.end()) {
                material_iter = (*type_iter).second.insert({ material, {} }).first;
            }

            // Finally, see if we need to add an entry for this entity
            std::unordered_map<ECS::entity_t, Tools::Array<const ECS::Mesh*>>::iterator entity_iter = (*material_iter).second.find(entity);
            if (entity_iter == (*material_iter).second.end()) {
                entity_iter = (*material_iter).second.insert({ entity, Tools::Array<const ECS::Mesh*>(16) }).first;
            }

            // Finally, add this mesh to the list
            while ((*entity_iter).second.size() >= (*entity_iter).second.capacity()) { (*entity_iter).second.reserve(2 * (*entity_iter).second.capacity()); }
            (*entity_iter).second.push_back(&mesh);
        }
    }

    // Done, return the map
    return result;
}



/* Returns a muteable reference to the list of all materials of the given type so that it can be iterated over. */
AssociativeArray<material_t, Rendering::MaterialData>& MaterialSystem::get_list(MaterialType material_type) {
    switch(material_type) {
        case MaterialType::simple:
            return this->simple;

        case MaterialType::simple_coloured:
            return this->simple_coloured;
        
        case MaterialType::simple_textured:
            return this->simple_textured;
        
        default:
            logger.fatalc(MaterialSystem::channel, "Cannot return list of unknown material type '", material_type_names[(int) material_type], "'");

    }
}



/* Adds a new material that uses the simple lighting model and no textures. The colour given is the colour for the entire object. Returns the ID of the new material. */
material_t MaterialSystem::create_simple_coloured(const glm::vec3& colour) {
    // Check if a material with this colour already exists
    for (uint32_t i = 0; i < this->simple_coloured.size(); i++) {
        if (this->simple_coloured[i].colour == colour) {
            // Simply return this material index
            return this->simple_coloured.get_abstract_index(i);
        }
    }

    // First, find a valid material ID
    material_t material = this->get_available_id("SimpleColoured");

    // Store the ID internally
    this->material_ids.insert({ material, MaterialType::simple_coloured });
    // Simply add a new structure of this type
    MaterialData data{};
    data.colour = colour;
    this->simple_coloured.add(material, data);

    // Return the material ID we found
    return material;
}

/* Adds a new material that uses the simple lighting model with a texture. The texture used is the given one. */
material_t MaterialSystem::create_simple_textured(const Textures::Texture& texture) {
    // First, find a valid material ID
    material_t material = this->get_available_id("SimpleTextured");

    // Store the ID internally
    this->material_ids.insert({ material, MaterialType::simple_textured });
    // Simply add a new structure of this type
    MaterialData data{};
    /* TBD */
    this->simple_textured.add(material, data);

    // Return the material ID we found
    return material;
}

/* Loads a new material that uses the simple lighting model and no textures from the given file. Returns the ID of the new material(s) loaded this way. */
Tools::Array<std::pair<std::string, material_t>> MaterialSystem::load_simple_coloured(const std::string& filepath, MaterialFormat format) {
    // Switch on material types
    std::unordered_map<std::string, glm::vec3> new_formats;
    switch(format) {
        case MaterialFormat::mtl:
            // Load using the MtlLoader
            load_mtl_lib(new_formats, filepath);
            break;
        
        default:
            logger.fatalc(MaterialSystem::channel, "Cannot load file as SimpleColoured material with unknown format '", material_format_names[(int) format], "'");

    }

    // Add the materials internally
    Tools::Array<std::pair<std::string, material_t>> result(static_cast<uint32_t>(new_formats.size()));
    for (const auto& p : new_formats) {
        result.push_back({ p.first, this->create_simple_coloured(p.second) });
    }

    // Done
    return result;
}

/* Removes the material with the given ID from the system. Throws errors if no such material exists. */
void MaterialSystem::remove(material_t material) {
    // Try to find the material ID
    std::unordered_map<material_t, MaterialType>::iterator iter = this->material_ids.find(material);
    if (iter == this->material_ids.end()) {
        logger.fatalc(MaterialSystem::channel, "Cannot remove unknown material with ID ", material, '.');
    }

    // Remove the material from its component map
    switch((*iter).second) {
        case MaterialType::simple:
            logger.fatalc(MaterialSystem::channel, "Cannot remove the default material.");

        case MaterialType::simple_coloured:
            this->simple_coloured.remove(material);
            break;

        case MaterialType::simple_textured:
            this->simple_textured.remove(material);
            break;

        default:
            logger.fatalc(MaterialSystem::channel, "Cannot remove material with ID ", material, " because it has an unsupported type '", material_type_names[(int) (*iter).second], "'.");

    }

    // Remove from the list of IDs, freeing this ID again
    this->material_ids.erase(iter);
}



/* Swap operator for the MaterialSystem class. */
void Materials::swap(MaterialSystem& ms1, MaterialSystem& ms2) {
    #ifndef NDEBUG
    if (ms1.gpu != ms2.gpu) { logger.fatalc(MaterialSystem::channel, "Cannot swap memory managers with different GPUs."); }
    #endif

    using std::swap;

    swap(ms1.material_ids, ms2.material_ids);
    swap(ms1.simple, ms2.simple);
    swap(ms1.simple_coloured, ms2.simple_coloured);
    swap(ms1.simple_textured, ms2.simple_textured);
}
