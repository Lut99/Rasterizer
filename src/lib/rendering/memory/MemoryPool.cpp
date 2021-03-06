/* MEMORY POOL.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 15:11:40
 * Last edited:
 *   9/20/2021, 8:18:11 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <sstream>

#include "tools/Logger.hpp"
#include "tools/Common.hpp"
#include "../auxillary/ErrorCodes.hpp"
#include "../auxillary/MemoryProperties.hpp"

#include "MemoryPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VKBufferCreateInfo struct. */
static void populate_buffer_info(VkBufferCreateInfo& buffer_info, VkDeviceSize n_bytes, VkBufferUsageFlags usage_flags, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags) {
    // Only set to default
    buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // Set the parameters passed
    buffer_info.size = n_bytes;
    buffer_info.usage = usage_flags;
    buffer_info.sharingMode = sharing_mode;
    buffer_info.flags = create_flags;
}

/* Populates a given VkImageCreateInfo struct. */
static void populate_image_info(VkImageCreateInfo& image_info, const VkExtent3D& image_size, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode, VkImageCreateFlags create_flags) {
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
}

/* Populates a given VkMemoryAllocateInfo struct. */
static void populate_allocate_info(VkMemoryAllocateInfo& allocate_info, uint32_t memory_type, VkDeviceSize n_bytes) {
    // Set to default & define the stryct type
    allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    // Set the allocation size of the memory
    allocate_info.allocationSize = n_bytes;
    // Set the type of the memory to allocate
    allocate_info.memoryTypeIndex = memory_type;
}





/***** HELPER FUNCTIONS *****/
/* Given some memory properties and optionally Buffer/Image usage flags, returns the index of the most suitable memory on the device. */
static uint32_t select_memory(const Rendering::GPU& gpu, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage, VkImageUsageFlags image_usage) {
    // Get the available memory in the internal device
    VkPhysicalDeviceMemoryProperties gpu_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_properties);

    // If we are given any buffer usage flags, create a temporary buffer to discover its memory properties
    VkMemoryRequirements* buffer_requirements = nullptr;
    if (buffer_usage != 0) {
        // Create the create info for the temporary buffer
        VkBufferCreateInfo buffer_info;
        populate_buffer_info(buffer_info, 64, buffer_usage, VK_SHARING_MODE_EXCLUSIVE, 0);

        // Create the buffer
        VkResult vk_result;
        VkBuffer dummy;
        if ((vk_result = vkCreateBuffer(gpu, &buffer_info, nullptr, &dummy)) != VK_SUCCESS) {
            logger.fatalc(MemoryPool::channel, "Could not allocate temporary dummy buffer: ", vk_error_map[vk_result]);
        }

        // Get the requirements for this buffer
        buffer_requirements = new VkMemoryRequirements;
        vkGetBufferMemoryRequirements(gpu, dummy, buffer_requirements);

        // Destroy the buffer again
        vkDestroyBuffer(gpu, dummy, nullptr);
    }

    // If we are given any image usage flags, create a temporary image as well to discover ??ts memory properties
    VkMemoryRequirements* image_requirements = nullptr;
    if (image_usage != 0) {
        // Create the create info for the temporary buffer
        VkExtent3D image_extent = { 16, 16, 1 };
        VkImageCreateInfo image_info;
        populate_image_info(image_info, image_extent, VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED, image_usage, VK_SHARING_MODE_EXCLUSIVE, 0);

        // Create the image
        VkResult vk_result;
        VkImage dummy;
        if ((vk_result = vkCreateImage(gpu, &image_info, nullptr, &dummy)) != VK_SUCCESS) {
            logger.fatalc(MemoryPool::channel, "Could not allocate temporary dummy image: ", vk_error_map[vk_result]);
        }

        // Get the requirements for this image
        image_requirements = new VkMemoryRequirements;
        vkGetImageMemoryRequirements(gpu, dummy, image_requirements);

        // Destroy the image again
        vkDestroyImage(gpu, dummy, nullptr);
    }

    // Try to find suitable memory (i.e., check if the device has enough memory bits(?) and if the required properties match)
    for (uint32_t i = 0; i < gpu_properties.memoryTypeCount; i++) {
        // Check if this memory matches with our requirements
        if ((gpu_properties.memoryTypes[i].propertyFlags & memory_properties) == memory_properties) {
            // Next, check if the buffer requirements are here
            if (buffer_requirements != nullptr && !(buffer_requirements->memoryTypeBits & (1 << i))) {
                continue;
            }

            // Also check the image requirements
            if (image_requirements != nullptr && !(image_requirements->memoryTypeBits & (1 << i))) {
                continue;
            }

            // Passed the checks, this memory is suitable
            return i;
        }
    }

    // Didn't find any
    logger.fatalc(MemoryPool::channel, "No suitable memory on device for given memory demands.");
    return std::numeric_limits<uint32_t>::max();
}





/***** MEMORYPOOL CLASS *****/
/* Constructor for the MemoryPool class, which takes the GPU where it lives, the size of its memory, the memory properties and optionally some buffer memory usage flags to take into account when selecting memory. */
MemoryPool::MemoryPool(const Rendering::GPU& gpu, VkDeviceSize pool_size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage, VkImageUsageFlags image_usage) :
    gpu(gpu),

    vk_properties(memory_properties)
{
    logger.logc(Verbosity::important, MemoryPool::channel, "Initializing...");

    #ifndef NDEBUG
    if (logger.get_verbosity() >= Verbosity::debug) {
        // Do some debug printing first
        std::stringstream sstr;
        sstr << "Memory property flags for this pool: ";
        bool first = true;
        for (size_t i = 0; i < 8 * sizeof(uint32_t); i++) {
            VkMemoryPropertyFlagBits bit = (VkMemoryPropertyFlagBits) (memory_properties & (0x1 << i));
            if (bit) {
                if (first) { first = false; }
                else { sstr << ", "; }
                sstr << vk_memory_property_map.at(bit);
            }
        }
        logger.logc(Verbosity::debug, MemoryPool::channel, sstr.str());
    }
    #endif

    // Try to find suitable memory
    this->memory_type = select_memory(gpu, memory_properties, buffer_usage, image_usage);

    // Prepare a create info for the device memory we'll allocate
    logger.logc(Verbosity::details, MemoryPool::channel, "Allocating ", Tools::bytes_to_string(pool_size),  " on device '", gpu.name(), "'...");
    VkMemoryAllocateInfo allocate_info;
    populate_allocate_info(allocate_info, memory_type, pool_size);

    // Do the allocation
    VkResult vk_result;
    if ((vk_result = vkAllocateMemory(this->gpu, &allocate_info, nullptr, &this->vk_memory)) != VK_SUCCESS) {
        logger.fatalc(MemoryPool::channel, "Could not allocate memory on device: ", vk_error_map[vk_result]);
    }

    // Cave Johnson, we're done here
    logger.logc(Verbosity::important, MemoryPool::channel, "Init success.");
}

/* Move constructor for the MemoryPool class. */
MemoryPool::MemoryPool(MemoryPool&& other) :
    gpu(other.gpu),

    memory_type(other.memory_type),
    vk_memory(other.vk_memory),

    vk_properties(other.vk_properties),

    objects(std::move(other.objects))
{
    other.vk_memory = nullptr;
    other.objects.clear();
}

/* Destructor for the MemoryPool class. */
MemoryPool::~MemoryPool() {
    logger.logc(Verbosity::important, MemoryPool::channel, "Cleaning...");

    // First, deallocate any buffers or images or whatnot there may be
    if (this->objects.size() > 0) {
        logger.logc(Verbosity::details, MemoryPool::channel, "Deallocating allocated objects...");
        for (const MemoryObject* object : this->objects) {
            if (object->type == MemoryObjectType::buffer) {
                vkDestroyBuffer(this->gpu, ((Buffer*) object)->vk_buffer, nullptr);
            } else {
                vkDestroyImage(this->gpu, ((Image*) object)->vk_image, nullptr);
            }
            delete object;
        }
    }

    // If the memory hasn't been stolen, deallocate it
    if (this->vk_memory != nullptr) {
        logger.logc(Verbosity::details, MemoryPool::channel, "Deallocating device memory...");
        vkFreeMemory(this->gpu, this->vk_memory, nullptr);
    }

    logger.logc(Verbosity::important, MemoryPool::channel, "Cleaned.");
}



/* Tries to allocate a new Buffer of the given size (in bytes) and with the given usage flags. Optionally, one can set the sharing mode and any create flags. */
Buffer* MemoryPool::allocate(VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkSharingMode sharing_mode, VkBufferCreateFlags create_flags) {
    // First, create the buffer object itself
    VkBufferCreateInfo buffer_info;
    populate_buffer_info(buffer_info, buffer_size, buffer_usage, sharing_mode, create_flags);

    VkResult vk_result;
    VkBuffer buffer;
    if ((vk_result = vkCreateBuffer(this->gpu, &buffer_info, nullptr, &buffer)) != VK_SUCCESS) {
        logger.fatalc(MemoryPool::channel, "Could not create new buffer: ", vk_error_map[vk_result]);
    }

    // Fetch more detailed memory requirements for this buffer
    VkMemoryRequirements buffer_requirements;
    vkGetBufferMemoryRequirements(this->gpu, buffer, &buffer_requirements);

    // With the buffer in place, allocate memory and bind it
    VkDeviceSize offset = this->_allocate(buffer_requirements);
    vkBindBufferMemory(this->gpu, buffer, this->vk_memory, offset);

    // Create the new Buffer object and insert it in our own list
    Buffer* to_return = new Buffer(*this, buffer, offset, buffer_size, buffer_requirements, buffer_usage, sharing_mode, create_flags);
    this->objects.insert((MemoryObject*) to_return);

    // Done
    return to_return;
}

/* Tries to allocate a new Buffer that is a copy of the given Buffer. */
Buffer* MemoryPool::allocate(const Buffer* other) {
    // Create the buffer object with the properties of the old one
    VkBufferCreateInfo buffer_info;
    populate_buffer_info(buffer_info, other->buffer_size, other->init_data.buffer_usage, other->init_data.sharing_mode, other->init_data.create_flags);

    VkResult vk_result;
    VkBuffer buffer;
    if ((vk_result = vkCreateBuffer(this->gpu, &buffer_info, nullptr, &buffer)) != VK_SUCCESS) {
        logger.fatalc(MemoryPool::channel, "Could not create buffer copy: ", vk_error_map[vk_result]);
    }

    // Fetch more detailed memory requirements for this buffer
    VkMemoryRequirements buffer_requirements;
    vkGetBufferMemoryRequirements(this->gpu, buffer, &buffer_requirements);

    // With the buffer in place, allocate memory and bind it
    VkDeviceSize offset = this->_allocate(buffer_requirements);
    vkBindBufferMemory(this->gpu, buffer, this->vk_memory, offset);

    // Create the new Buffer object and insert it in our own list
    Buffer* to_return = new Buffer(*this, buffer, offset, other->buffer_size, buffer_requirements, other->init_data.buffer_usage, other->init_data.sharing_mode, other->init_data.create_flags);
    this->objects.insert((MemoryObject*) to_return);

    // Done!
    return to_return;
}



/* Tries to allocate a new Image of the given size (in pixels), the given format, the given layout and with the given usage flags. Optionally, one can set the sharing mode and any create flags. */
Image* MemoryPool::allocate(const VkExtent2D& image_extent, VkFormat image_format, VkImageLayout image_layout, VkImageUsageFlags usage_flags, VkSharingMode sharing_mode, VkImageCreateFlags create_flags) {
    // First, create the buffer object itself
    VkExtent3D vk_extent3D = { image_extent.width, image_extent.height, 1 };
    VkImageCreateInfo image_info;
    populate_image_info(image_info, vk_extent3D, image_format, image_layout, usage_flags, sharing_mode, create_flags);

    VkResult vk_result;
    VkImage image;
    if ((vk_result = vkCreateImage(this->gpu, &image_info, nullptr, &image)) != VK_SUCCESS) {
        logger.fatalc(MemoryPool::channel, "Could not create new image: " + vk_error_map[vk_result]);
    }

    // Fetch more detailed memory requirements for this buffer
    VkMemoryRequirements image_requirements;
    vkGetImageMemoryRequirements(this->gpu, image, &image_requirements);

    // With the buffer in place, allocate memory and bind it
    VkDeviceSize offset = this->_allocate(image_requirements);
    vkBindImageMemory(this->gpu, image, this->vk_memory, offset);

    // Create the new Buffer object and insert it in our own list
    Image* to_return = new Image(*this, image, offset, image_extent, image_format, image_layout, image_requirements, usage_flags, sharing_mode, create_flags);
    this->objects.insert((MemoryObject*) to_return);

    // Done
    return to_return;
}

/* Tries to allocate a new Image that is a copy of the given Image. */
Image* MemoryPool::allocate(const Image* other) {
    // First, create the buffer object itself
    VkExtent3D vk_extent3D = { other->vk_extent.width, other->vk_extent.height, 1 };
    VkImageCreateInfo image_info;
    populate_image_info(image_info, vk_extent3D, other->vk_format, other->vk_layout, other->init_data.image_usage, other->init_data.sharing_mode, other->init_data.create_flags);

    VkResult vk_result;
    VkImage image;
    if ((vk_result = vkCreateImage(this->gpu, &image_info, nullptr, &image)) != VK_SUCCESS) {
        logger.fatalc(MemoryPool::channel, "Could not create image copy: " + vk_error_map[vk_result]);
    }

    // Fetch more detailed memory requirements for this buffer
    VkMemoryRequirements image_requirements;
    vkGetImageMemoryRequirements(this->gpu, image, &image_requirements);

    // With the buffer in place, allocate memory and bind it
    VkDeviceSize offset = this->_allocate(image_requirements);
    vkBindImageMemory(this->gpu, image, this->vk_memory, offset);

    // Create the new Buffer object and insert it in our own list
    Image* to_return = new Image(*this, image, offset, other->vk_extent, other->vk_format, other->vk_layout, image_requirements, other->init_data.image_usage, other->init_data.sharing_mode, other->init_data.create_flags);
    this->objects.insert((MemoryObject*) to_return);

    // Done!
    return to_return;
}



/* Deallocates the given MemoryObject. */
void MemoryPool::free(const MemoryObject* object) {
    // Try to remove the pointer from the list
    std::unordered_set<MemoryObject*>::iterator iter = this->objects.find(const_cast<MemoryObject*>(object));
    if (iter == this->objects.end()) {
        if (object->type == MemoryObjectType::buffer) {
            logger.fatalc(MemoryPool::channel, "Tried to free Buffer that was not allocated with this pool.");
        } else if (object->type == MemoryObjectType::image) {
            logger.fatalc(MemoryPool::channel, "Tried to free Image that was not allocated with this pool.");
        } else {
            logger.fatalc(MemoryPool::channel, "Tried to free object that was not allocated with this pool.");
        }
    }

    // Free the memory in the freelist
    this->_free((*iter)->object_offset);

    // Destroy either the buffer or the image
    if ((*iter)->type == MemoryObjectType::buffer) {
        vkDestroyBuffer(this->gpu, ((Buffer*) (*iter))->vk_buffer, nullptr);
    } else {
        vkDestroyImage(this->gpu, ((Image*) (*iter))->vk_image, nullptr);
    }
    
    // Destroy the pointer itself, then remove it from the internal list
    delete (*iter);
    this->objects.erase(iter);
}

/* Resets the memory pool, freeing all allocated buffers and junk. */
void MemoryPool::reset() {
    // Go through the list to deallocate it all
    for (const MemoryObject* object : this->objects) {
        if (object->type == MemoryObjectType::buffer) {
            vkDestroyBuffer(this->gpu, ((Buffer*) object)->vk_buffer, nullptr);
        } else {
            vkDestroyImage(this->gpu, ((Image*) object)->vk_image, nullptr);
        }
        delete object;
    }

    // Clear the list and the freelist
    this->objects.clear();
    this->_reset();
}



/* Swap operator for the MemoryPool class. */
void Rendering::swap(MemoryPool& mp1, MemoryPool& mp2) {
    #ifndef NDEBUG
    if (mp1.gpu != mp2.gpu) { logger.fatalc(MemoryPool::channel, "Cannot swap memory pools with different GPUs"); }
    #endif

    // Simply swap it all
    using std::swap;
    
    swap(mp1.memory_type, mp2.memory_type);
    swap(mp1.vk_memory, mp2.vk_memory);
    
    swap(mp1.vk_properties, mp2.vk_properties);

    swap(mp1.objects, mp2.objects);
}
