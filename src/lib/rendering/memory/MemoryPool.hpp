/* MEMORY POOL.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 14:58:51
 * Last edited:
 *   16/08/2021, 14:58:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Second version of the MemoryPool class, which manages buffer and image
 *   memory on the device.
**/

#ifndef RENDERING_MEMORY_POOL_HPP
#define RENDERING_MEMORY_POOL_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "tools/Freelist.hpp"
#include "../gpu/GPU.hpp"

#include "MemoryObject.hpp"
#include "Buffer.hpp"
#include "Image.hpp"

namespace Rasterizer::Rendering {
    /* The MemoryPool class, which manages all Buffers and Images in the Rasterizer. */
    class MemoryPool {
    public:
        /* Channel name for the MemoryPool class. */
        static constexpr const char* channel = "MemoryPool";

        /* The GPU to which the pool is bound. */
        const Rendering::GPU& gpu;
    
    private:
        /* The memory type index we chose for this pool. */
        uint32_t memory_type;
        /* The memory we allocated for the pool. */
        VkDeviceMemory vk_memory;
        /* The Freelist in charge of keeping track of what is allocated. */
        Freelist free_list;

        /* The memory properties given to us at creation. */
        VkMemoryPropertyFlags vk_properties;

        /* List of all allocated objects in the pool. */
        Tools::Array<MemoryObject*> objects;


        /* Private helper function that does the actual memory allocation part. */
        VkDeviceSize _allocate(const VkMemoryRequirements& requirements);
    
    public:
        /* Constructor for the MemoryPool class, which takes the GPU where it lives, the size of its memory, the memory properties and optionally some buffer memory usage flags to take into account when selecting memory. */
        MemoryPool(const Rendering::GPU& gpu, VkDeviceSize pool_size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage = 0, VkImageUsageFlags image_usage = 0);
        /* Copy constructor for the MemoryPool class. */
        MemoryPool(const MemoryPool& other);
        /* Move constructor for the MemoryPool class. */
        MemoryPool(MemoryPool&& other);
        /* Destructor for the MemoryPool class. */
        ~MemoryPool();

        /* Tries to allocate a new Buffer of the given size (in bytes) and with the given usage flags. Optionally, one can set the sharing mode and any create flags. */
        Buffer* allocate(VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags create_flags = 0);
        /* Tries to allocate a new Buffer that is a copy of the given Buffer. */
        Buffer* allocate(const Buffer* other);

        /* Tries to allocate a new Image of the given size (in pixels), the given format, the given layout and with the given usage flags. Optionally, one can set the sharing mode and any create flags. */
        inline Image* allocate(uint32_t width, uint32_t height, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0)  { return this->allocate(VkExtent2D({ width, height }), image_format, image_layout, usage_flags, sharing_mode, create_flags); }
        /* Tries to allocate a new Image of the given size (in pixels), the given format, the given layout and with the given usage flags. Optionally, one can set the sharing mode and any create flags. */
        Image* allocate(const VkExtent2D& image_extent, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0);
        /* Tries to allocate a new Image that is a copy of the given Image. */
        Image* allocate(const Image* other);

        /* Deallocates the given MemoryObject. */
        void free(const MemoryObject* object);

        /* Returns the number of bytes used in the MemoryPool. */
        inline VkDeviceSize size() const { return static_cast<VkDeviceSize>(this->free_list.size()); }
        /* Returns the total number of bytes in the MemoryPool. */
        inline VkDeviceSize capacity() const { return static_cast<VkDeviceSize>(this->free_list.capacity()); }
        /* Returns the memory properties given to this pool. */
        inline VkMemoryPropertyFlags properties() const { return this->vk_properties; }
        /* Explicitly returns the internal VkDeviceMemory object. */
        inline const VkDeviceMemory& memory() const { return this->vk_memory; }
        /* Implicitly returns the internal VkDeviceMemory object. */
        inline operator const VkDeviceMemory&() const { return this->vk_memory; }

        /* Copy assignment operator for the MemoryPool class. */
        inline MemoryPool& operator=(const MemoryPool& other) { return *this = MemoryPool(other); }
        /* Move assignment operator for the MemoryPool class. */
        inline MemoryPool& operator=(MemoryPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the MemoryPool class. */
        friend void swap(MemoryPool& mp1, MemoryPool& mp2);

    };

    /* Swap operator for the MemoryPool class. */
    void swap(MemoryPool& mp1, MemoryPool& mp2);
}

#endif
