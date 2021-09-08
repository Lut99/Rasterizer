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
#include "ecs/components/Transform.hpp"
#include "ecs/components/Meshes.hpp"
#include "ecs/components/Camera.hpp"
#include "ecs/components/Textures.hpp"
#include "models/ModelSystem.hpp"

#include "auxillary/ErrorCodes.hpp"
#include "auxillary/Rectangle.hpp"
#include "auxillary/Vertex.hpp"
#include "auxillary/Index.hpp"

#include "RenderSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::ECS;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSubmitInfo struct. */
static void populate_submit_info(VkSubmitInfo& submit_info, const CommandBuffer* cmd, const Semaphore& wait_for_semaphore,  const Tools::Array<VkPipelineStageFlags>& wait_for_stages, const Semaphore& signal_after_semaphore) {
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
}

/* Populates the given VkPresentInfo struct. */
static void populate_present_info(VkPresentInfoKHR& present_info, const Swapchain& swapchain, const uint32_t& swapchain_index, const Semaphore& wait_for_semaphore) {
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
}





/***** RENDERSYSTEM CLASS *****/
/* Constructor for the RenderSystem, which takes a window, a memory manager to render (to and draw memory from, respectively), a model system to schedule the model buffers with and a texture system to schedule texture images with. */
RenderSystem::RenderSystem(Window& window, MemoryManager& memory_manager, const Models::ModelSystem& model_system, const Textures::TextureSystem& texture_system) :
    window(window),
    memory_manager(memory_manager),
    model_system(model_system),
    texture_system(texture_system),

    global_descriptor_layout(this->window.gpu()),
    object_descriptor_layout(this->window.gpu()),

    depth_stencil(this->window.gpu(), this->memory_manager.draw_pool, this->window.swapchain().extent()),

    vertex_shader(this->window.gpu(), "bin/shaders/vertex_v5.spv"),
    fragment_shader(this->window.gpu(), "bin/shaders/frag_v1.spv"),
    render_pass(this->window.gpu()),
    pipeline(this->window.gpu()),

    frame_data(this->window.swapchain().size()),

    image_ready_semaphores(Semaphore(this->window.gpu()), RenderSystem::max_frames_in_flight),
    render_ready_semaphores(Semaphore(this->window.gpu()), RenderSystem::max_frames_in_flight),
    frame_in_flight_fences(Fence(this->window.gpu(), VK_FENCE_CREATE_SIGNALED_BIT), RenderSystem::max_frames_in_flight),
    // image_in_flight_fences((Fence*) nullptr, this->window.swapchain().size()),

    current_frame(0)
{

    // Initialize the descriptor set layout for the global data
    this->global_descriptor_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    this->global_descriptor_layout.finalize();

    // Initialize the descriptor set layout for the per-object data
    this->object_descriptor_layout.add_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
    this->object_descriptor_layout.finalize();

    // Initialize the render pass
    uint32_t col_index = this->render_pass.add_attachment(this->window.swapchain().format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    uint32_t dep_index = this->render_pass.add_attachment(this->depth_stencil.format(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    this->render_pass.add_subpass({ std::make_pair(col_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) }, std::make_pair(dep_index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
    this->render_pass.add_dependency(VK_SUBPASS_EXTERNAL, col_index, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    this->render_pass.finalize();

    // Prepare the pipeline by choosing all its settings
    this->pipeline.init_shader_stage(this->vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
    this->pipeline.init_shader_stage(this->fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
    this->pipeline.init_input_assembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    this->pipeline.init_depth_testing(VK_TRUE, VK_COMPARE_OP_LESS);
    this->pipeline.init_viewport_transformation(Rectangle(0.0, 0.0, this->window.swapchain().extent()), Rectangle(0.0, 0.0, this->window.swapchain().extent()));
    this->pipeline.init_rasterizer(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE); //VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    this->pipeline.init_multisampling();
    this->pipeline.init_color_blending(0, VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    this->pipeline.init_color_logic(VK_FALSE, VK_LOGIC_OP_NO_OP);
    this->pipeline.init_pipeline_layout({ this->global_descriptor_layout, this->object_descriptor_layout }, {});
    this->pipeline.finalize(this->render_pass, 0);
    
    // Prepare the per-frame data
    this->_create_frames(this->window.swapchain().size());

    // Done initializing
    logger.logc(Verbosity::important, RenderSystem::channel, "Init success.");
}

/* Move constructor for the RenderSystem class. */
RenderSystem::RenderSystem(RenderSystem&& other)  :
    window(other.window),
    memory_manager(other.memory_manager),
    model_system(other.model_system),
    texture_system(other.texture_system),

    global_descriptor_layout(other.global_descriptor_layout),
    object_descriptor_layout(other.object_descriptor_layout),

    depth_stencil(other.depth_stencil),

    vertex_shader(other.vertex_shader),
    fragment_shader(other.fragment_shader),
    render_pass(other.render_pass),
    pipeline(other.pipeline),

    frame_data(other.frame_data),

    image_ready_semaphores(other.image_ready_semaphores),
    render_ready_semaphores(other.render_ready_semaphores),
    frame_in_flight_fences(other.frame_in_flight_fences),
    // image_in_flight_fences((Fence*) nullptr, this->window.swapchain().size()),

    current_frame(other.current_frame)
{}

/* Destructor for the RenderSystem class. */
RenderSystem::~RenderSystem() {
    logger.logc(Verbosity::important, RenderSystem::channel, "Cleaning...");

    // Nothing as of yet

    logger.logc(Verbosity::important, RenderSystem::channel, "Cleaned.");
}



/* Private helper function that creates the framedatas. */
void RenderSystem::_create_frames(uint32_t n_frames) {
    for (uint32_t i = 0; i < n_frames; i++) {
        this->frame_data.push_back({
            // Get the framebuffer for this frame from the swapchain
            this->window.swapchain().get_framebuffer(i, this->render_pass, this->depth_stencil),
            // Allocate a command buffer for draw calls to this frame
            this->memory_manager.draw_cmd_pool.allocate(),
            // Initialize the fence for this frame to not yet used.
            nullptr,
            
            // Allocate a descriptor set for this frame's global resources
            this->memory_manager.descr_pool.allocate(this->global_descriptor_layout),
            // Prepare the descriptor pool for this frame
            Rendering::DescriptorPool(
                this->window.gpu(), {
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 10 }
                }, 10, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
            ),
            // Declare the empty list for the per-object sets
            {},

            // Allocate the buffer for the camera data
            this->memory_manager.draw_pool.allocate(sizeof(RenderSystem::CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
            // Declare the empty list for the per-object buffers
            {}
        });
    }
}

/* Private helper function that resizes all required structures for a new window size. */
void RenderSystem::_resize() {
    // First, wait until the device is idle
    this->window.gpu().wait_for_idle();

    // Then, resize the window
    this->window.resize();

    // Re-create the depth stencil with a new size
    logger.debug("Window size: ", this->window.real_extent().width, 'x', this->window.real_extent().height);
    this->depth_stencil.resize(this->window.real_extent());

    // Re-create all frames
    this->frame_data.clear();
    this->_create_frames(this->window.swapchain().size());
}



/* Runs a single iteration of the game loop. Returns whether or not the RenderSystem is asked to close the window (false) or not (true). */
bool RenderSystem::render_frame(const ECS::EntityManager& entity_manager) {
    /* PREPARATION */
    // First, handle window events
    bool can_continue = this->window.loop();
    if (!can_continue) {
        return false;
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
        return true;
    } else if (vk_result != VK_SUCCESS) {
        logger.fatalc(RenderSystem::channel, "Could not get image from swapchain: ", vk_error_map[vk_result]);
    }

    // If another frame is already using this image, then wait for it to become available
    if (this->frame_data[swapchain_index].in_flight_fence != (Fence*) nullptr) {
        vkWaitForFences(this->window.gpu(), 1, &this->frame_data[swapchain_index].in_flight_fence->fence(), VK_TRUE, UINT64_MAX);
    }
    // Mark the swapchain to be used by the current conceptual frame
    this->frame_data[swapchain_index].in_flight_fence = &this->frame_in_flight_fences[this->current_frame];



    /***** FRAME PREPARATION *****/
    RenderSystem::FrameData& frame_data = this->frame_data[swapchain_index];

    // Prepare a staging buffer to use throughout the preparation
    VkDeviceSize stage_size = std::max({ sizeof(RenderSystem::CameraData), sizeof(glm::mat4) });
    Rendering::Buffer* stage = this->memory_manager.stage_pool.allocate(stage_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    // Get the camera matrices and send them to the GPU in a uniform buffer
    const Camera& cam = entity_manager.get_list<Camera>()[0];
    RenderSystem::CameraData cam_data{ cam.proj, cam.view };
    frame_data.camera_buffer->set((void*) &cam_data, sizeof(RenderSystem::CameraData), stage, this->memory_manager.copy_cmd);

    // Bind it to the descriptor set in the framebuffer
    frame_data.global_set->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { frame_data.camera_buffer });

    // Next, get the list of meshes to render
    const ECS::ComponentList<ECS::Meshes>& meshes = entity_manager.get_list<ECS::Meshes>();

    // Create enough descriptor sets for the objects, clearing any old ones
    frame_data.descr_pool.reset();
    frame_data.object_sets = frame_data.descr_pool.nallocate(meshes.size(), this->object_descriptor_layout);
    // And the same for the buffers
    for (uint32_t i = 0; i < frame_data.object_buffers.size(); i++) {
        this->memory_manager.draw_pool.free(frame_data.object_buffers[i]);
    }
    frame_data.object_buffers.clear();
    for (uint32_t i = 0; i < meshes.size(); i++) {
        frame_data.object_buffers.push_back(this->memory_manager.draw_pool.allocate(sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
    }



    /***** RECORDING *****/
    // Begin recording the command buffer and its render pass, already scheduling the pipeline
    frame_data.draw_cmd->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    this->render_pass.start_scheduling(frame_data.draw_cmd, frame_data.framebuffer);
    this->pipeline.schedule(frame_data.draw_cmd);

    // Bind the global descriptor set for this frame
    frame_data.global_set->schedule(frame_data.draw_cmd, this->pipeline.layout());

    // Next, schedule draw calls, which is one per entity mesh
    for (ECS::component_list_size_t i = 0; i < meshes.size(); i++) {
        // Get the relevant components for this entity
        entity_t entity = meshes.get_entity(i);
        const ECS::Meshes& obj_meshes = meshes[i];
        const ECS::Transform& transform = entity_manager.get_component<Transform>(entity);
        // const ECS::Texture& texture = entity_manager.get_component<Texture>(entity);

        // Populate the buffer for this entity
        frame_data.object_buffers[i]->set((void*) &transform.translation, sizeof(glm::vec4), stage, this->memory_manager.copy_cmd);
        // Populate the per-object data for this entity
        frame_data.object_sets[i]->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { frame_data.object_buffers[i] });
        frame_data.object_sets[i]->schedule(frame_data.draw_cmd, this->pipeline.layout(), 1);

        // Dive into all the meshes bound to this entity
        for (uint32_t j = 0; j < obj_meshes.size(); j++) {
            // // // Schedule the texture
            // this->texture_system.schedule(this->draw_cmds[swapchain_index], texture, this->texture_sets[swapchain_index]);
            // // // Schedule the descriptor itself
            // this->texture_sets[swapchain_index]->schedule(this->draw_cmds[swapchain_index], this->pipeline.layout());

            // Schedule the mesh data
            this->model_system.schedule(frame_data.draw_cmd, obj_meshes[j]);

            // Schedule the draw for this mesh
            this->pipeline.schedule_draw_indexed(frame_data.draw_cmd, obj_meshes[j].n_indices, 1);
        }
    }

    // Finish recording
    this->render_pass.stop_scheduling(frame_data.draw_cmd);
    frame_data.draw_cmd->end();



    /* SUBMITTING */
    // Prepare to submit the command buffer
    Tools::Array<VkPipelineStageFlags> wait_stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submit_info;
    populate_submit_info(submit_info, frame_data.draw_cmd, this->image_ready_semaphores[this->current_frame], wait_stages, this->render_ready_semaphores[this->current_frame]);

    // Submit to the queue
    vkResetFences(this->window.gpu(), 1, &frame_in_flight_fences[this->current_frame].fence());
    Tools::Array<VkQueue> graphics_queues = this->window.gpu().queues(QueueType::graphics);
    if ((vk_result = vkQueueSubmit(graphics_queues[0], 1, &submit_info, this->frame_in_flight_fences[this->current_frame])) != VK_SUCCESS) {
        logger.fatalc(RenderSystem::channel, "Could not submit to queue: ", vk_error_map[vk_result]);
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
        logger.fatalc(RenderSystem::channel, "Could not present result: ", vk_error_map[vk_result]);
    }



    // Done with this iteration
    this->memory_manager.stage_pool.free(stage);
    this->current_frame = (this->current_frame + 1) % RenderSystem::max_frames_in_flight;
    // printf("Rendered frame.\n");
    return true;
}



/* Swap operator for the RenderSystem class. */
void Rendering::swap(RenderSystem& rs1, RenderSystem& rs2) {
    #ifndef NDEBUG
    if (&rs1.window != &rs2.window) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different windows"); }
    if (&rs1.memory_manager != &rs2.memory_manager) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different memory managers"); }
    if (&rs1.model_system != &rs2.model_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different model systems"); }
    if (&rs1.texture_system != &rs2.texture_system) { logger.fatalc(RenderSystem::channel, "Cannot swap render systems with different texture systems"); }
    #endif

    // Simply swap everything
    using std::swap;

    swap(rs1.global_descriptor_layout, rs2.global_descriptor_layout);
    swap(rs1.object_descriptor_layout, rs2.object_descriptor_layout);

    swap(rs1.depth_stencil, rs2.depth_stencil);

    swap(rs1.vertex_shader, rs2.vertex_shader);
    swap(rs1.fragment_shader, rs2.fragment_shader);
    swap(rs1.render_pass, rs2.render_pass);
    swap(rs1.pipeline, rs2.pipeline);

    swap(rs1.frame_data, rs2.frame_data);

    swap(rs1.image_ready_semaphores, rs2.image_ready_semaphores);
    swap(rs1.render_ready_semaphores, rs2.render_ready_semaphores);
    swap(rs1.frame_in_flight_fences, rs2.frame_in_flight_fences);
    // image_in_flight_fences((Fence*) nullptr, this->window.swapchain().size()),

    swap(rs1.current_frame, rs2.current_frame);
}
