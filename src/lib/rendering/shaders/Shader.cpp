/* SHADER.cpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 22:07:48
 * Last edited:
 *   17/09/2021, 22:07:48
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Shader class, which wraps a single VkShaderModule object
 *   allocated with the ShaderPool.
**/

#include "Shader.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** SHADER CLASS *****/
/* Constructor for the Shader class, which takes its GPU, the VkShaderModule to wrap and the path where it is from (for debugging purposes). */
Shader::Shader(const Rendering::GPU& gpu, const VkShaderModule& vk_shader_module, const std::string& filepath) :
    gpu(gpu),
    vk_shader_module(vk_shader_module),
    filepath(filepath)
{}

/* Destructor for the Shader class, which is private. */
Shader::~Shader() {}
