/* COMMAND BUFFER.cpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:13:03
 * Last edited:
 *   19/06/2021, 12:13:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the CommandBuffer class, which is a wrapper around an already
 *   allocated VkCommandBuffer object. Only usable in the context of the
 *   CommandPool.
**/

#include "tools/CppDebugger.hpp"
#include "render_engine/auxillary/ErrorCodes.hpp"

#include "CommandPool.hpp"
#include "CommandBuffer.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Function that populates a given VkCommandBufferBeginInfo struct with the given values. */
static void populate_begin_info(VkCommandBufferBeginInfo& begin_info, VkCommandBufferUsageFlags usage_flags) {
    DENTER("populate_begin_info");

    // Set the deafult
    begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // Set the flags
    begin_info.flags = usage_flags;

    // Done
    DRETURN;
}

/* Function that populates a given VkSubmitINfo struct with the given values. */
static void populate_submit_info(VkSubmitInfo& submit_info, const VkCommandBuffer& vk_command_buffer) {
    DENTER("populate_submit_info");

    // Set to default
    submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Set the command buffer to submit
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_command_buffer;

    // Make sure that there are no semaphores used
    submit_info.signalSemaphoreCount = 0;
    submit_info.waitSemaphoreCount = 0;

    // Done
    DRETURN;
}






/***** COMMANDBUFFER CLASS *****/
/* Initializes the CommandBuffer to a default, unusable state. */
CommandBuffer::CommandBuffer() :
    vk_handle(CommandPool::NullHandle),
    vk_command_buffer(nullptr)
{}

/* Private constructor for the CommandBuffer, which only takes the handle to this buffer and the VkCommandBuffer object to wrap. */
CommandBuffer::CommandBuffer(command_buffer_h handle, VkCommandBuffer vk_command_buffer) :
    vk_handle(handle),
    vk_command_buffer(vk_command_buffer)
{}



/* Begins recording the command buffer. Overwrites whatever is already recorded here, for some reason. Takes optional usage flags for this recording. */
void CommandBuffer::begin(VkCommandBufferUsageFlags usage_flags) const {
    DENTER("Rendering::CommandBuffer::begin");

    // Populate the begin info struct
    VkCommandBufferBeginInfo begin_info;
    populate_begin_info(begin_info, usage_flags);

    // Initialize the recording
    VkResult vk_result;
    if ((vk_result = vkBeginCommandBuffer(this->vk_command_buffer, &begin_info)) != VK_SUCCESS) {
        DLOG(fatal, "Could not begin recording command buffer: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}

/* Ends recording the command buffer, but does not yet submit to any queue unless one is given. If so, then you can optionally specify to wait or not to wait for the queue to become idle. */
void CommandBuffer::end(VkQueue vk_queue, bool wait_queue_idle) const {
    DENTER("Rendering::CommandBuffer::end");

    // Whatever the parameters, always call the stop recording
    VkResult vk_result;
    if ((vk_result = vkEndCommandBuffer(this->vk_command_buffer)) != VK_SUCCESS) {
        DLOG(fatal, "Could not finish recording command buffer: " + vk_error_map[vk_result]);
    }

    // If a queue is given, then submit the buffer to that queue
    if (vk_queue != nullptr) {
        // Populate the submit info struct
        VkSubmitInfo submit_info;
        populate_submit_info(submit_info, this->vk_command_buffer);

        // Submit it to the queue
        if ((vk_result = vkQueueSubmit(vk_queue, 1, &submit_info, VK_NULL_HANDLE)) != VK_SUCCESS) {
            DLOG(fatal, "Could not submit command buffer to the given queue: " + vk_error_map[vk_result]);
        }

        // If we're told to wait, then do so
        if (wait_queue_idle) {
            if ((vk_result = vkQueueWaitIdle(vk_queue)) != VK_SUCCESS) {
                DLOG(fatal, "Could not wait for queue to become idle: " + vk_error_map[vk_result]);
            }
        }
    }

    // Done
    DRETURN;
}

/* Return the VkSubmitInfo for this command buffer. */
VkSubmitInfo CommandBuffer::get_submit_info() const {
    DENTER("Rendering::CommandBuffer::get_submit_info");
    
    // Populate the submit info struct
    VkSubmitInfo submit_info;
    populate_submit_info(submit_info, this->vk_command_buffer);

    // Done!
    DRETURN submit_info;
}

