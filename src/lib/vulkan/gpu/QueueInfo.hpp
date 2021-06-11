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

#ifndef VULKAN_QUEUE_INFO_HPP
#define VULKAN_QUEUE_INFO_HPP

#include <string>
#include <vulkan/vulkan.h>

#include "tools/Array.hpp"

namespace Rasterizer::Vulkan {
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



    /* The QueueFamily class, which describes a single queue family. */
    class QueueFamily {
    public:
        /* The type of this family. */
        QueueType type;
        /* The GPU-relevant index of this family. */
        uint32_t index;
        /* Whether or not this family is supported on the chosen device. */
        bool supported;
        /* The number of queues that this family maximally supports. */
        uint32_t n_queues;
    
    private:
        /* Default constructor for the QueueFamily class, which simply initializes everything to 0. */
        QueueFamily();

        /* Mark the QueueInfo as our friend. */
        friend class QueueInfo;

    public:
        /* Implicit conversion to the QueueFamily's index. */
        inline operator uint32_t() const { return this->index; }

    };



    /* The DeviceQueueInfo class, which describes the queues that a GPU supports. */
    class QueueInfo {
    public:
        /* Static constant that lists the number of queue families listed in the QueueInfo. */
        static const constexpr uint32_t n_queues = 4;

    private:
        /* Lists all of the queue families. */
        QueueFamily queue_families[n_queues];
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
        inline const QueueFamily& operator[](QueueType family) const { return this->queue_families[(int) family]; }
        /* Returns the queue family with the given queue family index. */
        const QueueFamily& operator[](uint32_t family) const;

        /* Returns a list of all queue indices in the QueueInfo. */
        inline const Tools::Array<uint32_t>& queues() const { return this->queue_indices; }
        /* Returns a list of all _unique_ queue indices in the QueueInfo. */
        inline const Tools::Array<uint32_t>& uqueues() const { return this->uqueues_indices; }

        /* Returns the QueueFamily struct for the graphics queue. */
        inline const QueueFamily& graphics() const { return this->queue_families[(int) QueueType::graphics]; }
        /* Returns the QueueFamily struct for the compute queue. */
        inline const QueueFamily& compute() const { return this->queue_families[(int) QueueType::compute]; }
        /* Returns the QueueFamily struct for the memory queue. */
        inline const QueueFamily& memory() const { return this->queue_families[(int) QueueType::memory]; }
        /* Returns the QueueFamily struct for the present queue. */
        inline const QueueFamily& present() const { return this->queue_families[(int) QueueType::present]; }

    };
}

#endif
