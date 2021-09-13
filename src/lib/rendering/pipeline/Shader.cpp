/* SHADER.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:30:31
 * Last edited:
 *   11/09/2021, 14:30:31
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <fstream>
#include <cstring>
#include <cerrno>

#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "Shader.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkShaderModuleCreateInfo struct. */
static void populate_shader_module_info(VkShaderModuleCreateInfo& shader_module_info, uint32_t* data, size_t data_size, VkShaderModuleCreateFlags create_flags) {
    // Set to default
    shader_module_info = {};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Link the data
    shader_module_info.pCode = data;
    shader_module_info.codeSize = data_size;

    // Possibly add some shader flags
    shader_module_info.flags = create_flags;

}





/***** SHADER CLASS *****/
/* Constructor for the Shader class, which takes a GPU where the Shader will live, the filename of the file to load and optionally some create flags. */
Shader::Shader(const Rendering::GPU& gpu, const std::string& filename, VkShaderModuleCreateFlags create_flags) :
    gpu(gpu),

    vk_create_flags(create_flags),

    name(filename.substr(filename.find_last_of("/") != string::npos ? filename.find_last_of("/") + 1 : 0))
{
    logger.logc(Verbosity::details, Shader::channel, "Initializing shader '", this->name, "'...");

    // Begin by trying to open a stream to the file
    logger.logc(Verbosity::details, Shader::channel, "Loading file '", filename, "'...");
    std::ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        std::string error;
        #ifdef _WIN32
        char buffer[BUFSIZ];
        strerror_s(buffer, errno);
        error = buffer;
        #else
        error = strerror(errno);
        #endif
        logger.fatalc(Shader::channel, "Cannot open file '", filename, "': ", error);
    }

    // Note the file position as the shader module size and compute the uint32_t size version of it
    this->n_bytes = static_cast<uint32_t>(file.tellg());
    uint32_t padding = (sizeof(uint32_t) - static_cast<uint32_t>(this->n_bytes) % sizeof(uint32_t)) % sizeof(uint32_t);
    this->shader_data_size = (static_cast<uint32_t>(this->n_bytes) + padding) / sizeof(uint32_t);

    // Allocate data for the shader
    this->shader_data = new uint32_t[this->shader_data_size];

    // Read all the shader code in one go
    file.seekg(0);
    file.read((char*) this->shader_data, this->n_bytes);
    file.close();
    logger.logc(Verbosity::details, Shader::channel, "Raw .SPV code is ", Tools::bytes_to_string(this->n_bytes), " bytes (", Tools::bytes_to_string(this->shader_data_size * sizeof(uint32_t)), " when aligned)");



    // Next, compile the shader code by wrapping it in a VkShaderModule
    logger.logc(Verbosity::details, Shader::channel, "Compiling shader...");
    VkShaderModuleCreateInfo shader_module_info;
    populate_shader_module_info(shader_module_info, this->shader_data, this->n_bytes, this->vk_create_flags);

    // Create the VkShaderModule
    VkResult vk_result;
    if ((vk_result = vkCreateShaderModule(this->gpu, &shader_module_info, nullptr, &this->vk_shader_module)) != VK_SUCCESS) {
        logger.fatalc(Shader::channel, "Cannot create VkShaderModule: ", vk_error_map[vk_result]);
    }



    // Done!
    logger.logc(Verbosity::details, Shader::channel, "Init success.");
}

/* Copy constructor for the Shader class. */
Shader::Shader(const Shader& other) :
    gpu(other.gpu),

    n_bytes(other.n_bytes),
    vk_create_flags(other.vk_create_flags),
    
    shader_data_size(other.shader_data_size),

    name(other.name)
{
    logger.logc(Verbosity::debug, Shader::channel, "Copying shader '", this->name, "'...");

    // First, copy the raw shader data
    this->shader_data = new uint32_t[this->shader_data_size];
    memcpy(this->shader_data, other.shader_data, this->shader_data_size * sizeof(uint32_t));

    // Next, re-create the shader module
    VkShaderModuleCreateInfo shader_module_info;
    populate_shader_module_info(shader_module_info, this->shader_data, this->n_bytes, this->vk_create_flags);

    // Create the VkShaderModule
    VkResult vk_result;
    if ((vk_result = vkCreateShaderModule(this->gpu, &shader_module_info, nullptr, &this->vk_shader_module)) != VK_SUCCESS) {
        logger.fatalc(Shader::channel, "Cannot re-create VkShaderModule: ", vk_error_map[vk_result]);
    }
    
    logger.logc(Verbosity::debug, Shader::channel, "Copy success.");
}

/* Move constructor for the Shader class. */
Shader::Shader(Shader&& other) :
    gpu(other.gpu),

    vk_shader_module(other.vk_shader_module),
    n_bytes(other.n_bytes),
    vk_create_flags(other.vk_create_flags),

    shader_data(other.shader_data),
    shader_data_size(other.shader_data_size),

    name(other.name)
{
    // Prevent the other deallocating shit
    other.vk_shader_module = nullptr;
    other.shader_data = nullptr;
}

/* Destructor for the Shader class. */
Shader::~Shader() {
    logger.logc(Verbosity::details, Shader::channel, "Cleaning shader '", this->name, "'...");

    // Deallocate the shader module
    if (this->vk_shader_module != nullptr) {
        logger.logc(Verbosity::details, Shader::channel, "Cleaning VkShaderModule...");
        vkDestroyShaderModule(this->gpu, this->vk_shader_module, nullptr);
    }
    // Deallocate the raw shader data
    if (this->shader_data != nullptr) {
        logger.logc(Verbosity::details, Shader::channel, "Cleaning raw shader data...");
        delete[] this->shader_data;
    }
    
    logger.logc(Verbosity::details, Shader::channel, "Cleaned.");
}



/* Swap operator for the Shader class. */
void Rendering::swap(Shader& s1, Shader& s2) {
    #ifndef NDEBUG
    if (s1.gpu != s2.gpu) { logger.fatalc(Shader::channel, "Cannot swap shaders with different GPUs"); }
    #endif

    using std::swap;

    swap(s1.vk_shader_module, s2.vk_shader_module);
    swap(s1.n_bytes, s2.n_bytes);
    swap(s1.vk_create_flags, s2.vk_create_flags);
    
    swap(s1.shader_data, s2.shader_data);
    swap(s1.shader_data_size, s2.shader_data_size);

    swap(s1.name, s2.name);
}
