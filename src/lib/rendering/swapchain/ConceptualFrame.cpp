/* CONCEPTUAL FRAME.cpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 19:09:54
 * Last edited:
 *   9/20/2021, 10:51:41 PM
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
using namespace Makma3D;
using namespace Makma3D::Rendering;


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
    submit_info.pCommandBuffers = &cmd->vulkan();

    // Attach the data for which we wait
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &wait_for_semaphore.semaphore();
    submit_info.pWaitDstStageMask = wait_for_stages.rdata();

    // Attach the semaphores which we signal when done
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_after_semaphore.semaphore();
}





/***** CONCEPTUALFRAME CLASS *****/
/* Constructor for the ConceptualFrame class, which takes a MemoryManager to be able to draw games, a descriptor set layout for the global descriptor, a descriptor set layout for per-material descriptors and a descriptor set layout for the per-entity descriptors. */
ConceptualFrame::ConceptualFrame(Rendering::MemoryManager& memory_manager, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& material_layout, const Rendering::DescriptorSetLayout& entity_layout) :
    memory_manager(memory_manager),

    swapchain_frame(nullptr),
    pipeline(nullptr),

    global_layout(global_layout),
    material_layout(material_layout),
    entity_layout(entity_layout),

    image_ready_semaphore(this->memory_manager.gpu),
    render_ready_semaphore(this->memory_manager.gpu),
    in_flight_fence(this->memory_manager.gpu, VK_FENCE_CREATE_SIGNALED_BIT)
{
    // Initialize the stage buffer
    this->stage_buffer = this->memory_manager.stage_pool.allocate(std::max({ sizeof(CameraData), sizeof(SimpleColouredData), sizeof(EntityData) }), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    logger.logc(Verbosity::debug, ConceptualFrame::channel, "Allocated stage buffer @ ", this->stage_buffer->offset());

    // Initialize the commandbuffer
    this->draw_cmd = this->memory_manager.draw_cmd_pool.allocate();

    // Initialize the pools
    this->memory_pool = new LinearMemoryPool(this->memory_manager.gpu, 10 * 1024 * 1024, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    this->descriptor_pool = new DescriptorPool(this->memory_manager.gpu, {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 }
    }, 64);

    // Initialize the global descriptor set & camera buffer
    this->camera_buffer = this->memory_manager.draw_pool.allocate(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // And that's it
}

/* Move constructor for the ConceptualFrame class. */
ConceptualFrame::ConceptualFrame(ConceptualFrame&& other) :
    memory_manager(other.memory_manager),

    swapchain_frame(std::move(other.swapchain_frame)),
    stage_buffer(std::move(other.stage_buffer)),
    pipeline(std::move(other.pipeline)),

    global_layout(std::move(other.global_layout)),
    material_layout(std::move(other.material_layout)),
    entity_layout(std::move(other.entity_layout)),

    draw_cmd(std::move(other.draw_cmd)),
    memory_pool(std::move(other.memory_pool)),
    descriptor_pool(std::move(other.descriptor_pool)),

    global_set(std::move(other.global_set)),
    camera_buffer(std::move(other.camera_buffer)),

    material_index_map(std::move(other.material_index_map)),
    material_sets(std::move(other.material_sets)),
    material_buffers(std::move(other.material_buffers)),

    entity_index_map(std::move(other.entity_index_map)),
    entity_sets(std::move(other.entity_sets)),
    entity_buffers(std::move(other.entity_buffers)),

    image_ready_semaphore(std::move(other.image_ready_semaphore)),
    render_ready_semaphore(std::move(other.render_ready_semaphore)),
    in_flight_fence(std::move(other.in_flight_fence))
{
    // Tell the other not to deallocate any of his resources
    other.stage_buffer = nullptr;
    other.draw_cmd = nullptr;
    other.memory_pool = nullptr;
    other.descriptor_pool = nullptr;
    other.global_set = nullptr;
    other.camera_buffer = nullptr;
    // No need to clear the material sets/buffers, as the Array's move function already makes sure they're reset to empty
    // No need to clear the entity sets/buffers, as the Array's move function already makes sure they're reset to empty
}

/* Destructor for the ConceptualFrame class. */
ConceptualFrame::~ConceptualFrame() {
    if (this->camera_buffer != nullptr) {
        this->memory_manager.draw_pool.free(this->camera_buffer);
    }
    if (this->descriptor_pool != nullptr) {
        delete this->descriptor_pool;
    }
    if (this->memory_pool != nullptr) {
        delete this->memory_pool;
    }
    if (this->draw_cmd != nullptr) {
        this->memory_manager.draw_cmd_pool.free(this->draw_cmd);
    }
    if (this->stage_buffer != nullptr) {
        this->memory_manager.stage_pool.free(this->stage_buffer);
    }
}



/* Prepares rendering the frame as new by throwing out old data preparing to render at most the given number of objects with at least the given number of materials different materials. */
void ConceptualFrame::prepare_render(uint32_t n_materials, uint32_t n_objects) {
    // Reset the index maps
    this->material_index_map.clear();
    this->entity_index_map.clear();

    // Reset the pools
    this->memory_pool->reset();
    this->descriptor_pool->reset();

    // Prepare enough space in the object arrays
    this->material_buffers.clear();
    this->entity_buffers.clear();
    this->material_buffers.resize_opt(n_materials);
    this->entity_buffers.resize_opt(n_objects);

    // Allocate the new descriptors
    this->global_set    = this->descriptor_pool->allocate(this->global_layout);
    this->material_sets = this->descriptor_pool->nallocate(n_materials, this->material_layout);
    this->entity_sets   = this->descriptor_pool->nallocate(n_objects, this->entity_layout);
}



/* Populates the internal camera buffer with the given projection and view matrices. */
void ConceptualFrame::upload_camera_data(const glm::mat4& proj_matrix, const glm::mat4& view_matrix) {
    // Prepare the struct to send
    CameraData data{ proj_matrix, view_matrix };

    // Send it to the camera buffer using the staging buffer
    this->camera_buffer->set((void*) &data, sizeof(CameraData), this->stage_buffer, this->memory_manager.copy_cmd);
    // Add the camera to the descriptor
    this->global_set->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { this->camera_buffer });
}

/* Uploads the given material to the GPU. What precisely will be uploaded is, of course, material dependent. */
void ConceptualFrame::upload_material_data(const Materials::Material* material) {
    // Map the material in the internal index map
    std::unordered_map<const Materials::Material*, uint32_t>::iterator iter = this->material_index_map.find((const Materials::Material*) material);
    if (iter == this->material_index_map.end()) {
        iter = this->material_index_map.insert({ (const Materials::Material*) material, static_cast<uint32_t>(this->material_index_map.size()) }).first;
    }
    uint32_t material_index = (*iter).second;

    #ifndef NDEBUG
    // Throw errors if out-of-range
    if (material_index > this->material_sets.size()) {
        logger.fatalc(ConceptualFrame::channel, "Material index ", material_index, " is out of range (prepared for only ", this->material_sets.size(), " materials)");
    }
    #endif

    // Select what to upload based on the given material
    switch (material->type()) {
        case Materials::MaterialType::simple:
            // Upload nothing
            break;
        
        case Materials::MaterialType::simple_coloured: {
            const Materials::SimpleColoured* simple_coloured = (const Materials::SimpleColoured*) material;

            // Upload the SimpleColoured data
            if (this->material_buffers[material_index] == nullptr) {
                // Allocate a new buffer first
                this->material_buffers[material_index] = this->memory_pool->allocate(sizeof(SimpleColouredData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            }

            // Populate the buffer with the stage data using the internal stage buffer
            SimpleColouredData data = simple_coloured->data();
            this->material_buffers[material_index]->set((void*) &data, sizeof(SimpleColouredData), this->stage_buffer, this->memory_manager.copy_cmd);

            // Bind the descriptor set
            this->material_sets[material_index]->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { this->material_buffers[material_index] });

            // Done
            break;
        }

        case Materials::MaterialType::simple_textured: {
            const Materials::SimpleTextured* simple_textured = (const Materials::SimpleTextured*) material;

            // Schedule the texture's sampler
            this->material_sets[material_index]->bind(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, { simple_textured->texture });

            // Done
            break;
        }
        
        default:
            logger.fatalc(ConceptualFrame::channel, "Unsupported material type '", Materials::material_type_names[(int) material->type()], "'");

    }

    // Done with uploading
}

/* Uploads entity data for the given entity to its buffer and its descriptor set. */
void ConceptualFrame::upload_entity_data(ECS::entity_t entity, const Rendering::EntityData& entity_data) {
    // Map the object
    std::unordered_map<ECS::entity_t, uint32_t>::iterator iter = this->entity_index_map.find(entity);
    if (iter == this->entity_index_map.end()) {
        iter = this->entity_index_map.insert({ entity, static_cast<uint32_t>(this->entity_index_map.size()) }).first;
    }
    uint32_t entity_index = (*iter).second;

    #ifndef NDEBUG
    // Throw errors if out-of-range
    if (entity_index > this->entity_buffers.size()) {
        logger.fatalc(ConceptualFrame::channel, "Entity index ", entity_index, " is out of range (prepared for only ", this->entity_buffers.size(), " entities)");
    }
    #endif

    // Otherwise, allocate a new buffer if needed
    if (this->entity_buffers[entity_index] == nullptr) { this->entity_buffers[entity_index] = this->memory_pool->allocate(sizeof(EntityData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT); }
    // Populate the buffer with this entity's data
    this->entity_buffers[entity_index]->set((void*) &entity_data, sizeof(EntityData), this->stage_buffer, this->memory_manager.copy_cmd);
    // Bind the correct object buffer to the correct set
    this->entity_sets[entity_index]->bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, { this->entity_buffers[entity_index] });
}



/* Starts to schedule the render pass associated with the wrapped SwapchainFrame on the internal draw queue. */
void ConceptualFrame::schedule_start() {
    #ifndef NDEBUG
    // Check if the swapchain frame is set
    if (this->swapchain_frame == nullptr) {
        logger.fatalc(ConceptualFrame::channel, "Cannot start scheduling without assigned swapchain frame.");
    }
    #endif

    // Begin the command buffer
    this->draw_cmd->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // First, schedule the render pass start
    this->swapchain_frame->render_pass.start_scheduling(this->draw_cmd, this->swapchain_frame->framebuffer(), this->swapchain_frame->extent());
}

/* Binds the given pipeline on the internal draw command queue. */
void ConceptualFrame::schedule_pipeline(const Rendering::Pipeline* pipeline) {
    // First, set the pipeline internally
    this->pipeline = pipeline;

    // Bind the pipeline to the command buffer
    pipeline->bind(this->draw_cmd);
}

/* Schedules frame-global descriptors on the internal draw queue (i.e., binds the camera data and the global descriptor). */
void ConceptualFrame::schedule_global() {
    // Bind the descriptor itself
    this->global_set->schedule(this->draw_cmd, this->pipeline->layout(), 0);
}

/* Schedules the stuff for the given material. Does have to have its data uploaded first, of course. */
void ConceptualFrame::schedule_material(const Materials::Material* material) {
    // Map the material in the internal index map
    std::unordered_map<const Materials::Material*, uint32_t>::iterator iter = this->material_index_map.find((const Materials::Material*) material);
    if (iter == this->material_index_map.end()) {
        logger.fatalc(ConceptualFrame::channel, "Cannot schedule material ", material, " for which nothing has been uploaded.");
    }
    uint32_t material_index = (*iter).second;

    #ifndef NDEBUG
    // Throw errors if out-of-range
    if (material_index > this->material_sets.size()) {
        logger.fatalc(ConceptualFrame::channel, "Material index ", material_index, " is out of range (prepared for only ", this->material_sets.size(), " materials)");
    }
    #endif

    // Choose what to schedule
    switch (material->type()) {
        case Materials::MaterialType::simple_coloured:
        case Materials::MaterialType::simple_textured:
            // Schedule its descriptor set
            this->material_sets[material_index]->schedule(this->draw_cmd, this->pipeline->layout(), 1);

        default:
            // Don't schedule
            break;

    }

    // Done
}

/* Schedules the given entity's descriptor set on the internal draw queue. */
void ConceptualFrame::schedule_entity(ECS::entity_t entity) {
    // Map the object
    std::unordered_map<ECS::entity_t, uint32_t>::iterator iter = this->entity_index_map.find(entity);
    if (iter == this->entity_index_map.end()) {
        logger.fatalc(ConceptualFrame::channel, "Cannot schedule entity ", entity, " for which nothing has been uploaded.");
    }
    uint32_t entity_index = (*iter).second;

    #ifndef NDEBUG
    // Throw errors if out-of-range
    if (entity_index > this->entity_sets.size()) {
        logger.fatalc(ConceptualFrame::channel, "Entity index ", entity_index, " is out of range (prepared for only ", this->entity_sets.size(), " entities)");
    }
    #endif

    // Schedule the object's descriptor set
    this->entity_sets[entity_index]->schedule(this->draw_cmd, this->pipeline->layout(), 2);
}

/* Binds the given vertex buffer to the internal draw queue. */
void ConceptualFrame::schedule_vertex_buffer(const Rendering::Buffer* vertex_buffer) {
    // Schedule the model's vertex buffer
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(this->draw_cmd->vulkan(), 0, 1, &vertex_buffer->vulkan(), offsets);
}

/* Schedules a draw command for the given index buffer (with the given number of indices) on the internal draw queue. */
void ConceptualFrame::schedule_draw(const Rendering::Buffer* index_buffer, uint32_t n_indices) {
    // First, schedule the mesh' index buffer
    vkCmdBindIndexBuffer(this->draw_cmd->vulkan(), index_buffer->vulkan(), 0, VK_INDEX_TYPE_UINT32);
    // Next, schedule the draw call
    this->pipeline->schedule_idraw(this->draw_cmd, n_indices, 1);
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
    swap(cf1.material_layout, cf2.material_layout);
    swap(cf1.entity_layout, cf2.entity_layout);
    
    swap(cf1.draw_cmd, cf2.draw_cmd);
    swap(cf1.memory_pool, cf2.memory_pool);
    swap(cf1.descriptor_pool, cf2.descriptor_pool);
    
    swap(cf1.global_set, cf2.global_set);
    swap(cf1.camera_buffer, cf2.camera_buffer);

    swap(cf1.material_index_map, cf2.material_index_map);
    swap(cf1.material_sets, cf2.material_sets);
    swap(cf1.material_buffers, cf2.material_buffers);

    swap(cf1.entity_index_map, cf2.entity_index_map);
    swap(cf1.entity_sets, cf2.entity_sets);
    swap(cf1.entity_buffers, cf2.entity_buffers);
    
    swap(cf1.image_ready_semaphore, cf2.image_ready_semaphore);
    swap(cf1.render_ready_semaphore, cf2.render_ready_semaphore);
    swap(cf1.in_flight_fence, cf2.in_flight_fence);
}
