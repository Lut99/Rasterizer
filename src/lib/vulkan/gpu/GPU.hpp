/* GPU.hpp
 *   by Lut99
 *
 * Created:
 *   16/04/2021, 17:21:54
 * Last edited:
 *   28/06/2021, 20:06:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Main library that is an abstraction of a Vulkan GPU, which binds
 *   itself to a GPU we choose and then interfaces us with the Vulkan
 *   library.
**/

#ifndef VULKAN_GPU_HPP
#define VULKAN_GPU_HPP

#include <vulkan/vulkan.h>

#include "vulkan/instance/Instance.hpp"
#include "vulkan/gpu/Surface.hpp"
#include "tools/Array.hpp"

#include "QueueInfo.hpp"
#include "SwapchainInfo.hpp"

namespace Rasterizer::Vulkan {
    /* The Vulkan device extensions we want to be enabled. */
    const Tools::Array<const char*> device_extensions({
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    });



    /* The GPU class, which is the main interface to our Vulkan compute library implementation. */
    class GPU {
    public:
        /* Constant reference to the instance where this GPU is declared with. */
        const Instance& instance;

    private:
        /* The physical GPU this class references. */
        VkPhysicalDevice vk_physical_device;
        /* The properties of said device, like its name. */
        VkPhysicalDeviceProperties vk_physical_device_properties;
        /* The queue information for this device. */
        QueueInfo vk_queue_info;
        /* The swapchain support of this device. */
        SwapchainInfo vk_swapchain_info;

        /* The logical device this class references. */
        VkDevice vk_device;
        /* The extensions enabled on the device. */
        Tools::Array<const char*> vk_extensions;
        
        /* The physical queues for this device, as a map of each family to an Array of queues. */
        Tools::Array<VkQueue> vk_queues[QueueInfo::n_queues];

    public:
        /* Constructor for the GPU class, which takes a Vulkan instance, the target surface and a list of required extensions to enable on the GPU. */
        GPU(const Instance& instance, const Surface& surface, const Tools::Array<uint32_t>& queue_counts = Tools::Array<uint32_t>({ 1, 1, 1, 1 }), const Tools::Array<const char*>& extensions = device_extensions);
        /* Copy constructor for the GPU class. */
        GPU(const GPU& other);
        /* Move constructor for the GPU class. */
        GPU(GPU&& other);
        /* Destructor for the GPU class. */
        ~GPU();

        /* Allows the GPU to be compared with another GPU class. */
        inline bool operator==(const GPU& other) const { return this->vk_device == other.vk_device; }
        /* Allows the GPU to be (negated) compared with another GPU class. */
        inline bool operator!=(const GPU& other) const { return this->vk_device != other.vk_device; }

        /* Blocks until the GPU is ready with its scheduled operations. */
        inline void wait_for_idle() const { vkDeviceWaitIdle(this->vk_device); }
        /* Returns the array that contains all queues of the given family. */
        inline const Tools::Array<VkQueue>& queues(QueueType family) const { return this->vk_queues[static_cast<uint32_t>(family)]; }

        /* Returns the name of the chosen GPU. */
        inline std::string name() const { return std::string(this->vk_physical_device_properties.deviceName); }
        /* Returns the queue information of the chosen GPU. */
        inline const QueueInfo& queue_info() const { return this->vk_queue_info; }
        /* Returns the swapchain information of the chosen GPU. */
        inline const SwapchainInfo& swapchain_info() const { return this->vk_swapchain_info; }
        
        /* Explicitly provides (read-only) access to the internal vk_physical_device object. */
        inline const VkPhysicalDevice& physical_device() const { return this->vk_physical_device; }
        /* Implicitly provides (read-only) access to the internal vk_physical_device object. */
        inline operator VkPhysicalDevice() const { return this->vk_physical_device; }
        /* Explicitly provides (read-only) access to the internal vk_device object. */
        inline const VkDevice& device() const { return this->vk_device; }
        /* Implicitly provides (read-only) access to the internal vk_device object. */
        inline operator VkDevice() const { return this->vk_device; }

        /* Copy assignment operator for the GPU class. */
        inline GPU& operator=(const GPU& other) { return *this = GPU(other); }
        /* Move assignment operator for the GPU class. */
        inline GPU& operator=(GPU&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the GPU class. */
        friend void swap(GPU& g1, GPU& g2);
  
    };

    /* Swap operator for the GPU class. */
    void swap(GPU& g1, GPU& g2);
}

#endif
