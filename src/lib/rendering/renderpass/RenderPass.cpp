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
#include "../auxillary/Formats.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "RenderPass.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;
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

/* Populates the given VkSubpassDependency struct. */
static void populate_dependency(VkSubpassDependency& dependency, uint32_t src_subpass, uint32_t dst_subpass, VkPipelineStageFlags src_stage, VkAccessFlags src_access, VkPipelineStageFlags dst_stage, VkAccessFlags dst_access) {
    DENTER("populate_dependency");

    // Set to default
    dependency = {};
    
    // Set the subpasses in question
    dependency.srcSubpass = src_subpass;
    dependency.dstSubpass = dst_subpass;

    // Set the source subpass's requirements
    dependency.srcStageMask = src_stage;
    dependency.srcAccessMask = src_access;

    // Set the destination subpass's requirements
    dependency.dstStageMask = dst_stage;
    dependency.dstAccessMask = dst_access;

    // Done
    DRETURN;
}

/* Populates the given VkRenderPassCreateInfo struct. */
static void populate_render_pass_info(VkRenderPassCreateInfo& render_pass_info, const Tools::Array<VkAttachmentDescription>& vk_attachments, const Tools::Array<VkSubpassDescription>& vk_subpasses, const Tools::Array<VkSubpassDependency>& vk_dependencies) {
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

    // Add dependencies, if any
    render_pass_info.dependencyCount = vk_dependencies.size();
    render_pass_info.pDependencies = vk_dependencies.rdata();

    // Done!
    DRETURN;
}

/* Populates a given VkRenderPassBeginInfo struct. */
static void populate_begin_info(VkRenderPassBeginInfo& begin_info, const VkRenderPass& vk_render_pass, const VkFramebuffer& vk_framebuffer, const VkRect2D& vk_render_area, const Tools::Array<VkClearValue>& clear_values) {
    DENTER("populate_begin_info");

    // Set to default
    begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    // Attach the render pass
    begin_info.renderPass = vk_render_pass;

    // Attach the framebuffer
    begin_info.framebuffer = vk_framebuffer;

    // Define the area to render to
    begin_info.renderArea = vk_render_area;

    // Set the colour for the background
    begin_info.clearValueCount = clear_values.size();
    begin_info.pClearValues = clear_values.rdata();

    // Done
    DRETURN;
}





/***** RENDERPASS CLASS *****/
/* Constructor for the RenderPass class, which takes a GPU, */
RenderPass::RenderPass(const Rendering::GPU& gpu) :
    gpu(gpu),
    vk_render_pass(nullptr)
{
    DENTER("Rendering::RenderPass::RenderPass");
    DLOG(info, "Started RenderPass initialization.");
    DLEAVE;
}

/* Copy constructor for the RenderPass class. */
RenderPass::RenderPass(const RenderPass& other) :
    gpu(other.gpu),
    vk_attachments(other.vk_attachments),
    vk_dependencies(other.vk_dependencies)
{
    DENTER("Rendering::RenderPass::RenderPass(copy)");

    // Re-create the render pass, if needed
    if (this->vk_render_pass != nullptr) {
        // Convert the list of subpasses to their vulkan counterpart
        Tools::Array<VkSubpassDescription> vk_subpasses(this->subpasses.size());
        for (uint32_t i = 0; i < this->subpasses.size(); i++) {
            vk_subpasses.push_back(this->subpasses[i]);
        }

        // Simply create the renderpass
        VkRenderPassCreateInfo render_pass_info;
        populate_render_pass_info(render_pass_info, this->vk_attachments, vk_subpasses, this->vk_dependencies);

        VkResult vk_result;
        if ((vk_result = vkCreateRenderPass(this->gpu, &render_pass_info, nullptr, &this->vk_render_pass)) != VK_SUCCESS) {
            DLOG(fatal, "Could not re-create render pass: " + vk_error_map[vk_result]);
        }
    }

    // Done
    DLEAVE;
}

/* Move constructor for the RenderPass class. */
RenderPass::RenderPass(RenderPass&& other) :
    gpu(other.gpu),
    vk_render_pass(other.vk_render_pass),
    subpasses(other.subpasses),
    vk_attachments(other.vk_attachments),
    vk_dependencies(other.vk_dependencies)
{
    // Be sure that the other object does not allocate anything we need
    other.vk_render_pass = nullptr;
}

/* Destructor for the RenderPass class. */
RenderPass::~RenderPass() {
    DENTER("Rendering::RenderPass::~RenderPass");
    DLOG(info, "Cleaning RenderPass...");
    DINDENT;

    if (this->vk_render_pass != nullptr) {
        DLOG(info, "Destroying internal render pass object...");
        vkDestroyRenderPass(this->gpu, this->vk_render_pass, nullptr);
    }

    DDEDENT;
    DLEAVE;
}



/* Adds a new attachment to the RenderPass. Note that the ordering matters w.r.t. indexing, but just to be sure, this function returns the index of the attachment. Takes the swapchain's image format, the load operation for the buffer, the store operation, the initial layout and the final layout after the subpass. */
uint32_t RenderPass::add_attachment(VkFormat vk_swapchain_format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout) {
    DENTER("Rendering::RenderPass::add_attachment");

    // First, populate the attachmentdescription
    VkAttachmentDescription attachment;
    populate_attachment(attachment, vk_swapchain_format, load_op, store_op, initial_layout, final_layout);

    // Add it to the list
    uint32_t index = this->vk_attachments.size();
    this->vk_attachments.push_back(attachment);

    // Return the index
    DINDENT;
    DLOG(info, "Added attachment with format " + vk_format_map[vk_swapchain_format] + " to the RenderPass.");
    DDEDENT;
    DRETURN index;
}

/* Adds a new subpass to the RenderPass. The list of indices determines which color attachments to link to the subpass, and the list of image layouts determines the layout we like during the subpass for that attachment. Optionally takes another bindpoint than the graphics bind point. */
void RenderPass::add_subpass(const Tools::Array<std::pair<uint32_t, VkImageLayout>>& color_attachment_refs, const std::pair<uint32_t, VkImageLayout>& depth_attachment_ref, VkPipelineBindPoint bind_point) {
    DENTER("Rendering::RenderPass::add_subpass");

    // Store that and the list of references internally
    this->subpasses.push_back(Subpass(color_attachment_refs, depth_attachment_ref, bind_point));

    // Done
    DINDENT;
    DLOG(info, "Added subpass to the RenderPass.");
    DDEDENT;
    DRETURN;
}

/* Adds a new dependency to the RenderPass. Needs the subpass before the barrier, the subpass after it, the stage of the subpass before it, the access mask of the stage before it, the stage of the subpass after it and the access mask of that stage. */
void RenderPass::add_dependency(uint32_t src_subpass, uint32_t dst_subpass, VkPipelineStageFlags src_stage, VkAccessFlags src_access, VkPipelineStageFlags dst_stage, VkAccessFlags dst_access) {
    DENTER("Rendering::RenderPass::add_dependency");

    // Prepare the dependency definition
    VkSubpassDependency dependency;
    populate_dependency(dependency, src_subpass, dst_subpass, src_stage, src_access, dst_stage, dst_access);

    // Store internally
    this->vk_dependencies.push_back(dependency);

    DINDENT;
    DLOG(info, "Added dependency to the RenderPass.");
    DDEDENT;
    DRETURN;
}

/* Finalizes the RenderPass. After this, no new subpasses can be defined without calling finalize() again. */
void RenderPass::finalize() {
    DENTER("Rendering::RenderPass::finalize");

    // Convert the list of subpasses to their vulkan counterpart
    Tools::Array<VkSubpassDescription> vk_subpasses(this->subpasses.size());
    for (uint32_t i = 0; i < this->subpasses.size(); i++) {
        vk_subpasses.push_back(this->subpasses[i].subpass());
    }

    // Simply create the renderpass
    VkRenderPassCreateInfo render_pass_info;
    populate_render_pass_info(render_pass_info, this->vk_attachments, vk_subpasses, this->vk_dependencies);

    VkResult vk_result;
    if ((vk_result = vkCreateRenderPass(this->gpu, &render_pass_info, nullptr, &this->vk_render_pass)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create render pass: " + vk_error_map[vk_result]);
    }

    // Done
    DINDENT;
    DLOG(info, "Initialized RenderPass.");
    DDEDENT;
    DRETURN;
}



/* Schedules the RenderPass to run in the given CommandBuffer. Also takes a framebuffer to render to and a background colour for the image. */
void RenderPass::start_scheduling(const Rendering::CommandBuffer& cmd, const Rendering::Framebuffer& framebuffer, const VkClearValue& vk_clear_colour, const VkClearValue& vk_clear_depth) {
    DENTER("Rendering::RenderPass::start_scheduling");

    // First, create the rect that we shall render to
    VkRect2D render_area = {};
    render_area.offset.x = 0;
    render_area.offset.y = 0;
    render_area.extent = framebuffer.extent();

    // Define the list of clear colours
    Tools::Array<VkClearValue> clear_values = { vk_clear_colour, vk_clear_depth };

    // Create the begin info and populate it
    VkRenderPassBeginInfo begin_info;
    populate_begin_info(begin_info, this->vk_render_pass, framebuffer.framebuffer(), render_area, clear_values);

    // Schedule it in the command buffer (and tell it to schedule everything in the primary command buffers instead of secondary ones)
    vkCmdBeginRenderPass(cmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    // Done
    DRETURN;
}

/* Finishes scheduling the RenderPass. */
void RenderPass::stop_scheduling(const Rendering::CommandBuffer& cmd) {
    DENTER("Rendering::RenderPass::stop_scheduling");

    // Simply call the stop
    vkCmdEndRenderPass(cmd);

    DRETURN;
}



/* Swap operator for the RenderPass class. */
void Rendering::swap(RenderPass& rp1, RenderPass& rp2) {
    DENTER("Rendering::swap(RenderPass)");

    #ifndef NDEBUG
    // If the GPU is not the same, then initialize to all nullptrs and everything
    if (rp1.gpu != rp2.gpu) {
        DLOG(fatal, "Cannot swap render passes with different GPUs");
    }
    #endif

    // Swap EVERYTHING but the GPU
    using std::swap;
    swap(rp1.vk_render_pass, rp2.vk_render_pass);
    swap(rp1.subpasses, rp2.subpasses);
    swap(rp1.vk_attachments, rp2.vk_attachments);
    swap(rp1.vk_dependencies, rp2.vk_dependencies);

    // Done
    DRETURN;
}
