/* MEMORY OBJECT.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 15:34:23
 * Last edited:
 *   16/08/2021, 15:34:23
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MemoryObject class, which is the common ancestor of both
 *   Buffers and Images in the MemoryPool.
**/

#ifndef RENDERING_MEMORY_OBJECT_HPP
#define RENDERING_MEMORY_OBJECT_HPP

#include <string>
#include "../gpu/GPU.hpp"

namespace Rasterizer::Rendering {
    /* The possible types of the MemoryObject class. */
    enum class MemoryObjectType {
        /* No type defined. */
        none = 0,

        /* The MemoryObject is actually a buffer. */
        buffer = 1,
        /* The MemoryObject is actually an image. */
        image = 2
    };

    /* Maps MemoryObjectType values to readable strings. */
    static const std::string memory_object_type_names[] = {
        "none",

        "buffer",
        "image"
    };



    /* Forward declaration of the MemoryPool class. */
    class MemoryPool;



    /* The MemoryObject class, which is the parent of the Buffer and Image classes. */
    class MemoryObject {
    public:
        /* Reference to the pool where this buffer was allocated. */
        const Rendering::MemoryPool& pool;
        /* The GPU where the object lives. */
        const Rendering::GPU& gpu;

    protected:
        /* The type of this object. */
        MemoryObjectType type;
        /* The offset of this object in the large memory pool. */
        VkDeviceSize object_offset;

        /* Declare the MemoryPool as friend. */
        friend class MemoryPool;
    
    protected:
        /* Constructor for the MemoryObject class, which takes the pool with which it was allocated, the type of its derived class and the offset of its memory in the large buffer. */
        MemoryObject(const MemoryPool& pool, MemoryObjectType type, VkDeviceSize offset);
    
    public:
        /* Virtual destructor for the MemoryObject class. */
        ~MemoryObject() = default;

    };

}

#endif
