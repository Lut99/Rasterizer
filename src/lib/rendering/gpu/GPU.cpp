/* GPU.cpp
 *   by Lut99
 *
 * Created:
 *   16/04/2021, 17:21:49
 * Last edited:
 *   8/1/2021, 4:33:01 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Main library that is an abstraction of a Vulkan GPU, which binds
 *   itself to a GPU we choose and then interfaces us with the Vulkan
 *   library.
**/

#include <cstring>

#include "tools/Logger.hpp"
#include "tools/Common.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "GPU.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates a list of VkDeviceQueueCreateInfo structs based on the given queue info of a device. */
static void populate_queue_infos(Tools::Array<VkDeviceQueueCreateInfo>& queue_infos, const QueueInfo& vk_queue_info, const Tools::Array<uint32_t>& queue_counts, const Tools::Array<Tools::Array<float>>& queue_priorities) {
    // Loop to populate the create infos
    queue_infos.resize(vk_queue_info.uqueues().size());
    for (uint32_t i = 0; i < vk_queue_info.uqueues().size(); i++) {
        // Set the meta properties of the struct
        queue_infos[i] = {};
        queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

        // Next, tell the struct that we want one queue per family
        queue_infos[i].queueFamilyIndex = vk_queue_info.uqueues()[i];
        queue_infos[i].queueCount = queue_counts[i];

        // Finally, give each queue the same priority
        queue_infos[i].pQueuePriorities = queue_priorities[i].rdata();
    }
}

/* Populates a VkPhysicalDeviceFeatures struct with hardcoded settings. */
static void populate_device_features(VkPhysicalDeviceFeatures& device_features, VkBool32 enable_anisotropy) {
    // None!
    device_features = {};

    // Enable anisotropy if asked to do so
    device_features.samplerAnisotropy = enable_anisotropy;
}

/* Populates a VkDeviceCreateInfo struct based on the given list of qeueu infos and the given device features. */
static void populate_device_info(VkDeviceCreateInfo& device_info, const Tools::Array<VkDeviceQueueCreateInfo>& queue_infos, const VkPhysicalDeviceFeatures& device_features, const Tools::Array<const char*>& device_extensions) {
    // Set the meta info first
    device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // Next, pass the queue infos
    device_info.queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos = queue_infos.rdata();

    // Pass the device features
    device_info.pEnabledFeatures = &device_features;

    // Finally, add the extensions we want to enable to the device
    device_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_info.ppEnabledExtensionNames = device_extensions.rdata();
}





/***** GPU SELECTION HELPER FUNCTIONS *****/
/* Given a physical device, checks if it supports the required list of extensions. */
static bool gpu_supports_extensions(const VkPhysicalDevice& vk_physical_device, const Tools::Array<const char*>& device_extensions) {
    // Get a list of all extensions supported on this device
    uint32_t n_supported_extensions = 0;
    VkResult vk_result;
    if ((vk_result = vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &n_supported_extensions, nullptr)) != VK_SUCCESS) {
        logger.warningc(GPU::channel, "Could not get the number of supported extensions on the GPU.");
        return false;
    }
    Tools::Array<VkExtensionProperties> supported_extensions(n_supported_extensions);
    if ((vk_result = vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &n_supported_extensions, supported_extensions.wdata(n_supported_extensions))) != VK_SUCCESS) {
        logger.warningc(GPU::channel, "Could not get the number of supported extensions on the GPU.");
        return false;
    }

    // Now simply make sure that all extensions in the list appear in the retrieved list
    for (uint32_t i = 0; i < device_extensions.size(); i++) {
        const char* extension = device_extensions[i];

        // Try to find it in the list of supported extensions
        bool found = false;
        for (uint32_t j = 0; j < supported_extensions.size(); j++) {
            if (strcmp(extension, supported_extensions[j].extensionName) == 0) {
                // Dope, continue
                found = true;
                break;
            }
        }
        if (!found) {
            // This extension is missing!
            logger.warning("GPU does not support extension '" + std::string(extension) + "'");
            return false;
        }
    }

    // However, if all extensions passed the test, we're done
    return true;
}

/* Given a physical device, checks if it meets our needs. */
static bool is_suitable_gpu(const VkPhysicalDevice& vk_physical_device, const Surface& surface, const Tools::Array<const char*>& device_extensions) {
    // First, we get a list of supported queues on this device
    QueueInfo queue_info(vk_physical_device, surface);

    // Next, check if the device supports the extensions we want
    bool supports_extensions = gpu_supports_extensions(vk_physical_device, device_extensions);

    // With those two, return it the GPU is suitable
    return queue_info.supports(QueueType::graphics) && queue_info.supports(QueueType::compute) && queue_info.supports(QueueType::memory) && queue_info.supports(QueueType::present) && supports_extensions;
}

/* Selects a suitable GPU from the ones that support Vulkan. */
static VkPhysicalDevice select_gpu(const Instance& instance, const Surface& surface, const Tools::Array<const char*>& device_extensions) {
    // Get how many Vulkan-capable devices are out there
    uint32_t n_available_devices = 0;
    VkResult vk_result;
    if ((vk_result = vkEnumeratePhysicalDevices(instance, &n_available_devices, nullptr)) != VK_SUCCESS) {
        logger.fatalc(GPU::channel, "Could not get the number of available GPUs: ", vk_error_map[vk_result]);
    }
    if (n_available_devices == 0) {
        logger.fatalc(GPU::channel, "No Vulkan-compatible GPUs found.");
    }

    // If there are GPUs, then we fetch a list
    Tools::Array<VkPhysicalDevice> available_devices(n_available_devices);
    if ((vk_result = vkEnumeratePhysicalDevices(instance, &n_available_devices, available_devices.wdata(n_available_devices))) != VK_SUCCESS) {
        logger.fatalc(GPU::channel, "Could not get list of available GPUs: ", vk_error_map[vk_result]);
    }

    // Iterate through each of them to find a suitable one
    for (uint32_t i = 0; i < available_devices.size(); i++) {
        // Just take the first suitable one
        if (is_suitable_gpu(available_devices[i], surface, device_extensions)) {
            return available_devices[i];
        }
    }

    // Otherwise, we didn't find any
    logger.fatalc(GPU::channel, "Could not find a supported GPU.");
}





/***** GPU CLASS *****/
/* Constructor for the GPU class, which takes a Vulkan instance, the target surface and a list of required extensions to enable on the GPU. */
GPU::GPU(const Instance& instance, const Surface& surface, const Tools::Array<uint32_t>& queue_counts, const Tools::Array<const char*>& extensions) :
    instance(instance),
    surface(surface),
    vk_extensions(extensions)
{
    logger.logc(Verbosity::important, GPU::channel, "Initializing...");

    // Then, we move to getting a phsysical device that supports our desired properties
    logger.logc(Verbosity::details, GPU::channel, "Choosing physical device...");

    // Start by selecting a proper one
    this->vk_physical_device = select_gpu(this->instance, this->surface, this->vk_extensions);

    // Next, get some of its properties, like the name & queue info
    vkGetPhysicalDeviceProperties(this->vk_physical_device, &this->vk_physical_device_properties);
    this->vk_queue_info = QueueInfo(this->vk_physical_device, this->surface);
    this->vk_swapchain_info = SwapchainInfo(this->vk_physical_device, this->surface);
    logger.logc(Verbosity::important, GPU::channel, "Selected GPU: '", this->vk_physical_device_properties.deviceName, '\'');

    // Set some optional features
    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(vk_physical_device, &supported_features);
    this->vk_supports_anisotropy = supported_features.samplerAnisotropy;



    // With the physical device, create the logical device
    logger.logc(Verbosity::details, GPU::channel, "Initializing logical device...");

    // Prepare the list of queue priorities (all set to 1.0f for now)
    Tools::Array<Tools::Array<float>> queue_priorities(queue_counts.size());
    for (uint32_t i = 0; i < queue_counts.size(); i++) {
        queue_priorities.push_back(Tools::Array<float>(1.0f, queue_counts[i]));
    }

    // Collect the qeuues we want to use for this device(s) by creating a list of queue create infos.
    Tools::Array<VkDeviceQueueCreateInfo> queue_infos;
    populate_queue_infos(queue_infos, this->vk_queue_info, queue_counts, queue_priorities);

    // Next, populate the list of features we like from our device.
    VkPhysicalDeviceFeatures device_features;
    populate_device_features(device_features, this->vk_supports_anisotropy);

    // Then, use the queue indices and the features to populate the create info for the device itself
    VkDeviceCreateInfo device_info;
    populate_device_info(device_info, queue_infos, device_features, this->vk_extensions);

    // With the device info ready, create it
    VkResult vk_result;
    if ((vk_result = vkCreateDevice(this->vk_physical_device, &device_info, nullptr, &this->vk_device)) != VK_SUCCESS) {
        logger.fatalc(GPU::channel, "Could not create the logical device: ", vk_error_map[vk_result]);
    }
    


    // For debugging purposes, print the extensions enabled
    for (uint32_t i = 0; i < extensions.size(); i++) {
        logger.logc(Verbosity::debug, GPU::channel, "Enabled extension '", std::string(extensions[i]), '\'');
    }



    // As a quick next step, fetch the relevant device queues
    logger.logc(Verbosity::details, GPU::channel, "Fetching device queues...");
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        this->vk_queues[i].resize(queue_counts[i]);
        for (uint32_t j = 0; j < queue_counts[i]; j++) {
            vkGetDeviceQueue(this->vk_device, this->vk_queue_info[(QueueType) i], j, &this->vk_queues[i][j]);
        }
    }

    // D0ne
    logger.logc(Verbosity::important, GPU::channel, "Init success.");
}

/* Copy constructor for the GPU class. */
GPU::GPU(const GPU& other) :
    instance(other.instance),
    surface(other.surface),
    vk_physical_device(other.vk_physical_device),
    vk_physical_device_properties(other.vk_physical_device_properties),
    vk_queue_info(other.vk_queue_info),
    vk_swapchain_info(other.vk_swapchain_info),
    vk_supports_anisotropy(other.vk_supports_anisotropy),
    vk_extensions(other.vk_extensions)
{
    logger.logc(Verbosity::debug, GPU::channel, "Copying...");

    // The physical device can be copied literally; just create a new instance of the logical device
    // But before we do, we prepare a new list of queue counts & priorities
    Tools::Array<uint32_t> queue_counts(QueueInfo::n_queues);
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        queue_counts.push_back(this->vk_queues[i].size());
    }
    Tools::Array<Tools::Array<float>> queue_priorities(queue_counts.size());
    for (uint32_t i = 0; i < queue_counts.size(); i++) {
        queue_priorities.push_back(Tools::Array<float>(1.0f, queue_counts[i]));
    }

    // Collect the qeuues we want to use for this device(s) by creating a list of queue create infos.
    Tools::Array<VkDeviceQueueCreateInfo> queue_infos;
    populate_queue_infos(queue_infos, this->vk_queue_info, queue_counts, queue_priorities);

    // Next, populate the list of features we like from our device.
    VkPhysicalDeviceFeatures device_features;
    populate_device_features(device_features, this->vk_supports_anisotropy);

    // Then, use the queue indices and the features to populate the create info for the device itself
    VkDeviceCreateInfo device_info;
    populate_device_info(device_info, queue_infos, device_features, this->vk_extensions);

    // With the device info ready, create it
    VkResult vk_result;
    if ((vk_result = vkCreateDevice(this->vk_physical_device, &device_info, nullptr, &this->vk_device)) != VK_SUCCESS) {
        logger.fatalc(GPU::channel, "Could not create the logical device: " + vk_error_map[vk_result]);
    }

    // Re-fetch the relevant device queues to make sure they are accurate
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        this->vk_queues[i].resize(queue_counts[i]);
        for (uint32_t j = 0; j < queue_counts[i]; j++) {
            vkGetDeviceQueue(this->vk_device, this->vk_queue_info[(QueueType) i], j, &this->vk_queues[i][j]);
        }
    }

    // D0ne
    logger.logc(Verbosity::debug, GPU::channel, "Copy success.");
}

/* Move constructor for the GPU class. */
GPU::GPU(GPU&& other) :
    instance(other.instance),
    surface(other.surface),
    vk_physical_device(other.vk_physical_device),
    vk_physical_device_properties(other.vk_physical_device_properties),
    vk_queue_info(other.vk_queue_info),
    vk_swapchain_info(other.vk_swapchain_info),
    vk_supports_anisotropy(other.vk_supports_anisotropy),
    vk_device(other.vk_device),
    vk_extensions(other.vk_extensions)
{
    // Also steal the other's queues
    for (uint32_t i = 0; i < QueueInfo::n_queues; i++) {
        this->vk_queues[i] = std::move(other.vk_queues[i]);
    }

    // Set the device to a nullptr so the now useless object doesn't destruct it
    other.vk_device = nullptr;
}

/* Destructor for the GPU class. */
GPU::~GPU() {
    logger.logc(Verbosity::important, GPU::channel, "Cleaning...");

    // Destroy them in reverse order: first, the logical device
    if (this->vk_device != nullptr) {
        logger.logc(Verbosity::details, GPU::channel, "Cleaning logical device...");
        vkDestroyDevice(this->vk_device, nullptr);
    }

    // Physical devices need no destructing

    logger.logc(Verbosity::important, GPU::channel, "Cleaned.");
}



/* Refreshes the swapchain info, based on the new surface. */
void GPU::refresh_swapchain_info() {
    // Simply generate a new one based on the physical device and the surface
    this->vk_swapchain_info = SwapchainInfo(this->vk_physical_device, this->surface.surface());
}



/* Swap operator for the GPU class. */
void Rendering::swap(GPU& g1, GPU& g2) {
    #ifndef NDEBUG
    // Check if the instances are actually the same
    if (g1.instance != g2.instance) { logger.fatalc(GPU::channel, "Cannot swap gpus with different instances"); }
    // And also check the surfaces
    if (g1.surface != g2.surface) { logger.fatalc(GPU::channel, "Cannot swap gpus with different surfaces"); }
    #endif

    // Swap all operators
    using std::swap;

    swap(g1.vk_physical_device, g2.vk_physical_device);
    swap(g1.vk_physical_device_properties, g2.vk_physical_device_properties);
    swap(g1.vk_queue_info, g2.vk_queue_info);
    swap(g1.vk_swapchain_info, g2.vk_swapchain_info);
    swap(g1.vk_device, g2.vk_device);
    swap(g1.vk_extensions, g2.vk_extensions);
    swap(g1.vk_queues, g2.vk_queues);
}
