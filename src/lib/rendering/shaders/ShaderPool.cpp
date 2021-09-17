/* SHADER POOL.cpp
 *   by Lut99
 *
 * Created:
 *   17/09/2021, 22:13:30
 * Last edited:
 *   17/09/2021, 22:13:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <fstream>

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "ShaderPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** HELPER FUNCTIONS *****/
/* Loads the given path as raw, binary shader data, and returns it in the newly heap-allocated array. Note that the data type is actually uint32_t, meaning the resulting array is aligned to its size. */
static void load_raw_shader_data(const std::string& path, uint32_t** data, size_t* data_size) {
    // Begin by trying to open a stream to the file
    std::ifstream file(path, ios::binary | ios::ate);
    if (!file.is_open()) {
        #ifdef _WIN32
        char error[BUFSIZ];
        strerror_s(error, errno);
        #else
        char* error = strerror(errno);
        #endif
        logger.fatalc(ShaderPool::channel, "Cannot open file '", path, "': ", error);
    }

    // Note the file position as the shader module size and compute the uint32_t size version of it
    size_t n_bytes = static_cast<uint32_t>(file.tellg());
    uint32_t padding = (sizeof(uint32_t) - static_cast<uint32_t>(n_bytes) % sizeof(uint32_t)) % sizeof(uint32_t);
    *data_size = (static_cast<uint32_t>(n_bytes) + padding) / sizeof(uint32_t);

    // Allocate data for the shader
    *data = new uint32_t[*data_size];

    // Read all the shader code in one go
    file.seekg(0);
    file.read((char*) *data, n_bytes);
    file.close();

    // Done
}





/***** POPULATE FUNCTIONS *****/
/* Populates the given VkShaderModuleCreateInfo struct. */
static void populate_shader_info(VkShaderModuleCreateInfo& shader_info, uint32_t* data, size_t data_size, VkShaderModuleCreateFlags create_flags) {
    // Set to default
    shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Link the data
    shader_info.pCode = data;
    shader_info.codeSize = data_size;

    // Possibly add some shader flags
    shader_info.flags = create_flags;

}





/***** SHADERPOOL CLASS *****/
/* Constructor for the ShaderPool class, which takes the GPU where the Shaders will live. */
ShaderPool::ShaderPool(const Rendering::GPU& gpu) :
    gpu(gpu)
{
    logger.init_success(Verbosity::important, ShaderPool::channel);
}

/* Move constructor for the ShaderPool class. */
ShaderPool::ShaderPool(ShaderPool&& other) :
    gpu(other.gpu),

    shaders(std::move(other.shaders))
{
    // The Array's move constructor guarantees us it's reset to empty
}

/* Destructor for the ShaderPool class. */
ShaderPool::~ShaderPool() {
    logger.clean_start(Verbosity::important, ShaderPool::channel);

    // Clean the Shaders, if any
    if (!this->shaders.empty()) {
        logger.logc(Verbosity::details, ShaderPool::channel, "Destroying shaders...");
        for (Shader* shader : this->shaders) {
            vkDestroyShaderModule(this->gpu, shader->vk_shader_module, nullptr);
            delete shader;
        }
    }

    logger.clean_success(Verbosity::important, ShaderPool::channel);
}



/* Allocates a new Shader, loaded from the given location as a .spv. Optionally, create flags for the VkShaderModule can be set. */
Rendering::Shader* ShaderPool::allocate(const std::string& path, VkShaderModuleCreateFlags create_flags) {
    // First, we try to load the shader data
    logger.logc(Verbosity::important, ShaderPool::channel, "Loading shader from file '", path, "'...");
    uint32_t* data; size_t data_size;
    load_raw_shader_data(path, &data, &data_size);

    // If we succeeded, continue by populating the create info for the VkShaderModule
    VkShaderModuleCreateInfo shader_info;
    populate_shader_info(shader_info, data, data_size, create_flags);

    // Create the shader module itself
    VkShaderModule vk_shader_module;
    VkResult vk_result;
    if ((vk_result = vkCreateShaderModule(this->gpu, &shader_info, nullptr, &vk_shader_module)) != VK_SUCCESS) {
        logger.fatalc(ShaderPool::channel, "Could not create VkShaderModule: ", vk_error_map[vk_result]);
    }

    // If created, use it to populate the Shader object
    Shader* result = new Shader(this->gpu, vk_shader_module, path);
    this->shaders.insert(result);

    // Deallocate the raw data and we're done
    delete[] data;
    return result;
}

/* Frees the given Shader from the internal pool. Throws error if it isn't allocated here. */
void ShaderPool::free(const Rendering::Shader* shader) {
    // First, try to find the Shader
    std::unordered_set<Rendering::Shader*>::iterator iter = this->shaders.find(const_cast<Rendering::Shader*>(shader));
    if (iter == this->shaders.end()) {
        logger.fatalc(ShaderPool::channel, "Cannot free shader @ ", shader, " that was not allocated with this pool.");
    }

    // If it was found, then delete it and return
    vkDestroyShaderModule(this->gpu, (*iter)->vulkan(), nullptr);
    delete *iter;

    // Remove it from the set
    this->shaders.erase(iter);
}



/* Resets the pool, deleting all internal shaders. */
void ShaderPool::reset() {
    // Simply delete everything first
    for (Shader* shader : this->shaders) {
        vkDestroyShaderModule(this->gpu, shader->vulkan(), nullptr);
        delete shader;
    }

    // Clear the set
    this->shaders.clear();
}



/* Swap operator for the ShaderPool class. */
void Rendering::swap(ShaderPool& sp1, ShaderPool& sp2) {
    #ifndef NDEBUG
    if (sp1.gpu != sp2.gpu) { logger.fatalc(ShaderPool::channel, "Cannot swap shader pools with different GPUs."); }
    #endif

    using std::swap;

    swap(sp1.shaders, sp2.shaders);
}
