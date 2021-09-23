/* MATERIAL DATA.hpp
 *   by Lut99
 *
 * Created:
 *   20/09/2021, 15:14:42
 * Last edited:
 *   20/09/2021, 15:14:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a struct for carrying material-related data to and from the
 *   GPU.
**/

#ifndef RENDERING_MATERIAL_DATA_HPP
#define RENDERING_MATERIAL_DATA_HPP

#include <vulkan/vulkan.h>
#include "glm/glm.hpp"

#include "../memory/Image.hpp"
#include "../sampling/Sampler.hpp"
#include "../views/ImageView.hpp"

namespace Makma3D::Rendering {
    /* The MaterialVertexData struct, which carries all data needed for the vertex shader. */
    struct MaterialVertexData {
        /* The colour of this Material. */
        glm::vec3 colour;
    };
    
    /* The MaterialFragmentData struct, which carries all data needed for the fragment shader. */
    struct MaterialFragmentData {
    };



    /* The MaterialData struct, which is used to carry material-specific data to the GPU. */
    struct MaterialData {
        /* The data needed for the vertex shader. */
        MaterialVertexData vertex;
        /* The data needed for the fragment shader. */
        MaterialFragmentData fragment;

        /* The texture that a material might have. */
        Rendering::Image* image;
        /* The ImageView of the image to use for textures. */
        Rendering::ImageView* view;
        /* The sampler we assign for the texture. */
        Rendering::Sampler* sampler;
    };

}

#endif
