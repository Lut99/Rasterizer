/* DEPTH STENCIL.hpp
 *   by Lut99
 *
 * Created:
 *   17/07/2021, 12:57:04
 * Last edited:
 *   17/07/2021, 12:57:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DepthStencil class, which wraps around an image and can
 *   be used by the pipeline to perform depth testing.
**/

#ifndef RENDERING_DEPTH_STENCIL_HPP
#define RENDERING_DEPTH_STENCIL_HPP

#include "render_engine/gpu/GPU.hpp"
#include "render_engine/memory/MemoryPool.hpp"

namespace Rasterizer::Rendering {
    /* The DepthStencil class, which wraps around an image to provide a memory buffer for depth testing. */
    class DepthStencil {
    public:
        /* The GPU where the DepthStencil lives. */
        const Rendering::GPU& gpu;
        /* The MemoryPool used to manage the internal image. */
        Rendering::MemoryPool& draw_pool;
    
    private:
        /* The Image that we wrap. */
        Rendering::Image* rendering_image;
        /* The VkImageView that we wrap. */
        VkImageView vk_image_view;

    public:
        /* Constructor for the DepthStencil class, which takes the GPU where the DepthStencil lives, a memory pool to allocate the internal image from and the size of the depth stencil (as an VkExtent2D). */
        DepthStencil(const Rendering::GPU& gpu, Rendering::MemoryPool& draw_pool, const VkExtent2D& image_extent);
        /* Copy constructor for the DepthStencil class. */
        DepthStencil(const DepthStencil& other);
        /* Move constructor for the DepthStencil class. */
        DepthStencil(DepthStencil&& other);
        /* Destructor for the DepthStencil class. */
        ~DepthStencil();

        /* Resizes the depth stencil to the given size. */
        void resize(const VkExtent2D& new_extent);

        /* Explicitly returns the image's format. */
        inline VkFormat format() const { return this->rendering_image->format(); }
        /* Explicitly returns the internal Rendering::Image object. */
        inline const Rendering::Image& image() const { return *this->rendering_image; }
        /* Implicitly returns the internal Rendering::Image object. */
        inline operator const Rendering::Image&() const { return *this->rendering_image; }
        /* Explicitly returns the internal VkImage object. Note that this VkImage is not a reference, and should thus used in Vulkan calls with care. */
        inline VkImage vk_image() const { return this->rendering_image->image(); }
        /* Implicitly returns the internal VkImage object. */
        inline operator VkImage() const { return this->rendering_image->image(); }
        /* Expliticlty returns the internal VkImageView object. */
        inline const VkImageView& view() const { return this->vk_image_view; }
        /* Implicitly returns the internal VkImageView object. */
        inline operator VkImageView() const { return this->vk_image_view; }

        /* Copy assignment operator for the DepthStencil class. */
        inline DepthStencil& operator=(const DepthStencil& other) { return *this = DepthStencil(other); }
        /* Move assignment operator for the DepthStencil class. */
        inline DepthStencil& operator=(DepthStencil&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the DepthStencil class. */
        friend void swap(DepthStencil& ds1, DepthStencil& ds2);

    };

    /* Swap operator for the DepthStencil class. */
    void swap(DepthStencil& ds1, DepthStencil& ds2);

}

#endif
