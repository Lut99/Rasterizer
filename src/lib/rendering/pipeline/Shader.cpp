/* SHADER.cpp
 *   by Lut99
 *
 * Created:
 *   27/04/2021, 14:55:16
 * Last edited:
 *   25/05/2021, 18:14:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Shader class, which loads, compiles & manages .spv files
 *   for use in pipelines.
**/

#include <vector>
#include <fstream>
#include <cstring>
#include <cerrno>

#include "tools/Logger.hpp"
#include "tools/Common.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "Shader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Function that populates the given VkShaderModuleCreateInfo struct with the given values. */
static void populate_shader_module_info(VkShaderModuleCreateInfo& shader_module_info, const std::vector<char>& raw_shader) {
    // Set to default
    shader_module_info = {};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Set the data of the code
    shader_module_info.codeSize = raw_shader.size();
    shader_module_info.pCode = reinterpret_cast<const uint32_t*>(raw_shader.data());
}





/***** SHADER CLASS *****/
/* Constructor for the Shader class, which takes the device to compile for and the path of the shader. Optionally also takes the entry function to the shader. */
Shader::Shader(const GPU& gpu, const std::string& path, const std::string& entry_function) :
    gpu(gpu),
    path_name(path.substr(path.find_last_of("/") != string::npos ? path.find_last_of("/") + 1 : 0)),
    filepath(path),
    entry(entry_function)
{
    logger.logc(Verbosity::important, Shader::channel, "Initializing shader '", this->path_name, '\'');

    // Simply call reload to do the work
    this->reload();

    logger.logc(Verbosity::important, Shader::channel, "Init success.");
}

/* Copy constructor for the Shader class. */
Shader::Shader(const Shader& other) :
    gpu(other.gpu),

    shader_data(other.shader_data),
    path_name(other.path_name),
    filepath(other.filepath),
    entry(other.entry)
{
    logger.logc(Verbosity::important, Shader::channel, "Copying shader '", this->path_name, "'...");
    
    // We only have to recompile
    this->compile();

    logger.logc(Verbosity::important, Shader::channel, "Copy success.");
}

/* Move constructor for the Shader class. */
Shader::Shader(Shader&& other) :
    gpu(other.gpu),

    shader_data(other.shader_data),
    vk_shader_module(other.vk_shader_module),
    path_name(other.path_name),
    filepath(other.filepath),
    entry(other.entry)
{
    // Set the shader module to nullptr to avoid deallocation
    other.vk_shader_module = nullptr;
}

/* Destructor for the Shader class. */
Shader::~Shader() {
    logger.logc(Verbosity::important, Shader::channel, "Cleaning shader '", this->path_name, "'...");
    
    if (this->vk_shader_module != nullptr) {
        vkDestroyShaderModule(this->gpu, this->vk_shader_module, nullptr);
    }

    logger.logc(Verbosity::important, Shader::channel, "Cleaned.");
}



/* Reloads the shader from disk, and recompiles it. */
void Shader::reload() {
    // Start by loading the file
    logger.logc(Verbosity::details, Shader::channel, "Loading file '", this->filepath, "'...");

    // Open a file handle, if at all possible
    std::ifstream h(this->filepath, std::ios::ate | std::ios::binary);
    if (!h.is_open()) {
        // Get the error as char array; this is mumumu SaFE oN wINdOwS
        #ifdef _WIN32
        char buffer[BUFSIZ];
        strerror_s(buffer, BUFSIZ, errno);
        #else
        char* buffer = strerror(errno);
        #endif
        logger.fatalc(Shader::channel, "Could not open shader file: ", buffer);
    }

    // Since we're at the end of the file, read the position to know the size of our file buffer
    size_t file_size = h.tellg();

    // Initialize the buffer. We're using the std::vector to get proper alignment
    this->shader_data.resize(file_size);

    // Next, we read all bytes in one go
    h.seekg(0);
    h.read(this->shader_data.data(), file_size);
    h.close();

    

    // Compile the shader
    this->compile();

    // Done!
    return;
}

/* Compiles the shader using the data read with reload() only. */
void Shader::compile() {
    // With the bytes read, create the shader module from it
    logger.logc(Verbosity::details, Shader::channel, "Compiling ", Tools::bytes_to_string(this->shader_data.size()), " of shader code...");

    // Populate the create info
    VkShaderModuleCreateInfo shader_module_info;
    populate_shader_module_info(shader_module_info, this->shader_data);

    // With the struct populated, create the actual module
    VkResult vk_result;
    if ((vk_result = vkCreateShaderModule(this->gpu, &shader_module_info, nullptr, &this->vk_shader_module)) != VK_SUCCESS) {
        logger.fatalc(Shader::channel, "Could not compile shader: ", vk_error_map[vk_result]);
    }
}



/* Swap operator for the Shader class. */
void Rendering::swap(Shader& s1, Shader& s2) {
    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (s1.gpu != s2.gpu) { logger.fatalc(Shader::channel, "Cannot swap shaders with different GPUs"); }
    #endif
    
    using std::swap;

    // Swap all fields
    swap(s1.shader_data, s2.shader_data);
    swap(s1.vk_shader_module, s2.vk_shader_module);
    swap(s1.path_name, s2.path_name);
    swap(s1.filepath, s2.filepath);
    swap(s1.entry, s2.entry);
}
