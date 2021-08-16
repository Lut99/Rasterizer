/* RENDER SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   20/07/2021, 15:10:25
 * Last edited:
 *   07/08/2021, 15:23:48
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the rendering system, which is capable of showing the entities
 *   that have a Render component on the screen. Also uses the Transform
 *   component to decide where to place the entity.
**/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "tools/CppDebugger.hpp"

#include "ecs/components/Transform.hpp"
#include "ecs/components/Meshes.hpp"
#include "ecs/components/Camera.hpp"
#include "models/ModelSystem.hpp"

#include "auxillary/ErrorCodes.hpp"
#include "auxillary/Rectangle.hpp"
#include "auxillary/Vertex.hpp"

#include "RenderSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSubmitInfo struct. */
static void populate_submit_info(VkSubmitInfo& submit_info, const CommandBuffer* cmd, const Semaphore& wait_for_semaphore,  const Tools::Array<VkPipelineStageFlags>& wait_for_stages, const Semaphore& signal_after_semaphore) {
    DENTER("populate_submit_info");

    // Set to default
    submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Attach the command buffer
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd->command_buffer();

    // Attach the data for which we wait
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &wait_for_semaphore.semaphore();
    submit_info.pWaitDstStageMask = wait_for_stages.rdata();

    // Attach the semaphores which we signal when done
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_after_semaphore.semaphore();

    // Done
    DRETURN;
}

/* Populates the given VkPresentInfo struct. */
static void populate_present_info(VkPresentInfoKHR& present_info, const Swapchain& swapchain, const uint32_t& swapchain_index, const Semaphore& wait_for_semaphore) {
    DENTER("populate_present_info");

    // Set to default
    present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Add which swapchain and which image to use
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain.swapchain();
    present_info.pImageIndices = &swapchain_index;
    
    // Set the semaphore count
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &wait_for_semaphore.semaphore();

    // Also set the results validation handlers to not needed, since we only have one swapchain
    present_info.pResults = nullptr;

    // Done
    DRETURN;
}





/***** RENDERSYSTEM CLASS *****/
/* Constructor for the RenderSystem, which takes a window, a memory manager to render (to and draw memory from, respectively) and a model system to schedule the model buffers with. */
RenderSystem::RenderSystem(Window& window, MemoryManager& memory_manager, const Models::ModelSystem& model_system) :
    window(window),
    memory_manager(memory_manager),
    model_system(model_system),

    depth_stencil(this->window.gpu(), this->memory_manager.draw_pool, this->window.swapchain().extent()),
    framebuffers(this->window.swapchain().size()),

    vertex_shader(this->window.gpu(), "bin/shaders/vertex_v3.spv"),
    fragment_shader(this->window.gpu(), "bin/shaders/frag_v1.spv"),

    render_pass(this->window.gpu()),
    pipeline(this->window.gpu()),

    draw_cmds(this->memory_manager.draw_cmd_pool.nallocate(this->window.swapchain().size())),

    image_ready_semaphores(Semaphore(this->window.gpu()), RenderSystem::max_frames_in_flight),
    render_ready_semaphores(Semaphore(this->window.gpu()), RenderSystem::max_frames_in_flight),
    frame_in_flight_fences(Fence(this->window.gpu(), VK_FENCE_CREATE_SIGNALED_BIT), RenderSystem::max_frames_in_flight),
    image_in_flight_fences((Fence*) nullptr, this->window.swapchain().size()),

    current_frame(0)
{
    DENTER("Rendering::RenderSystem::RenderSystem");

    // Initialize the render pass
    uint32_t col_index = this->render_pass.add_attachment(this->window.swapchain().format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    uint32_t dep_index = this->render_pass.add_attachment(this->depth_stencil.format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    this->render_pass.add_subpass({ std::make_pair(col_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }, std::make_pair(dep_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    this->render_pass.add_dependency(VK_SUBPASS_EXTERNAL, col_index, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    this->render_pass.finalize();

    // Initialize the frame buffers
    for (uint32_t i = 0; i < this->window.swapchain().size(); i++) {
        this->framebuffers.push_back(this->window.swapchain().get_framebuffer(i, this->render_pass, this->depth_stencil));
    }

    // Prepare the pipeline by choosing all its settings
    pipeline.init_shader_stage(this->vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
    pipeline.init_shader_stage(this->fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline.init_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline.init_depth_testing(VK_TRUE, VK_COMPARE_OP_LESS);
    pipeline.init_viewport_transformation(Rectangle(0.0, 0.0, this->window.swapchain().extent()), Rectangle(0.0, 0.0, this->window.swapchain().extent()));
    pipeline.init_rasterizer(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE); //VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    pipeline.init_multisampling();
    pipeline.init_color_blending(0, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    pipeline.init_color_logic(VK_FALSE, VK_LOGIC_OP_NO_OP);
    pipeline.init_pipeline_layout({ {} }, { { VK_SHADER_STAGE_VERTEX_BIT, (uint32_t) (sizeof(glm::mat4)) } });
    pipeline.finalize(this->render_pass, 0);

    // Done initializing
    DLOG(info, "Intialized the RenderSystem.");
    DLEAVE;
}

/* Copy constructor for the RenderSystem class. */
RenderSystem::RenderSystem(const RenderSystem& other) :
    window(other.window),
    memory_manager(other.memory_manager),
    model_system(other.model_system),

    depth_stencil(other.depth_stencil),
    framebuffers(other.framebuffers),

    vertex_shader(other.vertex_shader),
    fragment_shader(other.fragment_shader),

    render_pass(other.render_pass),
    pipeline(other.pipeline),

    draw_cmds(other.draw_cmds),

    image_ready_semaphores(other.image_ready_semaphores),
    render_ready_semaphores(other.render_ready_semaphores),
    frame_in_flight_fences(other.frame_in_flight_fences),
    image_in_flight_fences(other.image_in_flight_fences),

    current_frame(other.current_frame)
{
    DENTER("Rendering::RenderSystem::RenderSystem(copy)");

    // Nothing as of yet

    DLEAVE;
}

/* Move constructor for the RenderSystem class. */
RenderSystem::RenderSystem(RenderSystem&& other)  :
    window(other.window),
    memory_manager(other.memory_manager),
    model_system(other.model_system),

    depth_stencil(other.depth_stencil),
    framebuffers(other.framebuffers),

    vertex_shader(other.vertex_shader),
    fragment_shader(other.fragment_shader),

    render_pass(other.render_pass),
    pipeline(other.pipeline),

    draw_cmds(other.draw_cmds),

    image_ready_semaphores(other.image_ready_semaphores),
    render_ready_semaphores(other.render_ready_semaphores),
    frame_in_flight_fences(other.frame_in_flight_fences),
    image_in_flight_fences(other.image_in_flight_fences),

    current_frame(other.current_frame)
{}

/* Destructor for the RenderSystem class. */
RenderSystem::~RenderSystem() {
    DENTER("Rendering::RenderSystem::~RenderSystem");

    // Nothing as of yet

    DLEAVE;
}



/* Private helper function that resizes all required structures for a new window size. */
void RenderSystem::_resize() {
    DENTER("Rendering::RenderSystem::_resize");

    // First, wait until the device is idle
    this->window.gpu().wait_for_idle();

    // Then, resize the window
    this->window.resize();

    // Re-create the depth stencil with a new size
    this->depth_stencil.resize(this->window.real_extent());

    // Fetch a new list of framebuffers
    this->framebuffers.clear();
    this->framebuffers.reserve(this->window.swapchain().size());
    for (uint32_t i = 0; i < this->window.swapchain().size(); i++) {
        this->framebuffers.push_back(this->window.swapchain().get_framebuffer(i, this->render_pass, this->depth_stencil));
    }

    // Done
    DRETURN;
}



/* Runs a single iteration of the game loop. Returns whether or not the RenderSystem is asked to close the window (false) or not (true). */
bool RenderSystem::render_frame(const ECS::EntityManager& entity_manager) {
    DENTER("Rendering::RenderSystem::render_frame");

    /* PREPARATION */
    // First, do the window
    bool can_continue = this->window.loop();
    if (!can_continue) {
        DRETURN false;
    }

    // Before we get the swapchain images, be sure to wait for the current frame to be available
    vkWaitForFences(this->window.gpu(), 1, &this->frame_in_flight_fences[this->current_frame].fence(), VK_TRUE, UINT64_MAX);

    // Next, try to get a new swapchain image
    uint32_t swapchain_index;
    VkResult vk_result = vkAcquireNextImageKHR(this->window.gpu(), this->window.swapchain(), UINT64_MAX, this->image_ready_semaphores[this->current_frame], VK_NULL_HANDLE, &swapchain_index);
    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR) {
        // The swapchain is outdated or suboptimal (probably a resize); we resize to fit again
        this->_resize();

        // Next, we early quit, to make sure that the proper images are acquired
        DRETURN true;
    } else if (vk_result != VK_SUCCESS) {
        DLOG(fatal, "Could not get image from swapchain: " + vk_error_map[vk_result]);
    }

    // If another frame is already using this image, then wait for it to become available
    if (this->image_in_flight_fences[swapchain_index] != (Fence*) nullptr) {
        vkWaitForFences(this->window.gpu(), 1, &this->image_in_flight_fences[swapchain_index]->fence(), VK_TRUE, UINT64_MAX);
    }
    this->image_in_flight_fences[swapchain_index] = &this->frame_in_flight_fences[this->current_frame];



    /***** RENDERING *****/
    // Get the camera matrix
    const glm::mat4& cam_mat = entity_manager.get_list<Camera>()[0].proj_view;

    // Prepare the command buffer's recording
    this->draw_cmds[swapchain_index]->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    this->render_pass.start_scheduling(this->draw_cmds[swapchain_index], this->framebuffers[swapchain_index]);
    this->pipeline.schedule(this->draw_cmds[swapchain_index]);

    /* Do draw calls for each entity's group. */
    const ECS::ComponentList<ECS::Meshes>& list = entity_manager.get_list<ECS::Meshes>();
    Tools::Array<glm::mat4> cam_matrices(cam_mat, list.size());
    for (ECS::component_list_size_t i = 0; i < list.size(); i++) {
        // Get the relevant components for this entity
        const ECS::Meshes& meshes = list[i];
        const ECS::Transform& transform = entity_manager.get_component<Transform>(list.get_entity(i));

        // Compute the camera matrix for this entity
        cam_matrices[i] *= transform.translation;

        // Record the command buffer for this entity's groups
        this->pipeline.schedule_push_constants(this->draw_cmds[swapchain_index], VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), (void*) &cam_matrices[i]);
        for (uint32_t j = 0; j < meshes.size(); j++) {
            this->model_system.schedule(this->draw_cmds[swapchain_index], meshes[j]);
            this->pipeline.schedule_draw_indexed(this->draw_cmds[swapchain_index], meshes[j].n_indices, 1);
        }
    }

    // Finish recording
    this->render_pass.stop_scheduling(this->draw_cmds[swapchain_index]);
    this->draw_cmds[swapchain_index]->end();



    /* SUBMITTING */
    // Prepare to submit the command buffer
    Tools::Array<VkPipelineStageFlags> wait_stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submit_info;
    populate_submit_info(submit_info, this->draw_cmds[swapchain_index], this->image_ready_semaphores[this->current_frame], wait_stages, this->render_ready_semaphores[this->current_frame]);

    // Submit to the queue
    vkResetFences(this->window.gpu(), 1, &frame_in_flight_fences[this->current_frame].fence());
    Tools::Array<VkQueue> graphics_queues = this->window.gpu().queues(QueueType::graphics);
    if ((vk_result = vkQueueSubmit(graphics_queues[0], 1, &submit_info, this->frame_in_flight_fences[this->current_frame])) != VK_SUCCESS) {
        DLOG(fatal, "Could not submit to queue: " + vk_error_map[vk_result]);
    }



    /* PRESENTING */
    // Prepare the present info
    VkPresentInfoKHR present_info;
    populate_present_info(present_info, this->window.swapchain(), swapchain_index, this->render_ready_semaphores[this->current_frame]);

    // Present it using the queue present function
    Tools::Array<VkQueue> present_queues = this->window.gpu().queues(QueueType::present);
    vk_result = vkQueuePresentKHR(present_queues[0], &present_info);
    if (this->window.needs_resize() || vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR) {
        // The swapchain is outdated or suboptimal (probably a resize); we resize to fit again
        this->_resize();
    } else if (vk_result != VK_SUCCESS) {
        DLOG(fatal, "Could not present result: " + vk_error_map[vk_result]);
    }



    // Done with this iteration
    this->current_frame = (this->current_frame + 1) % RenderSystem::max_frames_in_flight;
    DRETURN true;
}



/* Swap operator for the RenderSystem class. */
void Rendering::swap(RenderSystem& rs1, RenderSystem& rs2) {
    DENTER("Rendering::swap(RenderSystem)");

    #ifndef NDEBUG
    if (&rs1.window != &rs2.window) {
        DLOG(fatal, "Cannot swap render systems with different windows");
    }
    if (&rs1.memory_manager != &rs2.memory_manager) {
        DLOG(fatal, "Cannot swap render systems with different memory managers");
    }
    if (&rs1.model_system != &rs2.model_system) {
        DLOG(fatal, "Cannot swap render systems with different model systems");
    }
    #endif

    // Simply swap everything
    using std::swap;

    swap(rs1.depth_stencil, rs2.depth_stencil);
    swap(rs1.framebuffers, rs2.framebuffers);

    swap(rs1.vertex_shader, rs2.vertex_shader);
    swap(rs1.fragment_shader, rs2.fragment_shader);

    swap(rs1.render_pass, rs2.render_pass);
    swap(rs1.pipeline, rs2.pipeline);

    swap(rs1.draw_cmds, rs2.draw_cmds);

    swap(rs1.image_ready_semaphores, rs2.image_ready_semaphores);
    swap(rs1.render_ready_semaphores, rs2.render_ready_semaphores);
    swap(rs1.frame_in_flight_fences, rs2.frame_in_flight_fences);
    swap(rs1.image_in_flight_fences, rs2.image_in_flight_fences);

    swap(rs1.current_frame, rs2.current_frame);

    // Done
    DRETURN;
}
