/* SEMAPHORE.cpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 15:26:14
 * Last edited:
 *   27/06/2021, 15:26:14
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Wraps the VkSemaphore class, mostly managing its memory.
**/

#include "../auxillary/ErrorCodes.hpp"

#include "Semaphore.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkSemaphoreCreateInfo struct. */
static void populate_semaphore_info(VkSemaphoreCreateInfo& semaphore_info) {
    // Set to default
    semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
}





/***** SEMAPHORE CLASS *****/
/* Constructor for the Semaphore class, which takes completely nothing! (except for the GPU where it lives, obviously) */
Semaphore::Semaphore(const Rendering::GPU& gpu) :
    gpu(gpu)
{
    // Populate the create info
    VkSemaphoreCreateInfo semaphore_info;
    populate_semaphore_info(semaphore_info);

    // Create the semaphore
    VkResult vk_result;
    if ((vk_result = vkCreateSemaphore(this->gpu, &semaphore_info, nullptr, &this->vk_semaphore)) != VK_SUCCESS) {
        logger.fatalc(Semaphore::channel, "Could not create semaphore: ", vk_error_map[vk_result]);
    }
}

/* Copy constructor for the Semaphore class. */
Semaphore::Semaphore(const Semaphore& other) :
    gpu(other.gpu)
{
    // Populate the create info
    VkSemaphoreCreateInfo semaphore_info;
    populate_semaphore_info(semaphore_info);

    // Create the semaphore
    VkResult vk_result;
    if ((vk_result = vkCreateSemaphore(this->gpu, &semaphore_info, nullptr, &this->vk_semaphore)) != VK_SUCCESS) {
        logger.fatalc(Semaphore::channel, "Could not re-create semaphore: ", vk_error_map[vk_result]);
    }
}

/* Move constructor for the Semaphore class. */
Semaphore::Semaphore(Semaphore&& other) :
    gpu(other.gpu),
    vk_semaphore(other.vk_semaphore)
{
    // Prevent the semaphore from being deallocated
    other.vk_semaphore = nullptr;
}

/* Destructor for the Semaphore class. */
Semaphore::~Semaphore() {
    if (this->vk_semaphore != nullptr) {
        vkDestroySemaphore(this->gpu, this->vk_semaphore, nullptr);
    }
}



/* Swap operator for the Semaphore class. */
void Rendering::swap(Semaphore& s1, Semaphore& s2) {
    

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (s1.gpu != s2.gpu) { logger.fatalc(Semaphore::channel, "Cannot swap semaphores with different GPUs"); }
    #endif

    // Swap EVERYTHING but the GPU
    using std::swap;
    swap(s1.vk_semaphore, s2.vk_semaphore);
}
