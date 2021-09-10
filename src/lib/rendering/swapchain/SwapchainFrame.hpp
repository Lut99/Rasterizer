/* SWAPCHAIN FRAME.hpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 15:36:31
 * Last edited:
 *   08/09/2021, 15:36:31
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains data needed to draw to a single frame returned by the
 *   swapchain.
**/

#ifndef RENDERING_FRAME_HPP
#define RENDERING_FRAME_HPP

#include <vulkan/vulkan.h>
#include <cstdint>

#include "../gpu/GPU.hpp"
#include "../renderpass/RenderPass.hpp"
#include "../synchronization/Fence.hpp"

namespace Makma3D::Rendering {
    /* The SwapchainFrame class, which is a collection of data that can be used to draw to a single frame. */
    class SwapchainFrame {
    public:
        /* The log channel for the SwapchainFrame class. */
        static constexpr const char* channel = "SwapchainFrame";

        /* The GPU where the SwapchainFrame lives. */
        const Rendering::GPU& gpu;
        /* The RenderPass associated with this SwapchainFrame. */
        const Rendering::RenderPass& render_pass;

    private:
        /* The index of the image that we wrap. */
        uint32_t vk_image_index;
        /* The image that we wrap. */
        VkImage vk_image;
        /* The format of the image. */
        VkFormat vk_format;
        /* The extent of the image. */
        VkExtent2D vk_extent;

        /* The image view for the frame's colour aspect. */
        VkImageView vk_color_view;
        /* The image view for the frame's depth aspect (is actually provided to us by a DepthStencil). */
        VkImageView vk_depth_view;
        /* The framebuffer wrapping the image and its various aspects. */
        VkFramebuffer vk_framebuffer;
    
    public:
        /* Reference to a Fence so we can keep track if this frame is not secretly being used by a ConceptualFrame. */
        Rendering::Fence* in_flight_fence;

    public:
        /* Constructor for the SwapchainFrame class, which takes a GPU where it lives, a renderpass to bind to, the index of the swapchain image we wrap, the image itself, its format, its size and a depth-aspect image view originating from a DepthStencil. */
        SwapchainFrame(const Rendering::GPU& gpu, const Rendering::RenderPass& render_pass, uint32_t vk_image_index, const VkImage& vk_image, VkFormat vk_image_format, const VkExtent2D& vk_image_extent, const VkImageView& vk_depth_view);
        /* Copy constructor for the SwapchainFrame class, which is deleted. */
        SwapchainFrame(const SwapchainFrame& other) = delete;
        /* Move constructor for the SwapchainFrame class. */
        SwapchainFrame(SwapchainFrame&& other);
        /* Destructor for the SwapchainFrame class. */
        ~SwapchainFrame();

        /* Returns the index of the internal image in the swapchain we came from. */
        inline uint32_t index() const { return this->vk_image_index; }
        /* Returns the format of the internal image. */
        inline VkFormat format() const { return this->vk_format; }
        /* Returns the extent of the internal image. */
        inline const VkExtent2D& extent() const { return this->vk_extent; }
        /* Explicitly returns the internal VkFramebuffer object. */
        inline const VkFramebuffer& framebuffer() const { return this->vk_framebuffer; }
        /* Implicitly returns the internal VkFramebuffer object. */
        inline operator const VkFramebuffer&() const { return this->vk_framebuffer; }

        /* Copy assignment operator for the SwapchainFrame class, which is deleted. */
        SwapchainFrame& operator=(const SwapchainFrame& other) = delete;
        /* Move assignment operator for the SwapchainFrame class. */
        inline SwapchainFrame& operator=(SwapchainFrame&& other) { if (this != &other) { swap(*this, other); } return *this; };
        /* Swap operator for the SwapchainFrame class. */
        friend void swap(SwapchainFrame& sf1, SwapchainFrame& sf2);

    };

    /* Swap operator for the SwapchainFrame class. */
    void swap(SwapchainFrame& sf1, SwapchainFrame& sf2);

}

#endif
