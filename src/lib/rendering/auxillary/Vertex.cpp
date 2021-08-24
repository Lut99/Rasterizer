/* VERTEX.cpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:37:11
 * Last edited:
 *   8/1/2021, 3:48:20 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Describes how a single Vertex looks like. Also contains code to create
 *   Vulkan descriptions for itself.
**/

#include "Vertex.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkVertexInputBindingDescription struct. */
static void populate_input_binding_description(VkVertexInputBindingDescription& vk_input_binding_description) {
    // Set to default
    vk_input_binding_description = {};
    
    // Set the index to bind to in the shader
    vk_input_binding_description.binding = 0;

    // Set the stride, i.e., the size of each element
    vk_input_binding_description.stride = sizeof(Vertex);

    // Finally, set the input rate. We do vertex rendering, so set to vertex.
    vk_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

/* Populates the given VkVertexInputAttributeDescription struct as if it was to describe 3D positions. */
static void populate_input_attribute_description_position(VkVertexInputAttributeDescription& vk_input_attribute_description) {
    // Set to default
    vk_input_attribute_description = {};

    // Set the binding and the index in the binding
    vk_input_attribute_description.binding = 0;
    vk_input_attribute_description.location = 0;

    // Set the format of the position
    vk_input_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;

    // Set the offset of the value in each vertex object
    vk_input_attribute_description.offset = offsetof(Vertex, pos);
}

/* Populates the given VkVertexInputAttributeDescription struct as if it was to describe RGB colors. */
static void populate_input_attribute_description_colour(VkVertexInputAttributeDescription& vk_input_attribute_description) {
    // Set to default
    vk_input_attribute_description = {};

    // Set the binding and the index in the binding
    vk_input_attribute_description.binding = 0;
    vk_input_attribute_description.location = 1;

    // Set the format of the position
    vk_input_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;

    // Set the offset of the value in each vertex object
    vk_input_attribute_description.offset = offsetof(Vertex, colour);
}





/***** VERTEX STRUCT *****/
/* Default constructor for the Vertex struct. */
Vertex::Vertex() :
    pos(0.0, 0.0, 0.0),
    colour(0.0, 0.0, 0.0)
{}

/* Constructor for the Vertex struct, which takes the position and colour. */
Vertex::Vertex(const glm::vec3& pos, const glm::vec3& colour) :
    pos(pos),
    colour(colour)
{}



/* Static function that returns the binding description for the vertex. */
VkVertexInputBindingDescription Vertex::input_binding_description() {
    // Create and populate
    VkVertexInputBindingDescription result;
    populate_input_binding_description(result);
}

/* Static function that returns the attribute descriptions for the vertex. */
Tools::Array<VkVertexInputAttributeDescription> Vertex::input_attribute_descriptions() {
    // Create and populate
    Tools::Array<VkVertexInputAttributeDescription> result;
    result.resize(2);
    populate_input_attribute_description_position(result[0]);
    populate_input_attribute_description_colour(result[1]);
}
