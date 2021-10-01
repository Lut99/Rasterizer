/* TEXTURE POOL.hpp
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
 *   Contains the TexturePool class, which is responsible for loading and
 *   managing textures. By pooling it, it also tries to minimise data
 *   duplication by avoiding double loads of the same file.
**/

#ifndef MATERIALS_TEXTURE_POOL_HPP
#define MATERIALS_TEXTURE_POOL_HPP

#include <unordered_map>

#include "rendering/memory_manager/MemoryManager.hpp"

#include "TextureFormat.hpp"
#include "Texture.hpp"

namespace Makma3D::Materials {
    /* The TexturePool class, which is responsible for loading and managing textures. */
    class TexturePool {
    public:
        /* Channel name for the TexturePool class. */
        static constexpr const char* channel = "TexturePool";

        /* The MemoryManager class used for allocating new images and stage buffers and the like. */
        Rendering::MemoryManager& memory_manager;
    
    private:
        /* The list of Textures that we allocated with this pool. */
        std::unordered_map<std::string, Materials::Texture*> textures;

    public:
        /* Constructor for the TexturePool class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
        TexturePool(Rendering::MemoryManager& memory_manager);
        /* Copy constructor for the TexturePool class, which is deleted. */
        TexturePool(const TexturePool& other) = delete;
        /* Move constructor for the TexturePool class. */
        TexturePool(TexturePool&& other);
        /* Destructor for the TexturePool class. */
        ~TexturePool();

        /* Given a string filename/filepath, deduces any of the possible TextureFormats from it. Will throw errors if no such format is found. */
        static TextureFormat deduce_format(const std::string& path);

        /* Allocates a new Texture object, and populates it with the data loaded at the given path as the given format. The filter and by how much to apply anisotropy apply to the texture's sampler. If the given format is "automatic", tries to deduce the format from the file's extension. */
        Materials::Texture* allocate(const std::string& path, VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy_level = 16.0f, TextureFormat format = TextureFormat::automatic);
        /* Destroys the given texture, freeing memory again. */
        void free(const Materials::Texture* texture);

        /* Copy assignment operator for the TexturePool class, which is deleted. */
        TexturePool& operator=(const TexturePool& other) = delete;
        /* Move assignment operator for the TexturePool class. */
        inline TexturePool& operator=(TexturePool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the TexturePool class. */
        friend void swap(TexturePool& ts1, TexturePool& ts2);

    };
    
    /* Swap operator for the TexturePool class. */
    void swap(TexturePool& ts1, TexturePool& ts2);

}

#endif
