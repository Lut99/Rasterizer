/* IMAGE VIEW.hpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:15:47
 * Last edited:
 *   25/08/2021, 16:15:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ImageView class, which wraps a single ImageView. Is
 *   managed by the ImageViewPool, though.
**/

#ifndef RENDERING_IMAGE_VIEW_HPP
#define RENDERING_IMAGE_VIEW_HPP

#include <vulkan/vulkan.h>
#include "../memory/Image.hpp"

namespace Rasterizer::Rendering {
    /* The ImageView class, which wraps a single ImageView. */
    class ImageView {
    public:
        /* Channel name for the ImageView class. */
        static constexpr const char* channel = "ImageView";

    private:
        /* The InitData struct, which is used to group everything needed for copying the views. */
        struct InitData {
            /* The image itself. */
            VkImage image;
            /* The format of the Image. */
            VkFormat view_format;
        };

    private:
        /* The ImageView we wrap. */
        VkImageView vk_view;

        /* Other data we need for copying the view. */
        InitData init_data;

        /* Declare the random ImageViewPool class as friend. */
        friend class ImageViewPool;


        /* Constructor for the ImageView class, which takes the ImageView to wrap, the image the view wraps and the view's format. */
        ImageView(VkImageView vk_view, VkImage image, VkFormat view_format);
        /* Private destructor for the ImageView class. */
        ~ImageView();
    
    public:
        /* Copy constructor for the ImageView class, which is deleted. */
        ImageView(const ImageView& other) = delete;
        /* Move constructor for the ImageView class, which is deleted. */
        ImageView(ImageView&& other) = delete;

        /* Explicitly returns the internal VkImageView object. */
        inline const VkImageView& view() const { return this->vk_view; }
        /* Implicitly returns the internal VkImageView object. */
        inline operator const VkImageView&() const { return this->vk_view; }

        /* Copy assignment operator for the ImageView class, which is deleted. */
        ImageView& operator=(const ImageView& other) = delete;
        /* Copy assignment operator for the ImageView class, which is deleted. */
        ImageView& operator=(ImageView&& other) = delete;

    };
}

#endif
