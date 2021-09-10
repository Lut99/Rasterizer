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
    memory_manager(other.memory_manager)
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

    
}
