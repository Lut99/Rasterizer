/* COMPONENT LIST.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:39:57
 * Last edited:
 *   8/1/2021, 3:33:46 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ComponentList class, which aims to efficiently store and
 *   return a single component from an entity.
**/

#ifndef ECS_COMPONENT_LIST_HPP
#define ECS_COMPONENT_LIST_HPP

#include "IComponentList.hpp"

namespace Makma3D::ECS {
    /* The ComponentList class, which aims to efficiently associate entity IDs with a single component. Note that the type is required to be at least default constructible, copy constructible and move constructible. */
    template <class T>
    class ComponentList: public IComponentList {
    public:
        /* The type of the Component. */
        using type = T;

        /* The channel used for all ComponentList-related log messages. */
        static constexpr const char* channel = "ComponentList";

    private:
        /* The array of components that we wrap. */
        T* entities;
    
    public:
        /* Constructor for the ComponentList class, which takes the type of the Component as a flag and an initial array size. */
        ComponentList(ComponentFlags type_flags = ComponentFlags::none, component_list_size_t initial_capacity = 16);
        /* Copy constructor for the ComponentList class. */
        ComponentList(const ComponentList<T>& other);
        /* Move constructor for the ComponentList class. */
        ComponentList(ComponentList<T>&& other);
        /* Destructor for the ComponentList class. */
        ~ComponentList();

        /* Returns the component associated to the given index (useful for iteration). */
        inline T& operator[](component_list_size_t index) { return this->entities[index]; }
        /* Returns the component associated to the given index (useful for iteration). */
        inline const T& operator[](component_list_size_t index) const { return this->entities[index]; }
        /* Returns the component associated to the given entity. */
        inline T& get(entity_t entity) { return this->entities[this->entity_map.at(entity)]; }
        /* Returns the component associated to the given entity. */
        inline const T& get(entity_t entity) const { return this->entities[this->entity_map.at(entity)]; }

        /* Stores a new 'entity', filling it with default values. */
        virtual void add(entity_t entity);
        /* Stores a new 'entity', by associating the given entity ID with the given Component data. */
        void add(entity_t entity, const T& component);
        /* Removes an 'entity', by de-associating the given entity ID and removing the Component from the internal list. */
        virtual void remove(entity_t entity);

        /* Reserves space for new entities by re-allocating the internal array. If the new capacity is lower than the current size, then entities at the end will be removed automatically. New entities will be left unitialised, since there's obviously no mapping available yet. */
        void reserve(component_list_size_t new_capcity);

        /* Copy assignment operator for the ComponentList class. */
        inline ComponentList<T>& operator=(const ComponentList<T>& other) { return *this = ComponentList<T>(other); }
        /* Move assignment operator for the ComponentList class. */
        inline ComponentList<T>& operator=(ComponentList<T>&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Allows the ComponentList to be copied virtually. */
        virtual ComponentList<T>* copy() const;
        /* Swap operator for the ComponentList class. */
        friend void swap(ComponentList<T>& cl1, ComponentList<T>& cl2) {
            using std::swap;

            swap((IComponentList&) cl1, (IComponentList&) cl2);
            swap(cl1.entities, cl2.entities);
        }

    };
}

// Also include the .cpp since it's a template
#include "ComponentList.cpp"

#endif
