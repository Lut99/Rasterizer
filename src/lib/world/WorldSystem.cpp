/* WORLD SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 12:17:08
 * Last edited:
 *   30/07/2021, 12:17:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the WorldSystem class, which is in charge of placing and
 *   keeping track of object locations. Also handles non-physics
 *   animations.
**/

#include "tools/CppDebugger.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ecs/components/Transform.hpp"

#include "WorldSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace Rasterizer::World;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Computes the translation matrix for one entity based on the given position, rotation and scale. */
static glm::mat4 compute_translation_matrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    DENTER("compute_translation_matrix");

    glm::mat4 result(1.0f);
    result = glm::translate(result, position);
    result = glm::rotate(result, rotation[0], glm::vec3(1.0, 0.0, 0.0));
    result = glm::rotate(result, rotation[1], glm::vec3(0.0, 1.0, 0.0));
    result = glm::rotate(result, rotation[2], glm::vec3(0.0, 0.0, 1.0));
    result = glm::scale(result, scale);

    // DOne
    DRETURN result;
}





/***** WORLDSYSTEM CLASS *****/
/* Default constructor for the WorldSystem, which initializes the world to an empty state. */
WorldSystem::WorldSystem() {}

/* Constructor for the WorldSystem, which takes an entity manager and generates an empty world (but with a floor). */
WorldSystem::WorldSystem(ECS::EntityManager& entity_manger) {
    DENTER("World::WorldSystem::WorldSystem(floor)");

    DLOG(fatal, "Not yet implemented.");

    DLEAVE;
}

/* Constructor for the WorldSystem, which takes an entity manager to spawn entities with an the path to a scene JSON. */
WorldSystem::WorldSystem(ECS::EntityManager& entity_manager, const std::string& scene_path) {
    DENTER("World::WorldSystem::WorldSystem(file)");

    DLOG(fatal, "Not yet implemented.");

    DLEAVE;
}



/* Sets an entity's position within the world, at the given location, with the given rotation and given scale. */
void WorldSystem::set(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const {
    DENTER("World::WorldSystem::set");

    // Get the entity's transform
    Transform& transform = entity_manager.get_component<Transform>(entity);

    // Set the values
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;

    // Compute the translation matrix
    transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);

    // Done
    DRETURN;
}

/* Moves given entity to a new position. */
void WorldSystem::move(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_position) const {
    DENTER("World::WorldSystem::move");

    // Get the entity's transform
    Transform& transform = entity_manager.get_component<Transform>(entity);

    // Set the values
    transform.position = new_position;

    // Compute the translation matrix
    transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);

    // Done
    DRETURN;
}

/* Rotates given entity to a new angle. */
void WorldSystem::rotate(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_rotation) const {
    DENTER("World::WorldSystem::rotate");

    // Get the entity's transform
    Transform& transform = entity_manager.get_component<Transform>(entity);

    // Set the values
    transform.rotation = new_rotation;

    // Compute the translation matrix
    transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);

    // Done
    DRETURN;
}

/* Re-scales given entity to a new scale. */
void WorldSystem::scale(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_scale) const {
    DENTER("World::WorldSystem::scale");

    // Get the entity's transform
    Transform& transform = entity_manager.get_component<Transform>(entity);

    // Set the values
    transform.scale = new_scale;

    // Compute the translation matrix
    transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);

    // Done
    DRETURN;
}
