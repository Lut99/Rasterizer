/* DESCRIPTOR SET.cpp
 *   by Lut99
 *
 * Created:
 *   19/06/2021, 12:49:22
 * Last edited:
 *   19/06/2021, 12:49:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorSet class, which wraps a pre-allocated
 *   VkDescriptorSet object. Only usable in the context of the
 *   DescriptorPool.
**/

#include "tools/CppDebugger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "DescriptorSet.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates a given VkDescriptorBufferInfo struct. */
static void populate_buffer_info(VkDescriptorBufferInfo& buffer_info, const Buffer& buffer) {
    DENTER("populate_buffer_info");

    // Set to default
    buffer_info = {};
    
    // Set the memory properties
    buffer_info.buffer = buffer.buffer();
    buffer_info.offset = 0; // Note that this offset is (probably) relative to the buffer itself, not the vk_memory object it was allocated with
    buffer_info.range = buffer.size();

    // Done
    DRETURN;
}

/* Populates a given VkDescriptorImageInfo struct. */
static void populate_image_info(VkDescriptorImageInfo& image_info, const VkImageView& image_view) {
    DENTER("populate_image_info");

    // Set to default
    image_info = {};
    
    // Set the image view associated with this info and the rest to nullptrs
    image_info.imageView = image_view;
    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    image_info.sampler = nullptr;

    // DOne
    DRETURN;
}

/* Populates a given VkWriteDescriptorSet struct to write a buffer. */
static void populate_write_info(VkWriteDescriptorSet& write_info, VkDescriptorSet vk_descriptor_set, VkDescriptorType vk_descriptor_type, uint32_t bind_index, const Tools::Array<VkDescriptorBufferInfo>& buffer_infos) {
    DENTER("populate_write_info(buffers)");

    // Set to default
    write_info = {};
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    // Set the set to which we write
    write_info.dstSet = vk_descriptor_set;

    // Set the binding to use; this one's equal to the sub-binding in the shader
    write_info.dstBinding = bind_index;
    
    // Set the element of the array. For now, we fix this at 0
    write_info.dstArrayElement = 0;

    // Next, set which type of descriptor this is and how many
    write_info.descriptorCount = static_cast<uint32_t>(buffer_infos.size());
    write_info.descriptorType = vk_descriptor_type;

    // Then, the data to pass. This can be of multiple forms, but we pass a buffer.
    write_info.pBufferInfo = buffer_infos.rdata();
    write_info.pImageInfo = nullptr;
    write_info.pTexelBufferView = nullptr;

    // Done
    DRETURN;
}

/* Populates a given VkWriteDescriptorSet struct to write an image. */
static void populate_write_info(VkWriteDescriptorSet& write_info, VkDescriptorSet vk_descriptor_set, VkDescriptorType vk_descriptor_type, uint32_t bind_index, const Tools::Array<VkDescriptorImageInfo>& image_infos) {
    DENTER("populate_write_info(images)");

    // Set to default
    write_info = {};
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    // Set the set to which we write
    write_info.dstSet = vk_descriptor_set;

    // Set the binding to use; this one's equal to the sub-binding in the shader
    write_info.dstBinding = bind_index;
    
    // Set the element of the array. For now, we fix this at 0
    write_info.dstArrayElement = 0;

    // Next, set which type of descriptor this is and how many
    write_info.descriptorCount = static_cast<uint32_t>(image_infos.size());
    write_info.descriptorType = vk_descriptor_type;

    // Then, the data to pass. This can be of multiple forms, but we pass a buffer.
    write_info.pBufferInfo = nullptr;
    write_info.pImageInfo = image_infos.rdata();
    write_info.pTexelBufferView = nullptr;

    // Done
    DRETURN;
}





/***** DESCRIPTORSET CLASS *****/
/* Constructor for the DescriptorSet class, which takes the vk_descriptor_set it wraps. */
DescriptorSet::DescriptorSet(descriptor_set_h handle, VkDescriptorSet vk_descriptor_set) :
    vk_handle(handle),
    vk_descriptor_set(vk_descriptor_set)
{}

/* Binds this descriptor set with the contents of a given buffer to the given bind index. Must be enough buffers to actually populate all bindings of the given type. */
void DescriptorSet::bind(const GPU& gpu, VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<Buffer>& buffers) const {
    DENTER("Rendering::DescriptorSet::bind(Buffer)");

    // We first create a list of buffer infos
    Tools::Array<VkDescriptorBufferInfo> buffer_infos(buffers.size());
    for (uint32_t i = 0; i < buffers.size(); i++) {
        // Start by creating the buffer info so that the descriptor knows smthng about the buffer
        VkDescriptorBufferInfo buffer_info;
        populate_buffer_info(buffer_info, buffers[i]);

        // Add to the list
        buffer_infos.push_back(buffer_info);
    }

    // Next, generate a VkWriteDescriptorSet with which we populate the buffer information
    // Can also use copies, for descriptor-to-descriptor stuff.
    VkWriteDescriptorSet write_info;
    populate_write_info(write_info, this->vk_descriptor_set, descriptor_type, bind_index, buffer_infos);

    // With the write info populated, update this set. Note that this can be used to perform multiple descriptor write & copies simultaneously
    vkUpdateDescriptorSets(gpu, 1, &write_info, 0, nullptr);

    // Done!
    DRETURN;
}

/* Binds this descriptor set with the contents of a given image view to the given bind index. Must be enough views to actually populate all bindings of the given type. */
void DescriptorSet::bind(const GPU& gpu, VkDescriptorType descriptor_type, uint32_t bind_index, const Tools::Array<VkImageView>& image_views) const {
    DENTER("Rendering::DescriptorSet::bind(VkImageView)");

    // We first create a list of buffer infos
    Tools::Array<VkDescriptorImageInfo> image_infos(image_views.size());
    for (uint32_t i = 0; i < image_views.size(); i++) {
        // Start by creating the buffer info so that the descriptor knows smthng about the buffer
        VkDescriptorImageInfo image_info;
        populate_image_info(image_info, image_views[i]);

        // Add to the list
        image_infos.push_back(image_info);
    }

    // Next, generate a VkWriteDescriptorSet with which we populate the buffer information
    // Can also use copies, for descriptor-to-descriptor stuff.
    VkWriteDescriptorSet write_info;
    populate_write_info(write_info, this->vk_descriptor_set, descriptor_type, bind_index, image_infos);

    // With the write info populated, update this set. Note that this can be used to perform multiple descriptor write & copies simultaneously
    vkUpdateDescriptorSets(gpu, 1, &write_info, 0, nullptr);

    // Done!
    DRETURN;
}

/* Binds the descriptor to the given (compute) command buffer. We assume that the recording already started. */
void DescriptorSet::record(const CommandBuffer& buffer, VkPipelineLayout pipeline_layout) const {
    DENTER("Rendering::DescriptorSet::record");

    // Add the binding
    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0, 1, &this->vk_descriptor_set, 0, nullptr);

    DRETURN;
}



/* Swap operator for the DescriptorSet. */
void Rendering::swap(DescriptorSet& ds1, DescriptorSet& ds2) {
    using std::swap;

    swap(ds1.vk_handle, ds2.vk_handle);
    swap(ds1.vk_descriptor_set, ds2.vk_descriptor_set);
}

