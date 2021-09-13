/* ICOMPONENT LIST.cpp
 *   by Lut99
 *
 * Created:
 *   19/07/2021, 12:59:33
 * Last edited:
 *   19/07/2021, 12:59:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for the ComponentList, which doesn't yet rely on a specific
 *   component type. Can thus be used to construct arrays.
**/

#include "IComponentList.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::ECS;


/***** ICOMPONENTLIST CLASS *****/
/* Constructor for the IComponentList, which takes the type of component and the initial size. */
IComponentList::IComponentList(ComponentFlags type_flags, component_list_size_t init_capacity) :
    type_flags(type_flags),

    n_entities(0),
    max_entities(init_capacity)
{}



/* Swap operator for the IComponentList class. */
void ECS::swap(IComponentList& icl1, IComponentList& icl2) {
    using std::swap;
    
    swap(icl1.type_flags, icl2.type_flags);

    swap(icl1.entity_map, icl2.entity_map);
    swap(icl1.index_map, icl2.index_map);
    
    swap(icl1.n_entities, icl2.n_entities);
    swap(icl1.max_entities, icl2.max_entities);
}
