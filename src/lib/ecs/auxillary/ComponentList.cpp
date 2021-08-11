/* COMPONENT LIST.cpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:39:54
 * Last edited:
 *   8/1/2021, 3:43:52 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ComponentList class, which aims to efficiently store and
 *   return a single component from an entity.
**/

#include <cstring>
#include <algorithm>

#include "tools/CppDebugger.hpp"
#include "tools/Typenames.hpp"

#include "ComponentList.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace CppDebugger::SeverityValues;


/***** COMPONENTLIST CLASS *****/
/* Constructor for the ComponentList class, which takes the type of the Component as a flag and an initial array size. */
template <class T>
ComponentList<T>::ComponentList(ComponentFlags type_flags, component_list_size_t initial_capacity) :
    IComponentList(type_flags, initial_capacity),
    entities((T*) malloc(initial_capacity * sizeof(T)))
{}

/* Copy constructor for the ComponentList class. */
template <class T>
ComponentList<T>::ComponentList(const ComponentList<T>& other) :
    IComponentList(other),
    entities((T*) malloc(other.max_entities * sizeof(T)))
{
    // Also copy the structs themselves
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(this->entities, other.entities, this->n_entities * sizeof(T));
    } else {
        for (component_list_size_t i = 0; i < this->n_entities; i++) {
            new(this->entities + i) T(other.entities[i]);
        }
    }
}

/* Move constructor for the ComponentList class. */
template <class T>
ComponentList<T>::ComponentList(ComponentList<T>&& other) :
    IComponentList(std::move(other)),
    entities(other.entities)
{
    other.entities = nullptr;
}

/* Destructor for the ComponentList class. */
template <class T>
ComponentList<T>::~ComponentList() {
    if (this->entities != nullptr) {
        if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
            for (component_list_size_t i = 0; i < this->n_entities; i++) {
                this->entities[i].~T();
            }
        }
        free(this->entities);
    }
}



/* Stores a new 'entity', filling it with default values. */
template <class T>
void ComponentList<T>::add(entity_t entity) {
    DENTER("ECS::ComponentList<" + Tools::type_sname<T>() + ">::add");
    this->add(entity, T());
    DRETURN;
}

/* Stores a new 'entity', by associating the given entity ID with the given Component data. */
template <class T>
void ComponentList<T>::add(entity_t entity, const T& component) {
    DENTER("ECS::ComponentList<" + Tools::type_sname<T>() + ">::add(component)");

    // Try to find if the entity already exists
    std::unordered_map<entity_t, component_list_size_t>::iterator iter = this->entity_map.find(entity);
    if (iter != this->entity_map.end()) {
        DLOG(warning, "Entity with ID " + std::to_string(entity) + " already exists in the ComponentList; will be overwritten.");
        this->entity_map.erase(iter);
    }

    // If needed, double the size of the array
    while (this->n_entities >= this->max_entities) {
        this->reserve(this->max_entities * 2);
    }

    // Assign an index
    component_list_size_t index = this->n_entities;
    // Add the component
    new(this->entities + index) T(component);
    // Add the mappings
    this->entity_map.insert(make_pair(entity, index));
    this->index_map.insert(make_pair(index, entity));

    // Done, increment the size
    ++this->n_entities;
    DRETURN;
}

/* Removes an 'entity', by de-associating the given entity ID and removing the Component from the internal list. */
template <class T>
void ComponentList<T>::remove(entity_t entity) {
    DENTER("ECS::ComponentList<" + Tools::type_sname<T>() + ">::remove");

    // Try to find the entity in our internal mapping
    std::unordered_map<entity_t, component_list_size_t>::iterator iter = this->entity_map.find(entity);
    if (iter == this->entity_map.end()) {
        DLOG(fatal, "Cannot remove non-mapped entity with ID " + std::to_string(entity));
    }
    component_list_size_t index = (*iter).second;

    // Remove the entity from the maps
    this->entity_map.erase(iter);
    this->index_map.erase(index);

    // Delete the entity if needed
    if constexpr (std::conjunction<std::is_destructible<T>, std::negation<std::is_trivially_destructible<T>>>::value) {
        this->entities[index].~T();
    }

    // If it's not the last entity, move the rest forward
    if constexpr (std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>>::value) {
        // Simply call memmove
        memmove(this->entities + index, this->entities + index + 1, (this->n_entities - index - 1) * sizeof(T));
        for (component_list_size_t i = index + 1; i < this->n_entities; i++) {
            entity_t moved_entity = this->index_map.at(i);
            this->entity_map.at(moved_entity) -= 1;
            this->index_map.erase(i);
            this->index_map[i - 1] = moved_entity;
        }
    } else {
        // Note that i begins on this->n_entities - 1
        for (component_list_size_t i = this->n_entities; i-- > index ;) {
            // Copy the entity with its move constructor
            new(this->entities + (i - 1)) T(std::move(this->entities[i]));

            // Update the entity's maps
            entity_t moved_entity = this->index_map.at(i);
            this->entity_map.at(moved_entity) -= 1;
            this->index_map.erase(i);
            this->index_map[i - 1] = moved_entity;
        }
    }

    // Done, decrement the size
    --this->n_entities;
    DRETURN;
}



/* Reserves space for new entities by re-allocating the internal array. If the new capacity is lower than the current size, then entities at the end will be removed automatically. New entities will be left unitialised, since there's obviously no mapping available yet. */
template <class T>
void ComponentList<T>::reserve(component_list_size_t new_capacity) {
    DENTER("ECS::ComponentList<" + Tools::type_sname<T>() + ">::reserve");

    // First, try to allocate the new capacity
    T* new_entities = (T*) malloc(new_capacity * sizeof(T));
    if (new_entities == nullptr) {
        DLOG(fatal, "Could not allocate new array of size " + std::to_string(new_capacity) + ".");
    }

    // Copy the elements from the old to the new array
    component_list_size_t n_to_copy = std::min(this->n_entities, new_capacity);
    if constexpr (std::conjunction<std::is_trivially_copy_constructible<T>, std::is_trivially_copy_assignable<T>>::value) {
        memcpy(new_entities, this->entities, n_to_copy * sizeof(T));
    } else {
        for (component_list_size_t i = 0; i < n_to_copy; i++) {
            new(new_entities + i) T(this->entities[i]);
        }
    }

    // When done, deallocate the old one
    free(this->entities);

    // Overwrite the internal pointers
    this->entities = new_entities;
    this->n_entities = n_to_copy;
    this->max_entities = new_capacity;

    // Done
    DRETURN;
}



/* Allows the ComponentList to be copied virtually. */
template <class T>
ComponentList<T>* ComponentList<T>::copy() const {
    return new ComponentList<T>(*this);
}
