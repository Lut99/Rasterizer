/* MEMORY PROPERTIES.hpp
 *   by Lut99
 *
 * Created:
 *   8/1/2021, 4:36:43 PM
 * Last edited:
 *   8/1/2021, 4:40:55 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a map from all VkMemoryPropertyFlags to readable names.
**/

#ifndef RENDERING_MEMORY_PROPERTIES_HPP
#define RENDERING_MEMORY_PROPERTIES_HPP

#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>

/* Simple macro that sets the given enum to its string representation. */
#define MAP_VK_PROPERTY(PROPERTY) \
    { (PROPERTY), (#PROPERTY) }
    
namespace Rasterizer::Rendering {
    /* Defines a map from all VkMemoryPropertyFlags to readable names. */
    static std::unordered_map<VkMemoryPropertyFlagBits, std::string> vk_memory_property_map({
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_HOST_CACHED_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_PROTECTED_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        MAP_VK_PROPERTY(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    });
}

#endif
