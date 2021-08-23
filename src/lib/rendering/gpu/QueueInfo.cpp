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

#include "tools/Tracer.hpp"

#include "QueueInfo.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** QUEUEINFO CLASS *****/
/* "Default" constructor for the QueueInfo class, which initializes the info to nothing supported. Use ::refresh() to populate it normally. */
QueueInfo::QueueInfo(const Tools::Logger::InitData& init_data) :
    logger(init_data, "QueueInfo"),
    queue_families({
        { QueueType::graphics, std::make_pair(-1, -1) },
        { QueueType::compute, std::make_pair(-1, -1) },
        { QueueType::memory, std::make_pair(-1, -1) },
        { QueueType::present, std::make_pair(-1, -1) }
    })
{}

/* Constructor for the QueueInfo class, which takes the initial data for the internal logger, a Vulkan physical device and surface and uses that to set its own properties. */
QueueInfo::QueueInfo(const Tools::Logger::InitData& init_data, const VkPhysicalDevice& vk_physical_device, const VkSurfaceKHR& vk_surface) :
    // Initialize the queue info to nothing being supported
    QueueInfo(init_data)
{
    TENTER("Rendering::QueueInfo::QueueInfo(gpu, surface)");

    // Simply call refresh()
    this->refresh(vk_physical_device, vk_surface);

    TLEAVE;
}



/* Refreshes the QueueInfo, i.e., re-populates its values according to the given device and surface. */
void QueueInfo::refresh(const VkPhysicalDevice& vk_physical_device, const VkSurfaceKHR& vk_surface) {
    this->logger.log(Verbosity::details, "Refreshing QueueInfo...");

    // First, get a list of all the queues supported by the GPU
    uint32_t n_supported_queues;
    TCALL(vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &n_supported_queues, nullptr));
    Array<VkQueueFamilyProperties> supported_queues(n_supported_queues);
    TCALL(vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &n_supported_queues, supported_queues.wdata(n_supported_queues)));

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
        TCALL(vkGetPhysicalDeviceSurfaceSupportKHR(vk_physical_device, i, vk_surface, &vk_can_present));
        capabilities[3] = (bool) vk_can_present;

        // Use that to count how many abilities the queue has
        uint32_t n_specialties = 0;
        for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
            n_specialties += capabilities[i] ? 1 : 0;
        }

        // Once that's done, check if we need to write this queue for any of the queue families
        for (const std::pair<QueueType, std::pair<int64_t, int64_t>>& p : this->queue_families) {
            uint32_t cap_i = static_cast<uint32_t>(p.first);
            if (capabilities[cap_i] && (p.second.first < 0 || n_specialties < best_specialities[cap_i])) {
                // Update the graphics family
                this->queue_families.at(p.first) = make_pair(static_cast<int64_t>(i), static_cast<int64_t>(supported_queues[cap_i].queueCount));

                // Update this family's best specialty measure
                best_specialities[cap_i] = n_specialties;
            }
        }
    }

    // Once we're done searching the queues, first populate the queue array
    this->queue_indices.clear();
    this->queue_indices.reserve(QueueInfo::n_queues);
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        int64_t index = this->queue_families.at(static_cast<QueueType>(i)).first;
        if (index >= 0) {
            this->queue_indices.push_back(static_cast<uint32_t>(index));
        }
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
    for (const std::pair<QueueType, std::pair<int64_t, int64_t>>& p : this->queue_families) {
        this->logger.log(Verbosity::details, queue_type_names[(int) p.first], " queue family has index ", p.second.first);
    }
}
