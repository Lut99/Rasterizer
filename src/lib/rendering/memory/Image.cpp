/* IMAGE.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 16:14:20
 * Last edited:
 *   16/08/2021, 16:14:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Image class, which wraps a VkImage managed by the
 *   MemoryPool.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ImageLayouts.hpp"

#include "MemoryPool.hpp"
#include "Image.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkImageMemoryBarrier struct. */
static void populate_image_barrier(VkImageMemoryBarrier& image_barrier, VkImage vk_image, VkImageLayout old_layout, VkImageLayout new_layout, VkAccessFlags source_mask, VkAccessFlags destination_mask) {
    // Initialize to default first
    image_barrier = {};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    // Define the layout transtition
    image_barrier.oldLayout = old_layout;
    image_barrier.newLayout = new_layout;

    // We don't need to transfer ownership rn
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    // Define the image in terms of subresource and w/e
    image_barrier.image = vk_image;
    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;

    // Define what operations to wait for before the pipeline can continue
    image_barrier.srcAccessMask = source_mask;
    image_barrier.dstAccessMask = destination_mask;
}





/***** IMAGE CLASS *****/
/* Constructor for the Image class, which takes the memory pool where it was allocated, the image object to wrap, its offset in the large pool, its extent, its format, its layout and its memory properties. Also takes other stuff that's needed to copy the image. */
Image::Image(const MemoryPool& pool, VkImage vk_image, VkDeviceSize offset, const VkExtent2D& vk_extent, VkFormat vk_format, VkImageLayout vk_layout, const VkMemoryRequirements& vk_requirements, VkImageUsageFlags image_usage, VkSharingMode sharing_mode, VkImageCreateFlags create_flags) :
    MemoryObject(pool, MemoryObjectType::image, offset),
    vk_image(vk_image),
    vk_extent(vk_extent),
    vk_format(vk_format),
    vk_layout(vk_layout),
    vk_requirements(vk_requirements),
    init_data({ image_usage, sharing_mode, create_flags })
{}

/* Destructor for the Buffer class. */
Image::~Image() {}



/* Schedules a layout transition on the given command buffer using a pipeline barrier. The layout is updated immediately internally. */
void Image::schedule_transition(const Rendering::CommandBuffer* command_buffer, VkImageLayout new_layout) {
    // Select the proper masks based on the old & new layouts
    VkAccessFlags source_mask, destination_mask;
    VkPipelineStageFlags source_stage, destination_stage;
    if (this->vk_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Set the mask (we have to be ready before writing)
        source_mask = 0;
        destination_mask = VK_ACCESS_TRANSFER_WRITE_BIT;

        // Set the shader stage mask (before we transfer images around)
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (this->vk_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Set the mask (transfer must be completed, shader not yet started)
        source_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        destination_mask = VK_ACCESS_SHADER_READ_BIT;

        // Set the shader stage mask (after transfer but before the fragment shader where we'll read)
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        // We call this transition unsupported for now
        logger.fatalc(Image::channel, "Unsupported transition from ", vk_image_layout_map[this->vk_layout], " to ", vk_image_layout_map[new_layout], '.');
    }

    // Prepare the image barrier
    VkImageMemoryBarrier image_barrier;
    populate_image_barrier(image_barrier, this->vk_image, this->vk_layout, new_layout, source_mask, destination_mask);

    // Schedule the pipeline barrier
    vkCmdPipelineBarrier(
        command_buffer->vulkan(),
        /* The stages where the pipeline will sit in between. */
        source_stage, destination_stage,
        /* Dependency flags on other resources. */
        0,
        /* 'Normal' memory barriers. */
        0, nullptr,
        /* Buffer memory barriers. */
        0, nullptr,
        /* Image memory barriers (yeS). */
        1, &image_barrier
    );

    // Update the internal layout, then done
    this->vk_layout = new_layout;
}
