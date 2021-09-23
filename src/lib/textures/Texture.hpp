/* TEXTURE.hpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 14:09:08
 * Last edited:
 *   10/09/2021, 14:09:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a struct that defines the information needed to load and then
 *   render a texture.
**/

#ifndef TEXTURES_TEXTURE_HPP
#define TEXTURES_TEXTURE_HPP

#include <vulkan/vulkan.h>

#include "rendering/memory/Image.hpp"
#include "rendering/views/ImageView.hpp"
#include "rendering/sampling/Sampler.hpp"

namespace Makma3D::Textures {
    /* The Texture struct, which carries the data needed to render a texture. */
    struct Texture {
        /* The image we loaded that contains the texture itself. */
        Rendering::Image* image;
        /* The extent of the image. */
        VkExtent2D extent;

        /* The ImageView used to look into the image. */
        Rendering::ImageView* view;
        /* The sampler that we'll use to load from this image. */
        Rendering::Sampler* sampler;

    };

}

#endif
