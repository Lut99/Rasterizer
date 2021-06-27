/* RENDER PASS.hpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 12:26:32
 * Last edited:
 *   27/06/2021, 12:26:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that wraps VkRenderPasses, which are used to describe
 *   how a render pipeline looks like in terms of memory management of the
 *   framebuffer.
**/

#ifndef VULKAN_RENDER_PASS_HPP
#define VULKAN_RENDER_PASS_HPP

#include <vulkan/vulkan.h>

#include "vulkan/gpu/GPU.hpp"
#include "tools/Array.hpp"

namespace Rasterizer::Vulkan {
    /* The RenderPass class, which wraps a VkRenderPass object. Also used to define subpasses for this render pass. */
    class RenderPass {
    public:
        /* The GPU where the RenderPass lives. */
        const Vulkan::GPU& gpu;
    
    private:
        /* The VkRenderPass object we wrap. */
        VkRenderPass vk_render_pass;

        /* List of the subpasses generated with the VkRenderPass class. */
        Tools::Array<VkSubpassDescription> vk_subpasses;
        /* List of the attachments used in the renderpass. */
        Tools::Array<VkAttachmentDescription> vk_attachments;
        /* List of attachment references used to re-create the subpasses. */
        Tools::Array<Tools::Array<VkAttachmentReference>> vk_attachment_refs;

    public:
        /* Constructor for the RenderPass class, which takes a GPU,  */
        RenderPass(const Vulkan::GPU& gpu);
        /* Copy constructor for the RenderPass class. */
        RenderPass(const RenderPass& other);
        /* Move constructor for the RenderPass class. */
        RenderPass(RenderPass&& other);
        /* Destructor for the RenderPass class. */
        ~RenderPass();

        /* Adds a new attachment to the RenderPass. Note that the ordering matters w.r.t. indexing, but just to be sure, this function returns the index of the attachment. Takes the swapchain's image format, the load operation for the buffer, the store operation, the initial layout and the final layout after the subpass. */
        uint32_t add_attachment(VkFormat vk_swapchain_format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout);
        /* Adds a new subpass to the RenderPass. The list of indices determines which color attachments to link to the subpass, and the list of image layouts determines the layout we like during the subpass for that attachment. Optionally takes another bindpoint than the graphics bind point. */
        void add_subpass(const Tools::Array<std::pair<uint32_t, VkImageLayout>>& attachment_refs, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);
        /* Finalizes the RenderPass. After this, no new subpasses can be defined without calling finalize() again. */
        void finalize();

        /* Expliticly returns the internal VkRenderPass object. */
        inline const VkRenderPass& render_pass() const { return this->vk_render_pass; }
        /* Implicitly returns the internal VkRenderPass object. */
        inline operator VkRenderPass() const { return this->vk_render_pass; }

        /* Copy assignment operator for the RenderPass class. */
        inline RenderPass& operator=(const RenderPass& other) { return *this = RenderPass(other); }
        /* Move assignment operator for the RenderPass class. */
        inline RenderPass& operator=(RenderPass&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the RenderPass class. */
        friend void swap(RenderPass& rp1, RenderPass& rp2);

    };

    /* Swap operator for the RenderPass class. */
    void swap(RenderPass& rp1, RenderPass& rp2);
}

#endif
