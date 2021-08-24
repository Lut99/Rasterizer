/* ENTITY MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:19:10
 * Last edited:
 *   07/08/2021, 15:20:06
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

#include "auxillary/ComponentHash.hpp"
#include "auxillary/IComponentList.hpp"
#include "auxillary/ComponentList.hpp"
#include "components/ComponentFlags.hpp"
#include "Entity.hpp"

namespace Rasterizer::ECS {
    /* The EntityManager class, which allows the user to spawn and manage entities. Also manages their models and textures using a ModelManager and TextureManager, respectively. */
    class EntityManager {
    public:
        /* The channel name used for the Logger. */
        static constexpr const char* channel = "EntityManager";
        /* The maximum number of entities supported by the EntityManager. */
        static constexpr const entity_t max_entities = 1024;
        /* The maximum number of components supported by the EntityManager. */
        static constexpr const uint32_t max_components = 4;

    private:
        /* List describing all entities. In particular, each entity is defined by having an index in this list. */
        std::unordered_map<entity_t, ComponentFlags> entities;
        /* The list of components, stored in their own ComponentList derivative. */
        IComponentList** components;

    public:
        /* Constructor for the EntityManager class. */
        EntityManager();
        /* Copy constructor for the EntityManager class. */
        EntityManager(const EntityManager& other);
        /* Move constructor for the EntityManager class. */
        EntityManager(EntityManager&& other);
        /* Destructor for the EntityManager class. */
        ~EntityManager();

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
        /* Returns a muteable reference to the templated component. Does not perform any checks on whether or not the entity has the given component. */
        template <class T>
        inline T& get_component(entity_t entity) { return ((ComponentList<T>*) this->components[hash_component<T>()])->get(entity); }
        /* Returns a immuteable reference to the templated component. Does not perform any checks on whether or not the entity has the given component. */
        template <class T>
        inline const T& get_component(entity_t entity) const { return ((ComponentList<T>*) this->components[hash_component<T>()])->get(entity); }

        /* Returns a muteable reference to the component list itself so that it can be iterated over. */
        template <class T>
        inline ComponentList<T>& get_list() { return (ComponentList<T>&) *this->components[hash_component<T>()]; }
        /* Returns an immuteable reference to the component list itself so that it can be iterated over. */
        template <class T>
        inline const ComponentList<T>& get_list() const { return (ComponentList<T>&) *this->components[hash_component<T>()]; }

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

        /* Copy assignment operator for the EntityManager class. */
        inline EntityManager& operator=(const EntityManager& other) { return *this = EntityManager(other); }
        /* Move assignment operator for the EntityManager class. */
        inline EntityManager& operator=(EntityManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the EntityManager class. */
        friend void swap(EntityManager& em1, EntityManager& em2);

    };
    
    /* Swap operator for the EntityManager class. */
    void swap(EntityManager& em1, EntityManager& em2);

}

#endif
