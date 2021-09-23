/* IMAGE.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 16:14:17
 * Last edited:
 *   16/08/2021, 16:14:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Image class, which wraps a VkImage managed by the
 *   MemoryPool.
**/

#ifndef RENDERING_IMAGE_HPP
#define RENDERING_IMAGE_HPP

#include <vulkan/vulkan.h>

#include "../commandbuffers/CommandBuffer.hpp"
#include "MemoryObject.hpp"

namespace Makma3D::Rendering {
    /* The Image class, which wraps a VkImage object and whos memory is managerd by the MemoryPool class. */
    class Image: public MemoryObject {
    public:
        /* Channel name for the Image class. */
        static constexpr const char* channel = "Image";

    private:
        /* The InitData struct, which is used to group everything needed for copying buffers. */
        struct InitData {
            /* The usage flags for this buffer. */
            VkImageUsageFlags image_usage;
            /* The sharing mode for this buffer. */
            VkSharingMode sharing_mode;
            /* The create flags for this buffer. */
            VkImageCreateFlags create_flags;
        };

    private:
        /* The Vulkan image object we wrap. */
        VkImage vk_image;
        /* The size (in pixels) of the image. */
        VkExtent2D vk_extent;
        /* The format of the image. */
        VkFormat vk_format;
        /* The layout of the image. */
        VkImageLayout vk_layout;
        /* The memory requirements of this specific object, including its real size (in bytes). */
        VkMemoryRequirements vk_requirements;

        /* Other data needed only for the buffer to be copyable. */
        InitData init_data;

        /* Declare the Buffer as friend. */
        friend class Buffer;
        /* Declare the random MemoryPool class as friend. */
        friend class MemoryPool;


        /* Constructor for the Image class, which takes the memory pool where it was allocated, the image object to wrap, its offset in the large pool, its extent, its format, its layout and its memory properties. Also takes other stuff that's needed to copy the image. */
        Image(const MemoryPool& pool, VkImage vk_image, VkDeviceSize offset, const VkExtent2D& vk_extent, VkFormat vk_format, VkImageLayout vk_layout, const VkMemoryRequirements& vk_requirements, VkImageUsageFlags image_usage, VkSharingMode sharing_mode, VkImageCreateFlags create_flags);
        /* Destructor for the Image class. */
        ~Image();
    
    public:
        /* Copy constructor for the Image class, which is deleted. */
        Image(const Image& other) = delete;
        /* Move constructor for the Image class, which is deleted. */
        Image(Image&& other) = delete;

        /* Schedules a layout transition on the given command buffer using a pipeline barrier. The layout is updated immediately internally. */
        void schedule_transition(const Rendering::CommandBuffer* command_buffer, VkImageLayout new_layout);

        /* Returns the extent of the image. */
        inline VkExtent2D extent() const { return this->vk_extent; }
        /* Returns the format of the image. */
        inline VkFormat format() const { return this->vk_format; }
        /* Returns the layout of the image. */
        inline VkImageLayout layout() const { return this->vk_layout; }
        /* Returns the memory offset of the buffer, in bytes. */
        inline VkDeviceSize offset() const { return this->object_offset; }
        /* Returns the conceptual size of the image, in bytes. */
        inline VkDeviceSize size() const { return 4 * this->vk_extent.width * this->vk_extent.height; }
        /* Returns the size of the image, in bytes. */
        inline VkDeviceSize rsize() const { return this->vk_requirements.size; }
        /* Explicit retrieval of the internal VkImage object. */
        inline const VkImage& vulkan() const { return this->vk_image; }
        /* Implicit retrieval of the internal VkImage object. */
        inline operator const VkImage&() const { return this->vk_image; }

        /* Copy assignment operator for the Image class, which is deleted. */
        Image& operator=(const Image& other) = delete;
        /* Copy assignment operator for the Image class, which is deleted. */
        Image& operator=(Image&& other) = delete;

    };

}

#endif
