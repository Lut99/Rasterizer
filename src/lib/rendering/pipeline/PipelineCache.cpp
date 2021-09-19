/* PIPELINE CACHE.cpp
 *   by Lut99
 *
 * Created:
 *   18/09/2021, 11:16:05
 * Last edited:
 *   18/09/2021, 11:16:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a simple wrapper around the VkPipelineCache object, that will
 *   helps us speed up pipeline creation.
**/

#include <fstream>

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "PipelineCache.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineCacheCreateInfo struct. */
static void populate_cache_info(VkPipelineCacheCreateInfo& cache_info, void* initial_data, size_t initial_data_size, VkPipelineCacheCreateFlags create_flags) {
    // Set to default
    cache_info = {};
    cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    // Set any initial data from previous pipeline caches
    cache_info.pInitialData = initial_data;
    cache_info.initialDataSize = initial_data_size;

    // Add possible create flags
    cache_info.flags = create_flags;
}





/***** PIPELINECACHE CLASS *****/
/* Constructor for the PipelineCache class, which takes the GPU where the cache will live and a filepath to store the cache once this class gets destructed. In case the cache file already exists, loads data from that cache first. Optionally takes create flags for the internal VkPipelineCache. */
PipelineCache::PipelineCache(const Rendering::GPU& gpu, const std::string& filepath, VkPipelineCacheCreateFlags create_flags) :
    gpu(gpu),
    filepath(filepath)
{
    logger.logc(Verbosity::details, PipelineCache::channel, "Loading pipeline cache file '", this->filepath, "'...");

    // Placeholders for the data we'll load
    void* data = nullptr;
    size_t data_size = 0;

    // Try to open the file and load the data
    ifstream cache_file(this->filepath, ios::binary | ios::ate);
    if (cache_file.is_open()) {
        // Store the file size
        data_size = static_cast<size_t>(cache_file.tellg());

        // Allocate enough data for the file
        data = (void*) new char[data_size];

        // Load the data
        cache_file.seekg(0);
        cache_file.read((char*) data, data_size);
        cache_file.close();

    } else {
        // Get the error
        #ifdef _WIN32
        char error[BUFSIZ];
        strerror_s(error, errno);
        #else
        char* error = strerror(errno);
        #endif

        // Show the error
        logger.errorc(PipelineCache::channel, "Could not open pipeline cache file '", this->filepath, "': ", error);
    }

    // Create the pipeline cache info with the data (or without if the loading failed)
    VkPipelineCacheCreateInfo cache_info;
    populate_cache_info(cache_info, data, data_size, create_flags);

    // Create the cache
    VkResult vk_result;
    if ((vk_result = vkCreatePipelineCache(this->gpu, &cache_info, nullptr, &this->vk_pipeline_cache)) != VK_SUCCESS) {
        logger.fatalc(PipelineCache::channel, "Could not create pipeline cache: ", vk_error_map[vk_result]);
    }

    // Done!
    logger.logc(Verbosity::details, PipelineCache::channel, "Loading success.");
}

/* Move constructor for the PipelineCache class. */
PipelineCache::PipelineCache(PipelineCache&& other) :
    gpu(other.gpu),
    vk_pipeline_cache(other.vk_pipeline_cache),
    filepath(other.filepath)
{
    other.vk_pipeline_cache = nullptr;
}

/* Destructor for the PipelineCache class. */
PipelineCache::~PipelineCache() {
    VkResult vk_result;

    if (this->vk_pipeline_cache != nullptr) {
        logger.logc(Verbosity::details, PipelineCache::channel, "Writing pipeline cache to '", this->filepath, "'...");
        
        // Get the pipeline cache data's size
        size_t data_size;
        if ((vk_result = vkGetPipelineCacheData(this->gpu, this->vk_pipeline_cache, &data_size, nullptr)) != VK_SUCCESS) {
            logger.errorc(PipelineCache::channel, "Could not get pipeline cache data size: ", vk_error_map[vk_result]);
            // Destroy the pipeline cache and return
            vkDestroyPipelineCache(this->gpu, this->vk_pipeline_cache, nullptr);
            return;
        }
        // Allocate the buffer, then fetch the actual data
        void* data = (void*) new char[data_size];
        if ((vk_result = vkGetPipelineCacheData(this->gpu, this->vk_pipeline_cache, &data_size, data)) != VK_SUCCESS) {
            logger.errorc(PipelineCache::channel, "Could not get pipeline cache data: ", vk_error_map[vk_result]);
            // Destroy the pipeline cache and return
            vkDestroyPipelineCache(this->gpu, this->vk_pipeline_cache, nullptr);
            return;
        }

        // With the data in memory, open the file to write to
        ofstream cache_file(this->filepath, ios::binary);
        if (!cache_file.is_open()) {
            // Get the error
            #ifdef _WIN32
            char error[BUFSIZ];
            strerror_s(error, errno);
            #else
            char* error = strerror(errno);
            #endif

            // Show the error
            logger.errorc(PipelineCache::channel, "Could not open cache file '", this->filepath, "' for writing: ", error);
            // Destroy the pipeline cache and return
            vkDestroyPipelineCache(this->gpu, this->vk_pipeline_cache, nullptr);
            return;
        }

        // Write, done
        cache_file.write((char*) data, data_size);
        cache_file.close();
        logger.logc(Verbosity::details, PipelineCache::channel, "Writing success.");

        // Finally, destroy the pipeline cache
        vkDestroyPipelineCache(this->gpu, this->vk_pipeline_cache, nullptr);
    }
}



/* Swap operator for the PipelineCache class. */
void Rendering::swap(PipelineCache& pc1, PipelineCache& pc2) {
    #ifndef NDEBUG
    if (pc1.gpu != pc2.gpu) { logger.fatalc(PipelineCache::channel, "Cannot swap pipeline caches with different GPUs"); }
    #endif

    using std::swap;

    swap(pc1.vk_pipeline_cache, pc2.vk_pipeline_cache);
    swap(pc1.filepath, pc2.filepath);
}
