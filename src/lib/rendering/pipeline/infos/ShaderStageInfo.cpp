/* SHADER STAGE INFO.cpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 14:09:52
 * Last edited:
 *   9/20/2021, 8:00:16 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderStageInfo class, which wraps a
 *   VkPipelineShaderStageCreateInfo struct and all associated lists, and
 *   manages them memory-wise.
**/

#include "ShaderStageInfo.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSpecializationInfo struct with the given entries and constant data. */
static void populate_specialization_info(VkSpecializationInfo& specialization_info, const VkSpecializationMapEntry* vk_entries, uint32_t vk_entries_size, const void* vk_data, size_t vk_data_size) {
    // Initialize to default
    specialization_info = {};

    // Add the entries
    specialization_info.mapEntryCount = vk_entries_size;
    specialization_info.pMapEntries = vk_entries;

    // Add the values
    specialization_info.dataSize = vk_data_size;
    specialization_info.pData = vk_data;
}

/* Populates the given VkPipelineShaderStageCreateInfo struct with the given shader module, shader stage and specialization info. */
static void populate_shader_stage_info(VkPipelineShaderStageCreateInfo& shader_stage_info, VkShaderModule vk_shader_module, VkShaderStageFlagBits vk_shader_stage, const VkSpecializationInfo* vk_specialization_info) {
    // Initialize to default
    shader_stage_info = {};
    shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    // Set the shader module
    shader_stage_info.module = vk_shader_module;
    shader_stage_info.pName = "main";

    // Set the shader stage
    shader_stage_info.stage = vk_shader_stage;

    // Finally, set the specialization info
    shader_stage_info.pSpecializationInfo = vk_specialization_info;
}





/***** SHADERSTAGEINFO CLASS *****/
/* Constructor for the ShaderStageInfo class, which takes the ShaderStage object to construct the Vulkan counterparts from. */
ShaderStageInfo::ShaderStageInfo(const Rendering::ShaderStage& shader_stage) {
    // We know the number of entries, so start by allocating the entries array
    this->vk_entries_size = static_cast<uint32_t>(shader_stage.specialization_constants.size());
    this->vk_entries = new VkSpecializationMapEntry[this->vk_entries_size];

    // Next, loop through the map of specialization constants to populate the arrays - noting the offsets while we're at it
    uint32_t offset = 0;
    uint32_t i = 0;
    for (const auto& constant : shader_stage.specialization_constants) {
        // Initialize the entry to default
        this->vk_entries[i] = {};

        // Populate it with the constantID, the value and the offset
        this->vk_entries[i].constantID = constant.first;
        this->vk_entries[i].offset = offset;
        this->vk_entries[i].size = constant.second.second;

        // Update the offset and then the counter before we continue
        offset += constant.second.second;
        ++i;
    }

    // The value is now the total size of all constant values, so use it to allocate the data string
    this->vk_data_size = static_cast<size_t>(offset);
    this->vk_data = (void*) new uint8_t[this->vk_data_size];

    // Loop through the entries now, copying their values to the data string
    for (uint32_t i = 0; i < this->vk_entries_size; i++) {
        // Simply copy to the correct offset
        memcpy((void*) ((uint8_t*) this->vk_data + this->vk_entries[i].offset), shader_stage.specialization_constants.at(this->vk_entries[i].constantID).first, this->vk_entries[i].size);
    }

    // With the entries and the constant value string prepared, we can create the specialization info
    this->vk_specialization_info = new VkSpecializationInfo();
    populate_specialization_info(*this->vk_specialization_info, this->vk_entries, this->vk_entries_size, this->vk_data, this->vk_data_size);

    // Finally, we can use that to populate the shader stage info itself
    populate_shader_stage_info(this->vk_shader_stage_info, shader_stage.shader->vulkan(), shader_stage.shader_stage, this->vk_specialization_info);
}

/* Copy constructor for the ShaderStageInfo class. */
ShaderStageInfo::ShaderStageInfo(const ShaderStageInfo& other) :
    vk_entries_size(other.vk_entries_size),
    vk_data_size(other.vk_data_size),
    vk_shader_stage_info(other.vk_shader_stage_info)
{
    // Copy the entries
    this->vk_entries = new VkSpecializationMapEntry[this->vk_entries_size];
    memcpy(this->vk_entries, other.vk_entries, this->vk_entries_size * sizeof(VkSpecializationMapEntry));

    // Copy the data
    this->vk_data = (void*) new uint8_t[this->vk_data_size];
    memcpy(this->vk_data, other.vk_data, this->vk_data_size);

    // Copy the specialization info
    this->vk_specialization_info = new VkSpecializationInfo(*other.vk_specialization_info);
    this->vk_specialization_info->pMapEntries = this->vk_entries;
    this->vk_specialization_info->pData = this->vk_data;

    // Finally, update the shader stage info's pointer
    this->vk_shader_stage_info.pSpecializationInfo = this->vk_specialization_info;
}

/* Move constructor for the ShaderStageInfo class. */
ShaderStageInfo::ShaderStageInfo(ShaderStageInfo&& other) :
    vk_entries(other.vk_entries),
    vk_entries_size(other.vk_entries_size),

    vk_data(other.vk_data),
    vk_data_size(other.vk_data_size),

    vk_specialization_info(other.vk_specialization_info),
    vk_shader_stage_info(other.vk_shader_stage_info)
{
    // Prevent the lot from being deallocated
    other.vk_entries = nullptr;
    other.vk_data = nullptr;
    other.vk_specialization_info = nullptr;
}

/* Destructor for the ShaderStageInfo class. */
ShaderStageInfo::~ShaderStageInfo() {
    if (this->vk_specialization_info != nullptr) {
        delete this->vk_specialization_info;
    }
    if (this->vk_data != nullptr) {
        delete[] (uint8_t*) this->vk_data;
    }
    if (this->vk_entries != nullptr) {
        delete[] this->vk_entries;
    }
}



/* Swap operator for the ShaderStageInfo class. */
void Rendering::swap(ShaderStageInfo& ssi1, ShaderStageInfo& ssi2) {
    using std::swap;

    swap(ssi1.vk_entries, ssi2.vk_entries);
    swap(ssi1.vk_entries_size, ssi2.vk_entries_size);
    
    swap(ssi1.vk_data, ssi2.vk_data);
    swap(ssi1.vk_data_size, ssi2.vk_data_size);
    
    swap(ssi1.vk_specialization_info, ssi2.vk_specialization_info);
    swap(ssi1.vk_shader_stage_info, ssi2.vk_shader_stage_info);
}
