/* RENDER PASS.cpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 12:26:36
 * Last edited:
 *   27/06/2021, 12:26:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that wraps VkRenderPasses, which are used to describe
 *   how a render pipeline looks like in terms of memory management of the
 *   framebuffer.
**/

#include "tools/CppDebugger.hpp"
#include "vulkan/ErrorCodes.hpp"

#include "RenderPass.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkAttachmentDescription struct. */
static void populate_attachment(VkAttachmentDescription& attachment, VkFormat vk_format, VkAttachmentLoadOp vk_load_op, VkAttachmentStoreOp vk_store_op, VkImageLayout vk_initial_layout, VkImageLayout vk_final_layout) {
    DENTER("populate_attachment");

    // Set to default
    attachment = {};

    // Set the format of the output framebuffer
    attachment.format = vk_format;

    // Set the multisampling mode of the frambuffer (hardcoded to 1 sample for now)
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // Set how to load & store the framebuffer
    attachment.loadOp = vk_load_op;
    attachment.storeOp = vk_store_op;

    // Since we're focussing on colours, set the stencil buffer load & store ops that we don't care
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // Finally, set the initial and final layout of the framebuffer
    attachment.initialLayout = vk_initial_layout;
    attachment.finalLayout = vk_final_layout;

    // done
    DRETURN;
}

/* Populates the given VkSubpassDescription struct. */
static void populate_subpass(VkSubpassDescription& subpass, const Tools::Array<VkAttachmentReference>& vk_attachment_refs, VkPipelineBindPoint vk_bind_point) {
    DENTER("populate_subpass");

    // Set to default
    subpass = {};

    // Set the bindpoint
    subpass.pipelineBindPoint = vk_bind_point;

    // Set the color attachments
    subpass.colorAttachmentCount = vk_attachment_refs.size();
    subpass.pColorAttachments = vk_attachment_refs.rdata();

    // In the future, more types of attachments may be given here

    // Done
    DRETURN;
}

/* Populates the given VkRenderPassCreateInfo struct. */
static void populate_render_pass_info(VkRenderPassCreateInfo& render_pass_info, const Tools::Array<VkAttachmentDescription>& vk_attachments, const Tools::Array<VkSubpassDescription>& vk_subpasses) {
    DENTER("populate_render_pass_info");

    // Set to default
    render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    // Attach the attachments
    render_pass_info.attachmentCount = vk_attachments.size();
    render_pass_info.pAttachments = vk_attachments.rdata();

    // Attach the subpasses
    render_pass_info.subpassCount = vk_subpasses.size();
    render_pass_info.pSubpasses = vk_subpasses.rdata();

    // Done!
    DRETURN;
}





/***** RENDERPASS CLASS *****/
/* Constructor for the RenderPass class, which takes a GPU, */
RenderPass::RenderPass(const Vulkan::GPU& gpu) :
    gpu(gpu),
    vk_render_pass(nullptr)
{}

/* Copy constructor for the RenderPass class. */
RenderPass::RenderPass(const RenderPass& other) :
    gpu(other.gpu),
    vk_attachments(other.vk_attachments),
    vk_attachment_refs(other.vk_attachment_refs)
{
    DENTER("Vulkan::RenderPass::RenderPass(copy)");

    // First, re-create the subpasses since they have pointer dependencies
    for (uint32_t i = 0; i < other.vk_subpasses.size(); i++) {
        // Create a new subpass with the given properties
        VkSubpassDescription subpass;
        populate_subpass(subpass, this->vk_attachment_refs[i], other.vk_subpasses[i].pipelineBindPoint);

        // Store it internally
        this->vk_subpasses.push_back(subpass);
    }

    // Re-create the render pass, if needed
    if (this->vk_render_pass != nullptr) {
        // Simply create the renderpass
        VkRenderPassCreateInfo render_pass_info;
        populate_render_pass_info(render_pass_info, this->vk_attachments, this->vk_subpasses);

        VkResult vk_result;
        if ((vk_result = vkCreateRenderPass(this->gpu, &render_pass_info, nullptr, &this->vk_render_pass)) != VK_SUCCESS) {
            DLOG(fatal, "Could not create render pass: " + vk_error_map[vk_result]);
        }
    }

    // Done
    DLEAVE;
}

/* Move constructor for the RenderPass class. */
RenderPass::RenderPass(RenderPass&& other) :
    gpu(other.gpu),
    vk_render_pass(other.vk_render_pass),
    vk_subpasses(other.vk_subpasses),
    vk_attachments(other.vk_attachments),
    vk_attachment_refs(other.vk_attachment_refs)
{
    // Be sure that the other object does not allocate anything we need
    other.vk_render_pass = nullptr;
}

/* Destructor for the RenderPass class. */
RenderPass::~RenderPass() {
    DENTER("Vulkan::RenderPass::~RenderPass");
    DLOG(info, "Cleaning RenderPass...");
    DINDENT;

    if (this->vk_render_pass != nullptr) {
        DLOG(info, "Destroying internal render pass object...");
        vkDestroyRenderPass(this->gpu, this->vk_render_pass, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Adds a new attachment to the RenderPass. Note that the ordering matters w.r.t. indexing, but just to be sure, this function returns the index of the attachment. Takes the swapchain to derive the image format from, the load operation for the buffer, the store operation, the initial layout and the final layout after the subpass. */
uint32_t RenderPass::add_attachment(const Vulkan::Swapchain& swapchain, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout) {
    DENTER("Vulkan::RenderPass::add_attachment");

    // First, populate the attachmentdescription
    VkAttachmentDescription attachment;
    populate_attachment(attachment, swapchain.format(), load_op, store_op, initial_layout, final_layout);

    // Add it to the list
    uint32_t index = this->vk_attachments.size();
    this->vk_attachments.push_back(attachment);

    // Return the index
    DRETURN index;
}

/* Adds a new subpass to the RenderPass. The list of indices determines which color attachments to link to the subpass, and the list of image layouts determines the layout we like during the subpass for that attachment. Optionally takes another bindpoint than the graphics bind point. */
void RenderPass::add_subpass(const Tools::Array<std::pair<uint32_t, VkImageLayout>>& attachment_refs, VkPipelineBindPoint bind_point) {
    DENTER("Vulkan::RenderPass::add_subpass");

    // Create the attachment references for each references attachment
    Tools::Array<VkAttachmentReference> vk_attachment_refs(attachment_refs.size());
    for (uint32_t i = 0; i < attachment_refs.size(); i++) {
        // Check if the noted attachment reference exists
        if (attachment_refs[i].first >= this->vk_attachments.size()) {
            DLOG(fatal, "No attachment exists with index " + std::to_string(attachment_refs[i].first));
        }

        // Add it
        vk_attachment_refs.push_back({});
        vk_attachment_refs[i].attachment = attachment_refs[i].first;
        vk_attachment_refs[i].layout = attachment_refs[i].second;
    }

    // When done, create a VkSubpassDescription for this shit
    VkSubpassDescription subpass;
    populate_subpass(subpass, vk_attachment_refs, bind_point);

    // Store that and the list of references internally
    this->vk_subpasses.push_back(subpass);
    this->vk_attachment_refs.push_back(vk_attachment_refs);

    // Done
    DRETURN;
}

/* Finalizes the RenderPass. After this, no new subpasses can be defined without calling finalize() again. */
void RenderPass::finalize() {
    DENTER("Vulkan::RenderPass::finalize");

    // Simply create the renderpass
    VkRenderPassCreateInfo render_pass_info;
    populate_render_pass_info(render_pass_info, this->vk_attachments, this->vk_subpasses);

    VkResult vk_result;
    if ((vk_result = vkCreateRenderPass(this->gpu, &render_pass_info, nullptr, &this->vk_render_pass)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create render pass: " + vk_error_map[vk_result]);
    }

    // Done
    DRETURN;
}



/* Swap operator for the RenderPass class. */
void Vulkan::swap(RenderPass& rp1, RenderPass& rp2) {
    DENTER("Vulkan::swap(RenderPass)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (rp1.gpu != rp2.gpu) {
        DLOG(fatal, "Cannot swap render passes with different GPUs");
    }
    #endif

    // Swap EVERYTHING but the GPU
    using std::swap;
    swap(rp1.vk_render_pass, rp2.vk_render_pass);
    swap(rp1.vk_subpasses, rp2.vk_subpasses);
    swap(rp1.vk_attachments, rp2.vk_attachments);
    swap(rp1.vk_attachment_refs, rp2.vk_attachment_refs);

    // Done
    DRETURN;
}
