/* MATERIAL MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   12/08/2021, 15:40:14
 * Last edited:
 *   12/08/2021, 15:40:14
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MaterialManager class, which manages and keeps track of
 *   materials. It also creates descriptor sets for each of those, allowing
 *   quick draws to be made per object.
**/

#ifndef MODELS_MATERIAL_MANAGER_HPP
#define MODELS_MATERIAL_MANAGER_HPP

#include <string>
#include <unordered_map>

#include "tools/Array.hpp"
#include "tools/StaticArray.hpp"
#include "rendering/gpu/GPU.hpp"
#include "rendering/descriptors/DescriptorSetLayout.hpp"
#include "rendering/descriptors/DescriptorPool.hpp"
#include "rendering/descriptors/DescriptorSet.hpp"

namespace Rasterizer::Models {
    /* The MaterialManager class, which manages all materials and their associated descriptor sets. */
    class MaterialManager {
    public:
        /* The number of different materials in this manager. */
        static constexpr const uint32_t n_materials = 1;

        /* The GPU to which this manager is bound. */
        const Rendering::GPU& gpu;

    private:
        /* List of descriptor set layouts. We have one descriptor set layout per type of material, matching each pool. */
        Tools::StaticArray<Rendering::DescriptorSetLayout, n_materials> descriptor_set_layouts;
        /* List of descriptor pools. We have one descriptor pool per type of material. */
        Tools::StaticArray<Rendering::DescriptorPool, n_materials> descriptor_pools;
        /* List of descriptor sets allocated with the pools. We have one per object. */
        Tools::Array<Rendering::DescriptorSet> descriptor_sets;

        /* Mapping of material names to descriptor layout/pool indices. */
        std::unordered_map<std::string, uint32_t> material_map;

    public:
        /* The MaterialManager class, which takes the GPU where it lives. */
        MaterialManager(const Rendering::GPU& gpu);
        /* Copy constructor for the MaterialManager class. */
        MaterialManager(const MaterialManager& other);
        /* Move constructor for the MaterialManager class. */
        MaterialManager(MaterialManager&& other);
        /* Destructor for the MaterialManager class. */
        ~MaterialManager();

        /* Copy assignment operator for the MaterialManager class. */
        inline MaterialManager& operator=(const MaterialManager& other) { return *this = MaterialManager(other); }
        /* Move assignment operator for the MaterialManager class. */
        inline MaterialManager& operator=(MaterialManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the MaterialManager class. */
        friend void swap(MaterialManager& mm1, MaterialManager& mm2);

    };

    /* Swap operator for the MaterialManager class. */
    void swap(MaterialManager& mm1, MaterialManager& mm2);

}

#endif
