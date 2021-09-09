/* CONCEPTUAL FRAME.cpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 19:09:54
 * Last edited:
 *   08/09/2021, 19:09:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that reprents a ConceptualFrame to which we can
 *   render. Builds upon a SwapchainFrame.
**/

#include "glm/glm.hpp"
#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "ConceptualFrame.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** STRUCTS *****/
/* Struct for transferring camera data to the GPU. */
struct CameraData {
    /* The projection matrix for the camera. */
    glm::mat4 proj;
    /* The view matrix for the camera. */
    glm::mat4 view;
};





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





/***** CONCEPTUALFRAME CLASS *****/
/* Constructor for the ConceptualFrame class, which takes a MemoryManager to be able to draw games, a descriptor set layout for the global descriptor and a descriptor set layout for the per-object descriptors. */
ConceptualFrame::ConceptualFrame(Rendering::MemoryManager& memory_manager, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& object_layout) :
    memory_manager(memory_manager),

    swapchain_frame(nullptr),
    pipeline(nullptr),

    global_layout(global_layout),
    object_layout(object_layout),

    image_ready_semaphore(this->memory_manager.gpu),
    render_ready_semaphore(this->memory_manager.gpu),
    in_flight_fence(this->memory_manager.gpu, VK_FENCE_CREATE_SIGNALED_BIT)
{
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Initializing...");

    // Initialize the stage buffer
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Allocating staging buffer...");
    this->stage_buffer = this->memory_manager.stage_pool.allocate(std::max({ sizeof(CameraData), sizeof(ObjectData) }), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    logger.logc(Verbosity::debug, ConceptualFrame::channel, "Allocated stage buffer @ ", this->stage_buffer->offset());

    // Initialize the commandbuffer
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Allocating command buffer...");
    this->draw_cmd = this->memory_manager.draw_cmd_pool.allocate();

    // Initialize the pool
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Initializing descriptor pool...");
    this->descriptor_pool = new DescriptorPool(this->memory_manager.gpu, {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 }
    }, 10);

    // Initialize the global descriptor set & camera buffer
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Initializing global frame data...");
    this->camera_buffer = this->memory_manager.draw_pool.allocate(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // And that's it
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Init success.");
}

/* Move constructor for the ConceptualFrame class. */
ConceptualFrame::ConceptualFrame(ConceptualFrame&& other) :
    memory_manager(other.memory_manager),

    swapchain_frame(std::move(other.swapchain_frame)),
    stage_buffer(std::move(other.stage_buffer)),
    pipeline(std::move(other.pipeline)),

    global_layout(std::move(other.global_layout)),
    object_layout(std::move(other.object_layout)),

    draw_cmd(std::move(other.draw_cmd)),
    descriptor_pool(std::move(other.descriptor_pool)),

    global_set(std::move(other.global_set)),
    camera_buffer(std::move(other.camera_buffer)),

    object_sets(std::move(other.object_sets)),
    object_buffers(std::move(other.object_buffers)),

    image_ready_semaphore(std::move(other.image_ready_semaphore)),
    render_ready_semaphore(std::move(other.render_ready_semaphore)),
    in_flight_fence(std::move(other.in_flight_fence))
{
    // Tell the other not to deallocate any of his resources
    other.stage_buffer = nullptr;
    other.draw_cmd = nullptr;
    other.descriptor_pool = nullptr;
    other.global_set = nullptr;
    other.camera_buffer = nullptr;
    // No need to clear the object sets/buffers, as the Array's move function already makes sure they're reset to empty
}

/* Destructor for the ConceptualFrame class. */
ConceptualFrame::~ConceptualFrame() {
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaning...");

    if (this->object_buffers.size() > 0) {
        logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaning object buffers...");
        for (uint32_t i = 0; i < this->object_buffers.size(); i++) {
            this->memory_manager.draw_pool.free(this->object_buffers[i]);
        }
    }
    if (this->camera_buffer != nullptr) {
        logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaning camera buffer...");
        this->memory_manager.draw_pool.free(this->camera_buffer);
    }
    if (this->descriptor_pool != nullptr) {
        logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaning descriptor pool...");
        delete this->descriptor_pool;
    }
    if (this->draw_cmd != nullptr) {
        logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaning command buffer...");
        this->memory_manager.draw_cmd_pool.free(this->draw_cmd);
    }
    if (this->stage_buffer != nullptr) {
        logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaning stage buffer...");
        this->memory_manager.stage_pool.free(this->stage_buffer);
    }

    logger.logc(Verbosity::details, ConceptualFrame::channel, "Cleaned.");
}



/* Prepares rendering the frame as new by throwing out old data preparing to render at most the given number of objects. */
void ConceptualFrame::prepare_render(uint32_t n_objects) {
    // Rescale the internal array to the desired number of objects
    if (this->object_buffers.size() < n_objects) {
        // Extent the object buffer to more space and allocate new buffers
        this->object_buffers.reserve_opt(n_objects);
        for (uint32_t i = this->object_buffers.size(); i < n_objects; i++) {
            this->object_buffers.push_back(this->memory_manager.draw_pool.allocate(sizeof(ObjectData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
        }
    } else if (this->object_buffers.size() > n_objects) {
        // Remove the obsolete ones
        for (uint32_t i = this->object_buffers.size(); i-- > n_objects ;) {
            this->memory_manager.draw_pool.free(this->object_buffers[i]);
            this->object_buffers.pop_back();
        }
    }

    // Allocate n_objects new descriptors, since we always reset the pool
    this->descriptor_pool->reset();
    this->global_set  = this->descriptor_pool->allocate(this->global_layout);
    this->object_sets = this->descriptor_pool->nallocate(n_objects, this->object_layout);
}



/* Populates the internal camera buffer with the given projection and view matrices. */
void ConceptualFrame::upload_camera_data(const glm::mat4& proj_matrix, const glm::mat4& view_matrix) {
    // Prepare the struct to send
    CameraData data{ proj_matrix, view_matrix };

    // Send it to the camera buffer using the staging buffer
    this->camera_buffer->set((void*) &data, sizeof(CameraData), this->stage_buffer, this->memory_manager.copy_cmd);
}

/* Uploads object data for the given object to its buffer. */
void ConceptualFrame::upload_object_data(uint32_t object_index, const Rendering::ObjectData& object_data) {
    #ifndef NDEBUG
    // Throw errors if out-of-range
    if (object_index > this->object_buffers.size()) {
        logger.fatalc(ConceptualFrame::channel, "Object index ", object_index, " is out of range (prepared for only ", this->object_buffers.size(), " objects)");
    }
    #endif

    // Otherwise, set the correct buffer using the staging buffer
    this->object_buffers[object_index]->set((void*) &object_data, sizeof(ObjectData), this->stage_buffer, this->memory_manager.copy_cmd);
}



/* Starts to schedule the render pass associated with the wrapped SwapchainFrame on the internal draw queue, followed by binding the given pipeline. */
void ConceptualFrame::schedule_start(const Rendering::Pipeline& pipeline) {
    #ifndef NDEBUG
    // Check if the swapchain frame is set
    if (this->swapchain_frame == nullptr) {
        logger.fatalc(ConceptualFrame::channel, "Cannot start scheduling without assigned swapchain frame.");
    }
    #endif
    
    // Store the pipeline internally
    this->pipeline = &pipeline;

    // Begin the command buffer
    this->draw_cmd->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // First, schedule the render pass start
    this->swapchain_frame->render_pass.start_scheduling(this->draw_cmd, this->swapchain_frame->framebuffer(), this->swapchain_frame->extent());
    // Then the pipeline bind
    this->pipeline->schedule(this->draw_cmd);
}

/* Schedules frame-global descriptors on the internal draw queue (i.e., binds the camera data and the global descriptor). */
void ConceptualFrame::schedule_global() {
    // Add the camera to the descriptor
    this->global_set->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { this->camera_buffer });
    // Bind the descriptor itself
    this->global_set->schedule(this->draw_cmd, this->pipeline->layout(), 0);
}

/* Schedules the given object's buffer (and thus descriptor set) on the internal draw queue. */
void ConceptualFrame::schedule_object(uint32_t object_index) {
    #ifndef NDEBUG
    // Throw errors if out-of-range
    if (object_index > this->object_buffers.size()) {
        logger.fatalc(ConceptualFrame::channel, "Object index ", object_index, " is out of range (prepared for only ", this->object_buffers.size(), " objects)");
    }
    #endif

    // Bind the correct object buffer to the correct set
    this->object_sets[object_index]->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { this->object_buffers[object_index] });
    // Schedule the object's descriptor set
    this->object_sets[object_index]->schedule(this->draw_cmd, this->pipeline->layout(), 1);
}

/* Schedules a draw command for the given mesh on the internal draw queue. */
void ConceptualFrame::schedule_draw(const Models::ModelSystem& model_system, const ECS::Mesh& mesh) {
    // First, schedule the mesh as an indexed vertex buffer
    model_system.schedule(this->draw_cmd, mesh);
    // Next, schedule the draw call
    this->pipeline->schedule_draw_indexed(this->draw_cmd, mesh.n_indices, 1);
}

/* Stops scheduling by stopping the render pass associated with the wrapped SwapchainFrame. Then also stops the command buffer itself. */
void ConceptualFrame::schedule_stop() {
    // Stop scheduling the render pass
    this->swapchain_frame->render_pass.stop_scheduling(this->draw_cmd);
    // Stop the buffer altogether
    this->draw_cmd->end();

    // Clear the pipeline association as well
    this->pipeline = nullptr;
}



/* "Renders" the frame by sending the internal draw queue to the given device queue. */
void ConceptualFrame::submit(const VkQueue& vk_queue) {
    // Prepare to submit the command buffer
    Tools::Array<VkPipelineStageFlags> wait_stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submit_info;
    populate_submit_info(submit_info, this->draw_cmd, this->image_ready_semaphore, wait_stages, this->render_ready_semaphore);

    // Submit to the queue
    this->in_flight_fence.reset();
    VkResult vk_result;
    if ((vk_result = vkQueueSubmit(vk_queue, 1, &submit_info, this->in_flight_fence)) != VK_SUCCESS) {
        logger.fatalc(ConceptualFrame::channel, "Could not submit frame to queue: ", vk_error_map[vk_result]);
    }

    // Done
}



/* Swap operator for the ConceptualFrame class. */
void Rendering::swap(ConceptualFrame& cf1, ConceptualFrame& cf2) {
    #ifndef NDEBUG
    if (&cf1.memory_manager != &cf2.memory_manager) { logger.fatalc(ConceptualFrame::channel, "Cannot swap conceptual frame with different memory managers."); }
    #endif

    using std::swap;
    
    swap(cf1.swapchain_frame, cf2.swapchain_frame);
    swap(cf1.stage_buffer, cf2.stage_buffer);
    swap(cf1.pipeline, cf2.pipeline);

    swap(cf1.global_layout, cf2.global_layout);
    swap(cf1.object_layout, cf2.object_layout);
    
    swap(cf1.draw_cmd, cf2.draw_cmd);
    swap(cf1.descriptor_pool, cf2.descriptor_pool);
    
    swap(cf1.global_set, cf2.global_set);
    swap(cf1.camera_buffer, cf2.camera_buffer);

    swap(cf1.object_sets, cf2.object_sets);
    swap(cf1.object_buffers, cf2.object_buffers);
    
    swap(cf1.image_ready_semaphore, cf2.image_ready_semaphore);
    swap(cf1.render_ready_semaphore, cf2.render_ready_semaphore);
    swap(cf1.in_flight_fence, cf2.in_flight_fence);
}
