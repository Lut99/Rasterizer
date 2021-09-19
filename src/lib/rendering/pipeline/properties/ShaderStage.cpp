/* SHADER STAGE.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:59:17
 * Last edited:
 *   11/09/2021, 14:59:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/Logger.hpp"

#include "ShaderStage.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** SHADERSTAGE CLASS *****/
/* Default constructor for the ShaderStage class. */
ShaderStage::ShaderStage() :
    ShaderStage(nullptr, VK_SHADER_STAGE_ALL_GRAPHICS, {})
{}

/* Constructor for the ShaderStage, which takes the Shader to load, where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
ShaderStage::ShaderStage(const Rendering::Shader* shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& specialization_constants) :
    shader(shader),
    shader_stage(shader_stage),
    specialization_constants(specialization_constants)
{}



/* Flattens the internal map of specialization constants into an array of specialization map entries. */
Tools::Array<VkSpecializationMapEntry> ShaderStage::flatten_specialization_entries() const {
    // We already know the size of the array, so allocate it
    Tools::Array<VkSpecializationMapEntry> result(static_cast<uint32_t>(this->specialization_constants.size()));

    // Loop through the array to construct all of the entries
    uint32_t offset = 0;
    std::unordered_map<uint32_t, std::pair<void*, uint32_t>>::const_iterator iter = this->specialization_constants.begin();
    for (uint32_t i = 0; iter != this->specialization_constants.end(); ++i, ++iter) {
        // Initialize the entry to an empty array
        result.push_back({});

        // Populate the constantID, offset and size for this entry
        result[i].constantID = (*iter).first;
        result[i].offset = offset;
        result[i].size = (*iter).second.second;

        // Update the offset to jump after this entry
        offset += static_cast<uint32_t>(result[i].size);
    }

    // DOne, return
    return result;
}

/* Flattens the internal map of specialization constants into a single string of binary data. Requires the list of VkSpecializationMapEntries to do this. */
std::pair<void*, size_t> ShaderStage::flatten_specialization_values(const Tools::Array<VkSpecializationMapEntry>& entries) const {
    // First, do a quick search of the total data size
    size_t data_size = 0;
    for (uint32_t i = 0; i < entries.size(); i++) {
        data_size += entries[i].size;
    }

    // Next, allocate the array for the data
    void* data = (void*) new uint8_t[data_size];

    // Populate the array by copying all the data over, one-by-one
    for (uint32_t i = 0; i < entries.size(); i++) {
        memcpy((void*) ((uint8_t*) data + entries[i].offset), this->specialization_constants.at(entries[i].constantID).first, entries[i].size);
    }

    // D0ne
    return { data, data_size };
}

/* Given a flattened list of entries and of values, returns a new VkSpecializationInfo struct. */
VkSpecializationInfo ShaderStage::create_specialization_info(const Tools::Array<VkSpecializationMapEntry>& entries, const std::pair<void*, size_t>& data) const {
    // Create the struct
    VkSpecializationInfo specialization_info{};

    // Add the entries
    specialization_info.mapEntryCount = entries.size();
    specialization_info.pMapEntries = entries.rdata();

    // Add the values
    specialization_info.dataSize = data.second;
    specialization_info.pData = data.first;

    // Return the struct
    return specialization_info;
}

/* Given a VkSpecializationInfo struct, returns a VkPipelineShaderStageCreateInfo struct that describes the shader stage properly for use in a Pipeline. */
VkPipelineShaderStageCreateInfo ShaderStage::create_info(const VkSpecializationInfo& specialization_info) const {
    #ifndef NDEBUG
    // Make sure the shader has been given a value
    if (this->shader == nullptr) { logger.fatalc(ShaderStage::channel, "Cannot create shader stage info without a shader being set."); }
    #endif

    // Create the struct to default
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    // Add the module of the internal shader
    info.module = this->shader->vulkan();
    info.pName = "main";
    // Set the shader stage
    info.stage = this->shader_stage;
    // Finally, add the specialization info
    info.pSpecializationInfo = &specialization_info;

    // Return the struct
    return info;
}
