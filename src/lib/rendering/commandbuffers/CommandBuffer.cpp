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

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "CommandPool.hpp"
#include "CommandBuffer.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Function that populates a given VkCommandBufferBeginInfo struct with the given values. */
static void populate_begin_info(VkCommandBufferBeginInfo& begin_info, VkCommandBufferUsageFlags usage_flags) {
    // Set the deafult
    begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // Set the flags
    begin_info.flags = usage_flags;
}

/* Function that populates a given VkSubmitINfo struct with the given values. */
static void populate_submit_info(VkSubmitInfo& submit_info, const VkCommandBuffer& vk_command_buffer) {
    // Set to default
    submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Set the command buffer to submit
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_command_buffer;

    // Make sure that there are no semaphores used
    submit_info.signalSemaphoreCount = 0;
    submit_info.waitSemaphoreCount = 0;
}





/***** COMMANDBUFFER CLASS *****/
/* Private constructor for the CommandBuffer, which only takes the handle to this buffer and the VkCommandBuffer object to wrap. */
CommandBuffer::CommandBuffer(VkCommandBuffer vk_command_buffer) :
    vk_command_buffer(vk_command_buffer)
{}

/* Private destructor for the CommandBuffer class. */
CommandBuffer::~CommandBuffer() {}



/* Begins recording the command buffer. Overwrites whatever is already recorded here, for some reason. Takes optional usage flags for this recording. */
void CommandBuffer::begin(VkCommandBufferUsageFlags usage_flags) const {
    // Populate the begin info struct
    VkCommandBufferBeginInfo begin_info;
    populate_begin_info(begin_info, usage_flags);

    // Initialize the recording
    VkResult vk_result;
    if ((vk_result = vkBeginCommandBuffer(this->vk_command_buffer, &begin_info)) != VK_SUCCESS) {
        logger.fatalc(CommandBuffer::channel, "Could not begin recording command buffer: ", vk_error_map[vk_result]);
    }
}

/* Ends recording the command buffer, but does not yet submit to any queue unless one is given. If so, then you can optionally specify to wait or not to wait for the queue to become idle. */
void CommandBuffer::end(VkQueue vk_queue, bool wait_queue_idle) const {
    // Whatever the parameters, always call the stop recording
    VkResult vk_result;
    if ((vk_result = vkEndCommandBuffer(this->vk_command_buffer)) != VK_SUCCESS) {
        logger.fatalc(CommandBuffer::channel, "Could not finish recording command buffer: ", vk_error_map[vk_result]);
    }

    // If a queue is given, then submit the buffer to that queue
    if (vk_queue != nullptr) {
        // Populate the submit info struct
        VkSubmitInfo submit_info;
        populate_submit_info(submit_info, this->vk_command_buffer);

        // Submit it to the queue
        if ((vk_result = vkQueueSubmit(vk_queue, 1, &submit_info, VK_NULL_HANDLE)) != VK_SUCCESS) {
            logger.fatalc(CommandBuffer::channel, "Could not submit command buffer to the given queue: ", vk_error_map[vk_result]);
        }

        // If we're told to wait, then do so
        if (wait_queue_idle) {
            if ((vk_result = vkQueueWaitIdle(vk_queue)) != VK_SUCCESS) {
                logger.fatalc(CommandBuffer::channel, "Could not wait for queue to become idle: ", vk_error_map[vk_result]);
            }
        }
    }
}

/* Return the VkSubmitInfo for this command buffer. */
VkSubmitInfo CommandBuffer::get_submit_info() const {
    // Populate the submit info struct
    VkSubmitInfo submit_info;
    populate_submit_info(submit_info, this->vk_command_buffer);

    // Done!
    return submit_info;
}
