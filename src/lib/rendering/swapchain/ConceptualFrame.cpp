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





/***** CONCEPTUALFRAME CLASS *****/
/* Constructor for the ConceptualFrame class, which takes a MemoryManager to be able to draw games, a descriptor set layout for the global descriptor and a descriptor set layout for the per-object descriptors. */
ConceptualFrame::ConceptualFrame(Rendering::MemoryManager& memory_manager, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& object_layout) :
    memory_manager(memory_manager),

    swapchain_frame(nullptr),
    stage_buffer(nullptr),

    global_layout(global_layout),
    object_layout(object_layout),

    descriptor_pool(new DescriptorPool(this->memory_manager.gpu, {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 }
    }, 10))
{
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Initializing...");

    // Initialize the commandbuffer
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Allocating command buffer...");
    this->draw_cmd = this->memory_manager.draw_cmd_pool.allocate();

    // Initialize the global descriptor set & camera buffer
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Initializing global frame data...");
    this->global_set = this->descriptor_pool->allocate(this->global_layout);
    this->camera_buffer = this->memory_manager.draw_pool.allocate(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    // And that's it
    logger.logc(Verbosity::details, ConceptualFrame::channel, "Init success.");
}

/* Move constructor for the ConceptualFrame class. */
ConceptualFrame::ConceptualFrame(ConceptualFrame&& other) :
    memory_manager(other.memory_manager),

    swapchain_frame(other.swapchain_frame),
    stage_buffer(other.stage_buffer),

    global_layout(other.global_layout),
    object_layout(other.object_layout),

    draw_cmd(other.draw_cmd),
    descriptor_pool(other.descriptor_pool),

    global_set(other.global_set),
    camera_buffer(other.camera_buffer),

    object_sets(other.object_sets),
    object_buffers(other.object_buffers)
{
    // Tell the other not to deallocate any of his resources
    other.stage_buffer = nullptr;
    other.draw_cmd = nullptr;
    other.descriptor_pool = nullptr;
    other.global_set = nullptr;
    other.camera_buffer = nullptr;
    other.object_sets.clear();
    other.object_buffers.clear();
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



/* Swap operator for the ConceptualFrame class. */
void Rendering::swap(ConceptualFrame& cf1, ConceptualFrame& cf2) {
    #ifndef NDEBUG
    if (&cf1.memory_manager != &cf2.memory_manager) { logger.fatalc(ConceptualFrame::channel, "Cannot swap conceptual frame with different memory managers."); }
    #endif

    using std::swap;
    
    swap(cf1.swapchain_frame, cf2.swapchain_frame);
    swap(cf1.stage_buffer, cf2.stage_buffer);

    swap(cf1.global_layout, cf2.global_layout);
    swap(cf1.object_layout, cf2.object_layout);
    
    swap(cf1.draw_cmd, cf2.draw_cmd);
    swap(cf1.descriptor_pool, cf2.descriptor_pool);
    
    swap(cf1.global_set, cf2.global_set);
    swap(cf1.camera_buffer, cf2.camera_buffer);

    swap(cf1.object_sets, cf2.object_sets);
    swap(cf1.object_buffers, cf2.object_buffers);
}
