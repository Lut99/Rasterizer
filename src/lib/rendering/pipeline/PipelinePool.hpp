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

#include "properties/ShaderStage.hpp"
#include "properties/InputAssemblyState.hpp"
#include "properties/DepthTesting.hpp"
#include "properties/ViewportTransformation.hpp"
#include "properties/Rasterization.hpp"
#include "properties/Multisampling.hpp"
#include "properties/ColorLogic.hpp"

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
        /* List of Pipelines allocated with this pool. */
        Tools::Array<Rendering::Pipeline*> pipelines;

    public:
        /* Constructor for the PipelinePool class, which takes a GPU where all the pipelines will live. */
        PipelinePool(const Rendering::GPU& gpu);
        /* Copy constructor for the PipelinePool class. */
        PipelinePool(const PipelinePool& other);
        /* Move constructor for the PipelinePool class. */
        PipelinePool(PipelinePool&& other);
        /* Destructor for the PipelinePool class. */
        ~PipelinePool();

        /* Spawns a completely new pipeline with the given properties. Will probably be relatively slow compared to other operations, but doesn't depent on other pipeline objects.
         *
         * @param shader_stages A list of ShaderStages to initialize, each containing custom shader code
         * @param input_assembly_state The description of what to do with the input vertices
         * @param depth_testing Whether to do depth testing or not and, if so, how so
         * @param viewport_transformation How the resulting viewport is sized/cutoff
         * @param rasterization What to do during the rasterization stage
         * @param multisampling How the pipeline should deal with multisampling
         * @param color_logic How to deal with pixels already present in the target framebuffer(s)
         */
        Rendering::Pipeline* allocate(const Tools::Array<Rendering::ShaderStage>& shader_stages,
                                      const Rendering::InputAssemblyState& input_assembly_state,
                                      const Rendering::DepthTesting& depth_testing,
                                      const Rendering::ViewportTransformation& viewport_transformation,
                                      const Rendering::Rasterization& rasterization,
                                      const Rendering::Multisampling& multisampling,
                                      const Rendering::ColorLogic& color_logic);
        /* Frees the given Pipeline object. */
        void free(Rendering::Pipeline* pipeline);

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
