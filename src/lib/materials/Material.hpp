/* MATERIAL.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:46:02
 * Last edited:
 *   09/09/2021, 16:46:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Material class, which is a MaterialPool-managed object that
 *   holds and manages the data needed for Materials. Doesn't technically hold
 *   anything itself, though, but forms a baseclass for material-specific
 *   containers.
**/

#ifndef MATERIALS_MATERIAL_HPP
#define MATERIALS_MATERIAL_HPP

#include <cstdint>

#include "MaterialType.hpp"

namespace Makma3D::Materials {
    /* The Material class, which forms the baseclass for all MaterialPool-managed, data-carrying objects for Materials. */
    class Material {
    protected:
        /* The type of Material. */
        MaterialType _type;
        /* The name of the Material (used for debugging). */
        std::string _name;

    protected:
        /* Constructor for the Material class, which takes its type and its name. */
        Material(MaterialType type, const std::string& name);
        /* Destructor for the Material class, which is virtual but private. */
        virtual ~Material();

        /* Declare the MaterialPool as our fwiend. */
        friend class MaterialPool;

    public:
        /* Copy constructor for the Material class, which is deleted. */
        Material(const Material& other) = delete;
        /* Move constructor for the Material class, which is deleted. */
        Material(Material&& other) = delete;

        /* Returns the Material's type. */
        inline MaterialType type() const { return this->_type; }
        /* Returns the Material's name. */
        inline const std::string& name() const { return this->_name; }

        /* Copy assignment operator for the Material class, which is deleted. */
        Material& operator=(const Material& other) = delete;
        /* Move assignment operator for the Material class, which is deleted. */
        Material& operator=(Material&& other) = delete;

    };
}

#endif
