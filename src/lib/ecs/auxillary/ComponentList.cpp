/* COMPONENT LIST.cpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:39:54
 * Last edited:
 *   18/07/2021, 12:39:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ComponentList class, which aims to efficiently store and
 *   return a single component from an entity.
**/

#include "tools/CppDebugger.hpp"
#include "tools/Typenames.hpp"

#include "ComponentList.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace CppDebugger::SeverityValues;


/***** COMPONENTLIST CLASS *****/
/* Constructor for the ComponentList class, which takes an initial array size. */
template <class T>
ComponentList<T>::ComponentList(component_list_size_t initial_capacity = 16) :
    entities((T*) malloc(initial_capacity * sizeof(T))),
    n_entities(0),
    max_entities(initial_capacity)
{}

/* Copy constructor for the ComponentList class. */
template <class T>
ComponentList<T>::ComponentList(const ComponentList<T>& other) :
    entity_map(other.entity_map),
    index_map(other.index_map),

    entities((T*) malloc(other.max_entities * sizeof(T))),
    n_entities(other.n_entities),
    max_entities(other.max_entities)
{
    // Also copy the structs themselves
    memcpy(this->entities, other.entities, this->n_entities * sizeof(T));
}

/* Move constructor for the ComponentList class. */
template <class T>
ComponentList<T>::ComponentList(ComponentList<T>&& other) :
    entity_map(std::move(other.entity_map)),
    index_map(std::move(other.index_map)),

    entities(other.entities),
    n_entities(other.n_entities),
    max_entities(other.max_entities)
{
    other.entities = nullptr;
}

/* Destructor for the ComponentList class. */
template <class T>
ComponentList<T>::~ComponentList() {
    if (this->entities != nullptr) {
        free(this->entities);
    }
}



/* Stores a new 'entity', by associating the given entity ID with the given Component data. */
template <class T>
void ComponentList<T>::add(entity_t entity, const T& component) {
    DENTER("ECS::ComponentList<" + Tools::type_sname<T>() + ">::add");

    // Try to find if the entity already exists
    std::unordered_map<entity_t, component_list_size_t>::iterator iter = this->entity_map.find(entity);
    if (iter != this->entity_map.end()) {
        DLOG(warning, "Entity with ID " + std::to_string(entity) + " already exists in the ComponentList; will be overwritten.");
        this->entity_map.erase(iter);
    }

    // If needed, double the size of the array
    while (this->n_entities >= this->max_entities) {
        this->reserve(this->capacity * 2);
    }

    // Assign an index
    component_list_size_t index = this->n_entities;
    // Add the component
    this->entities[index] = component;
    // Add the mapping
    this->entity_map.insert(make_pair(entity, index));
    this->index_map.insert(make_pair(index, entity));

    // Done
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

    // If it's not the last entity, move the rest forward
    // Note that i begins on this->n_entities - 1
    for (component_list_size_t i = this->n_entities; i-- > index ;) {
        this->entities[i - 1] = this->entities[i];
        this->entity_map.at(entity) -= 1;
        this->index_map.erase(i);
        this->index_map[i - 1] = entity;
    }

    // Done
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
    memcpy(new_entities, this->entities, std::min(this->n_components, new_capacity) * sizeof(T));

    // When done, deallocate the old one
    free(this->entities);

    // Overwrite the internal pointers
    this->entities = new_entities;
    this->n_entities = std::min(this->n_entities, new_capacity);
    this->max_entities = new_capacity;

    // Done
    DRETURN;
}



/* Swap operator for the ComponentList class. */
template <class T>
void ECS::swap(ComponentList<T>& cl1, ComponentList<T>& cl2) {
    using std::swap;

    swap(cl1.entity_map, cl2.entity_map);
    
    swap(cl1.entities, cl2.entities);
    swap(cl1.n_entities, cl2.n_entities);
    swap(cl1.max_entities, cl2.max_entities);
}
