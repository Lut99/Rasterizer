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

#include "tools/Logger.hpp"

#include "MemoryManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** MEMORYMANAGER CLASS *****/
/* Constructor for the MemoryManager class, which takes the GPU where it is defined for and the sizes of the two memory pools. */
MemoryManager::MemoryManager(const Rendering::GPU& gpu, VkDeviceSize draw_pool_size, VkDeviceSize stage_pool_size) :
    gpu(gpu),

    draw_cmd_pool(this->gpu, this->gpu.queue_info().graphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),
    mem_cmd_pool(this->gpu, this->gpu.queue_info().memory(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT),

    draw_pool(this->gpu, draw_pool_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    stage_pool(this->gpu, stage_pool_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),

    descr_pool(this->gpu, { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 } }, 1),

    view_pool(this->gpu),

    copy_cmd(this->mem_cmd_pool.allocate())
{}

/* Copy constructor for the MemoryManager class. */
MemoryManager::MemoryManager(const MemoryManager& other) :
    gpu(other.gpu),

    draw_cmd_pool(other.draw_cmd_pool),
    mem_cmd_pool(other.mem_cmd_pool),

    draw_pool(other.draw_pool),
    stage_pool(other.stage_pool),

    descr_pool(other.descr_pool),

    view_pool(other.view_pool),

    copy_cmd(this->mem_cmd_pool.allocate())

{}

/* Move constructor for the MemoryManager class. */
MemoryManager::MemoryManager(MemoryManager&& other) :
    gpu(other.gpu),

    draw_cmd_pool(other.draw_cmd_pool),
    mem_cmd_pool(other.mem_cmd_pool),

    draw_pool(other.draw_pool),
    stage_pool(other.stage_pool),

    descr_pool(other.descr_pool),

    view_pool(other.view_pool),

    copy_cmd(other.copy_cmd)
{
    other.copy_cmd = nullptr;
}

/* Destructor for the MemoryManager class. */
MemoryManager::~MemoryManager() {
    if (this->copy_cmd != nullptr) {
        this->mem_cmd_pool.free(this->copy_cmd);
    }
}



/* Swap operator for the MemoryManager class. */
void Rendering::swap(MemoryManager& mm1, MemoryManager& mm2) {
    #ifndef NDEBUG
    if (mm1.gpu != mm2.gpu) { logger.fatalc(MemoryManager::channel, "Cannot swap memory managers with different GPUs"); }
    #endif

    // Swap 'em all
    using std::swap;

    swap(mm1.draw_cmd_pool, mm2.draw_cmd_pool);
    swap(mm1.mem_cmd_pool, mm2.mem_cmd_pool);

    swap(mm1.draw_pool, mm2.draw_pool);
    swap(mm1.stage_pool, mm2.stage_pool);

    swap(mm1.descr_pool, mm2.descr_pool);

    swap(mm1.view_pool, mm2.view_pool);

    swap(mm1.copy_cmd, mm2.copy_cmd);
}
