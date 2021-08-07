/* WORLD SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 12:17:08
 * Last edited:
 *   07/08/2021, 15:27:49
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

#include "ecs/auxillary/ComponentList.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/Camera.hpp"
#include "ecs/components/Controllable.hpp"

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
/* (Default) Constructor for the WorldSystem, which initializes the world to an empty state. Stores the given time ratio internally. */
WorldSystem::WorldSystem(float time_ratio) :
    time_ratio(time_ratio),
    last_update(std::chrono::system_clock::now())
{}

/* Constructor for the WorldSystem, which takes an entity manager and generates an empty world (but with a floor). */
WorldSystem::WorldSystem(ECS::EntityManager& entity_manger, float time_ratio) :
    time_ratio(time_ratio),
    last_update(std::chrono::system_clock::now())
{
    DENTER("World::WorldSystem::WorldSystem(floor)");

    DLOG(fatal, "Not yet implemented.");

    DLEAVE;
}

/* Constructor for the WorldSystem, which takes an entity manager to spawn entities with an the path to a scene JSON. */
WorldSystem::WorldSystem(ECS::EntityManager& entity_manager, const std::string& scene_path, float time_ratio) :
    time_ratio(time_ratio),
    last_update(std::chrono::system_clock::now())
{
    DENTER("World::WorldSystem::WorldSystem(file)");

    DLOG(fatal, "Not yet implemented.");

    DLEAVE;
}



/* Sets the movement speeds of a given Controllable. */
void WorldSystem::set_controllable(ECS::EntityManager& entity_manager, entity_t entity, float movement_speed, float rotation_speed) const {
    DENTER("World::WorldSystem::set_controllable");

    // Get the controllable component
    Controllable& controllable = entity_manager.get_component<Controllable>(entity);

    // Set the properties
    controllable.mov_speed = movement_speed;
    controllable.rot_speed = rotation_speed;

    // We're done here
    DRETURN;    
}

/* Sets the position of a camera in the WorldSystem, recomputing the necessary camera matrices in addition to its transform matrices. */
void WorldSystem::set_cam(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& position, const glm::vec3& rotation, float fov, float aspect_ratio) const {
    DENTER("World::WorldSystem::set_cam");

    // Get the entity's transform & camera components
    Transform& transform = entity_manager.get_component<Transform>(entity);
    Camera& camera = entity_manager.get_component<Camera>(entity);

    // Set the values
    transform.position = position;
    transform.rotation = rotation;
    transform.scale    = { 1.0f, 1.0f, 1.0f };
    camera.fov = fov;
    camera.ratio = aspect_ratio;

    // Compute the transform translation matrix and the camera matrices
    transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);
    glm::mat4 proj = glm::perspective(camera.fov, camera.ratio, 0.1f, 10.0f);
    glm::mat4 view = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    proj[1][1] *= -1;
    camera.proj_view = proj * view;

    // Done!
    DRETURN;
}



/* Sets an entity's position within the world, at the given location, with the given rotation and given scale. */
void WorldSystem::set(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const {
    DENTER("World::WorldSystem::set");

    // Get the entity's transform
    Transform& transform = entity_manager.get_component<Transform>(entity);

    // Set the values
    transform.position = position;
    transform.rotation = rotation;
    transform.scale    = scale;

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



/* Updates all relevant objects, either by physics or by window input. */
void WorldSystem::update(ECS::EntityManager& entity_manager, const Window& window) {
    DENTER("World::WorldSystem::update");

    // Compute the number of seconds passed since last update
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    float passed = std::chrono::duration_cast<std::chrono::milliseconds>(this->last_update - now).count();

    // First, handle Controllable updates
    ComponentList<Controllable>& controllables = entity_manager.get_list<Controllable>();
    for (component_list_size_t i = 0; i < controllables.size(); i++) {
        entity_t entity = controllables.get_entity(i);

        // Get the controllable for the speeds, but also the transform to update position
        Controllable& controllable = controllables[i];
        Transform& transform = entity_manager.get_component<Transform>(entity);
        
        // Check keyboard input
        float speed = passed / 1000.0 * controllable.mov_speed;
        if (window.key_pressed(GLFW_KEY_LEFT_SHIFT) || window.key_pressed(GLFW_KEY_RIGHT_SHIFT)) {
            // Double that speed
            speed *= 2;
        }

        if (window.key_pressed(GLFW_KEY_W) || window.key_pressed(GLFW_KEY_UP)) {
            // Move forward with the given speed
            transform.position.y -= speed;
        } else if (window.key_pressed(GLFW_KEY_S) || window.key_pressed(GLFW_KEY_DOWN)) {
            // Move backward with the given speed
            transform.position.y += speed;   
        }
        if (window.key_pressed(GLFW_KEY_D) || window.key_pressed(GLFW_KEY_RIGHT)) {
            // Move right with the given speed
            transform.position.x -= speed;
        } else if (window.key_pressed(GLFW_KEY_A) || window.key_pressed(GLFW_KEY_LEFT)) {
            // Move left with the given speed
            transform.position.x += speed;
        }

        // When done, update the transform matrix, and update the camera matrix too if the entity is a camera
        transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);
        if (entity_manager.has_component(entity, ComponentFlags::camera)) {
            Camera& camera = entity_manager.get_component<Camera>(entity);
            glm::mat4 proj = glm::perspective(camera.fov, camera.ratio, 0.1f, 10.0f);
            glm::mat4 view = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            proj[1][1] *= -1;
            camera.proj_view = proj * view;
        }
    }

    // When done, update the last-update-time and quit
    this->last_update = now;
    DRETURN;
}
