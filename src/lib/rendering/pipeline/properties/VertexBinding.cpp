/* VERTEX BINDING.cpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:16:04
 * Last edited:
 *   12/09/2021, 14:16:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexBinding class, which groups together the
 *   information needed to describe a vertex buffer binding.
**/

#include "VertexBinding.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkVertexInputBindingDescription object. */
static void populate_binding_description(VkVertexInputBindingDescription& binding_description, uint32_t bind_index, uint32_t vertex_size) {
    // Set to default
    binding_description = {};

    // Set the binding index
    binding_description.binding = bind_index;

    // Set the description of the vertices
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    binding_description.stride = vertex_size;
}





/***** VERTEXBINDING CLASS *****/
/* Constructor for the VertexBinding class, which takes the binding index and the size of each Vertex object in this buffer. */
VertexBinding::VertexBinding(uint32_t bind_index, uint32_t vertex_size) {
    // Simply populate the binding description
    populate_binding_description(this->vk_binding_description, bind_index, vertex_size);
}
