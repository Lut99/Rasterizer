/* IMAGE.cpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:33:24
 * Last edited:
 *   19/06/2021, 12:33:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Image class, which wraps an already allocated VkImage
 *   object. Only usable in the context of the MemoryPool.
**/

#include "../auxillary/ErrorCodes.hpp"

#include "Image.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** IMAGE CLASS *****/
/* Private constructor for the Buffer class, which takes the buffer, the buffer's size and the properties of the pool's memory. */
Image::Image(image_h handle, VkImage image, VkExtent2D vk_extent, VkFormat vk_format, VkImageLayout vk_layout, VkImageUsageFlags vk_usage_flags, VkSharingMode vk_sharing_mode, VkImageCreateFlags vk_create_flags, VkDeviceMemory vk_memory, VkDeviceSize memory_offset, VkDeviceSize memory_size, VkDeviceSize req_memory_size, VkMemoryPropertyFlags memory_properties) :
    vk_handle(handle),
    vk_image(image),
    vk_extent(vk_extent),
    vk_format(vk_format),
    vk_layout(vk_layout),
    vk_usage_flags(vk_usage_flags),
    vk_sharing_mode(vk_sharing_mode),
    vk_create_flags(vk_create_flags),
    vk_memory(vk_memory),
    vk_memory_offset(memory_offset),
    vk_memory_size(memory_size),
    vk_req_memory_size(req_memory_size),
    vk_memory_properties(memory_properties)
{}



/* Swap operator for the Image class. */
void Rendering::swap(Image& i1, Image& i2) {
    using std::swap;

    swap(i1.vk_handle, i2.vk_handle);
    swap(i1.vk_image, i2.vk_image);
    swap(i1.vk_extent, i2.vk_extent);
    swap(i1.vk_format, i2.vk_format);
    swap(i1.vk_layout, i2.vk_layout);
    swap(i1.vk_usage_flags, i2.vk_usage_flags);
    swap(i1.vk_sharing_mode, i2.vk_sharing_mode);
    swap(i1.vk_create_flags, i2.vk_create_flags);
    swap(i1.vk_memory, i2.vk_memory);
    swap(i1.vk_memory_offset, i2.vk_memory_offset);
    swap(i1.vk_memory_size, i2.vk_memory_size);
    swap(i1.vk_req_memory_size, i2.vk_req_memory_size);
    swap(i1.vk_memory_properties, i2.vk_memory_properties);
}
