/* MEMORY OBJECT.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 15:39:13
 * Last edited:
 *   16/08/2021, 15:39:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the MemoryObject class, which is the common ancestor of both
 *   Buffers and Images in the MemoryPool.
**/

#include "MemoryPool.hpp"
#include "MemoryObject.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** MEMORYOBJECT CLASS *****/
/* Constructor for the MemoryObject class, which takes the pool with which it was allocated, the type of its derived class and the offset of its memory in the large buffer. */
MemoryObject::MemoryObject(const MemoryPool& pool, MemoryObjectType type, VkDeviceSize offset) :
    pool(pool),
    gpu(pool.gpu),
    type(type),
    object_offset(offset)
{}
