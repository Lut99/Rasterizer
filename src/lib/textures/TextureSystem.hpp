/* TEXTURE SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:49:33
 * Last edited:
 *   9/19/2021, 5:56:37 PM
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

#include "rendering/memory_manager/MemoryManager.hpp"
#include "rendering/commandbuffers/CommandBuffer.hpp"
#include "rendering/sampling/Sampler.hpp"
#include "rendering/sampling/SamplerPool.hpp"

#include "TextureFormat.hpp"
#include "Texture.hpp"

namespace Makma3D::Textures {
    /* The TextureSystem class, which is responsible for loading and managing textures. */
    class TextureSystem {
    public:
        /* Channel name for the TextureSystem class. */
        static constexpr const char* channel = "TextureSystem";

        /* The MemoryManager class used for allocating new images and stage buffers and the like. */
        Rendering::MemoryManager& memory_manager;
    
    private:
        /* The SamplerPool used to allocate new samplers from. */
        Rendering::SamplerPool sampler_pool;

    public:
        /* Constructor for the TextureSystem class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
        TextureSystem(Rendering::MemoryManager& memory_manager);
        /* Copy constructor for the TextureSystem class, which is deleted. */
        TextureSystem(const TextureSystem& other) = delete;
        /* Move constructor for the TextureSystem class. */
        TextureSystem(TextureSystem&& other);
        /* Destructor for the TextureSystem class. */
        ~TextureSystem();

        /* Loads a new texture from the given file with the given format into a Texture struct and returns that. */
        Texture load_texture(const std::string& path, TextureFormat format = TextureFormat::png);
        /* Destroys the data in the given texture struct. */
        void unload_texture(const Texture& texture);

        /* Copy assignment operator for the TextureSystem class, which is deleted. */
        TextureSystem& operator=(const TextureSystem& other) = delete;
        /* Move assignment operator for the TextureSystem class. */
        inline TextureSystem& operator=(TextureSystem&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the TextureSystem class. */
        friend void swap(TextureSystem& ts1, TextureSystem& ts2);

    };
    
    /* Swap operator for the TextureSystem class. */
    void swap(TextureSystem& ts1, TextureSystem& ts2);

}

#endif
