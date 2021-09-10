/* MATERIAL SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   09/09/2021, 16:28:57
 * Last edited:
 *   09/09/2021, 16:28:57
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MaterialSystem, which is in charge of loading, managing
 *   and then rendering materials. Models will be able to load materials
 *   for their meshes.
**/

#ifndef MATERIALS_MATERIAL_SYSTEM_HPP
#define MATERIALS_MATERIAL_SYSTEM_HPP

#include "tools/Array.hpp"
#include "rendering/memory/MemoryManager.hpp"

#include "Material.hpp"
#include "auxillary/ComponentList.hpp"
#include "components/Diffused.hpp"
#include "components/Textured.hpp"

namespace Makma3D::Materials {
    /* The MaterialSystem class, which is in charge of loading, managing and rendering materials. */
    class MaterialSystem {
    public:
        /* The logging channel for the MaterialSystem class. */
        static constexpr const char* channel = "MaterialSystem";

        /* Reference to the MemoryManager used to allocate structures. */
        Rendering::MemoryManager& memory_manager;
    
    private:
        /* List of all materials in the MaterialSystem. */
        Tools::Array<material_t> materials;
        /* ComponentList in charge of storing all materials that use diffuse lighting. */
        ComponentList<Materials::Diffused> diffused_list;
        /* ComponentList in charge of storing all materials that use textures. */
        ComponentList<Materials::Textured> textured_list;

    public:
        /* Constructor for the MaterialSystem class, which takes a MemoryManager so it can allocate GPU memory structures. */
        MaterialSystem(Rendering::MemoryManager& memory_manager);
        /* Copy constructor for the MaterialSystem, which is deleted. */
        MaterialSystem(const MaterialSystem& other) = delete;
        /* Move constructor for the MaterialSystem. */
        MaterialSystem(MaterialSystem&& other);
        /* Destructor for the MaterialSystem. */
        ~MaterialSystem();



        /* Copy assignment operator for the MaterialSystem class, which is deleted. */
        MaterialSystem& operator=(const MaterialSystem& other) = delete;
        /* Move assignment operator for the MaterialSystem class. */
        inline MaterialSystem& operator=(MaterialSystem&& other);
        /* Swap operator for the MaterialSystem class. */
        friend void swap(MaterialSystem& ms1, MaterialSystem& ms2);

    };
    
    /* Swap operator for the MaterialSystem class. */
    void swap(MaterialSystem& ms1, MaterialSystem& ms2);

}

#endif
