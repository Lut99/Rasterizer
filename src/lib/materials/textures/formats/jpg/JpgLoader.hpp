/* JPG LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   06/09/2021, 14:47:46
 * Last edited:
 *   9/19/2021, 5:56:38 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Function that loads the given file as if it were a .jpg picture.
**/

#ifndef MATERIALS_JPG_LOADER_HPP
#define MATERIALS_JPG_LOADER_HPP

#include <string>

#include "rendering/memory/Image.hpp"
#include "rendering/memory_manager/MemoryManager.hpp"

#include "../../Texture.hpp"

namespace Makma3D::Materials {
    /* Loads the file at the given path as a .jpg file, and returns a populated Image. */
    Rendering::Image* load_jpg_texture(Rendering::MemoryManager& memory_manager, const std::string& path);

}

#endif
