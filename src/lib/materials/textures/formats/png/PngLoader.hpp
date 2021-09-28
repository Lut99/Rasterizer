/* PNG LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 12:14:46
 * Last edited:
 *   9/19/2021, 5:56:34 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Function that loads the given file as if it were a file in PNG-format.
**/

#ifndef MATERIALS_PNG_LOADER_HPP
#define MATERIALS_PNG_LOADER_HPP

#include <string>

#include "rendering/memory/Image.hpp"
#include "rendering/memory_manager/MemoryManager.hpp"

#include "../../Texture.hpp"

namespace Makma3D::Materials {
    /* Loads the file at the given path as a .png file, and returns a populated Image. */
    Rendering::Image* load_png_texture(Rendering::MemoryManager& memory_manager, const std::string& path);

}

#endif
