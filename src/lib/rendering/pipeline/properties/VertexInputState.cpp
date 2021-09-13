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


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineVertexInputStateCreateInfo struct. */
static void populate_vertex_input_state(VkPipelineVertexInputStateCreateInfo& vertex_state_info, const Tools::Array<VkVertexInputBindingDescription>& vk_binding_descriptions, const Tools::Array<VkVertexInputAttributeDescription>& vk_attribute_descriptions) {
    // Set to default
    vertex_state_info = {};
    vertex_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Set the bindings
    vertex_state_info.vertexBindingDescriptionCount = vk_binding_descriptions.size();
    vertex_state_info.pVertexBindingDescriptions = vk_binding_descriptions.rdata();
    
    // Set the attributes
    vertex_state_info.vertexAttributeDescriptionCount = vk_attribute_descriptions.size();
    vertex_state_info.pVertexAttributeDescriptions = vk_attribute_descriptions.rdata();
}





/***** VERTEXINPUTSTATE CLASS *****/
/* Constructor for the VertexInputState, which takes a list of VertexInputBindings and a list of VertexAttributes that describe how the vertices look like. */
VertexInputState::VertexInputState(const Tools::Array<VertexBinding>& vertex_bindings, const Tools::Array<VertexAttribute>& vertex_attributes) :
    vertex_bindings(vertex_bindings),

    vertex_attributes(vertex_attributes)
{
    // Create the casted list of bindings
    this->vk_vertex_bindings.reserve(this->vertex_bindings.size());
    for (uint32_t i = 0; i < this->vertex_bindings.size(); i++) {
        this->vk_vertex_bindings.push_back(this->vertex_bindings[i].description());
    }
    
    // Create the casted list of attributes
    this->vk_vertex_attributes.reserve(this->vertex_attributes.size());
    for (uint32_t i = 0; i < this->vertex_attributes.size(); i++) {
        this->vk_vertex_attributes.push_back(this->vertex_attributes[i].description());
    }
    
    // Create the state info itself
    populate_vertex_input_state(this->vk_vertex_input_state, this->vk_vertex_bindings, this->vk_vertex_attributes);
}

/* Copy constructor for the VertexInputState class. */
VertexInputState::VertexInputState(const VertexInputState& other) :
    vertex_bindings(other.vertex_bindings),

    vertex_attributes(other.vertex_attributes)
{
    // Re-create the casted list of bindings
    this->vk_vertex_bindings.reserve(this->vertex_bindings.size());
    for (uint32_t i = 0; i < this->vertex_bindings.size(); i++) {
        this->vk_vertex_bindings.push_back(this->vertex_bindings[i].description());
    }
    
    // Re-create the casted list of attributes
    this->vk_vertex_attributes.reserve(this->vertex_attributes.size());
    for (uint32_t i = 0; i < this->vertex_attributes.size(); i++) {
        this->vk_vertex_attributes.push_back(this->vertex_attributes[i].description());
    }

    // Re-create the state info itself
    populate_vertex_input_state(this->vk_vertex_input_state, this->vk_vertex_bindings, this->vk_vertex_attributes);
}

/* Move constructor for the VertexInputState class. */
VertexInputState::VertexInputState(VertexInputState&& other) :
    vertex_bindings(std::move(other.vertex_bindings)),
    vk_vertex_bindings(std::move(other.vk_vertex_bindings)),

    vertex_attributes(std::move(other.vertex_attributes)),
    vk_vertex_attributes(std::move(other.vk_vertex_attributes))
{
    // Only re-create the input state itself
    populate_vertex_input_state(this->vk_vertex_input_state, this->vk_vertex_bindings, this->vk_vertex_attributes);
}

/* Destructor for the VertexInputState class. */
VertexInputState::~VertexInputState() {}



/* Swap operator for the VertexInputState class. */
void Rendering::swap(VertexInputState& vis1, VertexInputState& vis2) {
    using std::swap;

    swap(vis1.vertex_bindings, vis2.vertex_bindings);
    swap(vis1.vk_vertex_bindings, vis2.vk_vertex_bindings);

    swap(vis1.vertex_attributes, vis2.vertex_attributes);
    swap(vis1.vk_vertex_attributes, vis2.vk_vertex_attributes);

    swap(vis1.vk_vertex_input_state, vis2.vk_vertex_input_state);
}
