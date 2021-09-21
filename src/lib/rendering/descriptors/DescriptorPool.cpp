/* DESCRIPTOR POOL.cpp
 *   by Lut99
 *
 * Created:
 *   26/04/2021, 14:38:48
 * Last edited:
 *   25/05/2021, 18:14:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorPool class, which serves as a memory pool for
 *   descriptors, which in turn describe how a certain buffer or other
 *   piece of memory should be accessed on the GPU.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "DescriptorPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VkDescriptorPoolSize struct. */
static void populate_descriptor_pool_size(VkDescriptorPoolSize& descriptor_pool_size, const std::tuple<VkDescriptorType, uint32_t>& descriptor_type) {
    // Initialize to default
    descriptor_pool_size = {};
    descriptor_pool_size.type = std::get<0>(descriptor_type);
    descriptor_pool_size.descriptorCount = std::get<1>(descriptor_type);
}

/* Populates a given VkDescriptorPoolCreateInfo struct. */
static void populate_descriptor_pool_info(VkDescriptorPoolCreateInfo& descriptor_pool_info, const Tools::Array<VkDescriptorPoolSize>& descriptor_pool_sizes, uint32_t n_sets, VkDescriptorPoolCreateFlags descriptor_pool_flags) {
    // Initialize to default
    descriptor_pool_info = {};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    // Set the pool size to use
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(descriptor_pool_sizes.size());
    descriptor_pool_info.pPoolSizes = descriptor_pool_sizes.rdata();

    // Set the maximum number of sets allowed
    descriptor_pool_info.maxSets = n_sets;

    // Set the flags to use
    descriptor_pool_info.flags = descriptor_pool_flags | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
}

/* Populates a given VkDescriptorSetAllocateInfo struct. */
static void populate_descriptor_set_info(VkDescriptorSetAllocateInfo& descriptor_set_info, VkDescriptorPool vk_descriptor_pool, const Tools::Array<VkDescriptorSetLayout>& vk_descriptor_set_layouts, uint32_t n_sets) {
    // Set to default
    descriptor_set_info = {};
    descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    // Set the pool that we use to allocate them
    descriptor_set_info.descriptorPool = vk_descriptor_pool;

    // Set the number of sets to allocate
    descriptor_set_info.descriptorSetCount = n_sets;

    // Set the layout for this descriptor
    descriptor_set_info.pSetLayouts = vk_descriptor_set_layouts.rdata();
}





/***** DESCRIPTORPOOL CLASS *****/
/* Constructor for the DescriptorPool class, which takes the GPU to create the pool on, the number of descriptors we want to allocate in the pool, the maximum number of descriptor sets that can be allocated and optionally custom create flags. */
DescriptorPool::DescriptorPool(const Rendering::GPU& gpu, const std::pair<VkDescriptorType, uint32_t>& descriptor_type, uint32_t max_sets, VkDescriptorPoolCreateFlags flags):
    gpu(gpu),
    vk_descriptor_types(Tools::Array<std::pair<VkDescriptorType, uint32_t>>({ descriptor_type })),
    vk_max_sets(max_sets),
    vk_create_flags(flags)
{
    logger.logc(Verbosity::important, DescriptorPool::channel, "Initializing with a single type...");



    // First, we define how large the pool will be
    logger.logc(Verbosity::details, DescriptorPool::channel, "Preparing structs...");
    VkDescriptorPoolSize descriptor_pool_size;
    populate_descriptor_pool_size(descriptor_pool_size, this->vk_descriptor_types[0]);

    // Prepare the create info
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    populate_descriptor_pool_info(descriptor_pool_info, Tools::Array<VkDescriptorPoolSize>({ descriptor_pool_size }), this->vk_max_sets, this->vk_create_flags);



    // Actually allocate the pool
    logger.logc(Verbosity::details, DescriptorPool::channel, "Allocating pool..");
    VkResult vk_result;
    if ((vk_result = vkCreateDescriptorPool(this->gpu, &descriptor_pool_info, nullptr, &this->vk_descriptor_pool)) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Could not allocate descriptor pool: ", vk_error_map[vk_result]);
    }



    // Before we leave, we can optimise by setting the map to the maximum memory size
    this->descriptor_sets.reserve(this->vk_max_sets);



    // D0ne
    logger.logc(Verbosity::important, DescriptorPool::channel, "Init success.");
}

/* Constructor for the DescriptorPool class, which takes the GPU to create the pool on, a list of descriptor types and their counts, the maximum number of descriptor sets that can be allocated and optionally custom create flags. */
DescriptorPool::DescriptorPool(const Rendering::GPU& gpu, const Tools::Array<std::pair<VkDescriptorType, uint32_t>>& descriptor_types, uint32_t max_sets, VkDescriptorPoolCreateFlags flags) :
    gpu(gpu),
    vk_descriptor_types(descriptor_types),
    vk_max_sets(max_sets),
    vk_create_flags(flags)
{
    logger.logc(Verbosity::important, DescriptorPool::channel, "Initializing with multiple types...");



    // First, we define how large the pool will be
    logger.logc(Verbosity::details, DescriptorPool::channel, "Preparing structs...");
    Tools::Array<VkDescriptorPoolSize> descriptor_pool_sizes;
    descriptor_pool_sizes.resize(this->vk_descriptor_types.size());
    for (uint32_t i = 0; i < this->vk_descriptor_types.size(); i++) {
        populate_descriptor_pool_size(descriptor_pool_sizes[i], this->vk_descriptor_types[i]);
    }

    // Prepare the create info
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    populate_descriptor_pool_info(descriptor_pool_info, descriptor_pool_sizes, this->vk_max_sets, this->vk_create_flags);



    // Actually allocate the pool
    logger.logc(Verbosity::details, DescriptorPool::channel, "Allocating pool..");
    VkResult vk_result;
    if ((vk_result = vkCreateDescriptorPool(this->gpu, &descriptor_pool_info, nullptr, &this->vk_descriptor_pool)) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Could not allocate descriptor pool: ", vk_error_map[vk_result]);
    }



    // Before we leave, we can optimise by setting the map to the maximum memory size
    this->descriptor_sets.reserve(this->vk_max_sets);



    // D0ne
    logger.logc(Verbosity::important, DescriptorPool::channel, "Init success.");
}

/* Copy constructor for the DescriptorPool. */
DescriptorPool::DescriptorPool(const DescriptorPool& other) :
    gpu(other.gpu),
    vk_descriptor_types(other.vk_descriptor_types),
    vk_max_sets(other.vk_max_sets),
    vk_create_flags(other.vk_create_flags)
{
    logger.logc(Verbosity::debug, DescriptorPool::channel, "Copying...");

    // First, we define how large the pool will be
    Tools::Array<VkDescriptorPoolSize> descriptor_pool_sizes;
    descriptor_pool_sizes.resize(this->vk_descriptor_types.size());
    for (uint32_t i = 0; i < this->vk_descriptor_types.size(); i++) {
        populate_descriptor_pool_size(descriptor_pool_sizes[i], this->vk_descriptor_types[0]);
    }

    // Prepare the create info
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    populate_descriptor_pool_info(descriptor_pool_info, descriptor_pool_sizes, this->vk_max_sets, this->vk_create_flags);

    // Actually allocate the pool
    VkResult vk_result;
    if ((vk_result = vkCreateDescriptorPool(this->gpu, &descriptor_pool_info, nullptr, &this->vk_descriptor_pool)) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Could not allocate descriptor pool: ", vk_error_map[vk_result]);
    }

    // Do not copy individual descriptors



    // Before we leave, we can optimise by setting the map to the maximum memory size
    this->descriptor_sets.reserve(this->vk_max_sets);

    // D0ne
    logger.logc(Verbosity::debug, DescriptorPool::channel, "Copy success.");
}
        
/* Move constructor for the DescriptorPool. */
DescriptorPool::DescriptorPool(DescriptorPool&& other):
    gpu(other.gpu),
    vk_descriptor_pool(other.vk_descriptor_pool),
    vk_descriptor_types(other.vk_descriptor_types),
    vk_max_sets(other.vk_max_sets),
    vk_create_flags(other.vk_create_flags),
    descriptor_sets(other.descriptor_sets)
{
    // Set the other's pool & sets to nullptr to avoid deallocation
    other.vk_descriptor_pool = nullptr;
    other.descriptor_sets.clear();
}

/* Destructor for the DescriptorPool. */
DescriptorPool::~DescriptorPool() {
    logger.logc(Verbosity::important, DescriptorPool::channel, "Cleaning...");

    VkResult vk_result;
    if (this->descriptor_sets.size() > 0) {
        logger.logc(Verbosity::details, DescriptorPool::channel, "Deallocating descriptor sets...");
        for (uint32_t i = 0; i < this->descriptor_sets.size(); i++) {
            if ((vk_result = vkFreeDescriptorSets(this->gpu, this->vk_descriptor_pool, 1, &this->descriptor_sets[i]->vulkan())) != VK_SUCCESS) {
                logger.errorc(DescriptorPool::channel, "Could not deallocate descriptor sets: ", vk_error_map[vk_result]);
            }
        }
    }

    if (this->vk_descriptor_pool != nullptr) {
        logger.logc(Verbosity::details, DescriptorPool::channel, "Deallocating pool...");
        vkDestroyDescriptorPool(this->gpu, this->vk_descriptor_pool, nullptr);
    }

    logger.logc(Verbosity::important, DescriptorPool::channel, "Cleaned.");
}



/* Allocates a single descriptor set with the given layout. Will fail with errors if there's no more space. */
DescriptorSet* DescriptorPool::allocate(const Rendering::DescriptorSetLayout& descriptor_set_layout) {
    // Check if we have enough space left
    if (static_cast<uint32_t>(this->descriptor_sets.size()) >= this->vk_max_sets) {
        logger.fatalc(DescriptorPool::channel, "Cannot allocate new DescriptorSet: already allocated maximum of ", this->vk_max_sets, " sets.");
    }

    // Put the layout in a struct s.t. we can pass it and keep it in memory until after the call
    Tools::Array<VkDescriptorSetLayout> vk_descriptor_set_layouts({ descriptor_set_layout.vulkan() });

    // Next, populate the create info
    VkDescriptorSetAllocateInfo descriptor_set_info;
    populate_descriptor_set_info(descriptor_set_info, this->vk_descriptor_pool, vk_descriptor_set_layouts, 1);

    // We can now call the allocate function
    VkResult vk_result;
    VkDescriptorSet set;
    if ((vk_result = vkAllocateDescriptorSets(this->gpu, &descriptor_set_info, &set)) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Failed to allocate new DescriptorSet: ", vk_error_map[vk_result]);
    }

    // Create the object and insert it
    DescriptorSet* descriptor_set = new DescriptorSet(this->gpu, set);
    this->descriptor_sets.push_back(descriptor_set);

    // With that done, return the handle
    return descriptor_set;
}

/* Allocates multiple descriptor sets with the given layout, returning them as an Array. Will fail with errors if there's no more space. */
Tools::Array<DescriptorSet*> DescriptorPool::nallocate(uint32_t n_sets, const Tools::Array<Rendering::DescriptorSetLayout>& descriptor_set_layouts) {
    #ifndef NDEBUG
    // If n_sets if null, nothing to do
    if (n_sets == 0) {
        logger.warningc(DescriptorPool::channel, "Request to allocate 0 sets received; nothing to do.");
        return {};
    }
    // If we aren't passed enough layouts, then tell us
    if (n_sets != descriptor_set_layouts.size()) {
        logger.fatalc(DescriptorPool::channel, "Not enough descriptor set layouts passed: got ", descriptor_set_layouts.size(), ", expected ", n_sets);
    }
    #endif
    // Check if we have enough space left
    if (static_cast<uint32_t>(this->descriptor_sets.size()) + n_sets > this->vk_max_sets) {
        logger.fatalc(DescriptorPool::channel, "Cannot allocate ", n_sets, " new DescriptorSets: only space for ", this->vk_max_sets - static_cast<uint32_t>(this->descriptor_sets.size()), " sets");
    }

    // Get the VkDescriptorSetLayout objects behind the layouts
    Tools::Array<VkDescriptorSetLayout> vk_descriptor_set_layouts(descriptor_set_layouts.size());
    for (uint32_t i = 0; i < descriptor_set_layouts.size(); i++) {
        vk_descriptor_set_layouts.push_back(descriptor_set_layouts[i]);
    }
    // Create a temporary list of result sets to which we can allocate
    Tools::Array<VkDescriptorSet> sets(n_sets);

    // Next, populate the create info
    VkDescriptorSetAllocateInfo descriptor_set_info;
    populate_descriptor_set_info(descriptor_set_info, this->vk_descriptor_pool, vk_descriptor_set_layouts, n_sets);
    // We can now call the allocate function
    VkResult vk_result;
    if ((vk_result = vkAllocateDescriptorSets(this->gpu, &descriptor_set_info, sets.wdata(n_sets))) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Failed to allocate ", n_sets, " new DescriptorSets: ", vk_error_map[vk_result]);
    }

    // Create the resulting DescriptorSet object for each pair, then return
    Tools::Array<DescriptorSet*> result(n_sets);
    for (uint32_t i = 0; i < n_sets; i++) {
        result.push_back(new DescriptorSet(this->gpu, sets[i]));
        this->descriptor_sets.push_back(result.last());
    }

    // We're done, so return the set of handles
    return result;
}

/* Allocates multiple descriptor sets with the given layout (repeating it), returning them as an Array. Will fail with errors if there's no more space. */
Tools::Array<DescriptorSet*> DescriptorPool::nallocate(uint32_t n_sets, const Rendering::DescriptorSetLayout& descriptor_set_layout) {
    #ifndef NDEBUG
    // If n_sets if null, nothing to do
    if (n_sets == 0) {
        logger.warningc(DescriptorPool::channel, "Request to allocate 0 sets received; nothing to do.");
        return {};
    }
    #endif
    // Check if we have enough space left
    if (static_cast<uint32_t>(this->descriptor_sets.size()) + n_sets > this->vk_max_sets) {
        logger.fatalc(DescriptorPool::channel, "Cannot allocate ", n_sets, " new DescriptorSets: only space for ", this->vk_max_sets - static_cast<uint32_t>(this->descriptor_sets.size()), " sets");
    }

    // Get the VkDescriptorSetLayout object behind the layout as an array
    Tools::Array<VkDescriptorSetLayout> vk_descriptor_set_layout(descriptor_set_layout.vulkan(), n_sets);
    // Create a temporary list of result sets to which we can allocate
    Tools::Array<VkDescriptorSet> sets(n_sets);

    // Next, populate the create info
    VkDescriptorSetAllocateInfo descriptor_set_info;
    populate_descriptor_set_info(descriptor_set_info, this->vk_descriptor_pool, vk_descriptor_set_layout, n_sets);
    // We can now call the allocate function
    VkResult vk_result;
    if ((vk_result = vkAllocateDescriptorSets(this->gpu, &descriptor_set_info, sets.wdata(n_sets))) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Failed to allocate ", n_sets, " new DescriptorSets: ", vk_error_map[vk_result]);
    }

    // Create the resulting DescriptorSet object for each pair, then return
    Tools::Array<DescriptorSet*> result(n_sets);
    for (uint32_t i = 0; i < n_sets; i++) {
        result.push_back(new DescriptorSet(this->gpu, sets[i]));
        this->descriptor_sets.push_back(result.last());
    }

    // We're done, so return the set of handles
    return result;
}



/* Deallocates the descriptor set with the given handle. */
void DescriptorPool::free(const DescriptorSet* set) {
    // Try to remove the pointer from the list
    bool found = false;
    for (uint32_t i = 0; i < this->descriptor_sets.size(); i++) {
        if (this->descriptor_sets[i] == set) {
            this->descriptor_sets.erase(i);
            found = true;
            break;
        }
    }
    if (!found) {
        logger.fatalc(DescriptorPool::channel, "Tried to free DescriptorSet that was not allocated with this pool.");
    }

    // Destroy the VkCommandBuffer
    vkFreeDescriptorSets(this->gpu, this->vk_descriptor_pool, 1, &set->vulkan());
    
    // Destroy the pointer itself
    delete set;
}

/* Deallocates an array of given descriptors sets. */
void DescriptorPool::nfree(const Tools::Array<DescriptorSet*>& sets) {
    // First, we check if all handles exist
    Tools::Array<VkDescriptorSet> to_remove(sets.size());
    for (uint32_t i = 0; i < sets.size(); i++) {
        bool found = false;
        for (uint32_t i = 0; i < this->descriptor_sets.size(); i++) {
            if (this->descriptor_sets[i] == sets[i]) {
                this->descriptor_sets.erase(i);
                found = true;
                break;
            }
        }
        if (!found) {
            logger.fatalc(DescriptorPool::channel, "Tried to free DescriptorSet that was not allocated with this pool.");
        }

        // Mark the Vk object for removal
        to_remove.push_back(sets[i]->vulkan());

        // Delete the pointer
        delete sets[i];
    }

    // All that's left is to actually remove the handles; do that
    VkResult vk_result;
    if ((vk_result = vkFreeDescriptorSets(this->gpu, this->vk_descriptor_pool, to_remove.size(), to_remove.rdata())) != VK_SUCCESS) {
        logger.fatalc(DescriptorPool::channel, "Could not deallocate descriptor sets: ", vk_error_map[vk_result]);
    }
}

/* Resets the pool in its entirety, quickly deallocating everything. */
void DescriptorPool::reset() {
    // Call the reset quickly (note the 0, the flags, is reserved for future use and thus fixed for us)
    vkResetDescriptorPool(this->gpu, this->vk_descriptor_pool, 0);

    // Delete all internal pointers
    for (uint32_t i = 0; i < this->descriptor_sets.size(); i++) {
        delete this->descriptor_sets[i];
    }
    this->descriptor_sets.clear();
}



/* Swap operator for the DescriptorPool class. */
void Rendering::swap(DescriptorPool& dp1, DescriptorPool& dp2) {
    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (dp1.gpu != dp2.gpu) { logger.fatalc(DescriptorPool::channel, "Cannot swap descriptor pools with different GPUs"); }
    #endif

    // Swap all fields
    using std::swap;

    swap(dp1.vk_descriptor_pool, dp2.vk_descriptor_pool);
    swap(dp1.vk_descriptor_types, dp2.vk_descriptor_types);
    swap(dp1.vk_max_sets, dp2.vk_max_sets);
    swap(dp1.vk_create_flags, dp2.vk_create_flags);
    swap(dp1.descriptor_sets, dp2.descriptor_sets);
}
