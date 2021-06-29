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
#include "tools/CppDebugger.hpp"
#include "vulkan/ErrorCodes.hpp"

#include "RenderEngine.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Returns a Tools::Array with the required extensions for GLFW. */
static Tools::Array<const char*> get_glfw_extensions() {
    DENTER("get_glfw_extensions");

    // We first collect a list of GLFW extensions
    uint32_t n_extensions = 0;
    const char** raw_extensions = glfwGetRequiredInstanceExtensions(&n_extensions);

    // Return them as an array
    DRETURN Tools::Array<const char*>(raw_extensions, n_extensions);
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
/* Constructor for the RenderEngine class, which takes a GLFW window to render to. */
RenderEngine::RenderEngine(GLFWwindow* glfw_window) :
    glfw_window(glfw_window),

    instance(instance_extensions + get_glfw_extensions()),
    surface(this->instance, this->glfw_window),
    gpu(this->instance, this->surface),
    swapchain(this->gpu, this->glfw_window, this->surface),

    vertex_shader(this->gpu, "bin/shaders/vert.spv"),
    fragment_shader(this->gpu, "bin/shaders/frag.spv"),

    render_pass(this->gpu),
    pipeline(this->gpu),

    draw_cmd_pool(this->gpu, this->gpu.queue_info().graphics()),

    framebuffers(this->swapchain.size()),

    image_ready_semaphores(Semaphore(this->gpu), RenderEngine::max_frames_in_flight),
    render_ready_semaphores(Semaphore(this->gpu), RenderEngine::max_frames_in_flight),
    frame_in_flight_fences(Fence(this->gpu, VK_FENCE_CREATE_SIGNALED_BIT), RenderEngine::max_frames_in_flight),
    image_in_flight_fences((Fence*) nullptr, this->swapchain.size()),

    current_frame(0)
{
    DENTER("Vulkan::RenderEngine::RenderEngine");

    /* RENDER PASS */
    // Prepare the render pass
    uint32_t index = this->render_pass.add_attachment(this->swapchain.format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    this->render_pass.add_subpass({ std::make_pair(index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) });
    this->render_pass.add_dependency(VK_SUBPASS_EXTERNAL, index, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    this->render_pass.finalize();

    // Fetch the framebuffers of the swapchain
    for (uint32_t i = 0; i < this->swapchain.size(); i++) {
        this->framebuffers.push_back(this->swapchain.get_framebuffer(i, this->render_pass));
    }



    /* PIPELINE */
    // Prepare the shader part of the graphics pipeline
    pipeline.init_shader_stage(this->vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
    pipeline.init_shader_stage(this->fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);

    // Prepare the static part
    pipeline.init_vertex_input();
    pipeline.init_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline.init_viewport_transformation(Rectangle(0.0, 0.0, this->swapchain.extent()), Rectangle(0.0, 0.0, this->swapchain.extent()));
    pipeline.init_rasterizer(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
    pipeline.init_multisampling();
    pipeline.init_color_blending(0, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    pipeline.init_color_logic(VK_FALSE, VK_LOGIC_OP_NO_OP);
    
    // Add the pipeline layout
    pipeline.init_pipeline_layout({}, {});

    // Finally, generate the pipeline itself
    pipeline.finalize(this->render_pass, 0);



    /* COMMAND BUFFERS */
    // We can already record the command buffers for each framebuffer
    this->draw_cmds = this->draw_cmd_pool.nallocate(this->framebuffers.size());
    Tools::Array<VkCommandBuffer> temp(this->draw_cmds.size());
    for (uint32_t i = 0; i < this->draw_cmds.size(); i++) {
        // Record it
        this->draw_cmds[i].begin();
        this->render_pass.start_scheduling(this->draw_cmds[i], this->framebuffers[i]);
        this->pipeline.schedule(this->draw_cmds[i]);
        this->pipeline.schedule_draw(this->draw_cmds[i], 3, 1);
        this->render_pass.stop_scheduling(this->draw_cmds[i]);
        this->draw_cmds[i].end();
    }



    /* DONE */
    DLOG(info, "Initialized RenderEngine.");
    DLEAVE;
}



/* Runs a single iteration of the game loop. */
void RenderEngine::loop() {
    DENTER("Vulkan::RenderEngine::loop");

    /* PREPARATION */
    // First, poll the GLFW events
    glfwPollEvents();

    // Before we get the swapchain images, be sure to wait for the current frame to be available
    vkWaitForFences(this->gpu, 1, &frame_in_flight_fences[this->current_frame].fence(), VK_TRUE, UINT64_MAX);

    // Next, try to get a new swapchain image
    uint32_t swapchain_index;
    VkResult vk_result;
    if ((vk_result = vkAcquireNextImageKHR(this->gpu, this->swapchain, UINT64_MAX, this->image_ready_semaphores[this->current_frame], VK_NULL_HANDLE, &swapchain_index)) != VK_SUCCESS) {
        DLOG(fatal, "Could not get image from swapchain: " + vk_error_map[vk_result]);
    }

    // If another frame is already using this image, then wait for it to become available
    if (this->image_in_flight_fences[swapchain_index] != (Fence*) nullptr) {
        vkWaitForFences(this->gpu, 1, &this->image_in_flight_fences[swapchain_index]->fence(), VK_TRUE, UINT64_MAX);
    }
    this->image_in_flight_fences[swapchain_index] = &this->frame_in_flight_fences[this->current_frame];



    /* RENDERING */
    // Prepare to submit the command buffer
    Tools::Array<VkPipelineStageFlags> wait_stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submit_info;
    populate_submit_info(submit_info, this->draw_cmds[swapchain_index], this->image_ready_semaphores[this->current_frame], wait_stages, this->render_ready_semaphores[this->current_frame]);

    // Submit to the queue
    vkResetFences(this->gpu, 1, &frame_in_flight_fences[this->current_frame].fence());
    Tools::Array<VkQueue> graphics_queues = this->gpu.queues(QueueType::graphics);
    if ((vk_result = vkQueueSubmit(graphics_queues[0], 1, &submit_info, this->frame_in_flight_fences[this->current_frame])) != VK_SUCCESS) {
        DLOG(fatal, "Could not submit to queue: " + vk_error_map[vk_result]);
    }



    /* PRESENTING */
    // Prepare the present info
    VkPresentInfoKHR present_info;
    populate_present_info(present_info, this->swapchain, swapchain_index, this->render_ready_semaphores[this->current_frame]);

    // Present it using the queue present function
    Tools::Array<VkQueue> present_queues = this->gpu.queues(QueueType::present);
    if ((vk_result = vkQueuePresentKHR(present_queues[0], &present_info)) != VK_SUCCESS) {
        DLOG(fatal, "Could not present result: " + vk_error_map[vk_result]);
    }



    // Done with this iteration
    this->current_frame = (this->current_frame + 1) % RenderEngine::max_frames_in_flight;
    DRETURN;
}
