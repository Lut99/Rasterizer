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

#include "properties/ShaderStage.hpp"

namespace Makma3D::Rendering {
    /* The Pipeline class, which functions a as a more convenient wrapper for the internal VkPipeline object. */
    class Pipeline {
    public:
        /* Channel name of the Pipeline class. */
        static constexpr const char* channel = "Pipeline";

        /* The GPU on which the Pipeline lives. */
        const Rendering::GPU& gpu;

    private:
        /* Constructor for the Pipeline class, which takes the GPU on which it should be created, a list of ShaderStages to initialize. */
        Pipeline(const Rendering::GPU& gpu, const Tools::Array<Rendering::ShaderStage>& shader_stages);
        /* Destructor for the Pipeline class. */
        ~Pipeline();

    public:
        /* Copy constructor for the Pipeline class, which is deleted. */
        Pipeline(const Pipeline& other) = delete;
        /* Move constructor for the Pipeline class, which is deleted. */
        Pipeline(Pipeline&& other) = delete;

        /* Copy assignment operator for the Pipeline class, which is deleted. */
        Pipeline& operator=(const Pipeline& other) = delete;
        /* Move assignment operator for the Pipeline class, which is deleted. */
        Pipeline& operator=(Pipeline&& other) = delete;

    };

}

#endif
