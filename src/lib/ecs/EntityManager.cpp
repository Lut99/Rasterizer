/* ENTITY MANAGER.cpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 15:49:49
 * Last edited:
 *   18/07/2021, 15:49:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/CppDebugger.hpp"

#include "EntityManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace CppDebugger::SeverityValues;


/***** ENTITYMANAGER CLASS *****/
/* Constructor for the EntityManager class. */
EntityManager::EntityManager()
{}



/* Spawns a new entity in the EntityManager that has the given components. Returns the assigned ID to that entity. */
entity_t EntityManager::add(ComponentFlags components) {
    DENTER("ECS::EntityManager::add");

    // First, search for the first free entity ID
    entity_t entity = 0;
    while (true) {
        if (this->entities.find(entity) == this->entities.end() && entity != NullEntity) {
            break;
        }
        ++entity;
    }

    // Add the ID to the list
    this->entities.insert(entity);
    this->entity_components.insert(make_pair(entity, components));

    // Next, create each of the components
    if (components & ComponentFlags::transform) {
        this->transforms.add(entity, {});
    }

    // We're done; return the ID
    DRETURN entity;
}

/* Despawns the given entity. Note that its ID may be re-used later, and should thus not be used to reference it anymore after this point. */
void EntityManager::remove(entity_t entity) {
    DENTER("ECS::EntityManager::remove");

    // Check if the entity exists
    if (this->entities.find(entity) == this->entities.end()) {
        DLOG(fatal, "Cannot remove entity with ID " + std::to_string(entity) + " because it doesn't exist.");
    }

    // If it does, then remove its components
    ComponentFlags components = this->entity_components.at(entity);
    if (components & ComponentFlags::transform) {
        this->transforms.remove(entity);
    }

    // Remove the entity from the manager itself
    this->entity_components.erase(entity);
    this->entities.erase(entity);

    // Done, it's fully erased
    DRETURN;
}
