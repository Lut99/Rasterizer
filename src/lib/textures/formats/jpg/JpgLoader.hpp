/* JPG LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   06/09/2021, 14:47:46
 * Last edited:
 *   06/09/2021, 14:47:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Function that loads the given file as if it were a .jpg picture.
**/

#ifndef TEXTURES_JPG_LOADER_HPP
#define TEXTURES_JPG_LOADER_HPP

#include <string>

#include "ecs/components/Textures.hpp"
#include "rendering/memory/MemoryManager.hpp"

namespace Makma3D::Textures {
    /* Loads the file at the given path as a .jpg file, and populates the given Texture component from it. */
    void load_jpg_texture(Rendering::MemoryManager& memory_manager, ECS::Texture& texture, const std::string& path);

}

#endif
