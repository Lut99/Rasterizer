/* ENTITY.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:34:38
 * Last edited:
 *   18/07/2021, 12:34:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the definition of an entity - in this case, defines the type
 *   for their ID.
**/

#ifndef ECS_ENTITY_HPP
#define ECS_ENTITY_HPP

#include <cstdint>

namespace Rasterizer::ECS {
    /* An Entity, which is an ID used by the EntityManager to, well, manage all entities. */
    using entity_t = uint32_t;

    /* The null value of the entity_t. */
    static constexpr const entity_t NullEntity = 0;

}

#endif
