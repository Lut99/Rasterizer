/* PIPELINE POOL.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:06:33
 * Last edited:
 *   11/09/2021, 14:06:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelinePool, which is a class that is in charge of
 *   managing and, more importantly, spawning new pipelines in an efficient
 *   way.
**/

#ifndef RENDERING_PIPELINE_POOL_HPP
#define RENDERING_PIPELINE_POOL_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "../gpu/GPU.hpp"
#include "../renderpass/RenderPass.hpp"

#include "PipelineProperties.hpp"

#include "Pipeline.hpp"

namespace Makma3D::Rendering {
    /* The PipelinePool class, which aims to spawn new pipelines in efficient ways and which then manages them. */
    class PipelinePool {
    public:
        /* Channel name for the PipelinePool class. */
        static constexpr const char* channel = "PipelinePool";

        /* Reference to the device where all the pipelines live. */
        const Rendering::GPU& gpu;
    
    private:
        /* Helper struct that groups everything we need for a preallocation into one object. */
        struct PipelinePreallocation {
            /* The properties of the to-be-allocated pipeline. */
            Rendering::PipelineProperties properties;
            /* The list of casted shader stages. */
            Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_stages;
            /* The VkGraphicsPipelineCreateInfo that combines all of the information above. */
            VkGraphicsPipelineCreateInfo vk_pipeline_info;

            /* Constructor for the PipelinePreallocation class, which moves the given properties to itself. */
            PipelinePreallocation(Rendering::PipelineProperties&& properties) :
                properties(std::move(properties))
            {}
        };

    private:
        /* List of Pipelines allocated with this pool. */
        Tools::Array<Rendering::Pipeline*> pipelines;

        /* List of PipelinePreallocation objects. */
        Tools::Array<PipelinePreallocation> pipeline_preallocations;

    public:
        /* Constructor for the PipelinePool class, which takes a GPU where all the pipelines will live. */
        PipelinePool(const Rendering::GPU& gpu);
        /* Copy constructor for the PipelinePool class. */
        PipelinePool(const PipelinePool& other);
        /* Move constructor for the PipelinePool class. */
        PipelinePool(PipelinePool&& other);
        /* Destructor for the PipelinePool class. */
        ~PipelinePool();

        /* Spawns a completely new pipeline with the given properties and the given render pass. Will probably be relatively slow compared to other operations, but doesn't depent on other pipeline objects. Copies the properties first. */
        inline Rendering::Pipeline* allocate(const Rendering::PipelineProperties& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) { return this->allocate(Rendering::PipelineProperties(properties), render_pass, first_subpass); }
        /* Spawns a completely new pipeline with the given properties and the given render pass. Will probably be relatively slow compared to other operations, but doesn't depent on other pipeline objects. */
        Rendering::Pipeline* allocate(Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass);
        /* Spawns a new pipeline that is based on the given pipeline. Works best if you copy the pipeline's PipelineProperties, and then adapt that so they are as alike as possible. Copies the properties first. */
        inline Rendering::Pipeline* allocate(const Rendering::Pipeline* pipeline, const Rendering::PipelineProperties& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) { return this->allocate(pipeline, Rendering::PipelineProperties(properties), render_pass, first_subpass); }
        /* Spawns a new pipeline that is based on the given pipeline. Works best if you copy the pipeline's PipelineProperties, and then adapt that so they are as alike as possible. Copies the properties first. */
        Rendering::Pipeline* allocate(const Rendering::Pipeline* pipeline, Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass);
        /* Pre-allocates a new pipeline with the given properties and the given render pass. Although it's still from scratch and relatively slow, it does allow to bulk-allocate multiple pipeline objects. Calling nallocate() will return the pipeline(s) created with the preallocate function since its last call. Copies the properties first. */
        inline void preallocate(const Rendering::PipelineProperties& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass) { return this->preallocate(Rendering::PipelineProperties(properties), render_pass, first_subpass); }
        /* Pre-allocates a new pipeline with the given properties and the given render pass. Although it's still from scratch and relatively slow, it does allow to bulk-allocate multiple pipeline objects. Calling nallocate() will return the pipeline(s) created with the preallocate function since its last call. */
        void preallocate(Rendering::PipelineProperties&& properties, const Rendering::RenderPass& render_pass, uint32_t first_subpass);
        /* Allocates all pipelines preallocated with preallocate() since either the start of the PipelinePool or the last nallocate() call. */
        Tools::Array<Rendering::Pipeline*> nallocate();

        /* Frees the given Pipeline object. */
        void free(const Rendering::Pipeline* pipeline);
        /* Frees a list of given pipeline objects. */
        void nfree(const Tools::Array<Rendering::Pipeline*>& pipelines);

        /* Resets the pool to an empty state, by deleting all the allocated pipelines. All objectes allocated with it will thus become invalid. */
        void reset();

        /* Copy assignment operator for the PipelinePool class. */
        inline PipelinePool& operator=(const PipelinePool& other) { return *this = PipelinePool(other); }
        /* Move assignment operator for the PipelinePool class. */
        inline PipelinePool& operator=(PipelinePool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the PipelinePool class. */
        friend void swap(PipelinePool& pp1, PipelinePool& pp2);

    };

    /* Swap operator for the PipelinePool class. */
    void swap(PipelinePool& pp1, PipelinePool& pp2);

}

#endif
