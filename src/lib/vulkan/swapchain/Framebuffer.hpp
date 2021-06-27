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

#ifndef VULKAN_FRAMEBUFFER_HPP
#define VULKAN_FRAMEBUFFER_HPP

#include <vulkan/vulkan.h>

#include "vulkan/gpu/GPU.hpp"
#include "tools/Array.hpp"

namespace Rasterizer::Vulkan {
    /* The Framebuffer class, which wraps the VkFramebuffer one. */
    class Framebuffer {
    public:
        /* The GPU where the framebuffer lives. */
        const Vulkan::GPU& gpu;
    
    private:
        /* The VkFramebuffer object we wrap. */
        VkFramebuffer vk_framebuffer;

        /* The imageview we wrap around. */
        VkImageView vk_image_view;
        /* The create info used to create the buffer. */
        VkFramebufferCreateInfo vk_framebuffer_info;

        /* Constructor for the Framebuffer class, which takes a GPU to allocate it on, a renderpass to bind to, a VkImageView to wrap around and an extent describing the buffer's size. */
        Framebuffer(const Vulkan::GPU& gpu, const VkRenderPass& vk_render_pass, const VkImageView& vk_image_view, const VkExtent2D& vk_extent);

        /* Mark the RenderPass class as friend. */
        friend class Swapchain;

    public:
        /* Copy constructor for the Framebuffer class. */
        Framebuffer(const Framebuffer& other);
        /* Move constructor for the Framebuffer class. */
        Framebuffer(Framebuffer&& other);
        /* Destructor for the Framebuffer class. */
        ~Framebuffer();

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
    void swap(Vulkan::Framebuffer& fb1, Vulkan::Framebuffer& fb2);
}

#endif
