/* VULKAN SWAP.cpp
 *   by Lut99
 *
 * Created:
 *   13/09/2021, 21:46:45
 * Last edited:
 *   13/09/2021, 21:46:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <utility>

#include "VulkanSwap.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** LIBRARY FUNCTIONS *****/
/* Swaps two VkOffset2D structs. */
void Rendering::swap(VkOffset2D& o1, VkOffset2D& o2) {
    using std::swap;

    swap(o1.x, o2.x);
    swap(o1.y, o2.y);
}

/* Swaps two VkExtent2D structs. */
void Rendering::swap(VkExtent2D& e1, VkExtent2D& e2) {
    using std::swap;

    swap(e1.width, e2.width);
    swap(e1.height, e2.height);
}



/* Swaps two VkViewport structs. */
void Rendering::swap(VkViewport& v1, VkViewport& v2) {
    using std::swap;

    swap(v1.x, v2.x);
    swap(v1.y, v2.y);
    swap(v1.width, v2.width);
    swap(v1.height, v2.height);
    swap(v1.minDepth, v2.minDepth);
    swap(v1.maxDepth, v2.maxDepth);
}

/* Swaps two VkRect2D structs. */
void Rendering::swap(VkRect2D& r1, VkRect2D& r2) {
    using std::swap;

    swap(r1.offset, r2.offset);
    swap(r1.extent, r2.extent);
}

/* Swaps two VkPipelineViewportStateCreateInfo structs. */
void Rendering::swap(VkPipelineViewportStateCreateInfo& pvs1, VkPipelineViewportStateCreateInfo& pvs2) {
    using std::swap;

    swap(pvs1.sType, pvs2.sType);

    swap(pvs1.viewportCount, pvs2.viewportCount);
    swap(pvs1.pViewports, pvs2.pViewports);

    swap(pvs1.scissorCount, pvs2.scissorCount);
    swap(pvs1.pScissors, pvs2.pScissors);

    swap(pvs1.flags, pvs2.flags);
    swap(pvs1.pNext, pvs2.pNext);
}
