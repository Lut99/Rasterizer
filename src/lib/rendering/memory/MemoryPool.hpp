/* MEMORY POOL.hpp
 *   by Lut99
 *
 * Created:
 *   25/04/2021, 11:36:35
 * Last edited:
 *   27/07/2021, 16:21:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MemoryPool class, which is in charge of a single pool of
 *   GPU memory that it can hand out to individual buffers.
**/

#ifndef RENDERING_MEMORY_POOL_HPP
#define RENDERING_MEMORY_POOL_HPP

#include <vulkan/vulkan.h>
#include <unordered_map>

#include "tools/Array.hpp"
#include "tools/Freelist.hpp"
#include "../gpu/GPU.hpp"
#include "../commandbuffers/CommandBuffer.hpp"

#include "MemoryHandle.hpp"
#include "Buffer.hpp"
#include "Image.hpp"

namespace Rasterizer::Rendering {
    /* The MemoryPool class serves as a memory manager for our GPU memory. */
    class MemoryPool {
    public:
        /* Immutable reference to the GPU object where this pool is linked to. */
        const Rendering::GPU& gpu;

    private:
        /* Internal enum used to differentiate between memory types. */
        enum class MemoryBlockType {
            none = 0,
            buffer = 1,
            image = 2
        };

        /* Internal base struct used to represent used memory blocks. */
        struct UsedBlock {
            /* The type of this block. */
            MemoryBlockType type;
            /* Pointer to the start of the block. */
            VkDeviceSize start;
            /* Length of the free block, in bytes. */
            VkDeviceSize length;
            /* The actual size of the block, as reported by memsize.size. */
            VkDeviceSize req_length;

            /* Constructor for the UsedBlock struct, which requires at least a type. */
            UsedBlock(MemoryBlockType type): type(type) {}
        };
        /* Internal specialized struct used to represent memory blocks used for buffers. */
        struct BufferBlock: public UsedBlock {
            /* The actual VkBuffer object that is bound to this memory location. */
            VkBuffer vk_buffer;
            /* The usage flags of the internal buffer. */
            VkBufferUsageFlags vk_usage_flags;
            /* The create flags used to create this buffer. */
            VkBufferCreateFlags vk_create_flags;
            /* The sharing mode of the buffer. */
            VkSharingMode vk_sharing_mode;

            /* Constructor for the BufferBlock struct. */
            BufferBlock(): UsedBlock(MemoryBlockType::buffer) {}
        };
        /* Internal specialized struct used to represent memory blocks used for images. */
        struct ImageBlock: public UsedBlock {
            /* The actual VkImage object that is bound to this memory location. */
            VkImage vk_image;
            /* The size of the image, as a VkExtent3D. */
            VkExtent3D vk_extent;
            /* The format of the image. */
            VkFormat vk_format;
            /* The initial layout of the image. */
            VkImageLayout vk_layout;
            /* The usage flags of the internal buffer. */
            VkImageUsageFlags vk_usage_flags;
            /* The create flags used to create this buffer. */
            VkImageCreateFlags vk_create_flags;
            /* The sharing mode of the buffer. */
            VkSharingMode vk_sharing_mode;

            /* Constructor for the ImageBlock struct. */
            ImageBlock(): UsedBlock(MemoryBlockType::image) {}
        };

        /* The allocated memory on the GPU. */
        VkDeviceMemory vk_memory;
        /* The type of memory we allocated for this. */
        uint32_t vk_memory_type;
        /* The total size of the allocated memory block. */
        VkDeviceSize vk_memory_size;
        /* The memory properties assigned to this buffer. */
        VkMemoryPropertyFlags vk_memory_properties;

        /* List of all allocated buffers in this pool. */
        std::unordered_map<memory_h, UsedBlock*> vk_used_blocks;
        /* Keeps track of the free blocks in the array of allocated memory */
        Tools::Freelist free_list;
        

        /* Private helper function that takes a BufferBlock, and uses it to initialize the given buffer. */
        inline static Buffer init_buffer(const Rendering::GPU& gpu, buffer_h handle, BufferBlock* block, VkDeviceMemory vk_memory, VkMemoryPropertyFlags memory_properties) { return Buffer(gpu, handle, block->vk_buffer, block->vk_usage_flags, block->vk_sharing_mode, block->vk_create_flags, vk_memory, block->start, block->length, block->req_length, memory_properties); }
        /* Private helper function that takes a UsedBlock, and uses it to initialize the given buffer. */
        inline static Image init_image(image_h handle, ImageBlock* block, VkDeviceMemory vk_memory, VkMemoryPropertyFlags memory_properties) { return Image(handle, block->vk_image, VkExtent2D({ block->vk_extent.width, block->vk_extent.height }), block->vk_format, block->vk_layout, block->vk_usage_flags, block->vk_sharing_mode, block->vk_create_flags, vk_memory, block->start, block->length, block->req_length, memory_properties); }

        /* Private helper function that actually performs memory allocation. Returns a reference to a UsedBlock that describes the block allocated. */
        memory_h allocate_memory(MemoryBlockType type, VkDeviceSize n_bytes, const VkMemoryRequirements& mem_requirements);

    public:
        /* The null handle for the pool. */
        const static constexpr memory_h NullHandle = 0;

        
        /* Constructor for the MemoryPool class, which takes a device to allocate on, the type of memory we will allocate on and the total size of the allocated block. */
        MemoryPool(const Rendering::GPU& gpu, uint32_t memory_type, VkDeviceSize n_bytes, VkMemoryPropertyFlags memory_properties = 0);
        /* Copy constructor for the MemoryPool class, which is deleted. */
        MemoryPool(const MemoryPool& other);
        /* Move constructor for the MemoryPool class. */
        MemoryPool(MemoryPool&& other);
        /* Destructor for the MemoryPool class. */
        ~MemoryPool();

        /* Returns a reference to the internal buffer with the given handle. Always performs out-of-bounds checking. */
        inline Buffer deref_buffer(buffer_h buffer) const { return init_buffer(this->gpu, buffer, (BufferBlock*) this->vk_used_blocks.at(buffer), this->vk_memory, this->vk_memory_properties); }
        /* Returns a reference to the internal image with the given handle. Always performs out-of-bounds checking. */
        inline Image deref_image(image_h image) const { return init_image(image, (ImageBlock*) this->vk_used_blocks.at(image), this->vk_memory, this->vk_memory_properties); }

        /* Tries to get a new buffer from the pool of the given size and with the given flags. Applies extra checks if NDEBUG is not defined. */
        inline Buffer allocate_buffer(VkDeviceSize n_bytes, VkBufferUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags create_flags = 0) { return this->deref_buffer(this->allocate_buffer_h(n_bytes, usage_flags, sharing_mode, create_flags)); }
        /* Allocates a new buffer that has the same specifications as the given Buffer object. Note that the given Buffer needn't be allocated with the same pool as this one. */
        inline Buffer allocate_buffer(const Buffer& buffer) { return this->allocate_buffer(buffer.vk_memory_size, buffer.vk_usage_flags, buffer.vk_sharing_mode, buffer.vk_create_flags); }
        /* Allocates a new buffer that has the same specifications as the given Buffer handle. Note that the buffer has to be allocated with this memory pool. */
        inline Buffer allocate_buffer(buffer_h buffer) { return this->allocate_buffer(this->deref_buffer(buffer)); }
        /* Tries to get a new buffer from the pool of the given size and with the given flags, returning only its handle. Applies extra checks if NDEBUG is not defined. */
        buffer_h allocate_buffer_h(VkDeviceSize n_bytes, VkBufferUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags create_flags = 0);
        /* Allocates a new buffer that has the same specifications as the given Buffer object, but we return only its handle. Note that the given Buffer needn't be allocated with the same pool as this one. */
        inline buffer_h allocate_buffer_h(const Buffer& buffer) { return this->allocate_buffer_h(buffer.vk_memory_size, buffer.vk_usage_flags, buffer.vk_sharing_mode, buffer.vk_create_flags); }
        /* Allocates a new buffer that has the same specifications as the given Buffer handle,returning another handle. Note that the buffer has to be allocated with this memory pool. */
        inline buffer_h allocate_buffer_h(buffer_h buffer) { return this->allocate_buffer_h(this->deref_buffer(buffer)); }
        /* Tries to get a new image from the pool of the given sizes and with the given flags. Applies extra checks if NDEBUG is not defined. */
        inline Image allocate_image(uint32_t width, uint32_t height, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0) { return this->deref_image(this->allocate_image_h(width, height, image_format, image_layout, usage_flags, sharing_mode, create_flags)); }
        /* Tries to get a new image from the pool of the given sizes (but as an extent this time) and with the given flags. Applies extra checks if NDEBUG is not defined. */
        inline Image allocate_image(const VkExtent2D& extent, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0) { return this->allocate_image(extent.width, extent.height, image_format, image_layout, usage_flags, sharing_mode, create_flags); }
        /* Allocates a new image that has the same specifications as the given Image object. Note that the given Image needn't be allocated with the same pool as this one. */
        inline Image allocate_image(const Image& image) { return this->allocate_image(image.vk_extent.width, image.vk_extent.height, image.vk_format, image.vk_layout, image.vk_usage_flags, image.vk_sharing_mode, image.vk_format); }
        /* Tries to get a new image from the pool of the given sizes and with the given flags, returning only its handle. Applies extra checks if NDEBUG is not defined. */
        image_h allocate_image_h(uint32_t width, uint32_t height, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0);
        /* Tries to get a new image from the pool of the given sizes (but as an extent this time) and with the given flags, returning only its handle. Applies extra checks if NDEBUG is not defined. */
        inline image_h allocate_image_h(const VkExtent2D& extent, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0) { return this->allocate_image_h(extent.width, extent.height, image_format, image_layout, usage_flags, sharing_mode, create_flags); }
        /* Allocates a new image that has the same specifications as the given Image object, returning only its handle. Note that the given Image needn't be allocated with the same pool as this one. */
        inline image_h allocate_image_h(const Image& image) { return this->allocate_image_h(image.vk_extent.width, image.vk_extent.height, image.vk_format, image.vk_layout, image.vk_usage_flags, image.vk_sharing_mode, image.vk_format); }
        /* Deallocates the buffer or image with the given handle. Does not throw an error if the handle doesn't exist, unless NDEBUG is not defined. */
        void deallocate(memory_h handle);

        /* Defragements the entire pool, aligning all buffers next to each other in memory to create a maximally sized free block. Note that existing handles will remain valid. */
        void defrag();

        /* Copy assignment operator for the MemoryPool class. */
        inline MemoryPool& operator=(const MemoryPool& other) { return *this = MemoryPool(other); }
        /* Move assignment operator for the MemoryPool class. */
        inline MemoryPool& operator=(MemoryPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the MemoryPool class. */
        friend void swap(MemoryPool& mp1, MemoryPool& mp2);

        /* Static function that helps users decide the best memory queue. */
        static uint32_t select_memory_type(const Rendering::GPU& gpu, VkBufferUsageFlags usage_flags = 0, VkMemoryPropertyFlags memory_properties = 0, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags create_flags = 0);
        /* Static function that helps users decide the best memory queue for images. */
        static uint32_t select_memory_type(const Rendering::GPU& gpu, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageCreateFlags create_flags = 0);

    };

    /* Swap operator for the MemoryPool class. */
    void swap(MemoryPool& mp1, MemoryPool& mp2);

}

#endif
