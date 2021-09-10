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



/* Adds a new material that uses the simple lighting model and no textures. The colour given is the colour for the entire object. Returns the ID of the new material. */
material_t MaterialSystem::create_simple_coloured(const glm::vec3& colour) {
    
}

/* Adds a new material that uses the simple lighting model with a texture. The texture used is the given one. */
material_t MaterialSystem::create_simple_textured(const Textures::Texture& texture) {
    
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
