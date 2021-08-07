/* WORLD SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 12:17:02
 * Last edited:
 *   07/08/2021, 15:02:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the WorldSystem class, which is in charge of placing and
 *   keeping track of object locations. Also handles non-physics
 *   animations.
**/

#ifndef WORLD_WORLDSYSTEM_HPP
#define WORLD_WORLDSYSTEM_HPP

#include <string>
#include <chrono>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

#include "ecs/Entity.hpp"
#include "ecs/EntityManager.hpp"
#include "window/Window.hpp"

namespace Rasterizer::World {
    /* The WorldSystem class, which is in charge of placing objects in a scene and letting them do non-physics animations and junk. */
    class WorldSystem {
    private:
        /* The update speed of the world system, i.e., how many faster or slower time should run in the simulation. */
        float time_ratio;
        /* The last time update() was called. */
        std::chrono::system_clock::time_point last_update;

    public:
        /* (Default) Constructor for the WorldSystem, which initializes the world to an empty state. Stores the given time ratio internally. */
        WorldSystem(float time_ratio = 1.0f);
        /* Constructor for the WorldSystem, which takes an entity manager and generates an empty world (but with a floor). */
        WorldSystem(ECS::EntityManager& entity_manger, float time_ratio = 1.0f);
        /* Constructor for the WorldSystem, which takes an entity manager to spawn entities with an the path to a scene JSON. */
        WorldSystem(ECS::EntityManager& entity_manager, const std::string& scene_path, float time_ratio = 1.0f);
        
        /* Sets the movement speeds of a given Controllable. */
        void set_controllable(ECS::EntityManager& entity_manager, entity_t entity, float movement_speed, float rotation_speed) const;
        /* Sets the position of a camera in the WorldSystem, recomputing the necessary camera matrices in addition to its transform matrices. */
        void set_cam(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& position, const glm::vec3& rotation, float fov, float aspect_ratio) const;

        /* Sets an entity's position within the world, at the given location, with the given rotation and given scale. */
        void set(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const;
        /* Moves given entity to a new position. */
        void move(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_position) const;
        /* Rotates given entity to a new angle. */
        void rotate(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_rotation) const;
        /* Re-scales given entity to a new scale. */
        void scale(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_scale) const;

        /* Updates all relevant objects, either by physics or by window input. */
        void update(ECS::EntityManager& entity_manager, const Window& window);

    };

}

#endif
