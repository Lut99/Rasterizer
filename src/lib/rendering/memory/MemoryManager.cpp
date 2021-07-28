/* MEMORY MANAGER.cpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 16:29:22
 * Last edited:
 *   27/07/2021, 16:29:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "tools/CppDebugger.hpp"

#include "MemoryManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** MEMORYMANAGER CLASS *****/
/* Constructor for the MemoryManager class, which takes the GPU where it is defined for. */
MemoryManager::MemoryManager(const Rendering::GPU& gpu) :
    gpu(gpu)
{}



/* Adds a new memory pool to the manager that has the given memory properties and the given size (in bytes). Optionally, buffer usage flags can be given to properly configure the memory type chosen for the memory pool. */
void MemoryManager::add_memory(VkDeviceSize size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage) {
    DENTER("Rendering::MemoryManager::add_memory");

    // Choose a memory type that best fits the bill
    uint32_t memory_type = MemoryPool::select_memory_type(this->gpu, buffer_usage, memory_properties);
    // Declare the pool with of that type
    this->memory_pools.push_back(std::move(MemoryPool(this->gpu, memory_type, size, memory_properties)));

    // DOne
    DRETURN;
}

/* Adds a new command pool to the manager for the given queue family and with the given VkCommandPoolCreateFlags. */
void MemoryManager::add_command(uint32_t queue_family, VkCommandPoolCreateFlags command_pool_flags) {
    DENTER("Rendering::MemoryManager::add_command");

    // Declare the pool with the given create flags
    this->command_pools.push_back(std::move(CommandPool(this->gpu, queue_family, command_pool_flags)));

    // D0ne
    DRETURN;
}

/* Adds a new descriptor pool to the manager with the given number of descriptor sets and number of descriptors per set. */
void MemoryManager::add_descriptor(const Tools::Array<std::pair<VkDescriptorType, uint32_t>>& descriptor_types, uint32_t max_sets) {
    DENTER("Rendering::MemoryManager::add_descriptor");

    // Declare the pool with the given create flags
    this->descriptor_pools.push_back(std::move(DescriptorPool(this->gpu, descriptor_types, max_sets)));

    // D0ne
    DRETURN;
}
