/* VERTEX ATTRIBUTE.cpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:21:37
 * Last edited:
 *   12/09/2021, 14:21:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexAttribute class, which describes how a single
 *   attribute (i.e., field) of the vertices given to a certain pipeline
 *   looks like.
**/

#include "VertexAttribute.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkVertexInputAttributeDescription object. */
static void populate_attribute_description(VkVertexInputAttributeDescription& attribute_description, uint32_t bind_index, uint32_t location, uint32_t offset, VkFormat vk_format) {
    // Set to default
    attribute_description = {};

    // Set the binding index & the location within the binding
    attribute_description.binding = bind_index;
    attribute_description.location = location;

    // Set the description of the attribute itself
    attribute_description.offset = offset;
    attribute_description.format = vk_format;
}





/***** VERTEXATTRIBUTE CLASS *****/
/* Constructor for the VertexAttribute class, which takes the binding index for the buffer, the location index of this attribute, the offset of the attribute in the Vertex and the VkFormat describing the size of the attribute. */
VertexAttribute::VertexAttribute(uint32_t bind_index, uint32_t location, uint32_t offset, VkFormat vk_format) {
    // Simply populate the attribute description
    populate_attribute_description(this->vk_attribute_description, bind_index, location, offset, vk_format);
}
