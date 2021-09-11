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

#include "../Shader.hpp"

namespace Makma3D::Rendering {
    /* The ShaderStage class, which functions as a property/group of properties for a Pipeline about a single shader stage. */
    class ShaderStage {
    public:
        /* Channel name for the ShaderStage class. */
        static constexpr const char* channel = "ShaderStage";

    private:
        /* The Shader this stage wraps. */
        Rendering::Shader shader;

        /* The list of specialization entries for this shader. */
        VkSpecializationMapEntry* vk_specialization_entries;
        /* The number of entries. */
        uint32_t vk_specialization_entries_size;
        
        /* A raw, binary string with specialization data. */
        uint8_t* specialization_data;
        /* The size (in bytes) of the specialization data. */
        size_t specialization_data_size;
        
        /* The info merging all the entries above into one place. */
        VkSpecializationInfo vk_specialization_info;
        /* The VkPipelineShaderStageCreateInfo that holds more properties about the shader module. */
        VkPipelineShaderStageCreateInfo vk_shader_stage_info;
    
    public:
        /* Constructor for the ShaderStage, which takes the Shader to load, where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
        ShaderStage(const Rendering::Shader& shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& specialization_constants = {});
        /* Copy constructor for the ShaderStage. */
        ShaderStage(const ShaderStage& other);
        /* Move constructor for the ShaderStage. */
        ShaderStage(ShaderStage&& other);
        /* Destructor for the ShaderStage. */
        ~ShaderStage();

        /* Explicitly returns the internal VkPipelineShaderStageCreateInfo object. */
        inline const VkPipelineShaderStageCreateInfo& info() const { return this->vk_shader_stage_info; }
        /* Implicitly returns the internal VkPipelineShaderStageCreateInfo object. */
        inline operator const VkPipelineShaderStageCreateInfo&() const { return this->vk_shader_stage_info; }

        /* Copy assignment operator for the ShaderStage class. */
        inline ShaderStage& operator=(const ShaderStage& other) { return *this = ShaderStage(other); }
        /* Move assignment operator for the ShaderStage class. */
        inline ShaderStage& operator=(ShaderStage&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ShaderStage class. */
        friend void swap(ShaderStage& ss1, ShaderStage& ss2);

    };

    /* Swap operator for the ShaderStage class. */
    void swap(ShaderStage& ss1, ShaderStage& ss2);

}

#endif
