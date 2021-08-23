/* PNG LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 12:22:16
 * Last edited:
 *   16/08/2021, 12:22:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Function that loads the given file as if it were a file in PNG-format.
 *   Populates the given Texture component.
**/

#include <vector>
#include "LodePNG.hpp"
#include "PngLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Textures;


/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .png file, and populates the given Texture component from it. */
void Textures::load_png_texture(Rendering::MemoryManager& memory_manager, ECS::Texture& texture, const std::string& path) {
    

    // Try to load the given .png file with lodepng
    std::vector<unsigned char> png;
    unsigned error = lodepng::load_file(png, path);
    if (error) {
        DLOG(fatal, "Could not load '" + path + "' as .png file: " + lodepng_error_text(error) + " (error code " + std::to_string(error) + ")");
    }

    // Decode the .png to raw image data
    std::vector<unsigned char> image;
    unsigned width, height;
    error = lodepng::decode(image, width, height, png);
    if (error) {
        DLOG(fatal, "Could not decode '" + path + "' as .png file: " + lodepng_error_text(error) + " (error code " + std::to_string(error) + ")");
    }

    // With the pixels in memory, allocate the Vulkan image
    texture.extent = VkExtent2D({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    texture.image_h = memory_manager.draw_pool.allocate_image_h(texture.extent, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    Rendering::Image image = memory_manager.draw_pool.deref_image(texture.image_h);

    // Allocate a staging buffer
    VkDeviceSize texture_size = 4 * texture.extent.width * texture.extent.height * sizeof(unsigned char);
    Rendering::Buffer stage = memory_manager.stage_pool.allocate_buffer(texture_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_mem;
    stage.map(&stage_mem);

    // Copy the image to the stage buffer
    memcpy(stage_mem, image.data(), texture_size);
    stage.flush();
    stage.unmap();

    // Copy the stage memory to the image
    memory_manager.
    stage.copyto(image, memory_manager.copy_cmd());

    return;
}
