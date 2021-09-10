/* IMAGE VIEW POOL.hpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:11:23
 * Last edited:
 *   25/08/2021, 16:11:23
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Pool that is in charge of managing image views for the Rasterizer.
 *   Unlike the other memory pools, does not actually allocate data on the
 *   GPU, but just manages CPU memory instead.
**/

#ifndef RENDERING_IMAGE_VIEW_POOL_HPP
#define RENDERING_IMAGE_VIEW_POOL_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "../gpu/GPU.hpp"
#include "../memory/Image.hpp"

#include "ImageView.hpp"

namespace Makma3D::Rendering {
    /* The ImageViewPool class, which is in charge for creating and managing the life cycles of image views. */
    class ImageViewPool {
    public:
        /* Channel name for the ImageViewPool class. */
        static constexpr const char* channel = "ImageViewPool";
        /* The GPU where all the image views live. */
        const Rendering::GPU& gpu;
    
    private:
        /* List of the ImageViews allocated with this pool. */
        Tools::Array<ImageView*> views;

    public:
        /* Constructor for the ImageViewPool class, which takes a GPU to allocate image views on. */
        ImageViewPool(const Rendering::GPU& gpu);
        /* Copy constructor for the ImageViewPool class. */
        ImageViewPool(const ImageViewPool& other);
        /* Move constructor for the ImageViewPool class. */
        ImageViewPool(ImageViewPool&& other);
        /* Destructor for the ImageViewPool class. */
        ~ImageViewPool();

        /* Allocates a new image view that offers insight in the given image under the given format. */
        ImageView* allocate(Rendering::Image* image, VkFormat view_format);
        /* Allocates a new image view that is a copy of the given image view. */
        ImageView* allocate(const ImageView* view);
        /* Deallocates the given ImageView. */
        void free(const ImageView* view);

        /* Copy assignment operator for the ImageViewPool class. */
        inline ImageViewPool& operator=(const ImageViewPool& other) { return *this = ImageViewPool(other); }
        /* Move assignment operator for the ImageViewPool class. */
        inline ImageViewPool& operator=(ImageViewPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ImageViewPool class. */
        friend void swap(ImageViewPool& ivp1, ImageViewPool& ivp2);
    };
    
    /* Swap operator for the ImageViewPool class. */
    void swap(ImageViewPool& ivp1, ImageViewPool& ivp2);
}

#endif
