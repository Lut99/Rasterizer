/* IMAGE.hpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:33:19
 * Last edited:
 *   19/06/2021, 12:33:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Image class, which wraps an already allocated VkImage
 *   object. Only usable in the context of the MemoryPool.
**/

#ifndef RENDERING_IMAGE_HPP
#define RENDERING_IMAGE_HPP

#include <vulkan/vulkan.h>

#include "MemoryHandle.hpp"

namespace Rasterizer::Rendering {
    /* Handle for image objects, which is used to reference images. */
    using image_h = memory_h;



    /* The Image class, which is a reference to an Image allocated by the MemoryPool. Do NOT change any fields in this class directly, as memory etc is managed by the memory pool. */
    class Image {
    private:
        /* Handle for this image object. */
        image_h vk_handle;

        /* The actual VkBuffer object constructed. */
        VkImage vk_image;
        /* The size of the image as a Vulkan extent. */
        VkExtent2D vk_extent;
        /* The format of the image. */
        VkFormat vk_format;
        /* The layout of the image. */
        VkImageLayout vk_layout;

        /* Describes the usage flags set for this image. */
        VkImageUsageFlags vk_usage_flags;
        /* Describes the sharing mode for this image. */
        VkSharingMode vk_sharing_mode;
        /* Describes the create flags for this image. */
        VkImageCreateFlags vk_create_flags;

        /* Reference to the large memory block where this image is allocated. */
        VkDeviceMemory vk_memory;
        /* The offset of the internal image. */
        VkDeviceSize vk_memory_offset;
        /* The size of the internal image. */
        VkDeviceSize vk_memory_size;
        /* The actual size of the internal image, as reported by memory_requirements.size. */
        VkDeviceSize vk_req_memory_size;
        /* The properties of the memory for this image. */
        VkMemoryPropertyFlags vk_memory_properties;

        /* Private constructor for the Buffer class, which takes the buffer, the buffer's size and the properties of the pool's memory. */
        Image(image_h handle, VkImage image, VkExtent2D vk_extent, VkFormat vk_format, VkImageLayout vk_layout, VkImageUsageFlags vk_usage_flags, VkSharingMode vk_sharing_mode, VkImageCreateFlags vk_create_flags, VkDeviceMemory vk_memory, VkDeviceSize memory_offset, VkDeviceSize memory_size, VkDeviceSize req_memory_size, VkMemoryPropertyFlags memory_properties);
        
        /* Mark the MemoryPool as friend. */
        friend class MemoryPool;

    public:
        /* Returns the extent of the image. */
        inline VkExtent2D extent() const { return this->vk_extent; }
        /* Returns the format of the image. */
        inline VkFormat format() const { return this->vk_format; }
        /* Returns the size of the image, in bytes. */
        inline VkDeviceSize size() const { return this->vk_memory_size; }
        /* Returns the memory offset of the buffer, in bytes. */
        inline VkDeviceSize offset() const { return this->vk_memory_offset; }
        /* Explicit retrieval of the internal buffer object. */
        inline VkImage image() const { return this->vk_image; }
        /* Implicit retrieval of the internal buffer object. */
        inline operator VkImage() const { return this->vk_image; }
        /* Explicit retrieval of the internal handle. */
        inline const image_h& handle() const { return this->vk_handle; }
        /* Implicit retrieval of the internal handle. */
        inline operator image_h() const { return this->vk_handle; }

        /* Swap operator for the Image class. */
        friend void swap(Image& i1, Image& i2);

    };

    /* Swap operator for the Image class. */
    void swap(Image& i1, Image& i2);
}

#endif
