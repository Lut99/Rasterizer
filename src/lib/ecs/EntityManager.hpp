/* ENTITY MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:19:10
 * Last edited:
 *   10/09/2021, 17:19:31
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
#include <unordered_map>

#include "auxillary/ComponentList.hpp"

#include "components/ComponentFlags.hpp"
#include "components/Transform.hpp"
#include "components/Model.hpp"
#include "components/Controllable.hpp"
#include "components/Camera.hpp"

#include "Entity.hpp"

namespace Makma3D::ECS {
    /* The EntityManager class, which allows the user to spawn and manage entities. Also manages their models and textures using a ModelManager and TextureManager, respectively. */
    class EntityManager {
    public:
        /* The channel name used for the Logger. */
        static constexpr const char* channel = "EntityManager";
        /* The maximum number of entities supported by the EntityManager. */
        static constexpr const entity_t max_entities = 1024;

    private:
        /* List describing all entities. In particular, each entity is defined by having an index in this list. */
        std::unordered_map<entity_t, ComponentFlags> entities;

        /* The Transform components of all entities. */
        ComponentList<Transform> transforms;
        /* The Model components of all entities. */
        ComponentList<Model> models;
        /* The Controllable components of all entities. */
        ComponentList<Controllable> controllables;
        /* The Camera components of all entities. */
        ComponentList<Camera> cameras;

    public:
        /* Constructor for the EntityManager class. */
        EntityManager();

        /* Spawns a new entity in the EntityManager that has the given components, automatically casting the given int to a ComponentsFlags. Returns the assigned ID to that entity. */
        inline entity_t add(int components) { return this->add((ComponentFlags) components); }
        /* Spawns a new entity in the EntityManager that has the given components. Returns the assigned ID to that entity. */
        entity_t add(ComponentFlags components);
        /* Despawns the given entity. Note that its ID may be re-used later, and should thus not be used to reference it anymore after this point. */
        void remove(entity_t entity);

        /* Returns whether or not the given entity exists. */
        inline bool exists(entity_t entity) const { return this->entities.find(entity) != this->entities.end(); }
        /* Returns whether or not the given entity has the given component(s). Also returns false if the entity does not exist. */
        inline bool has_component(entity_t entity, ComponentFlags components) const { return (this->entities.at(entity) & components) == components; }
        /* Returns a muteable reference to the templated component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
        template <class T>
        inline T& get_component(entity_t entity) { logger.fatalc(EntityManager::channel, "Unknown component '", type_name<T>(), "'"); }
        /* Returns a immuteable reference to the templated component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
        template <class T>
        inline const T& get_component(entity_t entity) const { logger.fatalc(EntityManager::channel, "Unknown component '", type_name<T>(), "'"); }

        /* Returns a muteable reference to the component list itself so that it can be iterated over. */
        template <class T>
        inline ComponentList<T>& get_list() { logger.fatalc(EntityManager::channel, "Unknown component '", type_name<T>(), "'"); }
        /* Returns an immuteable reference to the component list itself so that it can be iterated over. */
        template <class T>
        inline const ComponentList<T>& get_list() const { logger.fatalc(EntityManager::channel, "Unknown component '", type_name<T>(), "'"); }

        /* Returns a muteable iterator to the start of the entities. Note that the entities are, in fact, unordered. */
        inline std::unordered_map<entity_t, ComponentFlags>::iterator begin() { return this->entities.begin(); }
        /* Returns a muteable iterator to the end of the entities. Note that the entities are, in fact, unordered. */
        inline std::unordered_map<entity_t, ComponentFlags>::iterator end() { return this->entities.end(); }
        /* Returns a immuteable iterator to the start of the entities. Note that the entities are, in fact, unordered. */
        inline std::unordered_map<entity_t, ComponentFlags>::const_iterator begin() const { return this->entities.begin(); }
        /* Returns a immuteable iterator to the end of the entities. Note that the entities are, in fact, unordered. */
        inline std::unordered_map<entity_t, ComponentFlags>::const_iterator end() const { return this->entities.end(); }
        /* Returns a immuteable iterator to the start of the entities. Note that the entities are, in fact, unordered. */
        inline std::unordered_map<entity_t, ComponentFlags>::const_iterator cbegin() const { return this->entities.cbegin(); }
        /* Returns a immuteable iterator to the end of the entities. Note that the entities are, in fact, unordered. */
        inline std::unordered_map<entity_t, ComponentFlags>::const_iterator cend() const { return this->entities.cend(); }

    };



    /* Returns a muteable reference to the Transform component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline Transform& EntityManager::get_component<Transform>(entity_t entity) { return this->transforms.get(entity); }
    /* Returns a immuteable reference to the Transform component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline const Transform& EntityManager::get_component<Transform>(entity_t entity) const { return this->transforms.get(entity); }
    /* Returns a muteable reference to the Model component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline Model& EntityManager::get_component<Model>(entity_t entity) { return this->models.get(entity); }
    /* Returns a immuteable reference to the Model component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline const Model& EntityManager::get_component<Model>(entity_t entity) const { return this->models.get(entity); }
    /* Returns a muteable reference to the Controllable component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline Controllable& EntityManager::get_component<Controllable>(entity_t entity) { return this->controllables.get(entity); }
    /* Returns a immuteable reference to the Controllable component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline const Controllable& EntityManager::get_component<Controllable>(entity_t entity) const { return this->controllables.get(entity); }
    /* Returns a muteable reference to the Camera component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline Camera& EntityManager::get_component<Camera>(entity_t entity) { return this->cameras.get(entity); }
    /* Returns a immuteable reference to the Camera component of the given entity. Does not perform any checks on whether or not the entity has the given component. */
    template <> inline const Camera& EntityManager::get_component<Camera>(entity_t entity) const { return this->cameras.get(entity); }

    /* Returns a muteable reference to the component list itself so that it can be iterated over. */
    template <> inline ComponentList<Transform>& EntityManager::get_list<Transform>() { return this->transforms; }
    /* Returns an immuteable reference to the component list itself so that it can be iterated over. */
    template <> inline const ComponentList<Transform>& EntityManager::get_list<Transform>() const { return this->transforms; }
    /* Returns a muteable reference to the component list itself so that it can be iterated over. */
    template <> inline ComponentList<Model>& EntityManager::get_list<Model>() { return this->models; }
    /* Returns an immuteable reference to the component list itself so that it can be iterated over. */
    template <> inline const ComponentList<Model>& EntityManager::get_list<Model>() const { return this->models; }
    /* Returns a muteable reference to the component list itself so that it can be iterated over. */
    template <> inline ComponentList<Controllable>& EntityManager::get_list<Controllable>() { return this->controllables; }
    /* Returns an immuteable reference to the component list itself so that it can be iterated over. */
    template <> inline const ComponentList<Controllable>& EntityManager::get_list<Controllable>() const { return this->controllables; }
    /* Returns a muteable reference to the component list itself so that it can be iterated over. */
    template <> inline ComponentList<Camera>& EntityManager::get_list<Camera>() { return this->cameras; }
    /* Returns an immuteable reference to the component list itself so that it can be iterated over. */
    template <> inline const ComponentList<Camera>& EntityManager::get_list<Camera>() const { return this->cameras; }

}

#endif
