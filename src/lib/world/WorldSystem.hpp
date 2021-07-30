/* WORLD SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 12:17:02
 * Last edited:
 *   30/07/2021, 12:17:02
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
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

#include "ecs/Entity.hpp"
#include "ecs/EntityManager.hpp"

namespace Rasterizer::World {
    /* The WorldSystem class, which is in charge of placing objects in a scene and letting them do non-physics animations and junk. */
    class WorldSystem {
    public:
        /* Default constructor for the WorldSystem, which initializes the world to an empty state. */
        WorldSystem();
        /* Constructor for the WorldSystem, which takes an entity manager and generates an empty world (but with a floor). */
        WorldSystem(ECS::EntityManager& entity_manger);
        /* Constructor for the WorldSystem, which takes an entity manager to spawn entities with an the path to a scene JSON. */
        WorldSystem(ECS::EntityManager& entity_manager, const std::string& scene_path);
        
        /* Sets an entity's position within the world, at the given location, with the given rotation and given scale. */
        void set(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) const;
        /* Moves given entity to a new position. */
        void move(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_position) const;
        /* Rotates given entity to a new angle. */
        void rotate(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_rotation) const;
        /* Re-scales given entity to a new scale. */
        void scale(ECS::EntityManager& entity_manager, entity_t entity, const glm::vec3& new_scale) const;

    };

}

#endif
