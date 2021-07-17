/* RENDER ENGINE.cpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 15:05:32
 * Last edited:
 *   28/06/2021, 22:28:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains code that runs the Vulkan engine. In particular, sets up the
 *   required Vulkan structures and runs the game loop
 *   iteration-for-iteration.
**/

#include <chrono>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "tools/CppDebugger.hpp"
#include "render_engine/auxillary/ErrorCodes.hpp"

#include "RenderEngine.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Computes the three new camera matrices. */
static void compute_camera_matrices(glm::mat4& proj_mat, glm::mat4& view_mat, glm::mat4& model_mat, float aspect_ratio) {
    DENTER("compute_camera_matrices");

    // Note that start time of the rendering process as a static - i.e., continious between calls
    static std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // Get the current time too
    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    // Compute the difference between them
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // Compute the projection matrix: field of view and such?
    proj_mat = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 10.0f);
    // Compute the view matrix: translation from the world space to camera space
    view_mat = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // Compute the model matrix: used to rotate the world to provide the rotating camera
    model_mat = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Don't forget to flip the Y in the projection matrix, since GLM Y and Vulkan Y are actually inverted
    proj_mat[1][1] *= -1;

    // Done
    DRETURN;
}





/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSubmitInfo struct. */
static void populate_submit_info(VkSubmitInfo& submit_info, const CommandBuffer& cmd, const Semaphore& wait_for_semaphore,  const Tools::Array<VkPipelineStageFlags>& wait_for_stages, const Semaphore& signal_after_semaphore) {
    DENTER("populate_submit_info");

    // Set to default
    submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Attach the command buffer
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd.command_buffer();

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





/***** RENDERENGINE CLASS *****/
/* Constructor for the RenderEngine class, which takes a Window to render to, a command pool for draw commands, a command pool for memory commands, a pool to allocate GPU-only buffers with, a memory pool for stage buffers, a pool for descriptor sets and a model manager to load models from. */
RenderEngine::RenderEngine(Window& window, Rendering::CommandPool& draw_cmd_pool, Rendering::CommandPool& memory_cmd_pool, Rendering::MemoryPool& draw_pool, Rendering::MemoryPool& stage_pool, Rendering::DescriptorPool& descriptor_pool, const Models::ModelManager& model_manager) :
    window(window),
    draw_cmd_pool(draw_cmd_pool),
    mem_cmd_pool(memory_cmd_pool),
    draw_pool(draw_pool),
    stage_pool(stage_pool),
    descr_pool(descriptor_pool),
    model_manager(model_manager),

    depth_stencil(this->window.gpu(), this->draw_pool, this->window.swapchain().extent()),

    vertex_shader(this->window.gpu(), "bin/shaders/vertex_v3.spv"),
    fragment_shader(this->window.gpu(), "bin/shaders/frag_v1.spv"),

    render_pass(this->window.gpu()),
    pipeline(this->window.gpu()),

    framebuffers(this->window.swapchain().size()),

    image_ready_semaphores(Semaphore(this->window.gpu()), RenderEngine::max_frames_in_flight),
    render_ready_semaphores(Semaphore(this->window.gpu()), RenderEngine::max_frames_in_flight),
    frame_in_flight_fences(Fence(this->window.gpu(), VK_FENCE_CREATE_SIGNALED_BIT), RenderEngine::max_frames_in_flight),
    image_in_flight_fences((Fence*) nullptr, this->window.swapchain().size()),

    current_frame(0)
{
    DENTER("Rendering::RenderEngine::RenderEngine");

    /* RENDER PASS */
    // Prepare the render pass
    uint32_t col_index = this->render_pass.add_attachment(this->window.swapchain().format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    uint32_t dep_index = this->render_pass.add_attachment(this->depth_stencil.format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    this->render_pass.add_subpass({ std::make_pair(col_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }, std::make_pair(dep_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    this->render_pass.add_dependency(VK_SUBPASS_EXTERNAL, col_index, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    this->render_pass.finalize();

    // Fetch the framebuffers of the swapchain
    for (uint32_t i = 0; i < this->window.swapchain().size(); i++) {
        this->framebuffers.push_back(this->window.swapchain().get_framebuffer(i, this->render_pass, this->depth_stencil));
    }



    /* PIPELINE */
    // Prepare the shader part of the graphics pipeline
    pipeline.init_shader_stage(this->vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
    pipeline.init_shader_stage(this->fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);

    // Prepare the static part
    pipeline.init_vertex_input(this->model_manager.input_binding_description(), this->model_manager.input_attribute_descriptions());
    pipeline.init_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline.init_depth_testing(VK_TRUE, VK_COMPARE_OP_LESS);
    pipeline.init_viewport_transformation(Rectangle(0.0, 0.0, this->window.swapchain().extent()), Rectangle(0.0, 0.0, this->window.swapchain().extent()));
    pipeline.init_rasterizer(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE); //VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    pipeline.init_multisampling();
    pipeline.init_color_blending(0, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    pipeline.init_color_logic(VK_FALSE, VK_LOGIC_OP_NO_OP);
    
    // Add the pipeline layout
    pipeline.init_pipeline_layout({}, { { VK_SHADER_STAGE_VERTEX_BIT, (uint32_t) (3ULL * sizeof(glm::mat4)) } });

    // Finally, generate the pipeline itself
    pipeline.finalize(this->render_pass, 0);



    /* COMMAND BUFFERS */
    // Allocate the command buffers
    this->draw_cmds = this->draw_cmd_pool.nallocate(this->framebuffers.size());

    // We can already record the command buffers for each framebuffer
    this->refresh();



    /* DONE */
    DLOG(info, "Initialized RenderEngine.");
    DLEAVE;
}



/* Private helper function that actually performs the RenderEngine part of a resize. */
void RenderEngine::_resize() {
    DENTER("Rendering::RenderEngine::_resize");

    // Re-create the depth stencil with a new size
    this->depth_stencil.resize(this->window.real_extent());

    // Fetch a new list of framebuffers
    this->framebuffers.clear();
    this->framebuffers.reserve(this->window.swapchain().size());
    for (uint32_t i = 0; i < this->window.swapchain().size(); i++) {
        this->framebuffers.push_back(this->window.swapchain().get_framebuffer(i, this->render_pass, this->depth_stencil));
    }

    // Then, re-create the command buffers
    this->refresh();

    // Done ez pz lemon sqzy
    DRETURN;
}



/* Runs a single iteration of the game loop. Returns whether or not the RenderEngine allows the window to continue (true) or not because it's closed (false). */
bool RenderEngine::loop() {
    DENTER("Rendering::RenderEngine::loop");

    /* PREPARATION */
    // First, do the window
    bool can_continue = this->window.loop();
    if (!can_continue) {
        DRETURN false;
    }

    // Before we get the swapchain images, be sure to wait for the current frame to be available
    vkWaitForFences(this->window.gpu(), 1, &frame_in_flight_fences[this->current_frame].fence(), VK_TRUE, UINT64_MAX);

    // Next, try to get a new swapchain image
    uint32_t swapchain_index;
    VkResult vk_result = vkAcquireNextImageKHR(this->window.gpu(), this->window.swapchain(), UINT64_MAX, this->image_ready_semaphores[this->current_frame], VK_NULL_HANDLE, &swapchain_index);
    if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR) {
        // The swapchain is outdated or suboptimal (probably a resize); we resize to fit again
        this->resize();

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


    /* RENDERING */
    // Compute new camera matrices
    glm::mat4 cam_proj, cam_view, cam_model;
    compute_camera_matrices(cam_proj, cam_view, cam_model, (float) this->window.swapchain().extent().width / (float) this->window.swapchain().extent().height);

    // Record the command buffer
    this->draw_cmds[swapchain_index].begin();
    this->render_pass.start_scheduling(this->draw_cmds[swapchain_index], this->framebuffers[swapchain_index]);
    this->pipeline.schedule(this->draw_cmds[swapchain_index]);
    this->model_manager.schedule(this->draw_cmds[swapchain_index]);
    this->pipeline.schedule_push_constants(this->draw_cmds[swapchain_index], VK_SHADER_STAGE_VERTEX_BIT,                     0, sizeof(glm::mat4), &cam_proj);
    this->pipeline.schedule_push_constants(this->draw_cmds[swapchain_index], VK_SHADER_STAGE_VERTEX_BIT,     sizeof(glm::mat4), sizeof(glm::mat4), &cam_view);
    this->pipeline.schedule_push_constants(this->draw_cmds[swapchain_index], VK_SHADER_STAGE_VERTEX_BIT, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &cam_model);
    this->pipeline.schedule_draw_indexed(this->draw_cmds[swapchain_index], this->model_manager.isize(), 1);
    this->render_pass.stop_scheduling(this->draw_cmds[swapchain_index]);
    this->draw_cmds[swapchain_index].end();



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
        this->resize();
    } else if (vk_result != VK_SUCCESS) {
        DLOG(fatal, "Could not present result: " + vk_error_map[vk_result]);
    }



    // Done with this iteration
    this->current_frame = (this->current_frame + 1) % RenderEngine::max_frames_in_flight;
    DRETURN true;
}



/* Refreshes the RenderEngine by re-drawing the command buffers. Can optionally skip waiting for the device to be idle. */
void RenderEngine::refresh(bool wait_for_idle) {
    DENTER("Rendering::RenderEngine::refresh");

    // Wait until the device is ready
    if (wait_for_idle) {
        this->wait_for_idle();
    }

    // // If any, deallocate old command buffers
    // if (this->draw_cmds.size() > 0) {
    //     this->draw_cmd_pool.ndeallocate(this->draw_cmds);
    // }

    // // Allocate new command buffers
    // this->draw_cmds = this->draw_cmd_pool.nallocate(this->framebuffers.size());

    // // Loop through them to record them
    // for (uint32_t i = 0; i < this->draw_cmds.size(); i++) {
    //     uint32_t vertex_size = this->model_manager.size() * sizeof(Models::Vertex);

    //     // Record it
    //     this->draw_cmds[i].begin();
    //     this->render_pass.start_scheduling(this->draw_cmds[i], this->framebuffers[i]);
    //     this->pipeline.schedule(this->draw_cmds[i]);
    //     this->model_manager.schedule(this->draw_cmds[i]);
    //     this->pipeline.schedule_draw(this->draw_cmds[i], 3, 1);
    //     this->render_pass.stop_scheduling(this->draw_cmds[i]);
    //     this->draw_cmds[i].end();
    // }

    // Done
    DRETURN;
}

/* Resizes the window to the size of the given window. Note that this is a pretty slow operation, as it requires the device to be idle. */
void RenderEngine::resize() {
    DENTER("Rendering::RenderEngine::resize");

    // Resize the window first
    this->window.resize();

    // Do our own part too
    this->_resize();

    // Done, we can resume rendering
    DRETURN;
}

/* Resizes the window to the given size. Note that this is a pretty slow operation, as it requires the device to be idle. */
void RenderEngine::resize(uint32_t new_width, uint32_t new_height) {
    DENTER("Rendering::RenderEngine::resize(width, height)");

    // First, resize the window to that size
    this->window.resize(new_width, new_height);

    // Do our own part too
    this->_resize();

    // Done, we can resume rendering
    DRETURN;
}
