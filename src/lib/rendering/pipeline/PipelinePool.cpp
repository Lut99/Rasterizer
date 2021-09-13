/* PIPELINE POOL.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:06:26
 * Last edited:
 *   11/09/2021, 14:06:26
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelinePool, which is a class that is in charge of
 *   managing and, more importantly, spawning new pipelines in an efficient
 *   way.
**/

#include <fstream>
#include <cstring>
#include <cerrno>

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "PipelinePool.hpp"

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

/* Populates the given VkGraphicsPipelineCreateInfo struct. */
static void populate_pipeline_info(VkGraphicsPipelineCreateInfo& pipeline_info,
                                   const Tools::Array<VkPipelineShaderStageCreateInfo>& vk_shader_stages,
                                   const VkPipelineVertexInputStateCreateInfo& vk_vertex_input_state,
                                   const VkPipelineInputAssemblyStateCreateInfo& vk_input_assembly_state,
                                   const VkPipelineDepthStencilStateCreateInfo& vk_depth_testing,
                                   const VkPipelineViewportStateCreateInfo& vk_viewport_transformation,
                                   const VkPipelineRasterizationStateCreateInfo& vk_rasterization,
                                   const VkPipelineMultisampleStateCreateInfo& vk_multisampling,
                                   const VkPipelineColorBlendStateCreateInfo& vk_color_logic,
                                   const VkPipelineLayout& vk_pipeline_layout,
                                   const VkRenderPass& vk_render_pass, uint32_t first_subpass,
                                   const VkPipeline& vk_base_handle, int32_t base_index,
                                   VkPipelineCreateFlags create_flags)
{
    // Set to default
    pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // Set the shader stages
    pipeline_info.stageCount = vk_shader_stages.size();
    pipeline_info.pStages = vk_shader_stages.rdata();

    // Next, attach the fixed-function structures
    pipeline_info.pVertexInputState = &vk_vertex_input_state;
    pipeline_info.pInputAssemblyState = &vk_input_assembly_state;
    pipeline_info.pDepthStencilState = &vk_depth_testing;
    pipeline_info.pViewportState = &vk_viewport_transformation;
    pipeline_info.pRasterizationState = &vk_rasterization;
    pipeline_info.pMultisampleState = &vk_multisampling;
    pipeline_info.pColorBlendState = &vk_color_logic;
    pipeline_info.pDynamicState = nullptr;

    // We next set the pipeline layout
    pipeline_info.layout = vk_pipeline_layout;

    // Finally, set the renderpass and its first subpass
    pipeline_info.renderPass = vk_render_pass;
    pipeline_info.subpass = first_subpass;

    // Note: we won't use pipeline derivation for now
    pipeline_info.basePipelineHandle = vk_base_handle;
    pipeline_info.basePipelineIndex = base_index;

    // Finally, set the create flags
    pipeline_info.flags = create_flags;
}





/***** PIPELINEPOOL CLASS *****/
/* Constructor for the PipelinePool class, which takes a GPU where all the pipelines will live. Does not use previous pipeline caches, and may thus be slower on boot. */
PipelinePool::PipelinePool(const Rendering::GPU& gpu) :
    PipelinePool(gpu, "")
{}

/* Constructor for the PipelinePool class, which takes a GPU where all the pipelines will live, a filename with initial data for the pipeline cache and optional create flags for the cache. If the filename isn't found, initializes the cache to zero. */
PipelinePool::PipelinePool(const Rendering::GPU& gpu, const std::string& filename, VkPipelineCacheCreateFlags cache_create_flags) :
    gpu(gpu),

    pipeline_cache_filename(filename),
    pipeline_preallocations(16),

    pipelines(16)
{
    logger.logc(Verbosity::important, PipelinePool::channel, "Initializing...");

    // Try to load the pipeline cache file
    void* data = nullptr;
    size_t data_size = 0;
    if (!this->pipeline_cache_filename.empty()) {
        logger.logc(Verbosity::details, PipelinePool::channel, "Loading pipeline cache file '", this->pipeline_cache_filename, "'...");
        std::ifstream cache_file(this->pipeline_cache_filename, ios::binary | ios::ate);
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
            std::string error;
            #ifdef _WIN32
            char buffer[BUFSIZ];
            strerror_s(buffer, errno);
            error = buffer;
            #else
            error = strerror(errno);
            #endif

            // Show the error
            logger.errorc(PipelinePool::channel, "Could not open pipeline cache file '", this->pipeline_cache_filename, "': ", error);
        }
    }

    // Create the pipeline cache create info
    logger.logc(Verbosity::details, PipelinePool::channel, "Creating pipeline cache...");
    VkPipelineCacheCreateInfo cache_info;
    populate_cache_info(cache_info, data, data_size, cache_create_flags);

    // Use that to actually create the cache
    VkResult vk_result;
    if ((vk_result = vkCreatePipelineCache(this->gpu, &cache_info, nullptr, &this->vk_pipeline_cache)) != VK_SUCCESS) {
        logger.fatalc(PipelinePool::channel, "Could not create pipeline cache: ", vk_error_map[vk_result]);
    }

    // Clean the raw data when done
    if (data != nullptr) {
        delete[] data;
    }

    // DOne
    logger.logc(Verbosity::important, PipelinePool::channel, "Init success.");
}

/* Move constructor for the PipelinePool class. */
PipelinePool::PipelinePool(PipelinePool&& other) :
    gpu(other.gpu),

    vk_pipeline_cache(other.vk_pipeline_cache),
    pipeline_preallocations(std::move(other.pipeline_preallocations)),

    pipelines(std::move(other.pipelines))
{
    // The Array's move function promises us the other Array will be empty
    other.vk_pipeline_cache = nullptr;
}

/* Destructor for the PipelinePool class. */
PipelinePool::~PipelinePool() {
    logger.logc(Verbosity::important, PipelinePool::channel, "Cleaning...");

    // Deallocate all pipeline objects if needed
    if (this->pipelines.size() > 0) {
        logger.logc(Verbosity::details, PipelinePool::channel, "Cleaning allocated pipelines...");
        for (uint32_t i = 0; i < this->pipelines.size(); i++) {
            delete this->pipelines[i];
        }
    }
    // Deallocate the cache if needed
    if (this->vk_pipeline_cache != nullptr) {
        // But first, try to save it to the filepath if needed
        if (!this->pipeline_cache_filename.empty()) {
            logger.logc(Verbosity::details, PipelinePool::channel, "Saving pipeline cache to '", this->pipeline_cache_filename, "'...");

            // Get the cache data first
            size_t data_size;
            VkResult vk_result;
            if ((vk_result = vkGetPipelineCacheData(this->gpu, this->vk_pipeline_cache, &data_size, nullptr)) != VK_SUCCESS) {
                logger.errorc(PipelinePool::channel, "Could not get pipeline cache data size: ", vk_error_map[vk_result]);
            }
            void* data = (void*) new char[data_size];
            if ((vk_result = vkGetPipelineCacheData(this->gpu, this->vk_pipeline_cache, &data_size, data)) != VK_SUCCESS) {
                logger.errorc(PipelinePool::channel, "Could not get pipeline cache data: ", vk_error_map[vk_result]);
            }

            // Open a file to the target file
            std::ofstream cache_file(this->pipeline_cache_filename, ios::binary);
            if (!cache_file.is_open()) {
                // Get the error
                std::string error;
                #ifdef _WIN32
                char buffer[BUFSIZ];
                strerror_s(buffer, errno);
                error = buffer;
                #else
                error = strerror(errno);
                #endif

                // Show the error
                logger.errorc(PipelinePool::channel, "Could not open pipeline cache file '", this->pipeline_cache_filename, "': ", error);
            }

            // Write to the file
            cache_file.write((char*) data, data_size);
            cache_file.close();

            // Done, clean the array
            delete[] data;
        }

        // With that over, actually clean the pipeline cache
        logger.logc(Verbosity::details, PipelinePool::channel, "Cleaning pipeline cache...");
        vkDestroyPipelineCache(this->gpu, this->vk_pipeline_cache, nullptr);
    }
    
    logger.logc(Verbosity::important, PipelinePool::channel, "Cleaned.");
}



/* Spawns a completely new pipeline with the given properties and the given render pass. Will probably be relatively slow compared to other operations, but doesn't depent on other pipeline objects. */
Rendering::Pipeline* PipelinePool::allocate(Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) {
    // Cast the shader stages to an array of VkPipelineShaderStageCreateInfo objects forst
    Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_stages(properties.shader_stages.size());
    for (uint32_t i = 0; i < properties.shader_stages.size(); i++) {
        vk_shader_stages.push_back(properties.shader_stages[i].info());
    }

    // We can immediately populate the VkGraphicsPipelineCreateInfo for the pipeline we want
    VkGraphicsPipelineCreateInfo pipeline_info;
    populate_pipeline_info(
        pipeline_info,
        vk_shader_stages,
        properties.vertex_input_state.info(),
        properties.input_assembly_state.info(),
        properties.depth_testing.info(),
        properties.viewport_transformation.info(),
        properties.rasterization.info(),
        properties.multisampling.info(),
        properties.color_logic.info(),
        properties.pipeline_layout.layout(),
        render_pass, first_subpass,
        VK_NULL_HANDLE, -1,
        0
    );

    // Use that to create the pipeline
    VkResult vk_result;
    VkPipeline vk_pipeline;
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, this->vk_pipeline_cache, 1, &pipeline_info, nullptr, &vk_pipeline)) != VK_SUCCESS) {
        logger.fatalc(PipelinePool::channel, "Could not create new pipeline: ", vk_error_map[vk_result]);
    }

    // With the VkPipeline, we can populate our own Pipeline
    Pipeline* result = new Pipeline(this->gpu, vk_pipeline, std::move(properties));
    // Add it to the internal list
    while (this->pipelines.size() >= this->pipelines.capacity()) { this->pipelines.reserve(2 * this->pipelines.capacity()); }
    this->pipelines.push_back(result);

    // Done, return the object
    return result;
}

/* Spawns a new pipeline that is based on the given pipeline. Works best if you copy the pipeline's PipelineProperties, and then adapt that so they are as alike as possible. */
Rendering::Pipeline* PipelinePool::allocate(const Rendering::Pipeline* pipeline, Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) {
    // Cast the shader stages to an array of VkPipelineShaderStageCreateInfo objects forst
    Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_stages(properties.shader_stages.size());
    for (uint32_t i = 0; i < properties.shader_stages.size(); i++) {
        vk_shader_stages.push_back(properties.shader_stages[i].info());
    }

    // We can immediately populate the VkGraphicsPipelineCreateInfo for the pipeline we want, linking the old pipeline
    VkGraphicsPipelineCreateInfo pipeline_info;
    populate_pipeline_info(
        pipeline_info,
        vk_shader_stages,
        properties.vertex_input_state.info(),
        properties.input_assembly_state.info(),
        properties.depth_testing.info(),
        properties.viewport_transformation.info(),
        properties.rasterization.info(),
        properties.multisampling.info(),
        properties.color_logic.info(),
        properties.pipeline_layout.layout(),
        render_pass, first_subpass,
        pipeline->vk_pipeline, -1,
        VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT
    );

    // Use that to create the pipeline
    VkResult vk_result;
    VkPipeline vk_pipeline;
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, this->vk_pipeline_cache, 1, &pipeline_info, nullptr, &vk_pipeline)) != VK_SUCCESS) {
        logger.fatalc(PipelinePool::channel, "Could not create new pipeline: ", vk_error_map[vk_result]);
    }

    // With the VkPipeline, we can populate our own Pipeline
    Pipeline* result = new Pipeline(this->gpu, vk_pipeline, std::move(properties));
    // Add it to the internal list
    while (this->pipelines.size() >= this->pipelines.capacity()) { this->pipelines.reserve(2 * this->pipelines.capacity()); }
    this->pipelines.push_back(result);

    // Done, return the object
    return result;
}



/* Pre-allocates a new pipeline with the given properties and the given render pass. Although it's still from scratch and relatively slow, it does allow to bulk-allocate multiple pipeline objects. Calling nallocate() will return the pipeline(s) created with the preallocate function since its last call. */
void PipelinePool::preallocate(Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) {
    // Prepare the pre-allocate struct
    PipelinePreallocation pre_allocate(std::move(properties));

    // Cast the shader stages to an array of VkPipelineShaderStageCreateInfo objects
    pre_allocate.vk_shader_stages.reserve(pre_allocate.properties.shader_stages.size());
    for (uint32_t i = 0; i < pre_allocate.properties.shader_stages.size(); i++) {
        pre_allocate.vk_shader_stages.push_back(pre_allocate.properties.shader_stages[i].info());
    }

    // We can immediately populate the VkGraphicsPipelineCreateInfo for the pipeline we want
    VkGraphicsPipelineCreateInfo pipeline_info;
    populate_pipeline_info(
        pipeline_info,
        pre_allocate.vk_shader_stages,
        pre_allocate.properties.vertex_input_state.info(),
        pre_allocate.properties.input_assembly_state.info(),
        pre_allocate.properties.depth_testing.info(),
        pre_allocate.properties.viewport_transformation.info(),
        pre_allocate.properties.rasterization.info(),
        pre_allocate.properties.multisampling.info(),
        pre_allocate.properties.color_logic.info(),
        pre_allocate.properties.pipeline_layout.layout(),
        render_pass, first_subpass,
        VK_NULL_HANDLE, -1,
        0
    );

    // Store the preallocation in the internal list
    while (this->pipeline_preallocations.size() >= this->pipeline_preallocations.capacity()) {
        this->pipeline_preallocations.reserve(2 * this->pipeline_preallocations.capacity());
    }
    this->pipeline_preallocations.push_back(std::move(pre_allocate));
}

/* Pre-allocates a new pipeline that is based on the given pipeline. Works best if you copy the pipeline's PipelineProperties, and then adapt that so they are as alike as possible. */
void PipelinePool::preallocate(const Rendering::Pipeline* pipeline, Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) {
    // Prepare the pre-allocate struct
    PipelinePreallocation pre_allocate(std::move(properties));

    // Cast the shader stages to an array of VkPipelineShaderStageCreateInfo objects
    pre_allocate.vk_shader_stages.reserve(pre_allocate.properties.shader_stages.size());
    for (uint32_t i = 0; i < pre_allocate.properties.shader_stages.size(); i++) {
        pre_allocate.vk_shader_stages.push_back(pre_allocate.properties.shader_stages[i].info());
    }

    // We can immediately populate the VkGraphicsPipelineCreateInfo for the pipeline we want
    VkGraphicsPipelineCreateInfo pipeline_info;
    populate_pipeline_info(
        pipeline_info,
        pre_allocate.vk_shader_stages,
        pre_allocate.properties.vertex_input_state.info(),
        pre_allocate.properties.input_assembly_state.info(),
        pre_allocate.properties.depth_testing.info(),
        pre_allocate.properties.viewport_transformation.info(),
        pre_allocate.properties.rasterization.info(),
        pre_allocate.properties.multisampling.info(),
        pre_allocate.properties.color_logic.info(),
        pre_allocate.properties.pipeline_layout.layout(),
        render_pass, first_subpass,
        pipeline->vk_pipeline, -1,
        VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT
    );

    // Store the preallocation in the internal list
    while (this->pipeline_preallocations.size() >= this->pipeline_preallocations.capacity()) {
        this->pipeline_preallocations.reserve(2 * this->pipeline_preallocations.capacity());
    }
    this->pipeline_preallocations.push_back(std::move(pre_allocate));
}

/* Pre-allocates a new pipeline that is based on the given pipeline that is already defined with a preallocate call() in this batch. Works best if you copy the pipeline's PipelineProperties, and then adapt that so they are as alike as possible. */
void PipelinePool::preallocate(uint32_t pipeline_index, Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) {
    #ifndef NDEBUG
    // Make sure the index exists
    if (pipeline_index >= this->pipeline_preallocations.size()) {
        logger.fatalc(PipelinePool::channel, "Cannot create pipeline that derives for a pipeline with index ", pipeline_index, ", while only ", this->pipeline_preallocations.size(), " pipelines have been preallocated.");
    }
    #endif

    // Prepare the pre-allocate struct
    PipelinePreallocation pre_allocate(std::move(properties));

    // Cast the shader stages to an array of VkPipelineShaderStageCreateInfo objects
    pre_allocate.vk_shader_stages.reserve(pre_allocate.properties.shader_stages.size());
    for (uint32_t i = 0; i < pre_allocate.properties.shader_stages.size(); i++) {
        pre_allocate.vk_shader_stages.push_back(pre_allocate.properties.shader_stages[i].info());
    }

    // We can immediately populate the VkGraphicsPipelineCreateInfo for the pipeline we want
    VkGraphicsPipelineCreateInfo pipeline_info;
    populate_pipeline_info(
        pipeline_info,
        pre_allocate.vk_shader_stages,
        pre_allocate.properties.vertex_input_state.info(),
        pre_allocate.properties.input_assembly_state.info(),
        pre_allocate.properties.depth_testing.info(),
        pre_allocate.properties.viewport_transformation.info(),
        pre_allocate.properties.rasterization.info(),
        pre_allocate.properties.multisampling.info(),
        pre_allocate.properties.color_logic.info(),
        pre_allocate.properties.pipeline_layout.layout(),
        render_pass, first_subpass,
        VK_NULL_HANDLE, pipeline_index,
        VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT
    );

    // Store the preallocation in the internal list
    while (this->pipeline_preallocations.size() >= this->pipeline_preallocations.capacity()) {
        this->pipeline_preallocations.reserve(2 * this->pipeline_preallocations.capacity());
    }
    this->pipeline_preallocations.push_back(std::move(pre_allocate));
}

/* Allocates all pipelines preallocated with preallocate() since either the start of the PipelinePool or the last nallocate() call. */
Tools::Array<Rendering::Pipeline*> PipelinePool::nallocate() {
    // Loop through all preallocations to create a list of pipeline create infos
    Tools::Array<VkGraphicsPipelineCreateInfo> pipeline_infos(this->pipeline_preallocations.size());
    for (uint32_t i = 0; i < this->pipeline_preallocations.size(); i++) {
        pipeline_infos.push_back(this->pipeline_preallocations[i].vk_pipeline_info);
    }

    // Call the create
    VkResult vk_result;
    Tools::Array<VkPipeline> pipelines(pipeline_infos.size());
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, this->vk_pipeline_cache, pipeline_infos.size(), pipeline_infos.rdata(), nullptr, pipelines.wdata(pipeline_infos.size()))) != VK_SUCCESS) {
        logger.fatalc(PipelinePool::channel, "Could not create new pipelines: ", vk_error_map[vk_result]);
    }

    // For each of those pipelines, create a Pipeline object to wrap it
    Tools::Array<Rendering::Pipeline*> result_list(pipelines.size());
    for (uint32_t i = 0; i < pipelines.size(); i++) {
        // Get a reference to the preallocate info for this pipeline
        PipelinePreallocation& pre_allocate = this->pipeline_preallocations[i];

        // Create the Pipeline object
        Pipeline* result = new Pipeline(
            this->gpu,
            pipelines[i],
            std::move(pre_allocate.properties)
        );

        // Add to the internal list
        while (this->pipelines.size() >= this->pipelines.capacity()) { this->pipelines.reserve(2 * this->pipelines.capacity()); }
        this->pipelines.push_back(result);

        // Finally, add to the resulting list
        result_list.push_back(result);
    }

    // Done, return the list
    return result_list;
}



/* Frees the given Pipeline object. */
void PipelinePool::free(const Rendering::Pipeline* pipeline) {
    // See if we have the pipeline
    for (uint32_t i = 0; i < this->pipelines.size(); i++) {
        if (this->pipelines[i] == pipeline) {
            // Delete, then return
            delete this->pipelines[i];
            this->pipelines.erase(i);
            return;
        }
    }
 
    // Otherwise, not found
    logger.fatalc(PipelinePool::channel, "Cannot free pipeline @ ", pipeline, " because it's not allocated with this pool.");
}

/* Frees a list of given pipeline objects. */
void PipelinePool::nfree(const Tools::Array<Rendering::Pipeline*>& pipelines) {
    // See if we have the pipelines
    for (uint32_t i = 0; i < pipelines.size(); i++) {
        bool found = false;
        for (uint32_t j = 0; j < this->pipelines.size(); j++) {
            if (this->pipelines[j] == pipelines[i]) {
                // Delete, then return
                delete this->pipelines[j];
                this->pipelines.erase(j);
                found = true;
                break;
            }
        }
        if (found) { continue; }

        // Otherwise, not found
        logger.fatalc(PipelinePool::channel, "Cannot free pipeline ", i, " @ ", pipelines[i], " because it's not allocated with this pool.");
    }

    // Made it thru
}



/* Resets the pool to an empty state, by deleting all the allocated pipelines. All objectes allocated with it will thus become invalid. */
void PipelinePool::reset() {
    // Simply deallocate everything in the pool
    for (uint32_t i = 0; i < this->pipelines.size(); i++) {
        delete this->pipelines[i];
    }

    // Clear both arrays
    this->pipelines.clear();
    this->pipeline_preallocations.clear();
}



/* Swap operator for the PipelinePool class. */
void Rendering::swap(PipelinePool& pp1, PipelinePool& pp2) {
    #ifndef NDEBUG
    if (pp1.gpu != pp2.gpu) { logger.fatalc(PipelinePool::channel, "Cannot swap pipeline pools with different GPUs."); }
    #endif

    using std::swap;

    swap(pp1.pipelines, pp2.pipelines);
    swap(pp1.pipeline_preallocations, pp2.pipeline_preallocations);
}
