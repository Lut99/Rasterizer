/* TEXTURE SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:49:33
 * Last edited:
 *   16/08/2021, 11:49:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the TextureSystem class, which is responsible for loading and
 *   managing textures for an entity.
**/

#ifndef TEXTURES_TEXTURE_SYSTEM_HPP
#define TEXTURES_TEXTURE_SYSTEM_HPP

#include "ecs/EntityManager.hpp"
#include "ecs/components/Textures.hpp"

#include "rendering/memory/MemoryManager.hpp"
#include "rendering/sampling/Sampler.hpp"

#include "TextureFormat.hpp"

namespace Rasterizer::Textures {
    /* The TextureSystem class, which is responsible for loading and managing textures. */
    class TextureSystem {
    public:
        /* Channel name for the TextureSystem class. */
        static constexpr const char* channel = "TextureSystem";

        /* The MemoryManager class used for allocating new images and stage buffers and the like. */
        Rendering::MemoryManager& memory_manager;
    
    private:
        /* The Sampler to use for all textures. */
        Rendering::Sampler sampler;

    public:
        /* Constructor for the TextureSystem class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
        TextureSystem(Rendering::MemoryManager& memory_manager);
        /* Copy constructor for the TextureSystem class. */
        TextureSystem(const TextureSystem& other);
        /* Move constructor for the TextureSystem class. */
        TextureSystem(TextureSystem&& other);
        /* Destructor for the TextureSystem class. */
        ~TextureSystem();

        /* Loads a new texture for the given entity using the given format. */
        void load_texture(ECS::EntityManager& entity_manager, entity_t entity, const std::string& path, TextureFormat format = TextureFormat::png);
        /* Unloads the texture loaded for the given entity. */
        void unload_texture(ECS::EntityManager& entity_manager, entity_t entity);

        /* Binds the model-related buffers and junk for the given mesh component to the given command buffer. */
        void schedule(const ECS::Texture& entity_texture, const Rendering::CommandBuffer& draw_cmd) const;

        /* Copy assignment operator for the TextureSystem class. */
        inline TextureSystem& operator=(const TextureSystem& other) { return *this = TextureSystem(other); }
        /* Move assignment operator for the TextureSystem class. */
        inline TextureSystem& operator=(TextureSystem&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the TextureSystem class. */
        friend void swap(TextureSystem& ts1, TextureSystem& ts2);

    };
    
    /* Swap operator for the TextureSystem class. */
    void swap(TextureSystem& ts1, TextureSystem& ts2);

}

#endif
