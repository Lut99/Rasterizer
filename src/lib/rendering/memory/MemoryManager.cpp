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


/***** HELPER FUNCTIONS *****/
/* Selects the memory type used for the draw pool. */



/***** MEMORYMANAGER CLASS *****/
/* Constructor for the MemoryManager class, which takes the GPU where it is defined for. */
MemoryManager::MemoryManager(const Rendering::GPU& gpu) :
    gpu(gpu),

    draw_cmd_pool(this->gpu, this->gpu.queue_info().graphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
    mem_cmd_pool(this->gpu, this->gpu.queue_info().memory(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),

    draw_pool(this->gpu, Rendering::MemoryPool::select_memory_type(this->gpu, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), MemoryManager::draw_pool_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    stage_pool(this->gpu, Rendering::MemoryPool::select_memory_type(this->gpu, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), MemoryManager::stage_pool_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),

    descr_pool(this->gpu, { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 } }, 1)
{}
