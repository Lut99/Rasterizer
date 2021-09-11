/* VERTEX STATE.cpp
 *   by Lut99
 *
 * Created:
 *   30/07/2021, 11:53:47
 * Last edited:
 *   8/1/2021, 3:39:34 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexState class, which manages an
 *   VkPipelineVertexInputStateDescription struct and associated pointers.
**/

#include "../auxillary/ErrorCodes.hpp"

#include "VertexState.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineVertexInputStateCreateInfo struct. */
static void populate_vertex_state_info(VkPipelineVertexInputStateCreateInfo& vertex_state_info, VkVertexInputBindingDescription* vk_binding_description, const Tools::Array<VkVertexInputAttributeDescription>& vk_attribute_descriptions) {
    // Set to default
    vertex_state_info = {};
    vertex_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Set the bindings
    vertex_state_info.vertexBindingDescriptionCount = 1;
    vertex_state_info.pVertexBindingDescriptions = vk_binding_description;
    
    // Set the attributes
    vertex_state_info.vertexAttributeDescriptionCount = vk_attribute_descriptions.size();
    vertex_state_info.pVertexAttributeDescriptions = vk_attribute_descriptions.rdata();
}





/***** VERTEXSTATE CLASS *****/
/* Constructor for the VertexState class, which takes a VkVertexInputBindingDescription and a list of VkVertexInputAttributeDescriptions. */
VertexState::VertexState(const VkVertexInputBindingDescription& vk_vertex_binding, const Tools::Array<VkVertexInputAttributeDescription>& vk_vertex_attributes) {
    // Store the vertex binding & attributes safely
    this->vk_vertex_binding = new VkVertexInputBindingDescription(vk_vertex_binding);
    this->vk_vertex_attributes = Tools::Array<VkVertexInputAttributeDescription>(vk_vertex_attributes);

    // Populate the state info
    populate_vertex_state_info(this->vk_vertex_state_info, this->vk_vertex_binding, this->vk_vertex_attributes);
}

/* Copy constructor for the VertexState class. */
VertexState::VertexState(const VertexState& other) {
    // Copy the binding & attributes
    this->vk_vertex_binding = new VkVertexInputBindingDescription(*other.vk_vertex_binding);
    this->vk_vertex_attributes = Tools::Array<VkVertexInputAttributeDescription>(other.vk_vertex_attributes);

    // Re-populate our vertex state info
    populate_vertex_state_info(this->vk_vertex_state_info, this->vk_vertex_binding, this->vk_vertex_attributes);
}

/* Move constructor for the VertexState class. */
VertexState::VertexState(VertexState&& other) :
    vk_vertex_state_info(other.vk_vertex_state_info),
    vk_vertex_binding(other.vk_vertex_binding),
    vk_vertex_attributes(std::move(other.vk_vertex_attributes))
{
    other.vk_vertex_binding = nullptr;
}

/* Destructor for the VertexState class. */
VertexState::~VertexState() {
    // Only delete the vertex binding if needed
    if (this->vk_vertex_binding != nullptr) {
        delete this->vk_vertex_binding;
    }
}



/* Swap operator for the VertexState class. */
void Rendering::swap(VertexState& vs1, VertexState& vs2) {
    using std::swap;

    swap(vs1.vk_vertex_state_info, vs2.vk_vertex_state_info);
    swap(vs1.vk_vertex_binding, vs2.vk_vertex_binding);
    swap(vs1.vk_vertex_attributes, vs2.vk_vertex_attributes);
}
