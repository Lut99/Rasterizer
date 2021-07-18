/* ENTITY MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:19:10
 * Last edited:
 *   18/07/2021, 12:19:10
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the EntityManager class, which aims to manage, well,
 *   entities. In particular, it manages the several arrays and thus hosts
 *   the ability to spawn entities with different components.
**/

#ifndef ECS_ENTITY_MANAGER_HPP
#define ECS_ENTITY_MANAGER_HPP

#include <cstdint>
#include <unordered_set>

#include "model_manager/ModelManager.hpp"

#include "auxillary/ComponentList.hpp"
#include "components/ComponentFlags.hpp"
#include "components/Transform.hpp"
#include "components/Model.hpp"
#include "Entity.hpp"

namespace Rasterizer::ECS {
    /* The EntityManager class, which allows the user to spawn and manage entities. Also manages their models and textures using a ModelManager and TextureManager, respectively. */
    class EntityManager {
    public:
        /* The maximum number of entities supported by the EntityManager. */
        static const constexpr entity_t max_entities = 1024;

    private:
        /* The list of all assigned entities. Each entity is specified by its ID and whether it is assigned (true) or not (false). */
        std::unordered_set<entity_t> entities;
        /* Maps each entity to a list of component flags s.t. we know if it exists. */
        std::unordered_map<entity_t, ComponentFlags> entity_components;
        
        /* The list of transform components. */
        ComponentList<Transform> transforms;
        /* The list of model components. */
        ComponentList<Model> models;

    public:
        /* Constructor for the EntityManager class. */
        EntityManager();

        /* Spawns a new entity in the EntityManager that has the given components. Returns the assigned ID to that entity. */
        entity_t add(ComponentFlags components);
        /* Despawns the given entity. Note that its ID may be re-used later, and should thus not be used to reference it anymore after this point. */
        void remove(entity_t entity);

        /* Returns whether or not the given entity exists. */
        inline bool exists(entity_t entity) const { return this->entities.find(entity) != this->entities.end(); }
        /* Returns whether or not the given entity has the given component(s). Also returns false if the entity does not exist. */
        inline bool has_component(entity_t entity, ComponentFlags components) const { return this->entities.find(entity) != this->entities.end() && (this->entity_components.at(entity) & components) == components; }

        /* Returns the Transform component for the given entity. Will throw errors if that entity does not exists. */
        inline Transform& get_transform(entity_t entity) { return this->transforms[entity]; }
        /* Returns the Transform component for the given entity. Will throw errors if that entity does not exists. */
        inline const Transform& get_transform(entity_t entity) const { return this->transforms[entity]; }
        /* Returns the Model component for the given entity. Will throw errors if that entity does not exists or has no such component. */
        inline Model& get_model(entity_t entity) { return this->models[entity]; }
        /* Returns the Model component for the given entity. Will throw errors if that entity does not exists or has no such component. */
        inline const Model& get_model(entity_t entity) const { return this->models[entity]; }

        /* Returns a muteable iterator to the start of the EntityManager. */
        inline std::unordered_set<entity_t>::iterator begin() { return this->entities.begin(); }
        /* Returns a muteable iterator to the end of the EntityManager. */
        inline std::unordered_set<entity_t>::iterator end() { return this->entities.end(); }
        /* Returns an unmuteable iterator to the start of the EntityManager. */
        inline std::unordered_set<entity_t>::const_iterator begin() const { return this->entities.begin(); }
        /* Returns an unmuteable iterator to the end of the EntityManager. */
        inline std::unordered_set<entity_t>::const_iterator end() const { return this->entities.end(); }
        /* Returns an unmuteable iterator to the start of the EntityManager. */
        inline std::unordered_set<entity_t>::const_iterator cbegin() const { return this->entities.cbegin(); }
        /* Returns an unmuteable iterator to the end of the EntityManager. */
        inline std::unordered_set<entity_t>::const_iterator cend() const { return this->entities.cend(); }

    };

}

#endif
