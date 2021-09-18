/* PIPELINE CACHE.hpp
 *   by Lut99
 *
 * Created:
 *   18/09/2021, 11:16:01
 * Last edited:
 *   18/09/2021, 11:16:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a simple wrapper around the VkPipelineCache object, that will
 *   helps us speed up pipeline creation.
**/

#ifndef RENDERING_PIPELINE_CACHE_HPP
#define RENDERING_PIPELINE_CACHE_HPP

#include <string>
#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

namespace Makma3D::Rendering {
    /* The PipelineCache class, which wraps around a VkPipelineCache to speed up pipeline creation. */
    class PipelineCache {
    public:
        /* Channel name for the PipelineCache class. */
        static constexpr const char* channel = "PipelineCache";

        /* The GPU where the PipelineCache lives. */
        const Rendering::GPU& gpu;
    
    private:
        /* The VkPipelineCache we wrap. */
        VkPipelineCache vk_pipeline_cache;
        /* The path of the cache file we read from/write to. */
        std::string filepath;
    
    public:
        /* Constructor for the PipelineCache class, which takes the GPU where the cache will live and a filepath to store the cache once this class gets destructed. In case the cache file already exists, loads data from that cache first. Optionally takes create flags for the internal VkPipelineCache. */
        PipelineCache(const Rendering::GPU& gpu, const std::string& filepath, VkPipelineCacheCreateFlags create_flags = 0);
        /* Copy constructor for the PipelineCache class, which is deleted. */
        PipelineCache(const PipelineCache& other) = delete;
        /* Move constructor for the PipelineCache class. */
        PipelineCache(PipelineCache&& other);
        /* Destructor for the PipelineCache class. */
        ~PipelineCache();

        /* Explicitly returns the internal VkPipelineCache object. */
        inline const VkPipelineCache& vulkan() const { return this->vk_pipeline_cache; }
        /* Implicitly returns the internal VkPipelineCache object. */
        inline operator const VkPipelineCache&() const { return this->vk_pipeline_cache; }

        /* Copy assignment operator for the PipelineCache class, which is deleted. */
        PipelineCache& operator=(const PipelineCache& other) = delete;
        /* Move assignment operator for the PipelineCache class. */
        inline PipelineCache& operator=(PipelineCache&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelineCache class. */
        friend void swap(PipelineCache& pc1, PipelineCache& pc2);

    };
    
    /* Swap operator for the PipelineCache class. */
    void swap(PipelineCache& pc1, PipelineCache& pc2);

}

#endif
