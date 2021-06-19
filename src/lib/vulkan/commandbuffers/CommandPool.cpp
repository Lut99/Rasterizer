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

#include "tools/CppDebugger.hpp"
#include "vulkan/ErrorCodes.hpp"

#include "CommandPool.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Given a map with handles as keys, finds the first free handle that is not the null handle. */
static command_buffer_h find_handle(const std::unordered_map<command_buffer_h, VkCommandBuffer>& vk_command_buffers, command_buffer_h null_handle) {
    DENTER("find_handle");

    // Loop through the map and find the first free handle
    command_buffer_h result = 0;
    bool unique = false;
    while (!unique) {
        unique = true;
        for (const std::pair<command_buffer_h, VkCommandBuffer>& p : vk_command_buffers) {
            if (result == null_handle || result == p.first) {
                // If result is the maximum value, then throw an error
                if (result == std::numeric_limits<command_buffer_h>::max()) {
                    DLOG(fatal, "command_buffer_h overflow; cannot allocate more buffers.");
                }

                // Otherwise, increment and re-try
                ++result;
                unique = false;
                break;
            }
        }
    }

    // Return the result we found
    DRETURN result;
}





/***** POPULATE FUNCTIONS *****/
/* Functions that populates a given VkCommandPoolCreateInfo struct with the given values. */
static void populate_command_pool_info(VkCommandPoolCreateInfo& command_pool_info, uint32_t queue_index, VkCommandPoolCreateFlags create_flags) {
    DENTER("populate_command_pool_info");

    // Set to default
    command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    // Set the queue we're the pool for
    command_pool_info.queueFamilyIndex = queue_index;

    // Set the create flags
    command_pool_info.flags = create_flags;

    // Done!
    DRETURN;
}

/* Function that populates a given VkCommandBufferAllocateInfo struct with the given values. */
static void populate_allocate_info(VkCommandBufferAllocateInfo& allocate_info, VkCommandPool vk_command_pool, uint32_t n_buffers, VkCommandBufferLevel buffer_level) {
    DENTER("populate_allocate_info");

    // Set to default
    allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    // Set the buffer level for this buffer
    allocate_info.level = buffer_level;

    // Tell it which command pool to allocate with
    allocate_info.commandPool = vk_command_pool;

    // Tell it how many buffers to allocate in one go
    allocate_info.commandBufferCount = n_buffers;

    // Done!
    DRETURN;
}





/***** COMMANDPOOL CLASS *****/
/* Constructor for the CommandPool class, which takes the GPU to allocate for, the queue index for which this pool allocates buffers and optionally create flags for the pool. */
CommandPool::CommandPool(const GPU& gpu, uint32_t queue_index, VkCommandPoolCreateFlags create_flags) :
    gpu(gpu),
    vk_queue_index(queue_index),
    vk_create_flags(create_flags)
{
    DENTER("Vulkan::CommandPool::CommandPool");
    DLOG(info, "Initializing CommandPool for queue " + std::to_string(this->vk_queue_index) + "...");

    // Start by populating the create info
    VkCommandPoolCreateInfo command_pool_info;
    populate_command_pool_info(command_pool_info, this->vk_queue_index, create_flags);

    // Call the create
    VkResult vk_result;
    if ((vk_result = vkCreateCommandPool(this->gpu, &command_pool_info, nullptr, &this->vk_command_pool)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create CommandPool: " + vk_error_map[vk_result]);
    }

    // Done
    DLEAVE;
}

/* Copy constructor for the CommandPool class. */
CommandPool::CommandPool(const CommandPool& other) :
    gpu(other.gpu),
    vk_queue_index(other.vk_queue_index),
    vk_create_flags(other.vk_create_flags)
{
    DENTER("Vulkan::CommandPool::CommandPool(copy)");

    // Start by populating the create info
    VkCommandPoolCreateInfo command_pool_info;
    populate_command_pool_info(command_pool_info, this->vk_queue_index, this->vk_create_flags);

    // Call the create
    VkResult vk_result;
    if ((vk_result = vkCreateCommandPool(this->gpu, &command_pool_info, nullptr, &this->vk_command_pool)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create CommandPool: " + vk_error_map[vk_result]);
    }

    DLEAVE;
}

/* Move constructor for the CommandPool class. */
CommandPool::CommandPool(CommandPool&& other) :
    gpu(other.gpu),
    vk_command_pool(other.vk_command_pool),
    vk_queue_index(other.vk_queue_index),
    vk_command_buffers(std::move(other.vk_command_buffers))
{
    other.vk_command_pool = nullptr;
    other.vk_command_buffers.clear();
}

/* Destructor for the CommandPool class. */
CommandPool::~CommandPool() {
    DENTER("Vulkan::CommandPool::~CommandPool");
    DLOG(info, "Cleaning CommandPool for queue " + std::to_string(this->vk_queue_index) + "...");
    DINDENT;

    if (this->vk_command_buffers.size() > 0) {
        DLOG(info, "Cleaning command buffers...");
        for (const std::pair<command_buffer_h, VkCommandBuffer>& p : this->vk_command_buffers) {
            vkFreeCommandBuffers(this->gpu, this->vk_command_pool, 1, &p.second);
        }
    }

    if (this->vk_command_pool != nullptr) {
        DLOG(info, "Deallocating the pool...");
        vkDestroyCommandPool(this->gpu, this->vk_command_pool, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Allocates a single, new command buffer of the given level. Returns by handle. */
command_buffer_h CommandPool::allocate_h(VkCommandBufferLevel buffer_level) {
    DENTER("Vulkan::CommandPool::allocate_h");

    // Pick a suitable memory location for this buffer; either as a new buffer or a previously deallocated one
    command_buffer_h handle = find_handle(this->vk_command_buffers, CommandPool::NullHandle);

    // Then, prepare the create info
    VkCommandBufferAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, this->vk_command_pool, 1, buffer_level);

    // Call the create
    VkCommandBuffer buffer;
    VkResult vk_result;
    if ((vk_result = vkAllocateCommandBuffers(this->gpu, &allocate_info, &buffer)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate command buffer: " + vk_error_map[vk_result]);
    }

    // Inject the result in the map
    this->vk_command_buffers.insert(std::make_pair(handle, buffer));

    // Done, return the handle
    DRETURN handle;
}

/* Allocates a single, new command buffer of the given level. Returns new buffer objects. */
Tools::Array<CommandBuffer> CommandPool::nallocate(uint32_t n_buffers, VkCommandBufferLevel buffer_level) {
    DENTER("Vulkan::CommandPool::nallocate");
    
    // Allocate a list of handles
    Tools::Array<command_buffer_h> handles = this->nallocate_h(n_buffers, buffer_level);
    // Create a list to return
    Tools::Array<CommandBuffer> to_return(handles.size());
    // Convert all of the handles to buffers
    for (uint32_t i = 0; i < handles.size(); i++) {
        to_return.push_back(this->deref(handles[i]));
    }

    // Done, return
    DRETURN to_return;
}

/* Allocates N new command buffers of the given level. Returns by handles. */
Tools::Array<command_buffer_h> CommandPool::nallocate_h(uint32_t n_buffers, VkCommandBufferLevel buffer_level) {
    DENTER("Vulkan::CommandPool::nallocate_h");

    // Pick n_buffers suitable memory locations for this buffer; either as a new buffer or a previously deallocated one
    Tools::Array<command_buffer_h> handles(n_buffers);
    for (uint32_t i = 0; i < handles.size(); i++) {
        handles.push_back(find_handle(this->vk_command_buffers, CommandPool::NullHandle));
    }

    // Prepare some temporary local space for the buffers
    Tools::Array<VkCommandBuffer> buffers(n_buffers);

    // Then, prepare the create info
    VkCommandBufferAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, this->vk_command_pool, n_buffers, buffer_level);

    // Call the create
    VkResult vk_result;
    if ((vk_result = vkAllocateCommandBuffers(this->gpu, &allocate_info, buffers.wdata(n_buffers))) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate command buffers: " + vk_error_map[vk_result]);
    }

    // Inject each of the buffers into the map
    for (uint32_t i = 0; i < buffers.size(); i++) {
        this->vk_command_buffers.insert(std::make_pair(handles[i], buffers[i]));
    }

    // Done, return the list of handles
    DRETURN handles;
}



/* Deallocates the CommandBuffer behind the given handle. Note that all buffers are deallocated automatically when the CommandPool is destructed, but this could save you memory. */
void CommandPool::deallocate(command_buffer_h buffer) {
    DENTER("Vulkan::CommandPool::deallocate");

    // Check if the handle exists
    std::unordered_map<command_buffer_h, VkCommandBuffer>::iterator iter = this->vk_command_buffers.find(buffer);
    if (iter == this->vk_command_buffers.end()) {
        DLOG(fatal, "Given handle does not exist.");
    }

    // If it does, then first free the buffer
    vkFreeCommandBuffers(this->gpu, this->vk_command_pool, 1, &((*iter).second));

    // Remove it from the list
    this->vk_command_buffers.erase(iter);

    // Done
    DRETURN;
}

/* Deallocates an array of given command buffers. */
void CommandPool::ndeallocate(const Tools::Array<CommandBuffer>& buffers) {
    DENTER("CommandPool::ndeallocate(objects)");

    // Convert the list of descriptor sets to handles
    Tools::Array<command_buffer_h> handles(buffers.size());
    for (uint32_t i = 0; i < buffers.size(); i++) {
        handles.push_back(buffers[i].handle());
    }

    // Call the ndeallocate for handles
    this->ndeallocate(handles);

    DRETURN;
}

/* Deallocates an array of given command buffer handles. */
void CommandPool::ndeallocate(const Tools::Array<command_buffer_h>& handles) {
    DENTER("CommandPool::ndeallocate(handles)");

    // First, we check if all handles exist
    Tools::Array<VkCommandBuffer> to_remove(handles.size());
    for (uint32_t i = 0; i < handles.size(); i++) {
        // Do the check
        std::unordered_map<command_buffer_h, VkCommandBuffer>::iterator iter = this->vk_command_buffers.find(handles[i]);
        if (iter == this->vk_command_buffers.end()) {
            DLOG(fatal, "Handle at index " + std::to_string(i) + " does not exist.");
        }

        // Mark the Vk object for removal
        to_remove.push_back((*iter).second);

        // Remove the item from the internal map
        this->vk_command_buffers.erase(iter);
    }

    // All that's left is to actually remove the handles; do that
    vkFreeCommandBuffers(this->gpu, this->vk_command_pool, to_remove.size(), to_remove.rdata());

    // Done
    DRETURN;
}


/* Swap operator for the CommandPool class. */
void Vulkan::swap(CommandPool& cp1, CommandPool& cp2) {
    DENTER("Vulkan::swap(CommandPool)");

    using std::swap;

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (cp1.gpu != cp2.gpu) {
        DLOG(fatal, "Cannot swap command pools with different GPUs");
    }
    #endif

    swap(cp1.vk_command_pool, cp2.vk_command_pool);
    swap(cp1.vk_queue_index, cp2.vk_queue_index);
    swap(cp1.vk_create_flags, cp2.vk_create_flags);
    swap(cp1.vk_command_buffers, cp2.vk_command_buffers);

    DRETURN;
}
