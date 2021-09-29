/* SIMPLE.hpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 14:27:02
 * Last edited:
 *   29/09/2021, 14:27:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Simple class, which "carries" data for a material that
 *   just takes vertex colours and no lighting. Since it doesn't have any
 *   parameters, doesn't carry anything either; but is there to represent
 *   it.
**/

#ifndef MATERIALS_SIMPLE_HPP
#define MATERIALS_SIMPLE_HPP

#include "../../Material.hpp"

namespace Makma3D::Materials {
    /* The Simple class, which is used to represent the similarly named material (but doesn't actually carry any data). */
    class Simple: public Material {
    private:
        /* Constructor for the Simple class, which takes its name only. */
        Simple(const std::string& name);
        /* Destructor for the Simple class, which is virtual and private. */
        virtual ~Simple();

        /* Declare the MaterialPool as our friend. */
        friend class MaterialPool;
    
    public:
        /* Copy constructor for the Simple class, which is deleted. */
        Simple(const Simple& other) = delete;
        /* Move constructor for the Simple class, which is deleted. */
        Simple(Simple&& other) = delete;

        /* Copy assignment operator for the Simple class, which is deleted. */
        Simple& operator=(const Simple& other) = delete;
        /* Move assignment operator for the Simple class, which is deleted. */
        Simple& operator=(Simple&& other) = delete;

    };

}

#endif
