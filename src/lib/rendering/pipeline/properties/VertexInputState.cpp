/* VERTEX INPUT STATE.cpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:09:38
 * Last edited:
 *   12/09/2021, 14:09:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexInputState class, which describes how the vertices
 *   given to a pipeline look like.
**/

#include "VertexInputState.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** VERTEXINPUTSTATE CLASS *****/
/* Default constructor for the VertexInputState. */
VertexInputState::VertexInputState() :
    VertexInputState({}, {})
{}

/* Constructor for the VertexInputState, which takes a list of VertexInputBindings and a list of VertexAttributes that describe how the vertices look like. */
VertexInputState::VertexInputState(const Tools::Array<VertexBinding>& vertex_bindings, const Tools::Array<VertexAttribute>& vertex_attributes) :
    vertex_bindings(vertex_bindings),
    vertex_attributes(vertex_attributes)
{
    printf("Initializing VertexInputState\n");
}



/* Casts the internal list of VertexBindings to VkVertexInputBindingDescription structs. */
Tools::Array<VkVertexInputBindingDescription> VertexInputState::get_bindings() const {
    // Create the array with the descriptions
    Tools::Array<VkVertexInputBindingDescription> result(this->vertex_bindings.size());
    for (uint32_t i = 0; i < this->vertex_bindings.size(); i++) {
        // Add the struct as a default struct
        result.push_back({});

        // Set the input rate, which we will fix to vertices for now
        result[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Set the properties from the VertexBinding
        result[i].binding = this->vertex_bindings[i].bind_index;
        result[i].stride = this->vertex_bindings[i].vertex_size;
    }

    // Done, return
    return result;
}

/* Casts the internal list of VertexAttributes to VkVertexInputAttributeDescription structs. */
Tools::Array<VkVertexInputAttributeDescription> VertexInputState::get_attributes() const {
    // Create the array with the descriptions
    Tools::Array<VkVertexInputAttributeDescription> result(this->vertex_attributes.size());
    for (uint32_t i = 0; i < this->vertex_attributes.size(); i++) {
        // Add the struct as a default struct
        result.push_back({});

        // Set the binding & location for this attribute
        result[i].binding = this->vertex_attributes[i].bind_index;
        result[i].location = this->vertex_attributes[i].location;

        // Set the offset and format for the attribute
        result[i].offset = this->vertex_attributes[i].offset;
        result[i].format = this->vertex_attributes[i].format;
    }

    // Done, return
    return result;
}

/* Given a list of vertex input binding descriptions and input attribute descriptions, creates a new VkPipelineVertexInputStateCreateInfo struct. */
VkPipelineVertexInputStateCreateInfo VertexInputState::get_info(const Tools::Array<VkVertexInputBindingDescription>& vk_bindings, const Tools::Array<VkVertexInputAttributeDescription>& vk_attributes) const {
    // Create the struct as default
    VkPipelineVertexInputStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Set the bindings
    info.vertexBindingDescriptionCount = vk_bindings.size();
    info.pVertexBindingDescriptions = vk_bindings.rdata();
    
    // Set the attributes
    info.vertexAttributeDescriptionCount = vk_attributes.size();
    info.pVertexAttributeDescriptions = vk_attributes.rdata();

    // Done
    return info;
}
