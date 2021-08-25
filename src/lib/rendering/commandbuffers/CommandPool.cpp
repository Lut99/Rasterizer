/* COMMAND POOL.cpp
 *   by Lut99
 *
 * Created:
 *   27/04/2021, 13:03:50
 * Last edited:
 *   25/05/2021, 18:14:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the CommandPool class, which handles allocating and
 *   destroying command buffers for a single device queue.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "CommandPool.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Functions that populates a given VkCommandPoolCreateInfo struct with the given values. */
static void populate_command_pool_info(VkCommandPoolCreateInfo& command_pool_info, uint32_t queue_index, VkCommandPoolCreateFlags create_flags) {
    // Set to default
    command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    // Set the queue we're the pool for
    command_pool_info.queueFamilyIndex = queue_index;

    // Set the create flags
    command_pool_info.flags = create_flags;
}

/* Function that populates a given VkCommandBufferAllocateInfo struct with the given values. */
static void populate_allocate_info(VkCommandBufferAllocateInfo& allocate_info, VkCommandPool vk_command_pool, uint32_t n_buffers, VkCommandBufferLevel buffer_level) {
    // Set to default
    allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    // Set the buffer level for this buffer
    allocate_info.level = buffer_level;

    // Tell it which command pool to allocate with
    allocate_info.commandPool = vk_command_pool;

    // Tell it how many buffers to allocate in one go
    allocate_info.commandBufferCount = n_buffers;
}





/***** COMMANDPOOL CLASS *****/
/* Constructor for the CommandPool class, which takes the GPU to allocate for, the queue index for which this pool allocates buffers and optionally create flags for the pool. */
CommandPool::CommandPool(const GPU& gpu, uint32_t queue_index, VkCommandPoolCreateFlags create_flags) :
    gpu(gpu),
    vk_queue_index(queue_index),
    vk_create_flags(create_flags)
{
    logger.logc(Verbosity::important, CommandPool::channel, "Initializing for queue ", this->vk_queue_index, "...");

    // Start by populating the create info
    VkCommandPoolCreateInfo command_pool_info;
    populate_command_pool_info(command_pool_info, this->vk_queue_index, create_flags);

    // Call the create
    VkResult vk_result;
    if ((vk_result = vkCreateCommandPool(this->gpu, &command_pool_info, nullptr, &this->vk_command_pool)) != VK_SUCCESS) {
        logger.fatalc(CommandPool::channel, "Could not create CommandPool: ", vk_error_map[vk_result]);
    }

    // D0ne
    logger.logc(Verbosity::important, CommandPool::channel, "Init success.");
}

/* Copy constructor for the CommandPool class. */
CommandPool::CommandPool(const CommandPool& other) :
    gpu(other.gpu),
    vk_queue_index(other.vk_queue_index),
    vk_create_flags(other.vk_create_flags)
{
    logger.logc(Verbosity::debug, CommandPool::channel, "Copying CommandPool @ ", &other, "...");

    // Start by populating the create info
    VkCommandPoolCreateInfo command_pool_info;
    populate_command_pool_info(command_pool_info, this->vk_queue_index, this->vk_create_flags);

    // Call the create
    VkResult vk_result;
    if ((vk_result = vkCreateCommandPool(this->gpu, &command_pool_info, nullptr, &this->vk_command_pool)) != VK_SUCCESS) {
        logger.fatalc(CommandPool::channel, "Could not create CommandPool: ", vk_error_map[vk_result]);
    }

    // D0ne
    logger.logc(Verbosity::debug, CommandPool::channel, "Copy success.");
}

/* Move constructor for the CommandPool class. */
CommandPool::CommandPool(CommandPool&& other) :
    gpu(other.gpu),
    vk_command_pool(other.vk_command_pool),
    vk_queue_index(other.vk_queue_index),
    command_buffers(std::move(other.command_buffers))
{
    other.vk_command_pool = nullptr;
    other.command_buffers.clear();
}

/* Destructor for the CommandPool class. */
CommandPool::~CommandPool() {
    logger.logc(Verbosity::important, CommandBuffer::channel, "Cleaning for queue ", this->vk_queue_index, "...");

    if (this->command_buffers.size() > 0) {
        logger.logc(Verbosity::details, CommandPool::channel, "Cleaning command buffers...");
        for (uint32_t i = 0; i < this->command_buffers.size(); i++) {
            vkFreeCommandBuffers(this->gpu, this->vk_command_pool, 1, &this->command_buffers[i]->command_buffer());
        }
    }

    if (this->vk_command_pool != nullptr) {
        logger.logc(Verbosity::details, CommandPool::channel, "Deallocating the pool...");
        vkDestroyCommandPool(this->gpu, this->vk_command_pool, nullptr);
    }

    logger.logc(Verbosity::important, CommandBuffer::channel, "Cleaned.");
}



/* Allocates a single, new command buffer of the given level. Returns by handle. */
CommandBuffer* CommandPool::allocate(VkCommandBufferLevel buffer_level) {
    // Prepare the create info
    VkCommandBufferAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, this->vk_command_pool, 1, buffer_level);

    // Call the create
    VkCommandBuffer buffer;
    VkResult vk_result;
    if ((vk_result = vkAllocateCommandBuffers(this->gpu, &allocate_info, &buffer)) != VK_SUCCESS) {
        logger.fatalc(CommandPool::channel, "Could not allocate command buffer: ", vk_error_map[vk_result]);
    }

    // Create the resulting buffer
    CommandBuffer* cmd_buffer = new CommandBuffer(buffer);
    this->command_buffers.push_back(cmd_buffer);

    // Done, return the handle
    return cmd_buffer;
}

/* Allocates N new command buffers of the given level. Returns by handles. */
Tools::Array<CommandBuffer*> CommandPool::nallocate(uint32_t n_buffers, VkCommandBufferLevel buffer_level) {
    // Prepare some temporary local space for the buffers
    Tools::Array<VkCommandBuffer> buffers(n_buffers);

    // Then, prepare the create info
    VkCommandBufferAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, this->vk_command_pool, n_buffers, buffer_level);

    // Call the create
    VkResult vk_result;
    if ((vk_result = vkAllocateCommandBuffers(this->gpu, &allocate_info, buffers.wdata(n_buffers))) != VK_SUCCESS) {
        logger.fatalc(CommandPool::channel, "Could not allocate command buffers: ", vk_error_map[vk_result]);
    }

    // Generate the CommandBuffers around the vulkan ones
    Tools::Array<CommandBuffer*> result(buffers.size());
    this->command_buffers.reserve(this->command_buffers.size() + buffers.size());
    for (uint32_t i = 0; i < buffers.size(); i++) {
        result.push_back(new CommandBuffer(buffers[i]));
        this->command_buffers.push_back(result.last());
    }

    // Done, return the list of handles
    return result;
}



/* Deallocates the CommandBuffer behind the given handle. Note that all buffers are deallocated automatically when the CommandPool is destructed, but this could save you memory. */
void CommandPool::free(const CommandBuffer* buffer) {
    // Try to remove the pointer from the list
    bool found = false;
    for (uint32_t i = 0; i < this->command_buffers.size(); i++) {
        if (this->command_buffers[i] == buffer) {
            this->command_buffers.erase(i);
            found = true;
            break;
        }
    }
    if (!found) {
        logger.fatalc(CommandPool::channel, "Tried to free CommandBuffer that was not allocated with this pool.");
    }

    // Destroy the VkCommandBuffer
    vkFreeCommandBuffers(this->gpu, this->vk_command_pool, 1, &buffer->command_buffer());
    
    // Destroy the pointer itself
    delete buffer;
}

/* Deallocates an array of given command buffer handles. */
void CommandPool::nfree(const Tools::Array<CommandBuffer*>& buffers) {
    // First, we check if all handles exist
    Tools::Array<VkCommandBuffer> to_remove(buffers.size());
    for (uint32_t i = 0; i < buffers.size(); i++) {
        bool found = false;
        for (uint32_t i = 0; i < this->command_buffers.size(); i++) {
            if (this->command_buffers[i] == buffers[i]) {
                this->command_buffers.erase(i);
                found = true;
                break;
            }
        }
        if (!found) {
            logger.fatalc(CommandPool::channel, "Tried to free CommandBuffer that was not allocated with this pool.");
        }

        // Mark the Vk object for removal
        to_remove.push_back(buffers[i]->command_buffer());

        // Delete the pointer
        delete buffers[i];
    }

    // All that's left is to actually remove the handles; do that
    vkFreeCommandBuffers(this->gpu, this->vk_command_pool, to_remove.size(), to_remove.rdata());
}


/* Swap operator for the CommandPool class. */
void Rendering::swap(CommandPool& cp1, CommandPool& cp2) {
    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (cp1.gpu != cp2.gpu) { logger.fatalc(CommandPool::channel, "Cannot swap command pools with different GPUs"); }
    #endif

    using std::swap;

    swap(cp1.vk_command_pool, cp2.vk_command_pool);
    swap(cp1.vk_queue_index, cp2.vk_queue_index);
    swap(cp1.vk_create_flags, cp2.vk_create_flags);
    swap(cp1.command_buffers, cp2.command_buffers);
}
