/* DESCRIPTOR SET LAYOUT.cpp
 *   by Lut99
 *
 * Created:
 *   26/04/2021, 15:33:41
 * Last edited:
 *   25/05/2021, 18:14:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorSetLayout class, which describes the format for
 *   a single type of buffer.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "DescriptorSetLayout.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates given VkDescriptorSetLayoutBinding struct. */
static void populate_descriptor_set_binding(VkDescriptorSetLayoutBinding& descriptor_set_binding, uint32_t bind_index, VkDescriptorType descriptor_type, uint32_t n_descriptors, VkShaderStageFlags shader_stage) {
    // Set to default
    descriptor_set_binding = {};

    // Set the index of the binding, must be equal to the place in the shader
    descriptor_set_binding.binding = bind_index;
    
    // Set the type of the descriptor
    descriptor_set_binding.descriptorType = descriptor_type;
    
    // Set the number of descriptors to use
    descriptor_set_binding.descriptorCount = n_descriptors;
    
    // Set the stage flags
    descriptor_set_binding.stageFlags = shader_stage;
    
    // For now, we won't use the next field, as this is for multi-sampling and we don't use that yet
    descriptor_set_binding.pImmutableSamplers = nullptr;
}

/* Populates a given VkDescriptorSetLayoutCreateInfo struct. */
static void populate_descriptor_set_layout_info(VkDescriptorSetLayoutCreateInfo& descriptor_set_layout_info, const Tools::Array<VkDescriptorSetLayoutBinding>& vk_bindings) {
    // Initialize to default
    descriptor_set_layout_info = {};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    // Set the bindings to use
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(vk_bindings.size());
    descriptor_set_layout_info.pBindings = vk_bindings.rdata();
}





/***** DESCRIPTORSETLAYOUT CLASS *****/
/* Constructor for the DescriptorSetLayout class, which takes a gpu to bind the buffer to. Defining bindings and such will have to be done using individual functions. */
DescriptorSetLayout::DescriptorSetLayout(const GPU& gpu) :
    gpu(gpu),
    vk_descriptor_set_layout(nullptr)
{}

/* Copy constructor for the DescriptorSetLayout class, which is deleted. */
DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayout& other) :
    gpu(other.gpu),
    vk_descriptor_set_layout(other.vk_descriptor_set_layout),
    vk_bindings(other.vk_bindings)
{
    // If the descriptor set layout is not a nullptr, re-create it manually
    if (this->vk_descriptor_set_layout != nullptr) {
        this->vk_descriptor_set_layout = nullptr;
        this->finalize();
    }
}

/* Move constructor for the DescriptorSetLayout class. */
DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) :
    gpu(other.gpu),
    vk_descriptor_set_layout(other.vk_descriptor_set_layout),
    vk_bindings(other.vk_bindings)
{
    // Set the layout to a nullptr to avoid deallocation
    other.vk_descriptor_set_layout = nullptr;
}

/* Destructor for the DescriptorSetLayout class. */
DescriptorSetLayout::~DescriptorSetLayout() {
    if (this->vk_descriptor_set_layout != nullptr) {
        vkDestroyDescriptorSetLayout(this->gpu, this->vk_descriptor_set_layout, nullptr);
    }
}



/* Adds a binding to the DescriptorSetLayout; i.e., one type of resource that a single descriptorset will bind. Returns the binding index of this binding. */
uint32_t DescriptorSetLayout::add_binding(VkDescriptorType vk_descriptor_type, uint32_t n_descriptors, VkShaderStageFlags vk_shader_stage) {
    // If the layout has already been created, then crash
    if (this->vk_descriptor_set_layout != nullptr) {
        logger.fatalc(DescriptorSetLayout::channel, "Cannot add binding to DescriptorSetLayout after finalize() has been called.");
    }

    // Get the binding index for this binding
    uint32_t bind_index = static_cast<uint32_t>(this->vk_bindings.size());

    // We populate the struct
    VkDescriptorSetLayoutBinding binding;
    populate_descriptor_set_binding(binding, bind_index, vk_descriptor_type, n_descriptors, vk_shader_stage);

    // Add it to the internal list
    this->vk_bindings.push_back(binding);

    // Done, return the binding index
    return bind_index;
}

/* Finalizes the descriptor layout. Note that no more bindings can be added after this point. */
void DescriptorSetLayout::finalize() {
    // If the layout has already been created, then warning that nothing happens
    if (this->vk_descriptor_set_layout != nullptr) {
        logger.fatalc(DescriptorSetLayout::channel, "Calling finalize() more than once is useless.");
        return;
    }

    // Otherwise, prepare the bindings
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info;
    populate_descriptor_set_layout_info(descriptor_set_layout_info, this->vk_bindings);

    // Create the layout
    VkResult vk_result;
    if ((vk_result = vkCreateDescriptorSetLayout(this->gpu, &descriptor_set_layout_info, nullptr, &this->vk_descriptor_set_layout)) != VK_SUCCESS) {
        logger.fatalc(DescriptorSetLayout::channel, "Could not create descriptor set layout: ", vk_error_map[vk_result]);
    }

    // Do not clear the bindings, to keep the class copyable
}



/* Move assignment operator for the DescriptorSetLayout class. */
DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the DescriptorSetLayout class. */
void Rendering::swap(DescriptorSetLayout& dsl1, DescriptorSetLayout& dsl2) {
    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (dsl1.gpu != dsl2.gpu) { logger.fatalc(DescriptorSetLayout::channel, "Cannot swap descriptor set layouts with different GPUs"); }
    #endif

    using std::swap;

    // Swap all fields
    swap(dsl1.vk_descriptor_set_layout, dsl2.vk_descriptor_set_layout);
    swap(dsl1.vk_bindings, dsl2.vk_bindings);
}
