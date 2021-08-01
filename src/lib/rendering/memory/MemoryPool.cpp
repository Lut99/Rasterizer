/* MEMORY POOL.cpp
 *   by Lut99
 *
 * Created:
 *   25/04/2021, 11:36:42
 * Last edited:
 *   25/05/2021, 18:14:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MemoryPool class, which is in charge of a single pool of
 *   GPU memory that it can hand out to individual buffers.
**/

#include <limits>
#include "tools/CppDebugger.hpp"

#include "../auxillary/ErrorCodes.hpp"
#include "tools/Common.hpp"

#include "MemoryPool.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VKBufferCreateInfo struct. */
static void populate_buffer_info(VkBufferCreateInfo& buffer_info, VkDeviceSize n_bytes, VkBufferUsageFlags usage_flags, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags) {
    DENTER("populate_buffer_info");

    // Only set to default
    buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // Set the parameters passed
    buffer_info.size = n_bytes;
    buffer_info.usage = usage_flags;
    buffer_info.sharingMode = sharing_mode;
    buffer_info.flags = create_flags;

    // Done
    DRETURN;
}

/* Populates a given VkImageCreateInfo struct. */
static void populate_image_info(VkImageCreateInfo& image_info, const VkExtent3D& image_size, VkFormat image_format, VkImageLayout image_layout, VkBufferUsageFlags usage_flags, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags) {
    DENTER("populate_image_info");

    // Only set to default
    image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    // Set the parameters passed
    image_info.extent = image_size;
    image_info.format = image_format;
    image_info.initialLayout = image_layout;
    image_info.usage = usage_flags;
    image_info.sharingMode = sharing_mode;
    image_info.flags = create_flags;
    
    // Set the image-specific parameters
    image_info.arrayLayers = 1;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.mipLevels = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;

    // Since the sharing mode will be exlusive by default, let's for now hardcode this to not have a queue family
    image_info.queueFamilyIndexCount = 0;

    // Done
    DRETURN;
}

/* Populates a given VkMemoryAllocateInfo struct. */
static void populate_allocate_info(VkMemoryAllocateInfo& allocate_info, uint32_t memory_type, VkDeviceSize n_bytes) {
    DENTER("populate_allocate_info");

    // Set to default & define the stryct type
    allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    // Set the allocation size of the memory
    allocate_info.allocationSize = n_bytes;
    // Set the type of the memory to allocate
    allocate_info.memoryTypeIndex = memory_type;

    // Done
    DRETURN;
}





/***** MEMORYPOOL CLASS *****/
/* Constructor for the MemoryPool class, which takes a device to allocate on, the type of memory we will allocate on and the total size of the allocated block. */
MemoryPool::MemoryPool(const GPU& gpu, uint32_t memory_type, VkDeviceSize n_bytes, VkMemoryPropertyFlags memory_properties) :
    gpu(gpu),
    vk_memory_type(memory_type),
    vk_memory_size(n_bytes),
    vk_memory_properties(memory_properties),
    free_list(this->vk_memory_size)
{
    DENTER("Rendering::MemoryPool::MemoryPool");
    DLOG(info, "Initializing MemoryPool...");
    DINDENT;



    #ifndef NDEBUG
    DLOG(info, "Validating memory requirements...");

    // Get the memory properties of the GPU
    VkPhysicalDeviceMemoryProperties gpu_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_properties);

    // Check if the chosen memory type has the desired properties
    if (memory_type >= 32) {
        DLOG(fatal, "Memory type is out of range (0 <= memory_type < 32)");
    }
    if ((gpu_properties.memoryTypes[memory_type].propertyFlags & this->vk_memory_properties) != this->vk_memory_properties) {
        DLOG(fatal, "Chosen memory type with index " + std::to_string(memory_type) + " does not support the specified memory properties.");
    }

    #endif
    


    // Allocate the block of memory that we shall use
    DLOG(info, "Allocating memory on device '" + gpu.name() + "'...");
    VkMemoryAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, memory_type, n_bytes);

    // Do the allocation
    VkResult vk_result;
    if ((vk_result = vkAllocateMemory(this->gpu, &allocate_info, nullptr, &this->vk_memory)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate memory on device: " + vk_error_map[vk_result]);
    }



    DDEDENT;
    DLEAVE;
}

/* Copy constructor for the MemoryPool class. */
MemoryPool::MemoryPool(const MemoryPool& other) :
    gpu(other.gpu),
    vk_memory_type(other.vk_memory_type),
    vk_memory_size(other.vk_memory_size),
    vk_memory_properties(other.vk_memory_properties),
    free_list(this->vk_memory_size)
{
    DENTER("Rendering::MemoryPool::MemoryPool(copy)");

    // Allocate a new block of memory that we shall use, with the proper size
    VkMemoryAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, this->vk_memory_type, this->vk_memory_size);

    // Do the allocation
    VkResult vk_result;
    if ((vk_result = vkAllocateMemory(this->gpu, &allocate_info, nullptr, &this->vk_memory)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate memory on device: " + vk_error_map[vk_result]);
    }

    // Do not copy handles with us, as that doesn't really make a whole lotta sense

    DLEAVE;
}

/* Move constructor for the MemoryPool class. */
MemoryPool::MemoryPool(MemoryPool&& other):
    gpu(other.gpu),
    vk_memory(other.vk_memory),
    vk_memory_type(other.vk_memory_type),
    vk_memory_size(other.vk_memory_size),
    vk_memory_properties(other.vk_memory_properties),
    vk_used_blocks(other.vk_used_blocks),
    free_list(other.free_list)
{
    // Set the other's memory to nullptr to avoid deallocation
    other.vk_memory = nullptr;
    other.vk_used_blocks.clear();
}

/* Destructor for the MemoryPool class. */
MemoryPool::~MemoryPool() {
    DENTER("Rendering::MemoryPool::~MemoryPool");
    DLOG(info, "Cleaning MemoryPool...");
    DINDENT;

    // Delete all buffers & images
    if (this->vk_used_blocks.size() > 0) {
        DLOG(info, "Deallocating buffers & images...");
        for (const std::pair<memory_h, UsedBlock*>& p : this->vk_used_blocks) {
            // Either destroy the buffer or the image
            if (p.second->type == MemoryBlockType::buffer) {
                vkDestroyBuffer(this->gpu, ((BufferBlock*) p.second)->vk_buffer, nullptr);
            } else if (p.second->type == MemoryBlockType::image) {
                vkDestroyImage(this->gpu, ((ImageBlock*) p.second)->vk_image, nullptr);
            }

            // Then destroy the block itself
            delete p.second;
        }
    }

    // Deallocate the allocated memory
    if (this->vk_memory != nullptr) {
        DLOG(info, "Deallocating device memory...");
        vkFreeMemory(this->gpu, this->vk_memory, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Private helper function that actually performs memory allocation. Returns a reference to a UsedBlock that describes the block allocated. */
memory_h MemoryPool::allocate_memory(MemoryBlockType type, VkDeviceSize n_bytes, const VkMemoryRequirements& mem_requirements) {
    DENTER("Rendering::MemoryPool::allocate_memory");

    // Pick a suitable memory location for this block in the array of used blocks; either as a new block or use the location of a previously allocated one
    memory_h result = 0;
    bool unique = false;
    while (!unique) {
        unique = true;
        for (const std::pair<memory_h, UsedBlock*>& p : this->vk_used_blocks) {
            if (result == MemoryPool::NullHandle || result == p.first) {
                // If result is the maximum value, then throw an error
                if (result == std::numeric_limits<memory_h>::max()) {
                    DLOG(fatal, "Memory handle overflow; cannot allocate more buffers.");
                }

                // Otherwise, increment and re-try
                ++result;
                unique = false;
                break;
            }
        }
    }
    // Reserve space in our map
    this->vk_used_blocks.insert(std::make_pair(result, type == MemoryBlockType::buffer ? (UsedBlock*) new BufferBlock() : (UsedBlock*) new ImageBlock()));
    UsedBlock* block = this->vk_used_blocks.at(result);

    #ifndef NDEBUG
    // Next, make sure the given memory requirements are aligning with our internal type
    if (!(mem_requirements.memoryTypeBits & (1 << this->vk_memory_type))) {
        DLOG(fatal, "Buffer is not compatible with this memory pool.");
    }
    #endif

    // Next, find the first free block
    VkDeviceSize offset = this->free_list.reserve(mem_requirements.size, mem_requirements.alignment);
    if (offset == std::numeric_limits<freelist_size_t>::max()) { DLOG(fatal, "Could not allocate new buffer: not enough space left in pool (need " + Tools::bytes_to_string(mem_requirements.size) + ", but " + Tools::bytes_to_string(this->free_list.capacity() - this->free_list.size()) + " free)"); }
    else if (offset == std::numeric_limits<freelist_size_t>::max() - 1) { DLOG(fatal, "Could not allocate new buffer: no large enough block found, but we do have enough memory available; call defrag() first"); }

    // Store the chosen parameters in the buffer for easy re-creation
    block->start = offset;
    block->length = n_bytes;
    block->req_length = mem_requirements.size;

    // We're done, so return the block for further initialization of the image or buffer
    DRETURN result;
}



/* Tries to get a new buffer from the pool of the given size and with the given flags, returning only its handle. Applies extra checks if NDEBUG is not defined. */
buffer_h MemoryPool::allocate_buffer_h(VkDeviceSize n_bytes, VkBufferUsageFlags usage_flags, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags)  {
    DENTER("Rendering::MemoryPool::allocate_buffer_h");

    // First, prepare the buffer info struct
    VkBufferCreateInfo buffer_info;
    populate_buffer_info(buffer_info, n_bytes, usage_flags, sharing_mode, create_flags);

    // Next, create the buffer struct
    VkResult vk_result;
    VkBuffer buffer;
    if ((vk_result = vkCreateBuffer(this->gpu, &buffer_info, nullptr, &buffer)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create buffer: " + vk_error_map[vk_result]);
    }

    // Get the memory requirements of the buffer
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(this->gpu, buffer, &mem_requirements);

    // Use the helper function to do the allocation
    buffer_h result = this->allocate_memory(MemoryBlockType::buffer, n_bytes, mem_requirements);
    BufferBlock* block = (BufferBlock*) this->vk_used_blocks.at(result);

    // With the block, bind the memory to the new buffer
    if ((vk_result = vkBindBufferMemory(this->gpu, buffer, this->vk_memory, block->start)) != VK_SUCCESS) {
        DLOG(fatal, "Could not bind buffer memory: " + vk_error_map[vk_result]);
    }

    // Next, populate the buffer parts of this block
    block->vk_buffer = buffer;
    block->vk_usage_flags = usage_flags;
    block->vk_create_flags = create_flags;
    block->vk_sharing_mode = sharing_mode;

    // Done, so return the handle
    DRETURN result;
}

/* Tries to get a new image from the pool of the given sizes and with the given flags, returning only its handle. Applies extra checks if NDEBUG is not defined. */
image_h MemoryPool::allocate_image_h(uint32_t width, uint32_t height, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode, VkImageCreateFlags create_flags) {
    DENTER("Rendering::MemoryPool::allocate_image_h");

    // First, define the width & height as a 3D extent
    VkExtent3D image_size = {};
    image_size.width = width;
    image_size.height = height;
    image_size.depth = 1;

    // Next, populate the create info for the image
    VkImageCreateInfo image_info;
    populate_image_info(image_info, image_size, image_format, image_layout, usage_flags, sharing_mode, create_flags);

    // Create the image
    VkResult vk_result;
    VkImage image;
    if ((vk_result = vkCreateImage(this->gpu, &image_info, nullptr, &image)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create image: " + vk_error_map[vk_result]);
    }

    // Get the memory requirements of the image
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(this->gpu, image, &mem_requirements);

    // Next, allocate memory for the image
    image_h result = this->allocate_memory(MemoryBlockType::image, 3 * width * height * sizeof(uint8_t), mem_requirements);
    ImageBlock* block = (ImageBlock*) this->vk_used_blocks.at(result);

    // Bind that memory to the image
    if ((vk_result = vkBindImageMemory(this->gpu, image, this->vk_memory, block->start)) != VK_SUCCESS) {
        DLOG(fatal, "Could not bind image memory: " + vk_error_map[vk_result]);
    }

    // Populate the rest of the block before terminating
    block->vk_image = image;
    block->vk_extent = image_size;
    block->vk_format = image_format;
    block->vk_layout = image_layout;
    block->vk_usage_flags = usage_flags;
    block->vk_create_flags = create_flags;
    block->vk_sharing_mode = sharing_mode;

    // When done, return the handle
    DRETURN result;
}

/* Deallocates the buffer or image with the given handle. Does not throw an error if the handle doesn't exist, unless NDEBUG is not defined. */
void MemoryPool::deallocate(memory_h handle) {
    DENTER("Rendering::MemoryPool::deallocate");

    // First, try to fetch the given buffer
    std::unordered_map<memory_h, UsedBlock*>::iterator iter = this->vk_used_blocks.find(handle);
    if (iter == this->vk_used_blocks.end()) {
        DLOG(fatal, "Object with handle '" + std::to_string(handle) + "' does not exist.");
    }

    // Keep the block reference...
    UsedBlock* block = (*iter).second;
    // ...so we can safely delete it from the list
    if (block->type == MemoryBlockType::buffer) {
        vkDestroyBuffer(this->gpu, ((BufferBlock*) block)->vk_buffer, nullptr);
    } else if (block->type == MemoryBlockType::image) {
        // Always destroy the image itself
        vkDestroyImage(this->gpu, ((ImageBlock*) block)->vk_image, nullptr);
    }
    this->vk_used_blocks.erase(iter);

    // Next, release the block memory in our free list
    this->free_list.release(block->start);

    // // Get the start & offset of the buffer
    // VkDeviceSize buffer_start = block->start;
    // VkDeviceSize buffer_length = block->req_length;

    // // Generate a new free block for the memory released by this buffer. We insert it sorted.
    // bool inserted = false;
    // for (uint32_t i = 0; i < this->vk_free_blocks.size(); i++) {
    //     // Get the start & offset for this free block
    //     VkDeviceSize free_start = this->vk_free_blocks[i].start;
    //     VkDeviceSize free_length = this->vk_free_blocks[i].length;

    //     #ifndef NDEBUG
    //     // Sanity check
    //     if (free_start == buffer_start) {
    //         DLOG(fatal, "Free block " + std::to_string(i) + " has same offset as previously allocated buffer");
    //     }
    //     #endif

    //     // Check if we should insert it here
    //     if (free_start > buffer_start) {
    //         // It should become the new i'th block

    //         #ifndef NDEBUG

    //         // Couple of more sanity checks
    //         if (i > 0) {
    //             // Sanity check the previous buffer as well
    //             VkDeviceSize free_m1_start = this->vk_free_blocks[i - 1].start;
    //             VkDeviceSize free_m1_length = this->vk_free_blocks[i - 1].length;

    //             if (i > 0 && free_m1_start + free_m1_length > buffer_start) {
    //                 DLOG(fatal, "Free block " + std::to_string(i - 1) + " overlaps with previously allocated buffer (previous neighbour)");
    //             } else if (i > 0 && free_m1_start + free_m1_length > free_start) {
    //                 DLOG(fatal, "Free blocks " + std::to_string(i - 1) + " and " + std::to_string(i) + " overlap");
    //             }
    //         }
    //         // Sanity check the current buffer
    //         if (buffer_start + buffer_length > free_start) {
    //             DLOG(fatal, "Free block " + std::to_string(i) + " overlaps with previously allocated buffer (next neighbour)");
    //         } 
            
    //         #endif

    //         // However, first check if it makes more sense to merge with its neighbours first
    //         if (i > 0) {
    //             VkDeviceSize free_m1_start = this->vk_free_blocks[i - 1].start;
    //             VkDeviceSize free_m1_length = this->vk_free_blocks[i - 1].length;

    //             if (free_m1_start + free_m1_length == buffer_start && buffer_start + buffer_length == free_start) {
    //                 // The buffer happens to precisely fill the gap between two blocks; merge all memory into one free block
    //                 this->vk_free_blocks[i - 1].length += buffer_length + free_length;
    //                 this->vk_free_blocks.erase(i);
    //                 inserted = true;
    //             } else if (free_m1_start + free_m1_length == buffer_start) {
    //                 // The buffer is only mergeable with the previous block
    //                 this->vk_free_blocks[i - 1].length += buffer_length;
    //                 inserted = true;
    //             }
    //         }

    //         // Examine the next block in its onesy 
    //         if (buffer_start + buffer_length == free_start) {
    //             // The buffer is only mergeable with the next block
    //             this->vk_free_blocks[i].start -= buffer_length;
    //             this->vk_free_blocks[i].length += buffer_length;
    //             inserted = true;
    //         }
            
    //         // If not yet inserted, then we want to add a new free block
    //         if (!inserted) {
    //             // Not mergeable; insert a new block by moving all blocks to the right
    //             this->vk_free_blocks.resize(this->vk_free_blocks.size() + 1);
    //             for (uint32_t j = i; j < this->vk_free_blocks.size() - 1; j++) {
    //                 this->vk_free_blocks[j + 1] = this->vk_free_blocks[j];
    //             }

    //             // With space created, insert it
    //             this->vk_free_blocks[i] = MemoryPool::FreeBlock({ buffer_start, buffer_length });
    //             inserted = true;
    //         }

    //         // We always want to stop, though
    //         break;
    //     }
    // }

    // // If we did not insert, then append it as a free block at the end
    // if (!inserted) {
    //     // If there is a previous block and its mergeable, do that
    //     if (this->vk_free_blocks.size() > 0) {
    //         VkDeviceSize free_start = this->vk_free_blocks[this->vk_free_blocks.size() - 1].start;
    //         VkDeviceSize free_length = this->vk_free_blocks[this->vk_free_blocks.size() - 1].length;

    //         if (free_start + free_length == buffer_start) {
    //             // It matches; merge the old block
    //             this->vk_free_blocks[this->vk_free_blocks.size() - 1].length += buffer_length;
    //             inserted = true;
    //         }
    //     }

    //     // If we didn't merge it, append it as a new block
    //     if (!inserted) {
    //         this->vk_free_blocks.push_back(MemoryPool::FreeBlock({ buffer_start, buffer_length }));
    //         inserted = true;
    //     }
    // }

    // Deallocate the block itself
    delete block;

    // Done
    DRETURN;
}



/* Defragements the entire pool, aligning all buffers next to each other in memory to create a maximally sized free block. Note that existing handles will remain valid. */
void MemoryPool::defrag() {
    DENTER("Rendering::MemoryPool::defrag");

    // First, reset the free list
    this->free_list.clear();

    // We loop through all internal blocks
    VkDeviceSize offset = 0;
    VkResult vk_result;
    VkMemoryRequirements mem_requirements;
    for (const std::pair<memory_h, UsedBlock*>& p : this->vk_used_blocks) {
        // Get a reference to the block
        UsedBlock* block = this->vk_used_blocks.at(p.first);

        // Switch based on the type of block what to do next
        if (block->type == MemoryBlockType::buffer) {
            // It's a buffer
            BufferBlock* bblock = (BufferBlock*) block;

            // Destroy the old one
            vkDestroyBuffer(this->gpu, bblock->vk_buffer, nullptr);

            // Prepare a new struct for reallocation
            VkBufferCreateInfo buffer_info;
            populate_buffer_info(buffer_info, bblock->length, bblock->vk_usage_flags, bblock->vk_sharing_mode, bblock->vk_create_flags);

            // Create a new vk_buffer
            if ((vk_result = vkCreateBuffer(this->gpu, &buffer_info, nullptr, &bblock->vk_buffer)) != VK_SUCCESS) {
                DLOG(fatal, "Could not re-create VkBuffer object: " + vk_error_map[vk_result]);
            }

            // Get the memory requirements for this new buffer
            vkGetBufferMemoryRequirements(this->gpu, bblock->vk_buffer, &mem_requirements);

            // Try to reserve space for it in the freelist
            offset = this->free_list.reserve(mem_requirements.size, mem_requirements.alignment);
            if (offset == std::numeric_limits<freelist_size_t>::max()) { DLOG(fatal, "Could not defrag buffer: memory requirements changed (need " + Tools::bytes_to_string(mem_requirements.size) + ", but " + Tools::bytes_to_string(this->free_list.capacity() - this->free_list.size()) + " free)"); }

            // Bind new memory for, at the start of this index.
            if ((vk_result = vkBindBufferMemory(this->gpu, bblock->vk_buffer, this->vk_memory, offset)) != VK_SUCCESS) {
                DLOG(fatal, "Could not re-bind memory to buffer: " + vk_error_map[vk_result]);
            }
            
        } else if (block->type == MemoryBlockType::image) {
            // It's an image
            ImageBlock* iblock = (ImageBlock*) block;

            // Destroy the old one
            vkDestroyImage(this->gpu, iblock->vk_image, nullptr);

            // Prepare a new struct for reallocation
            VkImageCreateInfo image_info;
            populate_image_info(image_info, iblock->vk_extent, iblock->vk_format, iblock->vk_layout, iblock->vk_usage_flags, iblock->vk_sharing_mode, iblock->vk_create_flags);

            // Create a new VkImage
            if ((vk_result = vkCreateImage(this->gpu, &image_info, nullptr, &iblock->vk_image)) != VK_SUCCESS) {
                DLOG(fatal, "Could not re-create VkImage object: " + vk_error_map[vk_result]);
            }

            // Get the memory requirements for this new image
            vkGetImageMemoryRequirements(this->gpu, iblock->vk_image, &mem_requirements);

            // Try to reserve space for it in the freelist
            offset = this->free_list.reserve(mem_requirements.size, mem_requirements.alignment);
            if (offset == std::numeric_limits<freelist_size_t>::max()) { DLOG(fatal, "Could not defrag image: memory requirements changed (need " + Tools::bytes_to_string(mem_requirements.size) + ", but " + Tools::bytes_to_string(this->free_list.capacity() - this->free_list.size()) + " free)"); }

            // Bind new memory for, at the start of this index.
            if ((vk_result = vkBindImageMemory(this->gpu, iblock->vk_image, this->vk_memory, offset)) != VK_SUCCESS) {
                DLOG(fatal, "Could not re-bind memory to image: " + vk_error_map[vk_result]);
            }

        }

        // Finally, update the offset & possible size in the block
        block->start = offset;
        block->req_length = mem_requirements.size;

        // Increment the offset for the next buffer
        offset += mem_requirements.size;
    }

    // Done
    DRETURN;
}



/* Swap operator for the MemoryPool class. */
void Rendering::swap(MemoryPool& mp1, MemoryPool& mp2) {
    using std::swap;

    DENTER("Rendering::swap(MemoryPool)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (mp1.gpu != mp2.gpu) {
        DLOG(fatal, "Cannot swap memory pools with different GPUs");
    }
    #endif

    // Swap EVERYTHING but the GPU
    swap(mp1.vk_memory, mp2.vk_memory);
    swap(mp1.vk_memory_type, mp2.vk_memory_type);
    swap(mp1.vk_memory_size, mp2.vk_memory_size);
    swap(mp1.vk_memory_properties, mp2.vk_memory_properties),
    swap(mp1.vk_used_blocks, mp2.vk_used_blocks);
    swap(mp1.free_list, mp2.free_list);

    DRETURN;
}



/* Static function that helps users decide the best memory queue for buffers. */
uint32_t MemoryPool::select_memory_type(const GPU& gpu, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags) {
    DENTER("Rendering::MemoryPool::select_memory_type(buffer)");

    // Get the available memory in the internal device
    VkPhysicalDeviceMemoryProperties gpu_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_properties);

    // Next, temporarily create a buffer to discover its type
    VkBufferCreateInfo buffer_info;
    populate_buffer_info(buffer_info, 16, usage_flags, sharing_mode, create_flags);

    // Create the buffer
    VkResult vk_result;
    VkBuffer dummy;
    if ((vk_result = vkCreateBuffer(gpu, &buffer_info, nullptr, &dummy)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate temporary dummy buffer: " + vk_error_map[vk_result]);
    }

    // Ge the memory requirements of the buffer
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(gpu, dummy, &mem_requirements);

    // We can already destroy the buffer
    vkDestroyBuffer(gpu, dummy, nullptr);

    // Try to find suitable memory (i.e., check if the device has enough memory bits(?) and if the required properties match)
    for (uint32_t i = 0; i < gpu_properties.memoryTypeCount; i++) {
        if (mem_requirements.memoryTypeBits & (1 << i) && (gpu_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties) {
            DRETURN i;
        }
    }

    // Didn't find any
    DLOG(fatal, "No suitable memory on device for given buffer configuration.");
    DRETURN 0;
}

/* Static function that helps users decide the best memory queue for images. */
uint32_t MemoryPool::select_memory_type(const GPU& gpu, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_properties, VkSharingMode sharing_mode, VkImageCreateFlags create_flags) {
    DENTER("Rendering::MemoryPool::select_memory_type(image)");

    // Get the available memory in the internal device
    VkPhysicalDeviceMemoryProperties gpu_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_properties);

    // Next, temporarily create a buffer to discover its type
    VkImageCreateInfo image_info;
    populate_image_info(image_info, VkExtent3D({ 4, 4, 1 }), format, layout, usage_flags, sharing_mode, create_flags);

    // Create the buffer
    VkResult vk_result;
    VkImage dummy;
    if ((vk_result = vkCreateImage(gpu, &image_info, nullptr, &dummy)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate temporary dummy image: " + vk_error_map[vk_result]);
    }

    // Ge the memory requirements of the buffer
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(gpu, dummy, &mem_requirements);

    // We can already destroy the buffer
    vkDestroyImage(gpu, dummy, nullptr);

    // Try to find suitable memory (i.e., check if the device has enough memory bits(?) and if the required properties match)
    for (uint32_t i = 0; i < gpu_properties.memoryTypeCount; i++) {
        if (mem_requirements.memoryTypeBits & (1 << i) && (gpu_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties) {
            DRETURN i;
        }
    }

    // Didn't find any
    DLOG(fatal, "No suitable memory on device for given image configuration.");
    DRETURN 0;
}