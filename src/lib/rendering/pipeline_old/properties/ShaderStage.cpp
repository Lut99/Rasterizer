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


/***** HELPER FUNCTIONS *****/
/* Given an unordered map of constants, populates an array of VkSpecializationMapEntry's for those constants. Returns the total size of the raw constant data combined. The array is expecited to have space for at least one entry per entry in the constant map. */
static size_t generate_specialization_entries(const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& constant_map, VkSpecializationMapEntry* map_entries) {
    // Create an array of mapentries, and then a single, unified data array
    uint32_t offset = 0;
    uint32_t i = 0;
    for(const std::pair<uint32_t, std::pair<void*, uint32_t>>& p : constant_map) {
        // Start to intialize a map entry
        map_entries[i] = {};

        // Set the constant ID for this entry, together with the offset & size of its data
        map_entries[i].constantID = p.first;
        map_entries[i].offset = offset;
        map_entries[i].size = p.second.second;

        // Increment the offset & counter
        offset += p.second.second;
        ++i;
    }

    // Done, return the slightly longer version of the offset as size
    return static_cast<size_t>(offset);
}

/* Given an unordered map of constants, populates a data array containing all the constants's data. The array is expected to have the minimally required space (as returned by generate_specialization_entries()). */
static void copy_specialization_data(const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& constant_map, uint8_t* data) {
    // The offset is now also the size. Use that to populate the data array
    size_t offset = 0;
    for (const std::pair<uint32_t, std::pair<void*, uint32_t>>& p : constant_map) {
        // Copy the element's data to the array
        memcpy((void*) (data + offset), p.second.first, static_cast<size_t>(p.second.second));

        // Increment the offset once more
        offset += static_cast<size_t>(p.second.second);
    }
}





/***** POPULATE FUNCTIONS *****/
/* Function that populates a VkSpecializationInfo struct based on the given map of constant_ids to values. */
static void populate_specialization_info(VkSpecializationInfo& specialization_info, VkSpecializationMapEntry* map_entries, uint32_t n_entries, uint8_t* data, size_t data_size) {
    // Initialize to default
    specialization_info = {};
    
    // Set the specialization map
    specialization_info.mapEntryCount = n_entries;
    specialization_info.pMapEntries = map_entries;

    // Set the data array
    specialization_info.dataSize = data_size;
    specialization_info.pData = (void*) data;
}

/* Populates the given VkPipelineShaderStageCreateInfo struct. */
static void populate_stage_info(VkPipelineShaderStageCreateInfo& stage_info, const Shader& shader, VkShaderStageFlagBits shader_stage, const VkSpecializationInfo& specialization_info) {
    // Set to default
    stage_info = {};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    // Add the shader module and its entry function
    stage_info.module = shader.shader_module();
    stage_info.pName = "main";

    // Set the shader's stage
    stage_info.stage = shader_stage;

    // Finally, set the specialization info for this shader
    stage_info.pSpecializationInfo = &specialization_info;
}





/***** SHADERSTAGE CLASS *****/
/* Constructor for the ShaderStage, which takes the Shader to load (copying it), where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
ShaderStage::ShaderStage(const Rendering::Shader& shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& specialization_constants) :
    ShaderStage(Shader(shader), shader_stage, specialization_constants)
{}

/* Constructor for the ShaderStage, which takes the Shader to load (without copying it), where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
ShaderStage::ShaderStage(Rendering::Shader&& shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, std::pair<void*, uint32_t>>& specialization_constants):
    shader(std::move(shader))
{
    // Start by allocating enough specialization entries and flatten the specialization constants into those entries
    this->vk_specialization_entries_size = static_cast<uint32_t>(specialization_constants.size());
    this->vk_specialization_entries = new VkSpecializationMapEntry[this->vk_specialization_entries_size];
    this->specialization_data_size = generate_specialization_entries(specialization_constants, this->vk_specialization_entries);

    // Next, we know the total size of the binary string, so we can allocate it and then populate it
    this->specialization_data = new uint8_t[this->specialization_data_size];
    copy_specialization_data(specialization_constants, this->specialization_data);

    // With the entries and the data ready, populate the internal specialization info struct
    populate_specialization_info(this->vk_specialization_info, this->vk_specialization_entries, this->vk_specialization_entries_size, this->specialization_data, this->specialization_data_size);

    // Next, move to create the actual shader stage info
    populate_stage_info(this->vk_shader_stage_info, this->shader, shader_stage, this->vk_specialization_info);
}

/* Copy constructor for the ShaderStage. */
ShaderStage::ShaderStage(const ShaderStage& other) :
    shader(other.shader),

    vk_specialization_entries_size(other.vk_specialization_entries_size),

    specialization_data_size(other.specialization_data_size)
{
    // First, copy the entries over
    this->vk_specialization_entries = new VkSpecializationMapEntry[this->vk_specialization_entries_size];
    memcpy(this->vk_specialization_entries, other.vk_specialization_entries, this->vk_specialization_entries_size * sizeof(VkSpecializationMapEntry));

    // Next, copy the specialization data
    this->specialization_data = new uint8_t[this->specialization_data_size];
    memcpy(this->specialization_data, other.specialization_data, this->specialization_data_size * sizeof(uint8_t));

    // When done, re-initialize the specialization info and then the stage info
    populate_specialization_info(this->vk_specialization_info, this->vk_specialization_entries, this->vk_specialization_entries_size, this->specialization_data, this->specialization_data_size);
    populate_stage_info(this->vk_shader_stage_info, this->shader, other.vk_shader_stage_info.stage, this->vk_specialization_info);
}

/* Move constructor for the ShaderStage. */
ShaderStage::ShaderStage(ShaderStage&& other) :
    shader(std::move(other.shader)),

    vk_specialization_entries(other.vk_specialization_entries),
    vk_specialization_entries_size(other.vk_specialization_entries_size),
    
    specialization_data(other.specialization_data),
    specialization_data_size(other.specialization_data_size)
{
    // Prevent the other from deallocating the lists
    other.vk_specialization_entries = nullptr;
    other.specialization_data = nullptr;

    // Do re-initialize the structs
    populate_specialization_info(this->vk_specialization_info, this->vk_specialization_entries, this->vk_specialization_entries_size, this->specialization_data, this->specialization_data_size);
    populate_stage_info(this->vk_shader_stage_info, this->shader, other.vk_shader_stage_info.stage, this->vk_specialization_info);
}

/* Destructor for the ShaderStage. */
ShaderStage::~ShaderStage() {
    // Deallocate the lists necessary
    if (this->specialization_data != nullptr) {
        delete[] this->specialization_data;
    }
    if (this->vk_specialization_entries != nullptr) {
        delete[] this->vk_specialization_entries;
    }
}



/* Swap operator for the ShaderStage class. */
void Rendering::swap(ShaderStage& ss1, ShaderStage& ss2) {
    using std::swap;

    swap(ss1.shader, ss2.shader);

    swap(ss1.vk_specialization_entries, ss2.vk_specialization_entries);
    swap(ss1.vk_specialization_entries_size, ss2.vk_specialization_entries_size);
    
    swap(ss1.specialization_data, ss2.specialization_data);
    swap(ss1.specialization_data_size, ss2.specialization_data_size);

    swap(ss1.vk_specialization_info, ss2.vk_specialization_info);
    swap(ss1.vk_shader_stage_info, ss2.vk_shader_stage_info);
}
