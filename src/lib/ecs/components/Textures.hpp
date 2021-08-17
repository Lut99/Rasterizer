/* TEXTURES.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:58:54
 * Last edited:
 *   16/08/2021, 11:58:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the Texture(s) component, which contains the data needed for
 *   an entity to have a Texture.
**/

#ifndef ECS_TEXTURES_HPP
#define ECS_TEXTURES_HPP

#include <cstdint>
#include <vulkan/vulkan.h>

#include "tools/Array.hpp"
#include "tools/Typenames.hpp"
#include "rendering/memory/Image.hpp"

#include "../auxillary/ComponentHash.hpp"

namespace Rasterizer::ECS {
    /* The Texture component, which is used to add several model textures to an entity. */
    struct Texture {
        /* The image containing the texture. */
        Rendering::image_h image_h;
        /* The dimension of the texture. */
        VkExtent2D extent;
    };
    /* Shortcut for the list of Meshes. */
    using Textures = Tools::Array<Texture>;

    /* Hash function for the Textures list, which returns its 'hash' code. */
    template <> inline constexpr uint32_t hash_component<Textures>() { return 5; }
}



namespace Tools {
    /* The string name of the Texture component. */
    template <> inline constexpr const char* type_name<Rasterizer::ECS::Texture>() { return "ECS::Texture"; }
    /* The string name of the Textures list. */
    template <> inline constexpr const char* type_name<Rasterizer::ECS::Textures>() { return "ECS::Textures"; }
}

#endif