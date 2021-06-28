/* QUEUE INFO.cpp
 *   by Lut99
 *
 * Created:
 *   05/06/2021, 15:27:42
 * Last edited:
 *   05/06/2021, 15:27:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the QueueInfo class, which describes what queue families are
 *   present on the GPU and by what index.
**/

#include "tools/CppDebugger.hpp"

#include "QueueInfo.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** QUEUEINFO CLASS *****/
/* Default constructor for the QueueInfo class, which initializes the info to nothing supported. Use ::refresh() to populate it normally. */
QueueInfo::QueueInfo() :
    queue_families({
        { QueueType::graphics, -1 },
        { QueueType::compute, -1 },
        { QueueType::memory, -1 },
        { QueueType::present, -1 },
        { QueueType::none, -1 }
    })
{}

/* Constructor for the QueueInfo class, which takes a Vulkan physical device and surface and uses that to set its own properties. */
QueueInfo::QueueInfo(const VkPhysicalDevice& vk_physical_device, const VkSurfaceKHR& vk_surface) :
    // Initialize the queue info to nothing being supported
    QueueInfo()
{
    DENTER("Vulkan::QueueInfo::QueueInfo(gpu, surface)");

    // Simply call refresh()
    this->refresh(vk_physical_device, vk_surface);

    DLEAVE;
}



/* Refreshes the QueueInfo, i.e., re-populates its values according to the given device and surface. */
void QueueInfo::refresh(const VkPhysicalDevice& vk_physical_device, const VkSurfaceKHR& vk_surface) {
    DENTER("Vulkan::QueueInfo::refresh");

    // First, get a list of all the queues supported by the GPU
    uint32_t n_supported_queues;
    vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &n_supported_queues, nullptr);
    Array<VkQueueFamilyProperties> supported_queues(n_supported_queues);
    vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &n_supported_queues, supported_queues.wdata(n_supported_queues));

    // Prepare a list of UINT32_MAX uint32_t numbers that we'll use to determine the "speciality" of a queue
    uint32_t best_specialities[QueueInfo::n_queues];
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        best_specialities[i] = UINT32_MAX;
    }

    // Loop through the queues to find the compute queue
    VkBool32 vk_can_present;
    for (uint32_t i = 0; i < supported_queues.size(); i++) {
        // First, collect the properties of the queue family
        bool capabilities[QueueInfo::n_queues];
        capabilities[0] = supported_queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
        capabilities[1] = supported_queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
        capabilities[2] = supported_queues[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
        vkGetPhysicalDeviceSurfaceSupportKHR(vk_physical_device, i, vk_surface, &vk_can_present);
        capabilities[3] = (bool) vk_can_present;

        // Use that to count how many abilities the queue has
        uint32_t n_specialties = 0;
        for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
            n_specialties += capabilities[i] ? 1 : 0;
        }

        // Once that's done, check if we need to write this queue for any of the queue families
        for (const std::pair<QueueType, int64_t>& p : this->queue_families) {
            uint32_t cap_i = static_cast<uint32_t>(p.first);
            if (capabilities[cap_i] && (p.second < 0 || n_specialties < best_specialities[cap_i])) {
                // Update the graphics family
                this->queue_families.at(p.first) = i;

                // Update this family's best specialty measure
                best_specialities[cap_i] = n_specialties;
            }
        }
    }

    // Once we're done searching the queues, first populate the queue array
    this->queue_indices.clear();
    this->queue_indices.reserve(QueueInfo::n_queues);
    for (const std::pair<QueueType, int64_t>& p : this->queue_families) {
        if (p.second == -1) { continue; }
        this->queue_indices.push_back(p.second);
    }

    // Next, set the unique queue(count)
    this->uqueues_indices.clear();
    this->uqueues_indices.reserve(QueueInfo::n_queues);
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        // Check if it already occurs
        bool found = false;
        for (uint32_t j = 0; j < this->uqueues_indices.size(); j++) {
            if (this->uqueues_indices[j] == this->queue_indices[i]) {
                found = true;
                break;
            }
        }
        if (!found) {
            // It's unique; append it
            this->uqueues_indices.push_back(this->queue_indices[i]);
        }
    }

    // We're done refreshing!
    DRETURN;
}
