/* SHADER STAGE.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:23:52
 * Last edited:
 *   11/09/2021, 14:23:52
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderStage class, which serves as a property or aspect
 *   of a Pipeline. Basically wraps a normal Shader object, and can be used
 *   to tell the Pipeline where that object should be bound and if any push
 *   constants need to be set.
**/

#ifndef RENDERING_SHADER_STAGE_HPP
#define RENDERING_SHADER_STAGE_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "../../shaders/Shader.hpp"

namespace Makma3D::Rendering {
    /* The ShaderStage class, which functions as a property/group of properties for a Pipeline about a single shader stage. */
    class ShaderStage {
    public:
        /* Channel name for the ShaderStage class. */
        static constexpr const char* channel = "ShaderStage";

    public:
        /* The Shader this stage wraps. */
        const Rendering::Shader* shader;
        /* The shader stage for the shader. */
        VkShaderStageFlagBits shader_stage;
        /* The map of specialization constants. */
        std::unordered_map<uint32_t, std::pair<void*, uint32_t>> specialization_constants;

    public:
        /* Default constructor for the ShaderStage class. */
        ShaderStage();
        /* Constructor for the ShaderStage, which takes the Shader to load, where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
        ShaderStage(const Rendering::Shader* shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& specialization_constants = {});

        /* Flattens the internal map of specialization constants into an array of specialization map entries. */
        Tools::Array<VkSpecializationMapEntry> flatten_specialization_entries() const;
        /* Flattens the internal map of specialization constants into a single string of binary data. Requires the list of VkSpecializationMapEntries to do this. */
        std::pair<void*, size_t> flatten_specialization_values(const Tools::Array<VkSpecializationMapEntry>& entries) const;
        /* Given a flattened list of entries and of values, returns a new VkSpecializationInfo struct. */
        VkSpecializationInfo create_specialization_info(const Tools::Array<VkSpecializationMapEntry>& entries, const std::pair<void*, size_t>& data) const;
        /* Given a VkSpecializationInfo struct, returns a VkPipelineShaderStageCreateInfo struct that describes the shader stage properly for use in a Pipeline. */
        VkPipelineShaderStageCreateInfo create_info(const VkSpecializationInfo& specialization_info) const;

    };

}

#endif
