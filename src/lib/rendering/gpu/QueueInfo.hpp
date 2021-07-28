/* QUEU EINFO.hpp
 *   by Lut99
 *
 * Created:
 *   05/06/2021, 15:27:33
 * Last edited:
 *   05/06/2021, 15:27:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the QueueInfo class, which describes what queue families are
 *   present on the GPU and by what index.
**/

#ifndef RENDERING_QUEUE_INFO_HPP
#define RENDERING_QUEUE_INFO_HPP

#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include "tools/Array.hpp"

namespace Rasterizer::Rendering {
    /* Enum that lists the useful queue families for us. */
    enum class QueueType {
        graphics = 0,
        compute = 1,
        memory = 2,
        present = 3,
        none = 4
    };
    /* Static map that maps a queue type of a string name. */
    static const std::string queue_type_names[] = {
        "graphics",
        "compute",
        "memory",
        "present",
         "none"
    };



    /* The DeviceQueueInfo class, which describes the queues that a GPU supports. */
    class QueueInfo {
    public:
        /* Static constant that lists the number of queue families listed in the QueueInfo. */
        static const constexpr uint32_t n_queues = 4;

    private:
        /* Maps the queue families to their respective queue type. */
        std::unordered_map<QueueType, std::pair<int64_t, int64_t>> queue_families;

        /* A list that lists the indices of all queue families stored in the QueueInfo, in order. */
        Tools::Array<uint32_t> queue_indices;
        /* A list that lists the indices of all unique queue families stored in the QueueInfo, in order. */
        Tools::Array<uint32_t> uqueues_indices;

    public:
        /* Default constructor for the QueueInfo class, which initializes the info to nothing supported. Use ::refresh() to populate it normally. */
        QueueInfo();
        /* Constructor for the QueueInfo class, which takes a Vulkan physical device and surface and uses that to set its own properties. */
        QueueInfo(const VkPhysicalDevice& vk_physical_device, const VkSurfaceKHR& vk_surface);

        /* Refreshes the QueueInfo, i.e., re-populates its values according to the given device and surface. */
        void refresh(const VkPhysicalDevice& vk_physical_device, const VkSurfaceKHR& vk_surface);

        /* Returns the queue family of the given type. */
        inline uint32_t operator[](QueueType family) const { return static_cast<uint32_t>(this->queue_families.at(family).first); }
        /* Returns whether or not the given queue family exists. */
        inline bool supports(QueueType family) const { return this->queue_families.at(family).first >= 0; }
        /* Returns the number of queues supported for the given queue family. */
        inline uint32_t max_queues(QueueType family) const { return static_cast<uint32_t>(this->queue_families.at(family).second); }

        /* Returns a list of all queue indices in the QueueInfo. */
        inline const Tools::Array<uint32_t>& queues() const { return this->queue_indices; }
        /* Returns a list of all _unique_ queue indices in the QueueInfo. */
        inline const Tools::Array<uint32_t>& uqueues() const { return this->uqueues_indices; }

        /* Returns the QueueFamily struct for the graphics queue. */
        inline uint32_t graphics() const { return (*this)[QueueType::graphics]; }
        /* Returns the QueueFamily struct for the compute queue. */
        inline uint32_t compute() const { return (*this)[QueueType::compute]; }
        /* Returns the QueueFamily struct for the memory queue. */
        inline uint32_t memory() const { return (*this)[QueueType::memory]; }
        /* Returns the QueueFamily struct for the present queue. */
        inline uint32_t present() const { return (*this)[QueueType::present]; }

    };
}

#endif
