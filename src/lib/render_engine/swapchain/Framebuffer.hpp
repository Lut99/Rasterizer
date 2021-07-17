/* FRAMEBUFFER.hpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 14:11:43
 * Last edited:
 *   27/06/2021, 14:11:43
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a wrapper for the VkFramebuffer class. Is created by the
 *   RenderPass class, since its associated with its attachments.
**/

#ifndef RENDERING_FRAMEBUFFER_HPP
#define RENDERING_FRAMEBUFFER_HPP

#include <vulkan/vulkan.h>

#include "render_engine/gpu/GPU.hpp"
#include "tools/Array.hpp"

namespace Rasterizer::Rendering {
    /* The Framebuffer class, which wraps the VkFramebuffer one. */
    class Framebuffer {
    public:
        /* The GPU where the framebuffer lives. */
        const Rendering::GPU& gpu;
    
    private:
        /* The VkFramebuffer object we wrap. */
        VkFramebuffer vk_framebuffer;
        /* The extent of the framebuffer. */
        VkExtent2D vk_extent;

        /* The imageview we wrap around for the color. */
        VkImageView vk_color_view;
        /* The imageview we wrap around for the depth. */
        VkImageView vk_depth_view;
        /* The create info used to create the buffer. */
        VkFramebufferCreateInfo vk_framebuffer_info;

        /* Constructor for the Framebuffer class, which takes a GPU to allocate it on, a renderpass to bind to, a VkImageView to wrap around for the colour, a VkImageView to wrap around for the depth and an extent describing the buffer's size. */
        Framebuffer(const Rendering::GPU& gpu, const VkRenderPass& vk_render_pass, const VkImageView& vk_color_view, const VkImageView& vk_depth_view, const VkExtent2D& vk_extent);

        /* Mark the RenderPass class as friend. */
        friend class Swapchain;

    public:
        /* Copy constructor for the Framebuffer class. */
        Framebuffer(const Framebuffer& other);
        /* Move constructor for the Framebuffer class. */
        Framebuffer(Framebuffer&& other);
        /* Destructor for the Framebuffer class. */
        ~Framebuffer();

        /* Returns the extent of the framebuffer. */
        inline const VkExtent2D& extent() const { return this->vk_extent; }
        /* Expliticly returns the internal VkFramebuffer object. */
        inline const VkFramebuffer& framebuffer() const { return this->vk_framebuffer; }
        /* Implicitly returns the internal VkFramebuffer object. */
        inline operator VkFramebuffer() const { return this->vk_framebuffer; }

        /* Copy assignment operator for the Framebuffer class. */
        inline Framebuffer& operator=(const Framebuffer& other) { return *this = Framebuffer(other); }
        /* Move assignment operator for the Framebuffer class. */
        inline Framebuffer& operator=(Framebuffer&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Framebuffer class. */
        friend void swap(Framebuffer& fb1, Framebuffer& fb2);

    };

    /* Swap operator for the Framebuffer class. */
    void swap(Rendering::Framebuffer& fb1, Rendering::Framebuffer& fb2);
}

#endif
