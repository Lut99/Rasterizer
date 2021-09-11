/* ENTITY MANAGER.cpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 15:49:49
 * Last edited:
 *   10/09/2021, 17:21:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/Logger.hpp"

#include "components/Transform.hpp"
#include "components/Meshes.hpp"
#include "components/Camera.hpp"
#include "components/Controllable.hpp"
#include "components/Textures.hpp"

#include "EntityManager.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::ECS;


/***** ENTITYMANAGER CLASS *****/
/* Constructor for the EntityManager class. */
EntityManager::EntityManager() :
    transforms(ComponentFlags::transform),
    models(ComponentFlags::model),
    controllables(ComponentFlags::controllable),
    cameras(ComponentFlags::camera)
{}



/* Spawns a new entity in the EntityManager that has the given components. Returns the assigned ID to that entity. */
entity_t EntityManager::add(ComponentFlags components) {
    // First, search for the first free entity ID
    entity_t entity = 0;
    while (this->entities.find(entity) != this->entities.end() || entity == NullEntity) {
        if (entity == std::numeric_limits<entity_t>::max()) {
            logger.fatalc(EntityManager::channel, "Cannot add new entity: no entity ID available anymore.");
        }
        ++entity;
    }

    // Add the ID to the list
    this->entities.insert(make_pair(entity, components));

    // Next, create each of the components
    if (components & ComponentFlags::transform) {
        this->transforms.add(entity);
    }
    if (components & ComponentFlags::model) {
        this->models.add(entity);
    }
    if (components & ComponentFlags::controllable) {
        this->controllables.add(entity);
    }
    if (components & ComponentFlags::camera) {
        this->cameras.add(entity);
    }

    // We're done; return the ID
    return entity;
}

/* Despawns the given entity. Note that its ID may be re-used later, and should thus not be used to reference it anymore after this point. */
void EntityManager::remove(entity_t entity) {
    // Check if the entity exists
    if (this->entities.find(entity) == this->entities.end()) {
        logger.fatalc(EntityManager::channel, "Cannot remove entity with ID ", entity, " because it doesn't exist.");
    }

    // If it does, then remove its components
    ComponentFlags components = this->entities.at(entity);
    if (components & ComponentFlags::transform) {
        this->transforms.remove(entity);
    }
    if (components & ComponentFlags::model) {
        this->models.remove(entity);
    }
    if (components & ComponentFlags::controllable) {
        this->controllables.remove(entity);
    }
    if (components & ComponentFlags::camera) {
        this->cameras.remove(entity);
    }

    // Remove the entity from the manager itself
    this->entities.erase(entity);

    // Done, it's fully erased
    return;
}
