/* SIMPLE COLOURED.hpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 12:57:31
 * Last edited:
 *   29/09/2021, 12:57:31
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SimpleColoured class, which carries data needed for a
 *   material with a uniform colour and no lighting. Extends from the
 *   base Material class, and is thus managed by the MaterialPool.
**/

#ifndef MATERIALS_SIMPLE_COLOURED_HPP
#define MATERIALS_SIMPLE_COLOURED_HPP

#include "glm/glm.hpp"
#include "rendering/data/MaterialData.hpp"
#include "../../Material.hpp"

namespace Makma3D::Materials {
    /* The SimpleColoured class, which carries information needed for the similarly named material. */
    class SimpleColoured: public Materials::Material {
    public:
        /* The colour of the SimpleColoured material. */
        glm::vec3 colour;

    private:
        /* Constructor for the SimpleColoured class, which takes its name and the colour it is supposed to have. */
        SimpleColoured(const std::string& name, const glm::vec3& colour);
        /* Destructor for the SimpleColoured class, which is virtual and private. */
        virtual ~SimpleColoured();

        /* Declare the MaterialPool as our friend. */
        friend class MaterialPool;
    
    public:
        /* Copy constructor for the SimpleColoured class, which is deleted. */
        SimpleColoured(const SimpleColoured& other) = delete;
        /* Move constructor for the SimpleColoured class, which is deleted. */
        SimpleColoured(SimpleColoured&& other) = delete;

        /* Returns the properties of the SimpleColoured material as a SimpleColouredData, so it can be transferred to the GPU. */
        inline Rendering::SimpleColouredData data() const { return Rendering::SimpleColouredData({ this->colour }); }

        /* Copy assignment operator for the SimpleColoured class, which is deleted. */
        SimpleColoured& operator=(const SimpleColoured& other) = delete;
        /* Move assignment operator for the SimpleColoured class, which is deleted. */
        SimpleColoured& operator=(SimpleColoured&& other) = delete;

    };
}

#endif
