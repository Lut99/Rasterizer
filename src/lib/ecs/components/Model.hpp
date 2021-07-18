/* MODEL.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 16:09:36
 * Last edited:
 *   18/07/2021, 16:09:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Component for entities that have a model which needs to be managed.
**/

#ifndef ECS_MODEL_HPP
#define ECS_MODEL_HPP

#include "tools/Typenames.hpp"

namespace Rasterizer::ECS {
    /* The Model component, which describes everything needed to manage an object with a model. */
    struct Model {
        /* The id of the model. */
        std::string model_id;
    };

}

namespace Tools {
    /* The string name of the Model component. */
    template <> inline const char* type_name<Rasterizer::ECS::Model>() { return "Model"; }
}

#endif
