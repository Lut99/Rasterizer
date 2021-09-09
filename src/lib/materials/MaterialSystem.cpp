/* MATERIAL SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:32:42
 * Last edited:
 *   09/09/2021, 16:32:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/Logger.hpp"

#include "MaterialSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Materials;


/***** MODELSYSTEM CLASS *****/
/* Constructor for the MaterialSystem class, which takes a MemoryManager so it can allocate GPU memory structures. */
MaterialSystem::MaterialSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager),

    diffused_list(MaterialFlags::diffused),
    textured_list(MaterialFlags::textured)
{
    logger.logc(Verbosity::important, MaterialSystem::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, MaterialSystem::channel, "Init success.");
}

/* Move constructor for the MaterialSystem. */
MaterialSystem::MaterialSystem(MaterialSystem&& other) :
    memory_manager(other.memory_manager),

    materials(std::move(other.materials)),
    diffused_list(std::move(other.diffused_list)),
    textured_list(std::move(other.textured_list))
{}

/* Destructor for the MaterialSystem. */
MaterialSystem::~MaterialSystem() {
    logger.logc(Verbosity::important, MaterialSystem::channel, "Cleaning...");

    // Nothing as of yet

    logger.logc(Verbosity::important, MaterialSystem::channel, "Cleaned.");
}



/* Swap operator for the MaterialSystem class. */
void Materials::swap(MaterialSystem& ms1, MaterialSystem& ms2) {
    #ifndef NDEBUG
    if (&ms1.memory_manager != &ms2.memory_manager) { logger.fatalc(MaterialSystem::channel, "Cannot swap memory managers with different memory managers."); }
    #endif

    using std::swap;

    swap(ms1.materials, ms2.materials);
    swap(ms1.diffused_list, ms2.diffused_list);
    swap(ms1.textured_list, ms2.textured_list);
}
