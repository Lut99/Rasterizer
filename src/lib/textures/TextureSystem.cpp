/* TEXTURE SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:49:36
 * Last edited:
 *   16/08/2021, 11:49:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the TextureSystem class, which is responsible for loading and
 *   managing textures for an entity.
**/

#include "tools/Logger.hpp"

#include "formats/png/PngLoader.hpp"
#include "formats/jpg/JpgLoader.hpp"
#include "TextureSystem.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Textures;


/***** TEXTURESYSTEM CLASS *****/
/* Constructor for the TextureSystem class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
TextureSystem::TextureSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager),
    sampler_pool(memory_manager.gpu)
{
    logger.logc(Verbosity::important, TextureSystem::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, TextureSystem::channel, "Init success.");
}

/* Move constructor for the TextureSystem class. */
TextureSystem::TextureSystem(TextureSystem&& other) :
    memory_manager(other.memory_manager),
    sampler_pool(std::move(other.sampler_pool))
{}

/* Destructor for the TextureSystem class. */
TextureSystem::~TextureSystem() {
    logger.logc(Verbosity::important, TextureSystem::channel, "Cleaning...");

    // Nothing as of yet

    logger.logc(Verbosity::important, TextureSystem::channel, "Cleaned.");
}



/* Loads a new texture from the given file with the given format into a Texture struct and returns that. */
Texture TextureSystem::load_texture(const std::string& path, TextureFormat format = TextureFormat::png) {
    // Load the texture according to the given format
    Texture texture{};
    switch (format) {
        case TextureFormat::png:
            // Use the load function from the pngloader
            logger.logc(Verbosity::details, TextureSystem::channel, "Loading '", path, "' as .png file...");
            load_png_texture(this->memory_manager, texture, path);
            break;

        case TextureFormat::jpg:
            // Use the load function from the jpgloader
            logger.logc(Verbosity::details, TextureSystem::channel, "Loading '", path, "' as .jpg/.jpeg file...");
            load_jpg_texture(this->memory_manager, texture, path);
            break;

        default:
            logger.fatalc(TextureSystem::channel, "Unsupported texture format '", texture_format_names[(int) format], '\'');
            break;

    }
    // Do a debug print just for fun
    logger.logc(Verbosity::debug, TextureSystem::channel, "Loaded texture of ", texture.extent.width, 'x', texture.extent.height, " pixels.");

    // Initialize the image view for this image
    texture.view = this->memory_manager.view_pool.allocate(texture.image, VK_FORMAT_R8G8B8A8_SRGB);
    // And the Sampler
    texture.sampler = this->sampler_pool.allocate(VK_FILTER_LINEAR, VK_TRUE);

    // Done
}

/* Destroys the data in the given texture struct. */
void TextureSystem::unload_texture(const Texture& texture) {
    // Deallocate the sampler
    this->sampler_pool.free(texture.sampler);
    // Deallocate the view
    this->memory_manager.view_pool.free(texture.view);
    // Deallocate the image
    this->memory_manager.draw_pool.free(texture.image);
}



/* Swap operator for the TextureSystem class. */
void Makma3D::Textures::swap(TextureSystem& ts1, TextureSystem& ts2) {
    #ifndef NDEBUG
    if (&ts1.memory_manager != &ts2.memory_manager) { logger.fatalc(TextureSystem::channel, "Cannot swap texture systems with different memory managers."); }
    #endif

    // Simply swap it all
    using std::swap;

    swap(ts1.sampler_pool, ts2.sampler_pool);
}
