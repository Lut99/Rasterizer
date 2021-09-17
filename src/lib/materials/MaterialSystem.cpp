/* MATERIAL SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:32:42
 * Last edited:
 *   10/09/2021, 16:58:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/Logger.hpp"
#include "rendering/auxillary/Vertex.hpp"

#include "MaterialSystem.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;
using namespace Makma3D::Rendering;


/***** MODELSYSTEM CLASS *****/
/* Constructor for the MaterialSystem class, which takes a GPU where the pipelines referencing materials created here will live. */
MaterialSystem::MaterialSystem(const Rendering::GPU& gpu) :
    gpu(gpu),

    simple_coloured_layout(this->gpu)
{
    logger.logc(Verbosity::important, MaterialSystem::channel, "Initializing...");

    // Prepare the material's descriptor set layout for the SimpleColoured material
    this->simple_coloured_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10, VK_SHADER_STAGE_FRAGMENT_BIT);
    this->simple_coloured_layout.finalize();

    logger.logc(Verbosity::important, MaterialSystem::channel, "Init success.");
}

/* Move constructor for the MaterialSystem. */
MaterialSystem::MaterialSystem(MaterialSystem&& other) :
    gpu(other.gpu),

    simple_coloured_layout(std::move(other.simple_coloured_layout)),

    material_ids(std::move(other.material_ids)),
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



/* Returns the pipeline properties for the SimpleColoured material. The descriptor sets for the global and object's descriptors, have to be given. */
Rendering::PipelineProperties MaterialSystem::props_simple_coloured(Rendering::DescriptorSetLayout&& global_descriptor_layout, Rendering::DescriptorSetLayout&& object_descriptor_layout) const {
    logger.logc(Verbosity::important, MaterialSystem::channel, "Preparing pipeline properties for the SimpleColoured textures...");

    // Prepare the shaders for the SimpleColoured pipeline
    Tools::Array<ShaderStage> shader_stages(2);
    shader_stages.push_back(ShaderStage(Shader(this->gpu, "bin/shaders/materials/simple_coloured_vertex.spv"), VK_SHADER_STAGE_VERTEX_BIT, {}));
    shader_stages.push_back(ShaderStage(Shader(this->gpu, "bin/shaders/materials/simple_coloured_frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT, {}));

    // Done
    return Rendering::PipelineProperties(
        std::move(shader_stages),
        VertexInputState({ VertexBinding(0, sizeof(Vertex)) }, {
            VertexAttribute(0, 0, offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT),
            VertexAttribute(0, 1, offsetof(Vertex, colour), VK_FORMAT_R32G32B32_SFLOAT),
            VertexAttribute(0, 2, offsetof(Vertex, texel), VK_FORMAT_R32G32_SFLOAT)
        }),
        InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
        depth_testing,
        viewport_transformation,
        rasterization,
        Multisampling(),
        color_logic,
        PipelineLayout(this->gpu, { global_descriptor_layout, this->simple_coloured_layout, object_descriptor_layout }, {})
    );
}

/* Returns the pipeline properties for the SimpleTextured material. */
Rendering::PipelineProperties MaterialSystem::props_simple_textured() const {
    logger.fatalc(MaterialSystem::channel, "Not yet implemented.");
}



/* Adds a new material that uses the simple lighting model and no textures. The colour given is the colour for the entire object. Returns the ID of the new material. */
material_t MaterialSystem::create_simple_coloured(const glm::vec3& colour) {
    // First, find a valid material ID
    material_t material = this->get_available_id("SimpleColoured");

    // Store the ID internally
    this->material_ids.insert({ material, MaterialType::simple_coloured });
    // Simply add a new structure of this type
    this->simple_coloured.insert({ material, SimpleColoured({ colour }) });
}

/* Adds a new material that uses the simple lighting model with a texture. The texture used is the given one. */
material_t MaterialSystem::create_simple_textured(const Textures::Texture& texture) {
    // First, find a valid material ID
    material_t material = this->get_available_id("SimpleTextured");

    // Store the ID internally
    this->material_ids.insert({ material, MaterialType::simple_textured });
    // Simply add a new structure of this type
    this->simple_textured.insert({ material, SimpleTextured({ texture }) });
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
        case MaterialType::simple_coloured:
            this->simple_coloured.erase(material);
            break;
        
        case MaterialType::simple_textured:
            this->simple_textured.erase(material);
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

    swap(ms1.simple_coloured_layout, ms2.simple_coloured_layout);

    swap(ms1.material_ids, ms2.material_ids);
    swap(ms1.simple_coloured, ms2.simple_coloured);
    swap(ms1.simple_textured, ms2.simple_textured);
}
