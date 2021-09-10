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

#include "glm/glm.hpp"

#include "tools/Array.hpp"
#include "ecs/EntityManager.hpp"
#include "textures/Texture.hpp"
#include "rendering/memory/MemoryManager.hpp"

#include "Material.hpp"

namespace Makma3D::Materials {
    /* The MaterialSystem class, which is in charge of loading, managing and rendering materials. */
    class MaterialSystem {
    public:
        /* The logging channel for the MaterialSystem class. */
        static constexpr const char* channel = "MaterialSystem";

        /* Reference to the MemoryManager used to allocate structures. */
        Rendering::MemoryManager& memory_manager;

    private:
        /* List of materials that use the Simple lighting model and that are not textured. */
        /* List of materials that use the Simple lighting model and that are yes textured. */

    public:
        /* Constructor for the MaterialSystem class, which takes a MemoryManager so it can allocate GPU memory structures. */
        MaterialSystem(Rendering::MemoryManager& memory_manager);
        /* Copy constructor for the MaterialSystem, which is deleted. */
        MaterialSystem(const MaterialSystem& other) = delete;
        /* Move constructor for the MaterialSystem. */
        MaterialSystem(MaterialSystem&& other);
        /* Destructor for the MaterialSystem. */
        ~MaterialSystem();

        /* Adds a new material that uses the simple lighting model and no textures. The colour given is the colour for the entire object. Returns the ID of the new material. */
        material_t create_simple(const glm::vec3& colour);
        /* Adds a new material that uses the simple lighting model with a texture. The texture used is the given one. */
        material_t create_simple_textured(const Textures::Texture& texture);

        /* Renders the given list of objects with their assigned materials. */
        void render(const ECS::EntityManager& entity_manager);

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
