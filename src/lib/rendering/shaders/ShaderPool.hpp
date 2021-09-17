/* SHADER POOL.hpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 22:01:06
 * Last edited:
 *   17/09/2021, 22:01:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderPool class, which is a centralized place to load
 *   shaders and then refer to them later. Prevents unnecessary copying and
 *   junk.
**/

#ifndef RENDERING_SHADER_POOL_HPP
#define RENDERING_SHADER_POOL_HPP

#include <string>
#include <unordered_set>
#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

#include "Shader.hpp"

namespace Makma3D::Rendering {
    /* The ShaderPool class, which loads and manages Shaders in a centralized place to avoid unecessary copies. */
    class ShaderPool {
    public:
        /* Channel name for the ShaderPool class. */
        static constexpr const char* channel = "ShaderPool";

        /* The GPU where all the allocated Shaders will live. */
        const Rendering::GPU& gpu;

    private:
        /* List of internally allocated Shader objects. */
        std::unordered_set<Rendering::Shader*> shaders;
    
    public:
        /* Constructor for the ShaderPool class, which takes the GPU where the Shaders will live. */
        ShaderPool(const Rendering::GPU& gpu);
        /* Copy constructor for the ShaderPool class, which is deleted. */
        ShaderPool(const ShaderPool& other) = delete;
        /* Move constructor for the ShaderPool class. */
        ShaderPool(ShaderPool&& other);
        /* Destructor for the ShaderPool class. */
        ~ShaderPool();

        /* Allocates a new Shader, loaded from the given location as a .spv. Optionally, create flags for the VkShaderModule can be set. */
        Rendering::Shader* allocate(const std::string& path, VkShaderModuleCreateFlags create_flags = 0);
        /* Frees the given Shader from the internal pool. Throws errors if it isn't allocated here. */
        void free(const Rendering::Shader* shader);

        /* Resets the pool, deleting all internal shaders and their shader stages. */
        void reset();

        /* Copy assignment operator for the ShaderPool class, which is deleted. */
        ShaderPool& operator=(const ShaderPool& other) = delete;
        /* Move assignment operator for the ShaderPool class. */
        inline ShaderPool& operator=(ShaderPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ShaderPool class. */
        friend void swap(ShaderPool& sp1, ShaderPool& sp2);

    };

    /* Swap operator for the ShaderPool class. */
    void swap(ShaderPool& sp1, ShaderPool& sp2);

}

#endif
