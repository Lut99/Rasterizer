/* PIPELINE CONSTRUCTOR.cpp
 *   by Lut99
 *
 * Created:
 *   18/09/2021, 11:41:08
 * Last edited:
 *   18/09/2021, 11:41:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "infos/PipelineInfo.hpp"
#include "PipelineConstructor.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** PIPELINECONSTRUCTOR CLASS *****/
/* Constructor for the PipelineConstructor class, which takes a GPU to create the pipelines on and a PipelineCache we use for creating its pipelines. */
PipelineConstructor::PipelineConstructor(const Rendering::GPU& gpu, const Rendering::PipelineCache& pipeline_cache) :
    gpu(gpu),
    pipeline_cache(pipeline_cache),

    base_pipeline(nullptr)
{}

/* Copy constructor for the PipelineConstructor class. */
PipelineConstructor::PipelineConstructor(const PipelineConstructor& other) :
    gpu(other.gpu),
    pipeline_cache(other.pipeline_cache),

    base_pipeline(other.base_pipeline),

    shaders(other.shaders),
    vertex_input_state(other.vertex_input_state),
    input_assembly_state(other.input_assembly_state),
    depth_testing(other.depth_testing),
    viewport_transformation(other.viewport_transformation),
    rasterization(other.rasterization),
    multisampling(other.multisampling),
    color_logic(other.color_logic),
    pipeline_layout(other.pipeline_layout)
{}

/* Move constructor for the PipelineConstructor class. */
PipelineConstructor::PipelineConstructor(PipelineConstructor&& other) :
    gpu(other.gpu),
    pipeline_cache(other.pipeline_cache),

    base_pipeline(other.base_pipeline),

    shaders(std::move(other.shaders)),
    vertex_input_state(std::move(other.vertex_input_state)),
    input_assembly_state(std::move(other.input_assembly_state)),
    depth_testing(std::move(other.depth_testing)),
    viewport_transformation(std::move(other.viewport_transformation)),
    rasterization(std::move(other.rasterization)),
    multisampling(std::move(other.multisampling)),
    color_logic(std::move(other.color_logic)),
    pipeline_layout(std::move(other.pipeline_layout))
{}

/* Destructor for the PipelineConstructor class. */
PipelineConstructor::~PipelineConstructor() {}



/* Creates a new Pipeline with the internal properties and the given RenderPass & first subpass. Optionally takes create flags for the VkPipeline, too. */
Rendering::Pipeline* PipelineConstructor::construct(const Rendering::RenderPass& render_pass, uint32_t first_subpass, VkPipelineCreateFlags create_flags) const {
    VkResult vk_result;

    // Create the PipelineInfo for this pipeline
    PipelineInfo pipeline_info(
        this->shaders,
        this->vertex_input_state,
        this->input_assembly_state,
        this->depth_testing,
        this->viewport_transformation,
        this->rasterization,
        this->multisampling,
        this->color_logic,
        this->pipeline_layout,
        render_pass, first_subpass,
        this->base_pipeline != nullptr ? this->base_pipeline->vulkan() : VK_NULL_HANDLE, -1,
        create_flags
    );

    // Create the pipeline layout based on its create info
    VkPipelineLayout vk_pipeline_layout;
    if ((vk_result = vkCreatePipelineLayout(this->gpu, pipeline_info.vk_pipeline_layout_info, nullptr, &vk_pipeline_layout)) != VK_SUCCESS) {
        logger.fatalc(PipelineConstructor::channel, "Could not create pipeline layout: ", vk_error_map[vk_result]);
    }
    pipeline_info.vk_pipeline_info.layout = vk_pipeline_layout;

    // With the layout, we can create the pipeline
    VkPipeline vk_pipeline;
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, this->pipeline_cache, 1, &pipeline_info.vk_pipeline_info, nullptr, &vk_pipeline)) != VK_SUCCESS) {
        logger.fatalc(PipelineConstructor::channel, "Could not create pipeline: ", vk_error_map[vk_result]);
    }

    // Wrap it in a Pipeline class and we're as good as done
    return new Pipeline(this->gpu, vk_pipeline, vk_pipeline_layout);
}

/* Creates N new Pipelines with the internal properties and the given RenderPass & first subpass. Optionally takes create flags for the VkPipeline, too. */
Tools::Array<Rendering::Pipeline*> PipelineConstructor::nconstruct(uint32_t N, const Rendering::RenderPass& render_pass, uint32_t first_subpass, VkPipelineCreateFlags create_flags) const {
    VkResult vk_result;

    // Create the PipelineInfo for each pipeline
    PipelineInfo pipeline_info(
        this->shaders,
        this->vertex_input_state,
        this->input_assembly_state,
        this->depth_testing,
        this->viewport_transformation,
        this->rasterization,
        this->multisampling,
        this->color_logic,
        this->pipeline_layout,
        render_pass, first_subpass,
        this->base_pipeline != nullptr ? this->base_pipeline->vulkan() : VK_NULL_HANDLE, -1,
        create_flags
    );

    // Copy it into a list of N copies, each with their own pipeline layouts
    VkGraphicsPipelineCreateInfo* vk_pipeline_infos = new VkGraphicsPipelineCreateInfo[N];
    VkPipelineLayout* vk_pipeline_layouts = new VkPipelineLayout[N];
    for (uint32_t i = 0; i < N; i++) {
        // Do the copy
        vk_pipeline_infos[i] = pipeline_info.vk_pipeline_info;

        // Create and assign the layout
        if ((vk_result = vkCreatePipelineLayout(this->gpu, pipeline_info.vk_pipeline_layout_info, nullptr, vk_pipeline_layouts + i)) != VK_SUCCESS) {
            logger.fatalc(PipelineConstructor::channel, "Could not create pipeline layout ", i, ": ", vk_error_map[vk_result]);
        }
        vk_pipeline_infos[i].layout = vk_pipeline_layouts[i];
    }

    // With the layout, we can create the pipelines
    VkPipeline* vk_pipelines = new VkPipeline[N];
    if ((vk_result = vkCreateGraphicsPipelines(this->gpu, this->pipeline_cache, N, vk_pipeline_infos, nullptr, vk_pipelines)) != VK_SUCCESS) {
        logger.fatalc(PipelineConstructor::channel, "Could not create ", N, " pipelines: ", vk_error_map[vk_result]);
    }
    delete[] vk_pipeline_infos;

    // Wrap it in a list of Pipeline classes
    Tools::Array<Rendering::Pipeline*> result(N);
    for (uint32_t i = 0; i < N; i++) {
        result.push_back(new Pipeline(this->gpu, vk_pipelines[i], vk_pipeline_layouts[i]));
    }

    // Clean the remaining arrays and return
    delete[] vk_pipeline_layouts;
    delete[] vk_pipelines;
    return std::move(result);
}



/* Swap operator for the PipelineConstructor class. */
void Rendering::swap(PipelineConstructor& pc1, PipelineConstructor& pc2) {
    #ifndef NDEBUG
    if (pc1.gpu != pc2.gpu) { logger.fatalc(PipelineConstructor::channel, "Cannot swap pipeline constructors with different GPUs"); }
    if (&pc1.pipeline_cache != &pc2.pipeline_cache) { logger.fatalc(PipelineConstructor::channel, "Cannot swap pipeline constructors with different pipeline caches"); }
    #endif

    using std::swap;

    swap(pc1.base_pipeline, pc2.base_pipeline);

    swap(pc1.shaders, pc2.shaders);
    swap(pc1.vertex_input_state, pc2.vertex_input_state);
    swap(pc1.input_assembly_state, pc2.input_assembly_state);
    swap(pc1.depth_testing, pc2.depth_testing);
    swap(pc1.viewport_transformation, pc2.viewport_transformation);
    swap(pc1.rasterization, pc2.rasterization);
    swap(pc1.multisampling, pc2.multisampling);
    swap(pc1.color_logic, pc2.color_logic);
    swap(pc1.pipeline_layout, pc2.pipeline_layout);
}





/***** LIBRARY FUNCTIONS *****/
// /* Static function that takes a list of PipelineConstructors and uses the information in them to create that many new pipelines at once. */
// Tools::Array<Rendering::Pipeline*> Rendering::joint_construct(const Tools::Array<PipelineConstructor>& constructors, const Tools::Array<RenderPass>& render_passes, const Tools::Array<uint32_t>& first_subpasses, const Tools::Array<VkPipelineCreateFlags>& create_flags) {
//     VkResult vk_result;

//     // If there is no list, early quit
//     if (constructors.size() == 0) {
//         logger.warningc(PipelineConstructor::channel, "Called joint_construct() with an empty list of constructors.");
//         return {};
//     }

//     // Create a list of PipelineInfos for each constructor
//     Tools::Array<PipelineInfo> pipeline_infos(constructors.size());
//     VkPipelineLayout* vk_pipeline_layouts = new VkPipelineLayout[constructors.size()];
//     VkGraphicsPipelineCreateInfo* vk_pipeline_infos = new VkGraphicsPipelineCreateInfo[constructors.size()];
//     for (uint32_t i = 0; i < constructors.size(); i++) {
//         // Create the pipeline info
//         pipeline_infos.push_back(PipelineInfo(
//             constructors[i].shaders,
//             constructors[i].vertex_input_state,
//             constructors[i].input_assembly_state,
//             constructors[i].depth_testing,
//             constructors[i].viewport_transformation,
//             constructors[i].rasterization,
//             constructors[i].multisampling,
//             constructors[i].color_logic,
//             constructors[i].pipeline_layout,
//             render_passes[i], first_subpasses[i],
//             constructors[i].base_pipeline != nullptr ? constructors[i].base_pipeline->vulkan() : VK_NULL_HANDLE, -1,
//             create_flags[i]
//         ));
//         // Copy the vulkan info to the list of them
//         vk_pipeline_infos[i] = pipeline_infos[i].vk_pipeline_info;

//         // Create the pipeline layout for this info and assign it to the graphics pipeline info
//         if ((vk_result = vkCreatePipelineLayout(constructors[i].gpu, pipeline_infos[i].vk_pipeline_layout_info, nullptr, vk_pipeline_layouts + i)) != VK_SUCCESS) {
//             logger.fatalc(PipelineConstructor::channel, "Could not create pipeline layout ", i, ": ", vk_error_map[vk_result]);
//         }
//         vk_pipeline_infos[i].layout = vk_pipeline_layouts[i];
//     }

//     // With the layouts and infos prepared, we can create the pipelines
//     VkPipeline* vk_pipelines = new VkPipeline[constructors.size()];
//     if ((vk_result = vkCreateGraphicsPipelines(constructors.first().gpu, constructors.first().pipeline_cache, constructors.size(), vk_pipeline_infos, nullptr, vk_pipelines)) != VK_SUCCESS) {
//         logger.fatalc(PipelineConstructor::channel, "Could not create ", constructors.size(), " different pipelines: ", vk_error_map[vk_result]);
//     }
//     delete[] vk_pipeline_infos;

//     // Wrap it in a list of Pipeline classes
//     Tools::Array<Rendering::Pipeline*> result(constructors.size());
//     for (uint32_t i = 0; i < constructors.size(); i++) {
//         result.push_back(new Pipeline(constructors.first().gpu, vk_pipelines[i], vk_pipeline_layouts[i]));
//     }

//     // Clean the remaining arrays and return
//     delete[] vk_pipeline_layouts;
//     delete[] vk_pipelines;
//     return std::move(result);
// }
