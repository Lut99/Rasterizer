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
using namespace Makma3D;
using namespace Makma3D::Textures;


/***** CONSTANTS *****/
/* Channel name for the PngLoader function. */
static constexpr const char* channel = "PngLoader";





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .png file, and populates the given Texture struct from it. */
void Textures::load_png_texture(Rendering::MemoryManager& memory_manager, Texture& texture, const std::string& path) {
    // Try to load the given .png file with lodepng
    std::vector<unsigned char> png;
    unsigned error = lodepng::load_file(png, path);
    if (error) {
        logger.fatalc(channel, "Could not load '", path, "' as .png file: ", lodepng_error_text(error), " (error code ", error, ")");
    }

    // Decode the .png to raw image data
    std::vector<unsigned char> image;
    unsigned width, height;
    error = lodepng::decode(image, width, height, png);
    if (error) {
        logger.fatalc(channel, "Could not decode '", path, "' as .png file: ", lodepng_error_text(error), " (error code ", std::to_string(error), ")");
    }

    // With the pixels in memory, allocate the Vulkan image
    texture.extent = VkExtent2D({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    texture.image = memory_manager.draw_pool.allocate(texture.extent, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    // Allocate a staging buffer
    VkDeviceSize texture_size = 4 * texture.extent.width * texture.extent.height * sizeof(unsigned char);
    Rendering::Buffer* stage = memory_manager.stage_pool.allocate(texture_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    void* stage_mem;
    stage->map(&stage_mem);

    // Copy the image to the stage buffer
    memcpy(stage_mem, image.data(), texture_size);
    stage->flush();
    stage->unmap();

    // Copy the stage memory to the image once its in the correct layout
    Rendering::CommandBuffer* draw_cmd = memory_manager.draw_cmd_pool.allocate();
    stage->copyto(texture.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, draw_cmd, memory_manager.gpu.queues(Rendering::QueueType::graphics)[0]);
    memory_manager.draw_cmd_pool.free(draw_cmd);

    // Deallocate the staging buffer
    memory_manager.stage_pool.free(stage);
}
