/* PIPELINE.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 12:29:41
 * Last edited:
 *   20/06/2021, 12:29:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Implements our warpper around the pipeline that is used to define the
 *   rendering process.
**/

#ifndef VULKAN_PIPELINE_HPP
#define VULKAN_PIPELINE_HPP

#include <unordered_map>
#include <vulkan/vulkan.h>

#include "vulkan/auxillary/Rectangle.hpp"
#include "vulkan/auxillary/BinaryString.hpp"
#include "vulkan/gpu/GPU.hpp"
#include "vulkan/descriptors/DescriptorSetLayout.hpp"
#include "vulkan/renderpass/RenderPass.hpp"
#include "vulkan/commandbuffers/CommandBuffer.hpp"
#include "Shader.hpp"

namespace Rasterizer::Vulkan {
    /* The Pipeline class, which functions a as a more convenient wrapper for the internal VkPipeline object. */
    class Pipeline {
    public:
        /* The GPU on which the Pipeline lives. */
        const Vulkan::GPU& gpu;
    
    private:
        /* The VkPipeline object we wrap. */
        VkPipeline vk_pipeline;

        /* The list of descriptor set layouts that make up this pipeline. */
        Tools::Array<VkDescriptorSetLayout> vk_descriptor_set_layouts;
        /* The list of push constant sizes used for the layout. */
        Tools::Array<VkPushConstantRange> vk_push_constants;
        /* The create info for the pipeline layout. */
        VkPipelineLayoutCreateInfo vk_layout_info;
        /* The memory layout of the pipeline. */
        VkPipelineLayout vk_pipeline_layout;

        /* Lists all the shader modules that this pipeline uses. */
        Tools::Array<Shader> shaders;
        /* Lists all specialization map entries. */
        Tools::Array<Tools::Array<VkSpecializationMapEntry>> vk_specialization_entries;
        /* The constant datas that will be passed to the shader during compilation. */
        Tools::Array<Tools::Array<uint8_t>> vk_specialization_datas;
        /* The infos that describes the specialization for a shaders. */
        Tools::Array<VkSpecializationInfo> vk_specialization_infos;
        /* Create infos for all shader stages. */
        Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_infos;

        /* Describes how the vertex input looks like. */
        VkPipelineVertexInputStateCreateInfo vk_vertex_state_info;
        /* Describes what to do with the vertex input. */
        VkPipelineInputAssemblyStateCreateInfo vk_assembly_state_info;
        /* Describes how to stretch the resulting viewport. */
        VkViewport vk_viewport;
        /* Describes how to crop the resulting viewport. */
        VkRect2D vk_scissor;
        /* Combines vk_viewport and vk_scissor into one create info. */
        VkPipelineViewportStateCreateInfo vk_viewport_state_info;
        /* Describes how the rasterizer stage shall behave. */
        VkPipelineRasterizationStateCreateInfo vk_rasterizer_state_info;
        /* Describes how the pipeline handles multisampling. */
        VkPipelineMultisampleStateCreateInfo vk_multisample_state_info;
        /* Describes how the color blending works per output framebuffer. */
        Tools::Array<VkPipelineColorBlendAttachmentState> vk_color_blending;
        /* Describes how the general color blending works. */
        VkPipelineColorBlendStateCreateInfo vk_color_state_info;

    public:
        /* Constructor for the Pipeline class, which takes the GPU on which it should be created. Note that, unlike in other modules, the Pipeline's constructor does not leave it usable; instead, call the series of other initalizations functions as well, after which finalize() can be called. */
        Pipeline(const Vulkan::GPU& gpu);
        /* Copy constructor for the Pipeline class. */
        Pipeline(const Pipeline& other);
        /* Move constructor for the Pipeline class. */
        Pipeline(Pipeline&& other);
        /* Destructor for the Pipeline class. */
        ~Pipeline();

        /* Loads a shader in the given shader stage mask. */
        void init_shader_stage(VkShaderStageFlagBits vk_shader_stage, const Vulkan::Shader& shader, const std::unordered_map<uint32_t, BinaryString>& specialization_constants = {});
        /* Tells the Pipeline how its vertex input looks like. */
        void init_vertex_input();
        /* Tells the Pipeline what to do with the vertex we gave it. The topology specifies the geometry of the vertices (i.e., draw points, lines, triangles), and restart_enable does something that is beyond my knowledge. */
        void init_input_assembly(VkPrimitiveTopology topology, VkBool32 restart_enable = VK_FALSE);
        /* Tells the Pipeline how the render the output frame. The Viewport is used to stretch it, and the Scissor is used to cut it off. */
        void init_viewport_transformation(const Rectangle& viewport, const Rectangle& scissor);
        /* Tells the Pipeline how to configure the Rasterizer stage.
         *
         * @param cull_mode Describes how to cull faces that are behind others
         * @param front_face Describes what the front face of an object is (i.e., which side to cull)
         * @param depth_clamp Describes whether to clamp objects that are too far or too near to their plane instead of simply not rendering them. Needs a special GPU feature to enable.
         * @param polygon_mode Describes how to "colour" the given geometry. For example, can be set to fill the whole vertex, or only its lines, etc. Needs a special GPU feature for anything other than FILL.
         * @param line_width The width of the lines that the rasterizer draws. Needs a special GPU feature to grow beyond 1.0f.
         * @param disable_rasterizer If set to TRUE, then discards the output of the rasterizer, disabling it in effect.
         */
        void init_rasterizer(VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp = VK_FALSE, VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, float line_width = 1.0f, VkBool32 disable_rasterizer = VK_FALSE);
        /* Tells the pipeline how to handle multi-sampling. For now, perpetuably disabled. */
        void init_multisampling();
        /* Tells the Pipeline how to blend the new, computed color of a pixel with the one already in the framebuffer. This is per output framebuffer, and may thus need to be called multiple times.
         *
         * @param framebuffer The index of the framebuffer for which these settings hold.
         * @param enabled_blending Whether or not to enable color blending.
         * @param src_color_factor How much weight the new color will have while blending
         * @param dst_color_factor How much weight the existing color will have while blending
         * @param color_op How to combine the two colors into one
         * @param src_alpha_factor How much weight the new alpha value will have while blending
         * @param dst_alpha_factor How much weight the existing alpha value will have while blending
         * @param alpha_op How to combine the two alpha channels into one
         */
        void init_color_blending(uint32_t framebuffer, VkBool32 enable_blending, VkBlendFactor src_color_factor, VkBlendFactor dst_color_factor, VkBlendOp color_op, VkBlendFactor src_alpha_factor, VkBlendFactor dst_alpha_factor, VkBlendOp alpha_op);
        /* Tells the Pipeline how to generally blend the new, computed color of a pixel with the one already there. This one uses logical operations instead of a blending one, and holds for ALL framebuffers.
         * Note that if set to true, any framebuffer-specific operations will be ignored.
         * Also note that this function must be called, as it solidifies the usage of the other color blending functions as well.
         *
         * @param enable_logic Whether or not this global logic operator should be enabled.
         * @param logic_op Which logic operation to apply.
         */
        void init_color_logic(VkBool32 enable_logic, VkLogicOp logic_op);
        /* Initiates the pipeline layout based on the list of descriptor set layouts and on the list of push constants. The latter is a list of pairs, each describing the shader stages where they can be accessed and the size of each constant. */
        void init_pipeline_layout(const Tools::Array<DescriptorSetLayout>& layouts, const Tools::Array<std::pair<VkShaderStageFlags, uint32_t>>& push_constants);
        /* When called, completes the pipeline with the settings given by the other initialization functions. */
        void finalize(const Vulkan::RenderPass& render_pass, uint32_t first_subpass);

        /* Schedules the pipeline to be run and thus drawn in the given command buffer. Optionally takes another bind point. */
        void schedule(const Vulkan::CommandBuffer& cmd, VkPipelineBindPoint vk_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);
        /* Schedules the draw call for the pipeline, with the given numer of vertices, instances and vertex & instance offset. */
        void schedule_draw(const Vulkan::CommandBuffer& cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex = 0, uint32_t first_instance = 0);

        /* Expliticly returns the internal VkPipeline object. */
        inline const VkPipeline& pipeline() const { return this->vk_pipeline; }
        /* Implicitly returns the internal VkPipeline object. */
        inline operator VkPipeline() const { return this->vk_pipeline; }

        /* Copy assignment operator for the Pipeline class. */
        inline Pipeline& operator=(const Pipeline& other) { return *this = Pipeline(other); }
        /* Move assignment operator for the Pipeline class. */
        inline Pipeline& operator=(Pipeline&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Pipeline class. */
        friend void swap(Pipeline& p1, Pipeline& p2);

    };

    /* Swap operator for the Pipeline class. */
    void swap(Pipeline& p1, Pipeline& p2);

}

#endif
