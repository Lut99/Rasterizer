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

#include "MaterialSystem.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** MODELSYSTEM CLASS *****/
/* Constructor for the MaterialSystem class, which takes a MemoryManager so it can allocate GPU memory structures. */
MaterialSystem::MaterialSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager)
{
    logger.logc(Verbosity::important, MaterialSystem::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, MaterialSystem::channel, "Init success.");
}

/* Move constructor for the MaterialSystem. */
MaterialSystem::MaterialSystem(MaterialSystem&& other) :
    memory_manager(other.memory_manager),

    material_ids(other.material_ids),
    simple_coloured(other.simple_coloured),
    simple_textured(other.simple_textured)
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



/* Renders the given list of objects with their assigned materials. */
void MaterialSystem::render(const ECS::EntityManager& entity_manager) {
    
}



/* Swap operator for the MaterialSystem class. */
void Materials::swap(MaterialSystem& ms1, MaterialSystem& ms2) {
    #ifndef NDEBUG
    if (&ms1.memory_manager != &ms2.memory_manager) { logger.fatalc(MaterialSystem::channel, "Cannot swap memory managers with different memory managers."); }
    #endif

    using std::swap;

    swap(ms1.material_ids, ms2.material_ids);
    swap(ms1.simple_coloured, ms2.simple_coloured);
    swap(ms1.simple_textured, ms2.simple_textured);
}
