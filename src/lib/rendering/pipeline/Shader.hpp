/* SHADER.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:24:32
 * Last edited:
 *   11/09/2021, 14:24:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Shader class, which is used to load a .spv shader file
 *   from disk.
**/

#ifndef RENDERING_SHADER_HPP
#define RENDERING_SHADER_HPP

#include <string>
#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

namespace Makma3D::Rendering {
    /* The Shader class, which is used to load shader .spv files from disk. */
    class Shader {
    public:
        /* Channel name for the Shader class. */
        static constexpr const char* channel = "Shader";

        /* Reference to the GPU where the Shader code lives. */
        const Rendering::GPU& gpu;
    
    private:
        /* The VkShaderModule object wrapped by the Shader. */
        VkShaderModule vk_shader_module;
        /* The size of the shader as a number of bytes. */
        size_t n_bytes;
        /* The create flags for the shader module. */
        VkShaderModuleCreateFlags vk_create_flags;

        /* The raw shader data. */
        uint32_t* shader_data;
        /* The size of the shader data array, in elements. */
        uint32_t shader_data_size;

        /* The name of the shader, used for debugging only. */
        std::string name;

    public:
        /* Constructor for the Shader class, which takes a GPU where the Shader will live, the filename of the file to load and optionally some create flags. */
        Shader(const Rendering::GPU& gpu, const std::string& filename, VkShaderModuleCreateFlags create_flags = 0);
        /* Copy constructor for the Shader class. */
        Shader(const Shader& other);
        /* Move constructor for the Shader class. */
        Shader(Shader&& other);
        /* Destructor for the Shader class. */
        ~Shader();

        /* Explicitly returns the internal VkShaderModule object. */
        inline const VkShaderModule& shader_module() const { return this->vk_shader_module; }
        /* Implicitly returns the internal VkShaderModule object. */
        inline operator const VkShaderModule&() const { return this->vk_shader_module; }

        /* Copy assignment operator for the Shader class. */
        inline Shader& operator=(const Shader& other) { return *this = Shader(other); }
        /* Move assignment operator for the Shader class. */
        inline Shader& operator=(Shader&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Shader class. */
        friend void swap(Shader& s1, Shader& s2);

    };

    /* Swap operator for the Shader class. */
    void swap(Shader& s1, Shader& s2);

}

#endif
