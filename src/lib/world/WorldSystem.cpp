/* WORLD SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 12:17:08
 * Last edited:
 *   07/08/2021, 22:50:54
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
/* Computes a direction vector based on some yaw and pitch (in degrees). */
static inline glm::vec3 compute_direction_vector(float yaw, float pitch) {
    return glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));
}

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

/* Computes the camera matrix given a position, a yaw and pitch (in degrees), a field of view and an aspect ratio. */
static glm::mat4 compute_camera_matrix(const glm::vec3& position, float yaw, float pitch, float fov, float aspect_ratio) {
    DENTER("compute_camera_matrix");

    // Compute the direction vector from the yaw and the pitch
    glm::vec3 direction = compute_direction_vector(yaw, pitch);

    // Use that to compute the projection and the view matrices
    glm::mat4 proj = glm::perspective(fov, aspect_ratio, 0.001f, 10.0f);
    glm::mat4 view = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    proj[1][1] *= -1;

    // Done, so multiple and return
    DRETURN proj * view;
}





/***** WORLDSYSTEM CLASS *****/
/* (Default) Constructor for the WorldSystem, which initializes the world to an empty state. Stores the given time ratio internally. */
WorldSystem::WorldSystem(float time_ratio) :
    time_ratio(time_ratio),

    last_update(std::chrono::system_clock::now()),
    last_mouse(0.0f, 0.0f)
{}

/* Constructor for the WorldSystem, which takes an entity manager and generates an empty world (but with a floor). */
WorldSystem::WorldSystem(ECS::EntityManager& entity_manger, float time_ratio) :
    time_ratio(time_ratio),

    last_update(std::chrono::system_clock::now()),
    last_mouse(0.0f, 0.0f)
{
    DENTER("World::WorldSystem::WorldSystem(floor)");

    DLOG(fatal, "Not yet implemented.");

    DLEAVE;
}

/* Constructor for the WorldSystem, which takes an entity manager to spawn entities with an the path to a scene JSON. */
WorldSystem::WorldSystem(ECS::EntityManager& entity_manager, const std::string& scene_path, float time_ratio) :
    time_ratio(time_ratio),

    last_update(std::chrono::system_clock::now()),
    last_mouse(0.0f, 0.0f)
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
    camera.proj_view = compute_camera_matrix(transform.position, transform.rotation.y, transform.rotation.x, camera.fov, camera.ratio);

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

    // Compute the relative mouse speed
    glm::vec2 mouse = window.mouse_pos();
    float xspeed = mouse.x - this->last_mouse.x;
    float yspeed = mouse.y - this->last_mouse.y;

    // First, handle Controllable updates
    if (window.has_focus()) {
        ComponentList<Controllable>& controllables = entity_manager.get_list<Controllable>();
        for (component_list_size_t i = 0; i < controllables.size(); i++) {
            entity_t entity = controllables.get_entity(i);

            // Get the controllable for the speeds, but also the transform to update position
            Controllable& controllable = controllables[i];
            Transform& transform = entity_manager.get_component<Transform>(entity);

            // Define the actual speeds based on the time passed
            float mov_speed = passed / 1000.0 * controllable.mov_speed;
            float rot_speed = passed / 1000.0 * controllable.rot_speed;



            // Compute the new rotation vector
            transform.rotation.y -= rot_speed * xspeed;
            transform.rotation.x += rot_speed * yspeed;

            // Bind the y rotation
            if (transform.rotation.x > 89.0f) { transform.rotation.x = 89.0f; }
            else if (transform.rotation.x < -89.0f) { transform.rotation.x = -89.0f; }
            // if (xspeed > 0.0) { printf("xspeed: %f -> %f degrees\n", xspeed, transform.rotation.y); }
            // if (yspeed > 0.0) { printf("yspeed: %f -> %f degrees\n", yspeed, transform.rotation.x); }



            // Use that to update movement with the keyboard input
            if (window.key_pressed(GLFW_KEY_TAB)) {
                // Double that speed
                mov_speed *= 2;
            }

            // Compute the rotational vector forward and the one tangent to that
            glm::vec3 dir_forward = -compute_direction_vector(transform.rotation.y, transform.rotation.x);
            glm::vec3 dir_up      =  glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 dir_right   =  glm::normalize(glm::cross(dir_forward, dir_up));
            // if (xspeed > 0.0 || yspeed > 0.0) {
            //     printf("Forward: %f %f %f\n", dir_forward.x, dir_forward.y, dir_forward.z);
            //     printf("Up:      %f %f %f\n", dir_up.x, dir_up.y, dir_up.z);
            //     printf("Right:   %f %f %f\n", dir_right.x, dir_right.y, dir_right.z);
            // }

            // Check the directional keys for movement
            if (window.key_pressed(GLFW_KEY_W) || window.key_pressed(GLFW_KEY_UP)) {
                // Move forward with the given speed
                transform.position += mov_speed * dir_forward;
            } else if (window.key_pressed(GLFW_KEY_S) || window.key_pressed(GLFW_KEY_DOWN)) {
                // Move backward with the given speed
                transform.position -= mov_speed * dir_forward;   
            }
            if (window.key_pressed(GLFW_KEY_D) || window.key_pressed(GLFW_KEY_RIGHT)) {
                // Move right with the given speed
                transform.position += mov_speed * dir_right;
            } else if (window.key_pressed(GLFW_KEY_A) || window.key_pressed(GLFW_KEY_LEFT)) {
                // Move left with the given speed
                transform.position -= mov_speed * dir_right;
            }
            if (window.key_pressed(GLFW_KEY_SPACE)) {
                // Move up
                transform.position -= mov_speed * dir_up;
            } else if (window.key_pressed(GLFW_KEY_LEFT_SHIFT) || window.key_pressed(GLFW_KEY_RIGHT_SHIFT)) {
                // Move down
                transform.position += mov_speed * dir_up;
            }



            // When done, update the transform matrix, and update the camera matrix too if the entity is a camera
            transform.translation = compute_translation_matrix(transform.position, transform.rotation, transform.scale);
            if (entity_manager.has_component(entity, ComponentFlags::camera)) {
                Camera& camera = entity_manager.get_component<Camera>(entity);
                camera.proj_view = compute_camera_matrix(transform.position, transform.rotation.y, transform.rotation.x, camera.fov, camera.ratio);
            }
        }
    }

    // When done, update the last-update-time and quit
    this->last_update = now;
    this->last_mouse = mouse;
    DRETURN;
}
