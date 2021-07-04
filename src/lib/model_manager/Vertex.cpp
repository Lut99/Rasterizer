/* VERTEX.cpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:37:11
 * Last edited:
 *   01/07/2021, 14:37:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Describes how a single Vertex looks like. Also contains code to create
 *   Vulkan descriptions for itself.
**/

#include "tools/CppDebugger.hpp"

#include "Vertex.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Models;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkVertexInputBindingDescription struct. */
static void populate_input_binding_description(VkVertexInputBindingDescription& vk_input_binding_description) {
    DENTER("populate_input_binding_description");

    // Set to default
    vk_input_binding_description = {};
    
    // Set the index to bind to in the shader
    vk_input_binding_description.binding = 0;

    // Set the stride, i.e., the size of each element
    vk_input_binding_description.stride = sizeof(Vertex);

    // Finally, set the input rate. We do vertex rendering, so set to vertex.
    vk_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Done
    DRETURN;
}

/* Populates the given VkVertexInputAttributeDescription struct as if it was to describe 3D positions. */
static void populate_input_attribute_description_position(VkVertexInputAttributeDescription& vk_input_attribute_description) {
    DENTER("populate_input_attribute_description_position");

    // Set to default
    vk_input_attribute_description = {};

    // Set the binding and the index in the binding
    vk_input_attribute_description.binding = 0;
    vk_input_attribute_description.location = 0;

    // Set the format of the position
    vk_input_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;

    // Set the offset of the value in each vertex object
    vk_input_attribute_description.offset = offsetof(Vertex, pos);

    // Done
    DRETURN;
}

/* Populates the given VkVertexInputAttributeDescription struct as if it was to describe RGB colors. */
static void populate_input_attribute_description_colour(VkVertexInputAttributeDescription& vk_input_attribute_description) {
    DENTER("populate_input_attribute_description_colour");

    // Set to default
    vk_input_attribute_description = {};

    // Set the binding and the index in the binding
    vk_input_attribute_description.binding = 0;
    vk_input_attribute_description.location = 1;

    // Set the format of the position
    vk_input_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;

    // Set the offset of the value in each vertex object
    vk_input_attribute_description.offset = offsetof(Vertex, colour);

    // Done
    DRETURN;
}





/***** VERTEX STRUCT *****/
/* Static function that returns the binding description for the vertex. */
VkVertexInputBindingDescription Vertex::input_binding_description() {
    DENTER("Models::Vertex::input_binding_descriptions");

    // Create and populate
    VkVertexInputBindingDescription result;
    populate_input_binding_description(result);

    // Done
    DRETURN result;
}

/* Static function that returns the attribute descriptions for the vertex. */
Tools::Array<VkVertexInputAttributeDescription> Vertex::input_attribute_descriptions() {
    DENTER("Models::Vertex::input_attribute_descriptions");

    // Create and populate
    Tools::Array<VkVertexInputAttributeDescription> result;
    result.resize(2);
    populate_input_attribute_description_position(result[0]);
    populate_input_attribute_description_colour(result[1]);

    // Done
    DRETURN result;
}
