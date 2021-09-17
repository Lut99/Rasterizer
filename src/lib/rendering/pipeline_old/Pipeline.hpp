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
#include "../commandbuffers/CommandBuffer.hpp"

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

        /* Binds the pipeline to a given command buffer for future draw calls. */
        inline void bind(const Rendering::CommandBuffer* cmd, VkPipelineBindPoint vk_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS) const { vkCmdBindPipeline(cmd->command_buffer(), vk_bind_point, this->vk_pipeline); }
        /* Schedules the given value as one of the push constants on the given command buffer. The shader stage and offset determine which push constant, while the data and data_size relate to the push constant's value. */
        inline void schedule_push_constant(const Rendering::CommandBuffer* cmd, VkShaderStageFlags shader_stage, uint32_t offset, void* data, uint32_t data_size) const { vkCmdPushConstants(cmd->command_buffer(), this->properties.pipeline_layout.layout(), shader_stage, offset, data_size, data); }
        /* Schedules the given value as one of the push constants on the given command buffer. The shader stage and offset determine which push constant, while the data and data_size relate to the push constant's value. The element's size is automatically deduced from its type. */
        template <class T> inline void schedule_push_constant(const Rendering::CommandBuffer* cmd, VkShaderStageFlags shader_stage, uint32_t offset, const T& data) const { return this->schedule_push_constant(cmd, shader_stage, offset, (void*) &data, sizeof(T)); }
        /* Schedules a draw for this pipeline with the given number of vertices and the given number of instances. Optionally, an offset can be given in either arrays. */
        inline void schedule_draw(const Rendering::CommandBuffer* cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex = 0, uint32_t first_instance = 0) const { vkCmdDraw(cmd->command_buffer(), vertex_count, instance_count, first_vertex, first_instance); }
        /* Schedules an indexed draw for this pipeline with the given number of indices and the given number of instances. Optionally, an offset can be given in any of the three arrays. */
        inline void schedule_idraw(const Rendering::CommandBuffer* cmd, uint32_t index_count, uint32_t instance_count, uint32_t first_vertex = 0, uint32_t first_index = 0, uint32_t first_instance = 0) const { vkCmdDrawIndexed(cmd->command_buffer(), index_count, instance_count, first_index, first_vertex, first_instance); }

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
