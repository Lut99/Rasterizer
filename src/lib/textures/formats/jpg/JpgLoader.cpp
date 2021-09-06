/* JPG LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   06/09/2021, 14:47:54
 * Last edited:
 *   06/09/2021, 14:47:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Function that loads the given file as if it were a .jpg picture.
**/

#include <vector>
#include <unordered_map>
#include "jpgd.h"
#include "JpgLoader.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Textures;


/***** MACROS *****/
/* Enters the given enum value in a map under its own name. */
#define JPGD_STATUS_ENTRY(VAL) \
    { (VAL), (#VAL) }





/***** CONSTANTS *****/
/* Channel name for the JpgLoader function. */
static constexpr const char* channel = "JpgLoader";

/* Error map from jpgd error codes to some kind of readable error message. */
static const std::unordered_map<jpgd::jpgd_status, std::string> jpgd_status_names = {
    JPGD_STATUS_ENTRY(jpgd::JPGD_SUCCESS),
    JPGD_STATUS_ENTRY(jpgd::JPGD_FAILED),
    JPGD_STATUS_ENTRY(jpgd::JPGD_DONE),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DHT_COUNTS),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DHT_INDEX),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DHT_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DQT_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DQT_TABLE), 
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_PRECISION),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_HEIGHT),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_WIDTH),
    JPGD_STATUS_ENTRY(jpgd::JPGD_TOO_MANY_COMPONENTS), 
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_SOF_LENGTH),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_VARIABLE_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DRI_LENGTH),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_SOS_LENGTH),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_SOS_COMP_ID),
    JPGD_STATUS_ENTRY(jpgd::JPGD_W_EXTRA_BYTES_BEFORE_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_NO_ARITHMITIC_SUPPORT),
    JPGD_STATUS_ENTRY(jpgd::JPGD_UNEXPECTED_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_NOT_JPEG),
    JPGD_STATUS_ENTRY(jpgd::JPGD_UNSUPPORTED_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_DQT_LENGTH),
    JPGD_STATUS_ENTRY(jpgd::JPGD_TOO_MANY_BLOCKS),
    JPGD_STATUS_ENTRY(jpgd::JPGD_UNDEFINED_QUANT_TABLE),
    JPGD_STATUS_ENTRY(jpgd::JPGD_UNDEFINED_HUFF_TABLE),
    JPGD_STATUS_ENTRY(jpgd::JPGD_NOT_SINGLE_SCAN),
    JPGD_STATUS_ENTRY(jpgd::JPGD_UNSUPPORTED_COLORSPACE),
    JPGD_STATUS_ENTRY(jpgd::JPGD_UNSUPPORTED_SAMP_FACTORS),
    JPGD_STATUS_ENTRY(jpgd::JPGD_DECODE_ERROR),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_RESTART_MARKER),
    JPGD_STATUS_ENTRY(jpgd::JPGD_ASSERTION_ERROR),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_SOS_SPECTRAL),
    JPGD_STATUS_ENTRY(jpgd::JPGD_BAD_SOS_SUCCESSIVE),
    JPGD_STATUS_ENTRY(jpgd::JPGD_STREAM_READ),
    JPGD_STATUS_ENTRY(jpgd::JPGD_NOTENOUGHMEM),
    JPGD_STATUS_ENTRY(jpgd::JPGD_FILE_ERROR)
};





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .jpg file, and populates the given Texture component from it. */
void Textures::load_jpg_texture(Rendering::MemoryManager& memory_manager, ECS::Texture& texture, const std::string& path) {
    // Try to load the given .pjpg file with the jpgd.h library. Note that we tell it to return as RGBA (the 4).
    int width, height, comps;
    jpgd::jpgd_status status;
    unsigned char* jpg = jpgd::decompress_jpeg_image_from_file(path.c_str(), &status, &width, &height, &comps, 4);
    if (jpg == NULL) {
        logger.fatalc(channel, "Could not load '", path, "' as .jpg file: ", jpgd_status_names.at(status), " (error code ", status, ")");
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
    memcpy(stage_mem, jpg, texture_size);
    stage->flush();
    stage->unmap();

    // Deallocate the image
    free(jpg);

    // Copy the stage memory to the image once its in the correct layout
    Rendering::CommandBuffer* draw_cmd = memory_manager.draw_cmd_pool.allocate();
    stage->copyto(texture.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, draw_cmd, memory_manager.gpu.queues(Rendering::QueueType::graphics)[0]);
    memory_manager.draw_cmd_pool.free(draw_cmd);

    // Deallocate the staging buffer
    memory_manager.stage_pool.free(stage);
}

