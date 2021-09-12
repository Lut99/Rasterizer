/* PIPELINE.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 12:29:41
 * Last edited:
 *   28/06/2021, 22:26:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Implements our warpper around the pipeline that is used to define the
 *   rendering process.
**/

#ifndef RENDERING_PIPELINE_HPP
#define RENDERING_PIPELINE_HPP

#include "tools/Array.hpp"
#include "../gpu/GPU.hpp"

#include "PipelineProperties.hpp"

namespace Makma3D::Rendering {
    /* The Pipeline class, which functions a as a more convenient wrapper for the internal VkPipeline object. */
    class Pipeline {
    public:
        /* Channel name of the Pipeline class. */
        static constexpr const char* channel = "Pipeline";

        /* The GPU on which the Pipeline lives. */
        const Rendering::GPU& gpu;

    private:
        /* The actual VkPipeline object that we wrap. */
        VkPipeline vk_pipeline;
        /* The properties of this Pipeline. */
        Rendering::PipelineProperties properties;

        /* Mark the PipelinePool as a friend. */
        friend class PipelinePool;


        /* Constructor for the Pipeline class, which takes a GPU where it lives, the VkPipeline to wrap and its properties for copying purposes. */
        Pipeline(const Rendering::GPU& gpu, const VkPipeline& vk_pipeline, Rendering::PipelineProperties&& properties);
        /* Destructor for the Pipeline class. */
        ~Pipeline();

    public:
        /* Copy constructor for the Pipeline class, which is deleted. */
        Pipeline(const Pipeline& other) = delete;
        /* Move constructor for the Pipeline class, which is deleted. */
        Pipeline(Pipeline&& other) = delete;

        /* Returns a reference to the internal properties struct. */
        inline const Rendering::PipelineProperties& props() const { return this->properties; }
        /* Explicitly returns the internal VkPipeline object. */
        inline const VkPipeline& pipeline() const { return this->vk_pipeline; }
        /* Implicitly returns the internal VkPipeline object. */
        inline operator const VkPipeline&() const { return this->vk_pipeline; }

        /* Copy assignment operator for the Pipeline class, which is deleted. */
        Pipeline& operator=(const Pipeline& other) = delete;
        /* Move assignment operator for the Pipeline class, which is deleted. */
        Pipeline& operator=(Pipeline&& other) = delete;

    };

}

#endif
