/* SHADER STAGES.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 15:08:18
 * Last edited:
 *   8/1/2021, 3:38:07 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a map that takes a shader stage and maps it to a nice,
 *   readable name.
**/

#ifndef RENDERING_SHADER_STAGES_HPP
#define RENDERING_SHADER_STAGES_HPP

#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>

namespace Rasterizer::Rendering {
    /* Static map of VkFormats to their respective string representations. */
    static std::unordered_map<VkShaderStageFlagBits, std::string> vk_shader_stage_map({
        { VK_SHADER_STAGE_VERTEX_BIT, "vertex" },
        { VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT , "tesselation (control)" },
        { VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT , "tesselation (evaluation)" },
        { VK_SHADER_STAGE_GEOMETRY_BIT , "geometry" },
        { VK_SHADER_STAGE_FRAGMENT_BIT , "fragment" },
        { VK_SHADER_STAGE_COMPUTE_BIT , "compute" },
        { VK_SHADER_STAGE_ALL_GRAPHICS , "all (graphics)" },

        #ifdef VK_SHADER_STAGE_RAYGEN_BIT_KHR
        { VK_SHADER_STAGE_RAYGEN_BIT_KHR  , "raytracing raygen" },
        #endif
        #ifdef VK_SHADER_STAGE_ANY_HIT_BIT_KHR
        { VK_SHADER_STAGE_ANY_HIT_BIT_KHR  , "raytracing any hit" },
        #endif
        #ifdef VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR
        { VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR  , "raytracing closest hit" },
        #endif
        #ifdef VK_SHADER_STAGE_MISS_BIT_KHR
        { VK_SHADER_STAGE_MISS_BIT_KHR  , "raytracing miss" },
        #endif
        #ifdef VK_SHADER_STAGE_INTERSECTION_BIT_KHR
        { VK_SHADER_STAGE_INTERSECTION_BIT_KHR  , "raytracing intersection" },
        #endif
        #ifdef VK_SHADER_STAGE_CALLABLE_BIT_KHR
        { VK_SHADER_STAGE_CALLABLE_BIT_KHR  , "raytracing callable" },
        #endif
        
        { VK_SHADER_STAGE_TASK_BIT_NV   , "mesh task" },
        { VK_SHADER_STAGE_MESH_BIT_NV   , "mesh mesh" }
    });
}

#endif
