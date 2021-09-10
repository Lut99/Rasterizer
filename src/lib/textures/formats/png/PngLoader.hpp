/* PNG LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 12:14:46
 * Last edited:
 *   16/08/2021, 12:14:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Function that loads the given file as if it were a file in PNG-format.
 *   Populates the given Texture component.
**/

#ifndef TEXTURES_PNG_LOADER_HPP
#define TEXTURES_PNG_LOADER_HPP

#include <string>

#include "ecs/components/Textures.hpp"
#include "rendering/memory/MemoryManager.hpp"

namespace Makma3D::Textures {
    /* Loads the file at the given path as a .png file, and populates the given Texture component from it. */
    void load_png_texture(Rendering::MemoryManager& memory_manager, ECS::Texture& texture, const std::string& path);

}

#endif
