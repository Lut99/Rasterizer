/* COMPONENT LIST.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 17:16:41
 * Last edited:
 *   09/09/2021, 17:16:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a simplified ComponentList, which wraps around an Array to
 *   provide a list with mapped indices.
**/

#ifndef MATERIALS_COMPONENT_LIST_HPP
#define MATERIALS_COMPONENT_LIST_HPP

#include <unordered_map>
#include "tools/Array.hpp"

#include "../components/MaterialFlags.hpp"
#include "../Material.hpp"

namespace Rasterizer::Materials {
    /* The ComponentList class, which wraps around a normal Array to provide an Array with mapped indices. */
    template <class T>
    class ComponentList {
    public:
        /* The type of the list. */
        using type = T;

        /* Channel for the ComponentList class. */
        static constexpr const char* channel = "ComponentList";

    private:
        /* The flag for this specific ComponentList. */
        MaterialFlags type_flags;

        /* A map from material IDs to indices in the componentlist. */
        std::unordered_map<material_t, Tools::array_size_t> material_index_map;
        /* A map from indices in the componentlist to material IDS. */
        std::unordered_map<Tools::array_size_t, material_t> index_material_map;
        /* The Array with components we wrap. */
        Tools::Array<T> components;

    public:
        /* Constructor for the ComponentList, which takes the MaterialFlags it is and an initial size. */
        ComponentList(MaterialFlags type_flags, Tools::array_size_t intial_size = 16);

        /* Provides muteable access to the component at the given index. */
        inline T& operator[](Tools::array_size_t index) { return this->components[index]; }
        /* Provides immuteable access to the component at the given index. */
        inline const T& operator[](Tools::array_size_t index) const { return this->components[index]; }
        /* Returns a muteable reference to the component for the given entity. */
        inline T& get(material_t material) { return this->components[this->material_index_map.at(material)]; }
        /* Returns an immuteable reference to the component for the given entity. */
        inline const T& get(material_t material) const { return this->components[this->material_index_map.at(material)]; }

        /* Stores a new material, filling its component with default values. */
        void add(material_t material);
        /* Stores a new material by associating the given material ID with the given component data. */
        void add(material_t material, const T& component);
        /* Removes an material's component by de-associating the given material ID and removing the component from the internal list. */
        void remove(material_t material);

    };
}

// Also include the .cpp since it's a template
#include "ComponentList.cpp"

#endif
