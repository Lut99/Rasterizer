/* ICOMPONENT LIST.hpp
 *   by Lut99
 *
 * Created:
 *   19/07/2021, 12:59:22
 * Last edited:
 *   19/07/2021, 12:59:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for the ComponentList, which doesn't yet rely on a specific
 *   component type. Can thus be used to construct arrays.
**/

#ifndef ECS_I_COMPONENT_LIST_HPP
#define ECS_I_COMPONENT_LIST_HPP

#include <unordered_map>

#include "../Entity.hpp"
#include "../components/ComponentFlags.hpp"

namespace Rasterizer::ECS {
    /* Datatype for the indices in a componentlist. */
    using component_list_size_t = uint32_t;



    /* Baseclass for a typed ComponentList. Used for arrays and junk. */
    class IComponentList {
    protected:
        /* The flag describing this component type. */
        ComponentFlags type_flags;

        /* Maps a given entity ID to a given internal index. */
        std::unordered_map<entity_t, component_list_size_t> entity_map;
        /* Maps a given internal index to a given entity ID. */
        std::unordered_map<component_list_size_t, entity_t> index_map;

        /* The number of components currently stored. */
        component_list_size_t n_entities;
        /* The maximum number of components we can store before resizing. */
        component_list_size_t max_entities;

        
        /* Constructor for the IComponentList, which takes the type of component and the initial size. */
        IComponentList(ComponentFlags type_flags, component_list_size_t init_capacity);
    
    public:
        /* Virtual destructor for the IComponentList. */
        virtual ~IComponentList() = default;

        /* Stores a new 'entity', filling it with default values. */
        virtual void add(entity_t) = 0;
        /* Removes an 'entity', by de-associating the given entity ID and removing the Component from the internal list. */
        virtual void remove(entity_t entity) = 0;

        /* Returns the number of entities in the ComponentList. */
        inline component_list_size_t size() const { return this->n_entities; }
        /* Returns the maximum number of entities we can store in the ComponentList before we have to resize. */
        inline component_list_size_t capacity() const { return this->max_entities; }
        /* Returns the flags that describe this component's type. */
        inline ComponentFlags flags() const { return this->type_flags; }

        /* Allows the IComponentList to be copied virtually. */
        virtual IComponentList* copy() const = 0;
        /* Swap operator for the IComponentList class. */
        friend void swap(IComponentList& icl1, IComponentList& icl2);

    };

    /* Swap operator for the IComponentList class. */
    void swap(IComponentList& icl1, IComponentList& icl2);

}

#endif
