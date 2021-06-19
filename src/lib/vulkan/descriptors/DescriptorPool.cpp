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

#include "tools/CppDebugger.hpp"

#include "vulkan/ErrorCodes.hpp"

#include "DescriptorPool.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Given a map with handles as keys, finds the first free handle that is not the null handle. */
static descriptor_set_h find_handle(const std::unordered_map<descriptor_set_h, VkDescriptorSet>& vk_descriptor_sets, descriptor_set_h null_handle) {
    DENTER("find_handle");

    // Loop through the map and find the first free handle
    command_buffer_h result = 0;
    bool unique = false;
    while (!unique) {
        unique = true;
        for (const std::pair<descriptor_set_h, VkDescriptorSet>& p : vk_descriptor_sets) {
            if (result == null_handle || result == p.first) {
                // If result is the maximum value, then throw an error
                if (result == std::numeric_limits<descriptor_set_h>::max()) {
                    DLOG(fatal, "descriptor_set_h overflow; cannot allocate more sets.");
                }

                // Otherwise, increment and re-try
                ++result;
                unique = false;
                break;
            }
        }
    }

    // Return the result we found
    DRETURN result;
}





/***** POPULATE FUNCTIONS *****/
/* Populates a given VkDescriptorPoolSize struct. */
static void populate_descriptor_pool_size(VkDescriptorPoolSize& descriptor_pool_size, const std::tuple<VkDescriptorType, uint32_t>& descriptor_type) {
    DENTER("populate_descriptor_pool_size");

    // Initialize to default
    descriptor_pool_size = {};
    descriptor_pool_size.type = std::get<0>(descriptor_type);
    descriptor_pool_size.descriptorCount = std::get<1>(descriptor_type);

    // Done;
    DRETURN;
}

/* Populates a given VkDescriptorPoolCreateInfo struct. */
static void populate_descriptor_pool_info(VkDescriptorPoolCreateInfo& descriptor_pool_info, const Tools::Array<VkDescriptorPoolSize>& descriptor_pool_sizes, uint32_t n_sets, VkDescriptorPoolCreateFlags descriptor_pool_flags) {
    DENTER("populate_descriptor_pool_info");

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

    // Done;
    DRETURN;
}

/* Populates a given VkDescriptorSetAllocateInfo struct. */
static void populate_descriptor_set_info(VkDescriptorSetAllocateInfo& descriptor_set_info, VkDescriptorPool vk_descriptor_pool, const Tools::Array<VkDescriptorSetLayout>& vk_descriptor_set_layouts, uint32_t n_sets) {
    DENTER("populate_descriptor_set_info");

    // Set to default
    descriptor_set_info = {};
    descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    // Set the pool that we use to allocate them
    descriptor_set_info.descriptorPool = vk_descriptor_pool;

    // Set the number of sets to allocate
    descriptor_set_info.descriptorSetCount = n_sets;

    // Set the layout for this descriptor
    descriptor_set_info.pSetLayouts = vk_descriptor_set_layouts.rdata();

    // Done
    DRETURN;
}





/***** DESCRIPTORPOOL CLASS *****/
/* Constructor for the DescriptorPool class, which takes the GPU to create the pool on, the number of descriptors we want to allocate in the pool, the maximum number of descriptor sets that can be allocated and optionally custom create flags. */
DescriptorPool::DescriptorPool(const Vulkan::GPU& gpu, const std::pair<VkDescriptorType, uint32_t>& descriptor_type, uint32_t max_sets, VkDescriptorPoolCreateFlags flags):
    gpu(gpu),
    vk_descriptor_types(Tools::Array<std::pair<VkDescriptorType, uint32_t>>({ descriptor_type })),
    vk_max_sets(max_sets),
    vk_create_flags(flags)
{
    DENTER("Vulkan::DescriptorPool::DescriptorPool");
    DLOG(info, "Initializing DescriptorPool...");
    DINDENT;



    // First, we define how large the pool will be
    DLOG(info, "Preparing structs...");
    VkDescriptorPoolSize descriptor_pool_size;
    populate_descriptor_pool_size(descriptor_pool_size, this->vk_descriptor_types[0]);

    // Prepare the create info
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    populate_descriptor_pool_info(descriptor_pool_info, Tools::Array<VkDescriptorPoolSize>({ descriptor_pool_size }), this->vk_max_sets, this->vk_create_flags);



    // Actually allocate the pool
    DLOG(info, "Allocating pool...");
    VkResult vk_result;
    if ((vk_result = vkCreateDescriptorPool(this->gpu, &descriptor_pool_info, nullptr, &this->vk_descriptor_pool)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate descriptor pool: " + vk_error_map[vk_result]);
    }



    // Before we leave, we can optimise by setting the map to the maximum memory size
    this->vk_descriptor_sets.reserve(this->vk_max_sets);



    DDEDENT;
    DLEAVE;
}

/* Constructor for the DescriptorPool class, which takes the GPU to create the pool on, a list of descriptor types and their counts, the maximum number of descriptor sets that can be allocated and optionally custom create flags. */
DescriptorPool::DescriptorPool(const GPU& gpu, const Tools::Array<std::pair<VkDescriptorType, uint32_t>>& descriptor_types, uint32_t max_sets, VkDescriptorPoolCreateFlags flags) :
    gpu(gpu),
    vk_descriptor_types(descriptor_types),
    vk_max_sets(max_sets),
    vk_create_flags(flags)
{
    DENTER("Vulkan::DescriptorPool::DescriptorPool(multiple types)");
    DLOG(info, "Initializing DescriptorPool for multiple types...");
    DINDENT;



    // First, we define how large the pool will be
    DLOG(info, "Preparing structs...");
    Tools::Array<VkDescriptorPoolSize> descriptor_pool_sizes;
    descriptor_pool_sizes.resize(this->vk_descriptor_types.size());
    for (uint32_t i = 0; i < this->vk_descriptor_types.size(); i++) {
        populate_descriptor_pool_size(descriptor_pool_sizes[i], this->vk_descriptor_types[i]);
    }

    // Prepare the create info
    VkDescriptorPoolCreateInfo descriptor_pool_info;
    populate_descriptor_pool_info(descriptor_pool_info, descriptor_pool_sizes, this->vk_max_sets, this->vk_create_flags);



    // Actually allocate the pool
    DLOG(info, "Allocating pool...");
    VkResult vk_result;
    if ((vk_result = vkCreateDescriptorPool(this->gpu, &descriptor_pool_info, nullptr, &this->vk_descriptor_pool)) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate descriptor pool: " + vk_error_map[vk_result]);
    }



    // Before we leave, we can optimise by setting the map to the maximum memory size
    this->vk_descriptor_sets.reserve(this->vk_max_sets);



    DDEDENT;
    DLEAVE;
}

/* Copy constructor for the DescriptorPool. */
DescriptorPool::DescriptorPool(const DescriptorPool& other) :
    gpu(other.gpu),
    vk_descriptor_types(other.vk_descriptor_types),
    vk_max_sets(other.vk_max_sets),
    vk_create_flags(other.vk_create_flags)
{
    DENTER("Compute::DescriptorPool::DescriptorPool(copy)");

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
        DLOG(fatal, "Could not allocate descriptor pool: " + vk_error_map[vk_result]);
    }

    // Do not copy individual descriptors



    // Before we leave, we can optimise by setting the map to the maximum memory size
    this->vk_descriptor_sets.reserve(this->vk_max_sets);



    DLEAVE;
}
        
/* Move constructor for the DescriptorPool. */
DescriptorPool::DescriptorPool(DescriptorPool&& other):
    gpu(other.gpu),
    vk_descriptor_pool(other.vk_descriptor_pool),
    vk_descriptor_types(other.vk_descriptor_types),
    vk_max_sets(other.vk_max_sets),
    vk_create_flags(other.vk_create_flags),
    vk_descriptor_sets(other.vk_descriptor_sets)
{
    // Set the other's pool & sets to nullptr to avoid deallocation
    other.vk_descriptor_pool = nullptr;
    other.vk_descriptor_sets.clear();
}

/* Destructor for the DescriptorPool. */
DescriptorPool::~DescriptorPool() {
    DENTER("Compute::DescriptorPool::~DescriptorPool");
    DLOG(info, "Cleaning DescriptorPool...");
    DINDENT;

    VkResult vk_result;
    if (this->vk_descriptor_sets.size() > 0) {
        DLOG(info, "Deallocating descriptor sets...");
        for (const std::pair<descriptor_set_h, VkDescriptorSet>& p : this->vk_descriptor_sets) {
            if ((vk_result = vkFreeDescriptorSets(this->gpu, this->vk_descriptor_pool, 1, &p.second)) != VK_SUCCESS) {
                DLOG(nonfatal, "Could not deallocate descriptor sets: " + vk_error_map[vk_result]);
            }
        }
    }

    if (this->vk_descriptor_pool != nullptr) {
        DLOG(info, "Deallocating pool...");
        vkDestroyDescriptorPool(this->gpu, this->vk_descriptor_pool, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Allocates a single descriptor set with the given layout. Will fail with errors if there's no more space. */
descriptor_set_h DescriptorPool::allocate_h(const DescriptorSetLayout& descriptor_set_layout) {
    DENTER("Compute::DescriptorPool::allocate_h");

    // Check if we have enough space left
    if (static_cast<uint32_t>(this->vk_descriptor_sets.size()) >= this->vk_max_sets) {
        DLOG(fatal, "Cannot allocate new DescriptorSet: already allocated maximum of " + std::to_string(this->vk_max_sets) + " sets.");
    }

    // Find a handle for this set
    descriptor_set_h handle = find_handle(this->vk_descriptor_sets, DescriptorPool::NullHandle);
    
    // Put the layout in a struct s.t. we can pass it and keep it in memory until after the call
    Tools::Array<VkDescriptorSetLayout> vk_descriptor_set_layouts({ descriptor_set_layout.descriptor_set_layout() });

    // Next, populate the create info
    VkDescriptorSetAllocateInfo descriptor_set_info;
    populate_descriptor_set_info(descriptor_set_info, this->vk_descriptor_pool, vk_descriptor_set_layouts, 1);

    // We can now call the allocate function
    VkResult vk_result;
    VkDescriptorSet set;
    if ((vk_result = vkAllocateDescriptorSets(this->gpu, &descriptor_set_info, &set)) != VK_SUCCESS) {
        DLOG(fatal, "Failed to allocate new DescriptorSet: " + vk_error_map[vk_result]);
    }

    // Insert the set in the internal map
    this->vk_descriptor_sets.insert(std::make_pair(handle, set));

    // With that done, return the handle
    DRETURN handle;
}

/* Allocates multiple descriptor sets with the given layout, returning them as an Array. Will fail with errors if there's no more space. */
Tools::Array<DescriptorSet> DescriptorPool::nallocate(uint32_t n_sets, const Tools::Array<Vulkan::DescriptorSetLayout>& descriptor_set_layouts) {
    DENTER("Compute::DescriptorPool::nallocate");

    // Allocate a list of handles
    Tools::Array<descriptor_set_h> handles = this->nallocate_h(n_sets, descriptor_set_layouts);
    // Create a list to return
    Tools::Array<DescriptorSet> to_return(handles.size());
    // Convert all of the handles to buffers
    for (uint32_t i = 0; i < handles.size(); i++) {
        to_return.push_back(this->deref(handles[i]));
    }

    // Done, return
    DRETURN to_return;
}

/* Allocates multiple descriptor sets with the given layout, returning them as an Array. Will fail with errors if there's no more space. */
Tools::Array<descriptor_set_h> DescriptorPool::nallocate_h(uint32_t n_sets, const Tools::Array<DescriptorSetLayout>& descriptor_set_layouts) {
    DENTER("Compute::DescriptorPool::nallocate_h");

    #ifndef NDEBUG
    // If n_sets if null, nothing to do
    if (n_sets == 0) {
        DLOG(warning, "Request to allocate 0 sets received; nothing to do.");
        DRETURN Tools::Array<descriptor_set_h>();
    }
    // If we aren't passed enough layouts, then tell us
    if (n_sets != descriptor_set_layouts.size()) {
        DLOG(fatal, "Not enough descriptor set layouts passed: got " + std::to_string(descriptor_set_layouts.size()) + ", expected " + std::to_string(n_sets));
    }
    #endif
    // Check if we have enough space left
    if (static_cast<uint32_t>(this->vk_descriptor_sets.size()) + n_sets > this->vk_max_sets) {
        DLOG(fatal, "Cannot allocate " + std::to_string(n_sets) + " new DescriptorSets: only space for " + std::to_string(this->vk_max_sets - static_cast<uint32_t>(this->vk_descriptor_sets.size())) + " sets");
    }

    // Next, fetch enough handles for all the new sets
    Tools::Array<descriptor_set_h> handles(n_sets);
    for (uint32_t i = 0; i < handles.size(); i++) {
        handles.push_back(find_handle(this->vk_descriptor_sets, DescriptorPool::NullHandle));
    }
    // Get the VkDescriptorSetLayout objects behind the layouts
    Tools::Array<VkDescriptorSetLayout> vk_descriptor_set_layouts(descriptor_set_layouts.size());
    for (uint32_t i = 0; i < descriptor_set_layouts.size(); i++) {
        vk_descriptor_set_layouts.push_back(descriptor_set_layouts[i]);
    }
    // Create a temporary list of result sets to which we can allocate
    Tools::Array<VkDescriptorSet> temp;

    // Next, populate the create info
    VkDescriptorSetAllocateInfo descriptor_set_info;
    populate_descriptor_set_info(descriptor_set_info, this->vk_descriptor_pool, vk_descriptor_set_layouts, n_sets);
    // We can now call the allocate function
    VkResult vk_result;
    if ((vk_result = vkAllocateDescriptorSets(this->gpu, &descriptor_set_info, temp.wdata(n_sets))) != VK_SUCCESS) {
        DLOG(fatal, "Failed to allocate " + std::to_string(n_sets) + " new DescriptorSets: " + vk_error_map[vk_result]);
    }

    // Insert each of the new sets
    for (uint32_t i = 0; i < n_sets; i++) {
        this->vk_descriptor_sets.insert(make_pair(handles[i], temp[i]));
    }

    // We're done, so return the set of handles
    DRETURN handles;
}



/* Deallocates the given descriptor set. */
void DescriptorPool::deallocate(descriptor_set_h set) {
    DENTER("Vulkan::DescriptorPool::deallocate");

    // Check if the handle exists
    std::unordered_map<descriptor_set_h, VkDescriptorSet>::iterator iter = this->vk_descriptor_sets.find(set);
    if (iter == this->vk_descriptor_sets.end()) {
        DLOG(fatal, "Given handle does not exist.");
    }

    // If it does, then first free the buffer
    vkFreeDescriptorSets(this->gpu, this->vk_descriptor_pool, 1, &((*iter).second));

    // Remove it from the list
    this->vk_descriptor_sets.erase(iter);

    // Done
    DRETURN;
}

/* Deallocates an array of given descriptors set. */
void DescriptorPool::ndeallocate(const Tools::Array<DescriptorSet>& descriptor_sets) {
    DENTER("Compute::DescriptorPool::ndeallocate(objects)");

    // Convert the list of descriptor sets to handles
    Tools::Array<descriptor_set_h> handles(descriptor_sets.size());
    for (uint32_t i = 0; i < descriptor_sets.size(); i++) {
        handles.push_back(descriptor_sets[i].handle());
    }

    // Call the ndeallocate for handles
    this->ndeallocate(handles);

    // Done!
    DRETURN;
}

/* Deallocates an array of given descriptors set handles. */
void DescriptorPool::ndeallocate(const Tools::Array<descriptor_set_h>& handles) {
    DENTER("Compute::DescriptorPool::ndeallocate(handles)");

    // First, we check if all handles exist
    Tools::Array<VkDescriptorSet> to_remove(handles.size());
    for (uint32_t i = 0; i < handles.size(); i++) {
        // Do the check
        std::unordered_map<descriptor_set_h, VkDescriptorSet>::iterator iter = this->vk_descriptor_sets.find(handles[i]);
        if (iter == this->vk_descriptor_sets.end()) {
            DLOG(fatal, "Handle at index " + std::to_string(i) + " does not exist.");
        }

        // Mark the Vk object for removal
        to_remove.push_back((*iter).second);

        // Remove the item from the internal map
        this->vk_descriptor_sets.erase(iter);
    }

    // All that's left is to actually remove the handles; do that
    VkResult vk_result;
    if ((vk_result = vkFreeDescriptorSets(this->gpu, this->vk_descriptor_pool, to_remove.size(), to_remove.rdata())) != VK_SUCCESS) {
        DLOG(fatal, "Could not deallocate descriptor sets: " + vk_error_map[vk_result]);
    }

    // Done!
    DRETURN;
}



/* Swap operator for the DescriptorPool class. */
void Vulkan::swap(DescriptorPool& dp1, DescriptorPool& dp2) {
    DENTER("Vulkan::swap(DescriptorPool)");

    using std::swap;

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (dp1.gpu != dp2.gpu) {
        DLOG(fatal, "Cannot swap descriptor pools with different GPUs");
    }
    #endif

    // Swap all fields
    swap(dp1.vk_descriptor_pool, dp2.vk_descriptor_pool);
    swap(dp1.vk_descriptor_types, dp2.vk_descriptor_types);
    swap(dp1.vk_max_sets, dp2.vk_max_sets);
    swap(dp1.vk_create_flags, dp2.vk_create_flags);
    swap(dp1.vk_descriptor_sets, dp2.vk_descriptor_sets);

    // Done
    DRETURN;
}
