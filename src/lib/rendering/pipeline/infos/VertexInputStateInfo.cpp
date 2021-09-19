/* VERTEX INPUT STATE INFO.cpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 14:43:06
 * Last edited:
 *   19/09/2021, 14:43:06
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexInputStateInfo class, which bundles a
 *   VkPipelineVertexInputStateCreateInfo struct with the memory needed for
 *   it to exist.
**/

#include "VertexInputStateInfo.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkPipelineVertexInputStateCreateInfo struct with the given bindings and attributes. */
static void populate_vertex_input_state_info(VkPipelineVertexInputStateCreateInfo& vertex_input_state_info, const VkVertexInputBindingDescription* vk_bindings, uint32_t vk_bindings_size, const VkVertexInputAttributeDescription* vk_attributes, uint32_t vk_attributes_size) {
    // Set to deafult
    vertex_input_state_info = {};
    vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Set the bindings
    vertex_input_state_info.vertexBindingDescriptionCount = vk_bindings_size;
    vertex_input_state_info.pVertexBindingDescriptions = vk_bindings;
    
    // Set the attributes
    vertex_input_state_info.vertexAttributeDescriptionCount = vk_attributes_size;
    vertex_input_state_info.pVertexAttributeDescriptions = vk_attributes;
}





/***** VERTEXINPUTSTATEINFO CLASS *****/
/* Constructor for the VertexInputStateInfo class, which takes a normal VertexInputState object to initialize itself with. */
VertexInputStateInfo::VertexInputStateInfo(const Rendering::VertexInputState& vertex_input_state) {
    // Start by casting the list of input bindings
    this->vk_bindings_size = vertex_input_state.vertex_bindings.size();
    this->vk_bindings = new VkVertexInputBindingDescription[this->vk_bindings_size];
    for (uint32_t i = 0; i < vertex_input_state.vertex_bindings.size(); i++) {
        // Initialize the struct to default
        this->vk_bindings[i] = {};

        // Copy the attributes of the struct over
        this->vk_bindings[i].binding = vertex_input_state.vertex_bindings[i].bind_index;
        this->vk_bindings[i].stride = vertex_input_state.vertex_bindings[i].vertex_size;

        // Also set the hardcoded input rate
        this->vk_bindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    // Next, cast the list of attributes too
    this->vk_attributes_size = vertex_input_state.vertex_attributes.size();
    this->vk_attributes = new VkVertexInputAttributeDescription[this->vk_attributes_size];
    for (uint32_t i = 0; i < vertex_input_state.vertex_attributes.size(); i++) {
        // Initialize the struct to default
        this->vk_attributes[i] = {};

        // Set the binding & location for this attribute
        this->vk_attributes[i].binding = vertex_input_state.vertex_attributes[i].bind_index;
        this->vk_attributes[i].location = vertex_input_state.vertex_attributes[i].location;

        // Set the offset and format for the attribute
        this->vk_attributes[i].offset = vertex_input_state.vertex_attributes[i].offset;
        this->vk_attributes[i].format = vertex_input_state.vertex_attributes[i].format;
    }

    // Use that to populate the main struct
    populate_vertex_input_state_info(this->vk_vertex_input_state_info, this->vk_bindings, this->vk_bindings_size, this->vk_attributes, this->vk_attributes_size);
}

/* Copy constructor for the VertexInputStateInfo class. */
VertexInputStateInfo::VertexInputStateInfo(const VertexInputStateInfo& other) :
    vk_bindings_size(other.vk_bindings_size),
    vk_attributes_size(other.vk_attributes_size),
    vk_vertex_input_state_info(other.vk_vertex_input_state_info)
{
    // First, copy the bindings
    this->vk_bindings = new VkVertexInputBindingDescription[this->vk_bindings_size];
    memcpy(this->vk_bindings, other.vk_bindings, this->vk_bindings_size * sizeof(VkVertexInputBindingDescription));

    // Next, copy the attributes
    this->vk_attributes = new VkVertexInputAttributeDescription[this->vk_attributes_size];
    memcpy(this->vk_attributes, other.vk_attributes, this->vk_attributes_size * sizeof(VkVertexInputAttributeDescription));

    // Finally, remap the pointers in the state info
    this->vk_vertex_input_state_info.pVertexBindingDescriptions = this->vk_bindings;
    this->vk_vertex_input_state_info.pVertexAttributeDescriptions = this->vk_attributes;
}

/* Move constructor for the VertexInputStateInfo class. */
VertexInputStateInfo::VertexInputStateInfo(VertexInputStateInfo&& other) :
    vk_bindings(other.vk_bindings),
    vk_bindings_size(other.vk_bindings_size),

    vk_attributes(other.vk_attributes),
    vk_attributes_size(other.vk_attributes_size),

    vk_vertex_input_state_info(other.vk_vertex_input_state_info)
{
    // Prevent deallocation from the other
    other.vk_bindings = nullptr;
    other.vk_attributes = nullptr;
}

/* Destructor for the VertexInputStateInfo class. */
VertexInputStateInfo::~VertexInputStateInfo() {
    if (this->vk_attributes != nullptr) {
        delete[] this->vk_attributes;
    }
    if (this->vk_bindings != nullptr) {
        delete[] this->vk_bindings;
    }
}



/* Swap operator for the VertexInputStateInfo class. */
void Rendering::swap(VertexInputStateInfo& visi1, VertexInputStateInfo& visi2) {
    using std::swap;

    swap(visi1.vk_bindings, visi2.vk_bindings);
    swap(visi1.vk_bindings_size, visi2.vk_bindings_size);

    swap(visi1.vk_attributes, visi2.vk_attributes);
    swap(visi1.vk_attributes_size, visi2.vk_attributes_size);

    swap(visi1.vk_vertex_input_state_info, visi2.vk_vertex_input_state_info);
}
