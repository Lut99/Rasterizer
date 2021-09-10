/* SWAPCHAIN.hpp
 *   by Lut99
 *
 * Created:
 *   09/05/2021, 18:40:10
 * Last edited:
 *   10/09/2021, 10:55:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the wrapper around Vulkan's swapchain. Not only does it
 *   manage those resources, but also all memory-related buffer and image
 *   management to make it easy to manage those.
**/

#ifndef COMPUTE_SWAPCHAIN_HPP
#define COMPUTE_SWAPCHAIN_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../gpu/GPU.hpp"
#include "../gpu/Surface.hpp"
#include "../renderpass/RenderPass.hpp"
#include "../depthtesting/DepthStencil.hpp"
#include "SwapchainFrame.hpp"

namespace Makma3D::Rendering {
    /* The Swapchain class, which wraps and manages the swapchain and all images related to it. */
    class Swapchain {
    public:
        /* Channel name for the Swapchain class. */
        static constexpr const char* channel = "Swapchain";

        /* Immutable reference to the GPU where we got the swapchain from. */
        const GPU& gpu;
        /* The surface to which this swapchain is bound. */
        const Surface& surface;

    private:
        /* The internal VkSwapchainKHR object that we wrap. */
        VkSwapchainKHR vk_swapchain;

        /* The chosen format for this swapchain. */
        VkSurfaceFormatKHR vk_surface_format;
        /* The chosen presentation mode for this swapchain. */
        VkPresentModeKHR vk_surface_present_mode;
        /* The current size of the swapchain frames. */
        VkExtent2D vk_surface_extent;
        
        /* The number of images that are actually created in the swapchain. */
        uint32_t vk_actual_image_count;
        /* The number of images that we want to have in the swapchain. */
        uint32_t vk_desired_image_count;

        /* The images part of the swapchain. Will be variable in size. */
        Tools::Array<VkImage> vk_swapchain_images;

    public:
        /* Constructor for the Swapchain class, which takes the GPU where it will be constructed and the window to which it shall present. */
        Swapchain(const GPU& gpu, GLFWwindow* glfw_window, const Surface& surface);
        /* Copy constructor for the Swapchain class. */
        Swapchain(const Swapchain& other);
        /* Move constructor for the Swapchain class. */
        Swapchain(Swapchain&& other);
        /* Destructor for the Swapchain class. */
        ~Swapchain();

        /* Returns a list of SwapchainFrames from the internal images. They will be bound to the given RenderPass and DepthStencil. */
        Tools::Array<Rendering::SwapchainFrame> get_frames(const Rendering::RenderPass& render_pass, const Rendering::DepthStencil& depth_stencil) const;

        /* Resizes the swapchain to the given size. Note that this also re-creates it, so any existing handle to the internal VkSwapchain will be invalid. */
        void resize(uint32_t new_width, uint32_t new_height);
        /* Resizes the swapchain to the size of the given window. Note that this also re-creates it, so any existing handle to the internal VkSwapchain will be invalid. */
        void resize(GLFWwindow* glfw_window);

        /* Returns the number of images in the swapchain. */
        inline uint32_t size() const { return this->vk_actual_image_count; }
        /* Returns the actual extent of the swapchain. */
        inline const VkExtent2D& extent() const { return this->vk_surface_extent; }
        /* Returns the format of the swapchain images. */
        inline VkFormat format() const { return this->vk_surface_format.format; }

        /* Expliticly returns the internal VkSwapchainKHR object. */
        inline const VkSwapchainKHR& swapchain() const { return this->vk_swapchain; }
        /* Implicitly returns the internal VkSwapchainKHR object. */
        inline operator VkSwapchainKHR() const { return this->vk_swapchain; }

        /* Copy assignment operator for the Swapchain class. */
        inline Swapchain& operator=(const Swapchain& other) { return *this = Swapchain(other); }
        /* Move assignment operator for the Swapchain class. */
        inline Swapchain& operator=(Swapchain&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Swapchain class. */
        friend void swap(Swapchain& s1, Swapchain& s2);

    };

    /* Swap operator for the Swapchain class. */
    void swap(Swapchain& s1, Swapchain& s2);
}

#endif
