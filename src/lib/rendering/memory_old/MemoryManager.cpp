/* MEMORY MANAGER.cpp
 *   by Lut99
 *
 * Created:
 *   27/07/2021, 16:29:22
 * Last edited:
 *   8/1/2021, 5:06:15 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "MemoryManager.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** MEMORYMANAGER CLASS *****/
/* Constructor for the MemoryManager class, which takes the GPU where it is defined for and the sizes of the two memory pools. */
MemoryManager::MemoryManager(const Rendering::GPU& gpu, VkDeviceSize draw_pool_size, VkDeviceSize stage_pool_size) :
    gpu(gpu),

    draw_cmd_pool(this->gpu, this->gpu.queue_info().graphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
    mem_cmd_pool(this->gpu, this->gpu.queue_info().memory(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),

    draw_pool(this->gpu, Rendering::MemoryPool::select_memory_type(this->gpu, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), draw_pool_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    stage_pool(this->gpu, Rendering::MemoryPool::select_memory_type(this->gpu, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), stage_pool_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),

    descr_pool(this->gpu, { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 } }, 1),

    copy_cmd_h(this->mem_cmd_pool.allocate_h())
{}

/* Copy constructor for the MemoryManager class. */
MemoryManager::MemoryManager(const MemoryManager& other) :
    gpu(other.gpu),

    draw_cmd_pool(other.draw_cmd_pool),
    mem_cmd_pool(other.mem_cmd_pool),

    draw_pool(other.draw_pool),
    stage_pool(other.stage_pool),

    descr_pool(other.descr_pool),

    copy_cmd_h(this->mem_cmd_pool.allocate_h())

{}

/* Move constructor for the MemoryManager class. */
MemoryManager::MemoryManager(MemoryManager&& other) :
    gpu(other.gpu),

    draw_cmd_pool(other.draw_cmd_pool),
    mem_cmd_pool(other.mem_cmd_pool),

    draw_pool(other.draw_pool),
    stage_pool(other.stage_pool),

    descr_pool(other.descr_pool),

    copy_cmd_h(other.copy_cmd_h)
{
    other.copy_cmd_h = CommandPool::NullHandle;
}

/* Destructor for the MemoryManager class. */
MemoryManager::~MemoryManager() {
    

    if (this->copy_cmd_h != CommandPool::NullHandle) {
        this->mem_cmd_pool.deallocate(this->copy_cmd_h);
    }
}



/* Swap operator for the MemoryManager class. */
void Rendering::swap(MemoryManager& mm1, MemoryManager& mm2) {
    

    #ifndef NDEBUG
    if (mm1.gpu != mm2.gpu) {
        DLOG(fatal, "Cannot swap memory managers with different GPUs");
    }
    #endif

    // Swap 'em all
    using std::swap;

    swap(mm1.draw_cmd_pool, mm2.draw_cmd_pool);
    swap(mm1.mem_cmd_pool, mm2.mem_cmd_pool);

    swap(mm1.draw_pool, mm2.draw_pool);
    swap(mm1.stage_pool, mm2.stage_pool);

    swap(mm1.descr_pool, mm2.descr_pool);

    swap(mm1.copy_cmd_h, mm2.copy_cmd_h);

    // Done
    return;
}
