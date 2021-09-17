/* SHADER STAGE.hpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 21:52:40
 * Last edited:
 *   17/09/2021, 21:52:40
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderStage struct, which is used to temporarily store
 *   the ShaderStage information until we create a pipeline from it.
**/

#ifndef RENDERING_SHADER_STAGE_HPP
#define RENDERING_SHADER_STAGE_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include "../../shaders/Shader.hpp"

namespace Makma3D::Rendering {
    /* The ShaderStage struct, which stores information about how to fill in a single programmable stage in the Pipeline. */
    struct ShaderStage {
        /* The Shader that we will use at this stage. */
        const Rendering::Shader* shader;
        /* The shader stage in the pipeline which we describe. */
        VkShaderStageFlags vk_shader_stage;
        /* The list of specialization constants that we can use to specialize the shader. */
        std::unordered_map<uint32_t, std::pair<void*, uint32_t>> specialization_constants;

    };

}

#endif
