/* SHADER STAGE.cpp
 *   by Lut99
 *
 * Created:
 *   28/06/2021, 21:34:32
 * Last edited:
 *   28/06/2021, 22:21:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ShaderStage class, which is used internally by the
 *   Pipeline to properly manage shader memory.
**/

#include "tools/CppDebugger.hpp"
#include "render_engine/auxillary/ErrorCodes.hpp"

#include "ShaderStage.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Given an unordered map of constants, populates an array of VkSpecializationMapEntry's for those constants. Returns the total size of the raw constant data combined. The array is expecited to have space for at least one entry per entry in the constant map. */
static size_t generate_specialization_entries(const std::unordered_map<uint32_t, BinaryString>& constant_map, VkSpecializationMapEntry* map_entries) {
    DENTER("generate_specialization_entries");

    // Create an array of mapentries, and then a single, unified data array
    uint32_t offset = 0;
    uint32_t i = 0;
    for(const std::pair<uint32_t, BinaryString>& p : constant_map) {
        // Start to intialize a map entry
        map_entries[i] = {};

        // Set the constant ID for this entry, together with the offset & size of its data
        map_entries[i].constantID = p.first;
        map_entries[i].offset = offset;
        map_entries[i].size = p.second.size;

        // Increment the offset & counter
        offset += p.second.size;
        ++i;
    }

    // Done, return the slightly longer version of the offset as size
    DRETURN static_cast<size_t>(offset);
}

/* Given an unordered map of constants, populates a data array containing all the constants's data. The array is expected to have the minimally required space (as returned by generate_specialization_entries()). */
static void copy_specialization_data(const std::unordered_map<uint32_t, BinaryString>& constant_map, uint8_t* data) {
    DENTER("generate_specialization_data");

    // The offset is now also the size. Use that to populate the data array
    size_t offset = 0;
    for (const std::pair<uint32_t, BinaryString>& p : constant_map) {
        // Copy the element's data to the array
        memcpy((void*) (data + offset), p.second.data, static_cast<size_t>(p.second.size));

        // Increment the offset once more
        offset += static_cast<size_t>(p.second.size);
    }

    // Done
    DRETURN;
}





/***** POPULATE FUNCTIONS *****/
/* Function that populates a VkSpecializationInfo struct based on the given map of constant_ids to values. */
static void populate_specialization_info(VkSpecializationInfo& specialization_info, VkSpecializationMapEntry* map_entries, uint32_t n_entries, uint8_t* data, size_t data_size) {
    DENTER("populate_specialization_info");

    // Initialize to default
    specialization_info = {};
    
    // Set the specialization map
    specialization_info.mapEntryCount = n_entries;
    specialization_info.pMapEntries = map_entries;

    // Set the data array
    specialization_info.dataSize = data_size;
    specialization_info.pData = (void*) data;

    // Done
    DRETURN;
}

/* Populates the given VkPipelineShaderStageCreateInfo struct. */
static void populate_shader_info(VkPipelineShaderStageCreateInfo& shader_info, const Shader& shader, VkShaderStageFlagBits shader_stage, const VkSpecializationInfo& specialization_info) {
    DENTER("populate_shader_info");

    // Set to default
    shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    // Add the shader module and its entry function
    shader_info.module = shader.shader_module();
    shader_info.pName = shader.entry_function().c_str();

    // Set the shader's stage
    shader_info.stage = shader_stage;

    // Finally, set the specialization info for this shader
    shader_info.pSpecializationInfo = &specialization_info;

    DRETURN;
}





/***** SHADERSTAGE CLASS *****/
/* Constructor for the ShaderStage, which takes the Shader to load, where we should place it (its VkShaderStage) and optionally a map of specialization constants. */
ShaderStage::ShaderStage(const Rendering::Shader& shader, VkShaderStageFlagBits shader_stage, const std::unordered_map<uint32_t, Rendering::BinaryString>& specialization_constants) :
    shader(shader),
    specialization_entries(new VkSpecializationMapEntry[specialization_constants.size()]),
    n_entries(static_cast<uint32_t>(specialization_constants.size()))
{
    DENTER("Rendering::ShaderStage::ShaderStage");

    // First, populate the list of specialization entries
    this->specialization_data_size = generate_specialization_entries(specialization_constants, this->specialization_entries);
    // Next, retrieve the data stored in the dict in one array
    this->specialization_data = new uint8_t[this->specialization_data_size];
    copy_specialization_data(specialization_constants, this->specialization_data);

    // Next, use that data to populate the specialization info
    this->vk_specialization_info = new VkSpecializationInfo();
    populate_specialization_info(*this->vk_specialization_info, this->specialization_entries, this->n_entries, this->specialization_data, this->specialization_data_size);

    // With the specialization info in the pocket, prepare the shader stage
    populate_shader_info(this->vk_shader_stage, this->shader, shader_stage, *this->vk_specialization_info);

    // Done with this one
    DLEAVE;
}

/* Copy constructor for the ShaderStage. */
ShaderStage::ShaderStage(const ShaderStage& other) :
    vk_shader_stage(other.vk_shader_stage),
    shader(other.shader),
    vk_specialization_info(new VkSpecializationInfo(*other.vk_specialization_info)),
    specialization_entries(new VkSpecializationMapEntry[other.n_entries]),
    n_entries(other.n_entries),
    specialization_data(new uint8_t[other.specialization_data_size]),
    specialization_data_size(other.specialization_data_size)
{
    DENTER("Rendering::ShaderStage::ShaderStage(copy)");

    // First, copy the entries and constant data over
    memcpy(this->specialization_entries, other.specialization_entries, this->n_entries * sizeof(VkSpecializationMapEntry));
    memcpy(this->specialization_data, other.specialization_data, this->specialization_data_size * sizeof(uint8_t));

    // Next, update the specialization struct to include the new pointers to everything
    this->vk_specialization_info->pMapEntries = this->specialization_entries;
    this->vk_specialization_info->pData = this->specialization_data;

    // Finally, also update the shader stage info itself
    this->vk_shader_stage.pSpecializationInfo = this->vk_specialization_info;

    // Done
    DLEAVE;
}

/* Move constructor for the ShaderStage. */
ShaderStage::ShaderStage(ShaderStage&& other) :
    vk_shader_stage(other.vk_shader_stage),
    shader(other.shader),
    vk_specialization_info(other.vk_specialization_info),
    specialization_entries(other.specialization_entries),
    n_entries(other.n_entries),
    specialization_data(other.specialization_data),
    specialization_data_size(other.specialization_data_size)
{
    // Mark the deallocatabel stuff as 'do-not-touch'
    other.specialization_entries = nullptr;
    other.specialization_data = nullptr;
    other.vk_specialization_info = nullptr;
}

/* Destructor for the ShaderStage. */
ShaderStage::~ShaderStage() {
    DENTER("Rendering::ShaderStage::~ShaderStage");

    // Deallocate the data if we need to
    if (this->vk_specialization_info != nullptr) {
        delete this->vk_specialization_info;
    }
    if (this->specialization_data != nullptr) {
        delete[] this->specialization_data;
    }
    if (this->specialization_entries != nullptr) {
        delete[] this->specialization_entries;
    }

    DLEAVE;
}



/* Swap operator for the ShaderStage class. */
void Rendering::swap(ShaderStage& ss1, ShaderStage& ss2) {
    DENTER("Rendering::swap(ShaderStage)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (ss1.shader != ss2.shader) {
        DLOG(fatal, "Cannot swap shader stages with different shaders");
    }
    #endif

    // Simply swap it all
    using std::swap;
    swap(ss1.vk_shader_stage, ss2.vk_shader_stage);
    swap(ss1.vk_specialization_info, ss2.vk_specialization_info);
    swap(ss1.specialization_entries, ss2.specialization_entries);
    swap(ss1.n_entries, ss2.n_entries);
    swap(ss1.specialization_data, ss2.specialization_data);
    swap(ss1.specialization_data_size, ss2.specialization_data_size);

    // Done
    DRETURN;
}

