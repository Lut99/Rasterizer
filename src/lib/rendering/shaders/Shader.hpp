/* SHADER.hpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 22:07:46
 * Last edited:
 *   17/09/2021, 22:07:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Shader class, which wraps a single VkShaderModule object
 *   allocated with the ShaderPool.
**/

#ifndef RENDERING_SHADER_HPP
#define RENDERING_SHADER_HPP

#include <string>
#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

namespace Makma3D::Rendering {
    /* The Shader class, which wraps a single VkShaderModule and is managed by the ShaderPool class. */
    class Shader {
    public:
        /* The Channel name of the Shader class. */
        static constexpr const char* channel = "Shader";

        /* The GPU where the Shader lives. */
        const Rendering::GPU& gpu;
    
    private:
        /* The VkShaderModule that we wrap. */
        VkShaderModule vk_shader_module;
        /* The path where the Shader was loaded from. */
        std::string filepath;

        /* Constructor for the Shader class, which takes its GPU, the VkShaderModule to wrap and the path where it is from (for debugging purposes). */
        Shader(const Rendering::GPU& gpu, const VkShaderModule& vk_shader_module, const std::string& filepath);
        /* Destructor for the Shader class, which is private. */
        ~Shader();

        /* Declare the ShaderPool as our fwiend. */
        friend class ShaderPool;
    
    public:
        /* Copy constructor for the Shader class, which is deleted. */
        Shader(const Shader& other) = delete;
        /* Move constructor for the Shader class, which is deleted. */
        Shader(Shader&& other) = delete;

        /* Returns the path of the shader. */
        inline const std::string& path() const { return this->filepath; }
        /* Explicitly returns the internal VkShaderModule object. */
        inline const VkShaderModule& vulkan() const { return this->vk_shader_module; }
        /* Implicitly returns the internal VkShaderModule object. */
        inline operator const VkShaderModule&() const { return this->vk_shader_module; }

        /* Copy assignment operator for the Shader class, which is deleted. */
        Shader& operator=(const Shader& other) = delete;
        /* Move assignment operator for the Shader class, which is deleted. */
        Shader& operator=(Shader&& other) = delete;

    };

}

#endif
