/* COMPONENT LIST.cpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 17:16:30
 * Last edited:
 *   09/09/2021, 17:16:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a simplified ComponentList, which wraps around an Array to
 *   provide a list with mapped indices.
**/

#include "Tools/Logger.hpp"

#include "ComponentList.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Materials;


/***** COMPONENTLIST CLASS *****/
/* Constructor for the ComponentList, which takes the MaterialFlags it is and an initial size. */
template <class T>
ComponentList<T>::ComponentList(MaterialFlags type_flags, Tools::array_size_t intial_size) :
    type_flags(type_flags),

    components(initial_size)
{}



/* Stores a new material, filling its component with default values. */
template <class T>
void ComponentList<T>::add(material_t material) {
    // Check if the material is already mapped
    std::unordered_map<material_t, uint32_t>::iterator iter = this->material_index_map.find(material);
    if (iter != this->material_index_map.end()) {
        // It already exists, so overwrite the data with a warning
        this->components[(*iter).second] = T();
        logger.warningc(ComponentList<T>::channel, "Overwritten existing component with default values for material ", material);
    } else {
        // Add a new mapping
        this->material_index_map.insert({ material, this->components.size() });
        this->index_material_map.insert({ this->components.size(), material });

        // Add the default value
        this->components.push_back();
    }
}

/* Stores a new material by associating the given material ID with the given component data. */
template <class T>
void ComponentList<T>::add(material_t material, const T& component) {
    // Check if the material is already mapped
    std::unordered_map<material_t, uint32_t>::iterator iter = this->material_index_map.find(material);
    if (iter != this->material_index_map.end()) {
        // It already exists, so overwrite the data with a warning
        this->components[(*iter).second] = component;
        logger.warningc(ComponentList<T>::channel, "Overwritten existing component with new values for material ", material);
    } else {
        // Add a new mapping
        this->material_index_map.insert({ material, this->components.size() });
        this->index_material_map.insert({ this->components.size(), material });

        // Add the given value
        this->components.push_back(component);
    }
}

/* Removes an material's component by de-associating the given material ID and removing the component from the internal list. */
template <class T>
void ComponentList<T>::remove(material_t material) {
    // Check if we know the material
    std::unordered_map<material_t, uint32_t>::iterator iter = this->material_index_map.find(material);
    if (iter == this->material_index_map.end()) {
        logger.fatalc(ComponentList<T>::channel, "Cannot remove unknown material ", material, " from the ComponentList.");
    }

    // If we do, remove the item from the array
    this->components.erase((*iter).second);
    
    // Remove the old mappings
    this->index_material_map.erase((*iter).second);
    this->material_index_map.erase(iter);
    // Update the other mappings in the list
    for (uint32_t i = (*iter).second + 1; i < this->components.size(); i++) {
        material_t m = this->index_material_map.at(i);
        this->material_index_map.at(m) -= 1;
        this->index_material_map.erase(i);
        this->index_material_map.insert(i - 1) = m;
    }
}
