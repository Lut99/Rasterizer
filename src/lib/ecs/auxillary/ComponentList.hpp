/* COMPONENT LIST.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:39:57
 * Last edited:
 *   18/07/2021, 12:39:57
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ComponentList class, which aims to efficiently store and
 *   return a single component from an entity.
**/

#ifndef ECS_COMPONENT_LIST_HPP
#define ECS_COMPONENT_LIST_HPP

#include <unordered_map>

#include "entity_manager/Entity.hpp"

namespace Rasterizer::ECS {
    /* Datatype for the indices in a componentlist. */
    using component_list_size_t = uint32_t;



    /* The ComponentList class, which aims to efficiently associate entity IDs with a single component. */
    template <class T>
    class ComponentList {
    public:
        /* The type of the Component. */
        using component_type = T;

    private:
        /* Maps a given entity ID to a given internal index. */
        std::unordered_map<entity_t, component_list_size_t> entity_map;
        /* Maps a given internal index to a given entity ID. */
        std::unordered_map<component_list_size_t, entity_t> index_map;

        /* The array of components that we wrap. */
        T* entities;
        /* The number of components currently stored. */
        component_list_size_t n_entities;
        /* The maximum number of components we can store before resizing. */
        component_list_size_t max_entities;
    
    public:
        /* Constructor for the ComponentList class, which takes an initial array size. */
        ComponentList(component_list_size_t initial_capacity = 16);
        /* Copy constructor for the ComponentList class. */
        ComponentList(const ComponentList<T>& other);
        /* Move constructor for the ComponentList class. */
        ComponentList(ComponentList<T>&& other);
        /* Destructor for the ComponentList class. */
        ~ComponentList();

        /* Returns the component associated to the given entity. */
        inline T& operator[](entity_t entity) { return this->entities[this->entity_map.at(entity)]; }
        /* Returns the component associated to the given entity. */
        inline const T& operator[](entity_t entity) const { return this->entities[this->entity_map.at(entity)]; }

        /* Stores a new 'entity', by associating the given entity ID with the given Component data. */
        void add(entity_t entity, const T& component);
        /* Removes an 'entity', by de-associating the given entity ID and removing the Component from the internal list. */
        void remove(entity_t entity);

        /* Reserves space for new entities by re-allocating the internal array. If the new capacity is lower than the current size, then entities at the end will be removed automatically. New entities will be left unitialised, since there's obviously no mapping available yet. */
        void reserve(component_list_size_t new_capcity);

        /* Returns the number of entities in the ComponentList. */
        inline component_list_size_t size() const { return this->n_entities; }
        /* Returns the maximum number of entities we can store in the ComponentList before we have to resize. */
        inline component_list_size_t capacity() const { return this->max_entities; }

        /* Copy assignment operator for the ComponentList class. */
        inline ComponentList<T>& operator=(const ComponentList<T>& other) { return *this = ComponentList<T>(other); }
        /* Move assignment operator for the ComponentList class. */
        inline ComponentList<T>& operator=(ComponentList<T>&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ComponentList class. */
        friend void swap(ComponentList<T>& cl1, ComponentList<T>& cl2);

    };

    /* Swap operator for the ComponentList class. */
    template <class T>
    void swap(ComponentList<T>& cl1, ComponentList<T>& cl2);

}

// Also include the .cpp since it's a template
#include "ComponentList.cpp"

#endif
