/* SHADER STAGE.hpp
 *   by Lut99
 *
 * Created:
 *   28/06/2021, 21:34:37
 * Last edited:
 *   28/06/2021, 22:14:00
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderStage class, which is used internally by the
 *   Pipeline to properly manage shader memory.
**/

#ifndef RENDERING_SHADER_STAGE_HPP
#define RENDERING_SHADER_STAGE_HPP

#include <unordered_map>
#include <vulkan/vulkan.h>

#include "../auxillary/BinaryString.hpp"
#include "Shader.hpp"

namespace Rasterizer::Rendering {
    /* The ShaderStage class, which manages memory for a single shader and its affiliated data in the Pipeline. */
    class ShaderStage {
    private:
        /* The VkPipelineShaderStageCreateInfo that we in effect wrap. */
        VkPipelineShaderStageCreateInfo vk_shader_stage;

        /* The shader that this object is bound to. */
        const Shader& shader;

        /* Struct describing all the specialization constants together. */
        VkSpecializationInfo* vk_specialization_info;
        /* Map entries for the specialization constants. */
        VkSpecializationMapEntry* specialization_entries;
        /* The number of entries. */
        uint32_t n_entries;
        /* Binary string representing the values of the specialization constants. */
        uint8_t* specialization_data;
        /* The size of the binary string of data. */
        size_t specialization_data_size;

    public:
        /* Constructor for the ShaderStage, which takes the Shader to load, where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
        ShaderStage(const Rendering::Shader& shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, Rendering::BinaryString>& specialization_constants = {});
        /* Copy constructor for the ShaderStage. */
        ShaderStage(const ShaderStage& other);
        /* Move constructor for the ShaderStage. */
        ShaderStage(ShaderStage&& other);
        /* Destructor for the ShaderStage. */
        ~ShaderStage();

        /* Explicitly returns the internal VkPipelineShaderStageCreateInfo object. */
        inline const VkPipelineShaderStageCreateInfo& shader_stage() const { return this->vk_shader_stage; }
        /* Implicitly returns the internal VkPipelineShaderStageCreateInfo object. */
        inline operator const VkPipelineShaderStageCreateInfo&() const { return this->vk_shader_stage; }

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
