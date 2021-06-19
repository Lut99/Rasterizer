/* MEMORY HANDLE.hpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:21:46
 * Last edited:
 *   19/06/2021, 12:21:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Describes the MemoryHandle datatype used in the MemoryPool.
**/

#ifndef VULKAN_MEMORY_HANDLE_HPP
#define VULKAN_MEMORY_HANDLE_HPP

#include <cstdint>

namespace Rasterizer::Vulkan {
    /* "Base" handle for all memory objects allocated in the pool. */
    using memory_h = uint32_t;
}

#endif
