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

#include "components/Transform.hpp"
#include "components/Mesh.hpp"

#include "EntityManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace CppDebugger::SeverityValues;


/***** ENTITYMANAGER CLASS *****/
/* Constructor for the EntityManager class. */
EntityManager::EntityManager() {
    DENTER("ECS::EntityManager::EntityManager");

    // Register the components
    this->components = new IComponentList*[EntityManager::max_components];
    this->components[0] = (IComponentList*) new ComponentList<Transform>(ComponentFlags::transform);
    this->components[1] = (IComponentList*) new ComponentList<Mesh>(ComponentFlags::mesh);

    // Done
    DLEAVE;
}

/* Copy constructor for the EntityManager class. */
EntityManager::EntityManager(const EntityManager& other) :
    entities(other.entities)
{
    DENTER("ECS::EntityManager::EntityManager(copy)");

    // Allocate new lists
    this->components = new IComponentList*[EntityManager::max_components];
    for (uint32_t i = 0; i < EntityManager::max_components; i++) {
        this->components[i] = other.components[i]->copy();
    }

    // Done
    DLEAVE;
}

/* Move constructor for the EntityManager class. */
EntityManager::EntityManager(EntityManager&& other) :
    entities(other.entities),
    components(other.components)
{
    other.components = nullptr;
}

/* Destructor for the EntityManager class. */
EntityManager::~EntityManager() {
    DENTER("ECS::EntityManager::~EntityManager");

    // Delete the componentlists if needed
    if (this->components != nullptr) {
        for (uint32_t i = 0; i < EntityManager::max_components; i++) {
            delete this->components[i];
        }
        delete[] this->components;
    }

    // Done
    DLEAVE;
}



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
    this->entities.insert(make_pair(entity, components));

    // Next, create each of the components
    for (uint32_t i = 0; i < EntityManager::max_components; i++) {
        if (components & this->components[i]->flags()) {
            this->components[i]->add(entity);
        }
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
    ComponentFlags components = this->entities.at(entity);
    for (uint32_t i = 0; i < EntityManager::max_components; i++) {
        if (components & this->components[i]->flags()) {
            this->components[i]->remove(entity);
        }
    }

    // Remove the entity from the manager itself
    this->entities.erase(entity);

    // Done, it's fully erased
    DRETURN;
}



/* Swap operator for the EntityManager class. */
void ECS::swap(EntityManager& em1, EntityManager& em2) {
    using std::swap;

    swap(em1.entities, em2.entities);
    swap(em1.components, em2.components);
}
