/* SHADER STAGE INFO.hpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 14:09:25
 * Last edited:
 *   19/09/2021, 14:09:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderStageInfo class, which wraps a
 *   VkPipelineShaderStageCreateInfo struct and all associated lists, and
 *   manages them memory-wise.
**/

#ifndef RENDERING_SHADER_STAGE_INFO_HPP
#define RENDERING_SHADER_STAGE_INFO_HPP

#include <vulkan/vulkan.h>
#include "../properties/ShaderStage.hpp"

namespace Makma3D::Rendering {
    /* The ShaderStageInfo class, which wraps a VkPipelineShaderStageCreateInfo and associated memory. */
    class ShaderStageInfo {
    private:
        /* The list of VkSpecializationMapEntries that is used to identify the specialization constants. */
        VkSpecializationMapEntry* vk_entries;
        /* The number of VkSpecializationMapEntries. */
        uint32_t vk_entries_size;

        /* The raw data string that we use to store the values for the constants. */
        void* vk_data;
        /* The size (in bytes) of the raw data string. */
        size_t vk_data_size;

        /* The VkSpecializationInfo struct we use for the stage info. */
        VkSpecializationInfo* vk_specialization_info;
        /* The VkPipelineShaderStageCreateInfo we wrap. */
        VkPipelineShaderStageCreateInfo vk_shader_stage_info;
    
    public:
        /* Constructor for the ShaderStageInfo class, which takes the ShaderStage object to construct the Vulkan counterparts from. */
        ShaderStageInfo(const Rendering::ShaderStage& shader_stage);
        /* Copy constructor for the ShaderStageInfo class. */
        ShaderStageInfo(const ShaderStageInfo& other);
        /* Move constructor for the ShaderStageInfo class. */
        ShaderStageInfo(ShaderStageInfo&& other);
        /* Destructor for the ShaderStageInfo class. */
        ~ShaderStageInfo();

        /* Expliticly returns the internal VkPipelineShaderStageCreateInfo object. */
        inline const VkPipelineShaderStageCreateInfo& vulkan() const { return this->vk_shader_stage_info; }
        /* Implicitly returns the internal VkPipelineShaderStageCreateInfo object. */
        inline operator const VkPipelineShaderStageCreateInfo&() const { return this->vk_shader_stage_info; }

        /* Copy assignment operator for the ShaderStageInfo class. */
        inline ShaderStageInfo& operator=(const ShaderStageInfo& other) { return *this = ShaderStageInfo(other); }
        /* Move assignment operator for the ShaderStageInfo class. */
        inline ShaderStageInfo& operator=(ShaderStageInfo&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ShaderStageInfo class. */
        friend void swap(ShaderStageInfo& ssi1, ShaderStageInfo& ssi2);

    };

    /* Swap operator for the ShaderStageInfo class. */
    void swap(ShaderStageInfo& ssi1, ShaderStageInfo& ssi2);

}

#endif
