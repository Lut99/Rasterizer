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

#include <sstream>
#include "tools/Common.hpp"
#include "tools/Logger.hpp"

#include "formats/png/PngLoader.hpp"
#include "formats/jpg/JpgLoader.hpp"
#include "TextureSystem.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Textures;


/***** HELPER FUNCTIONS *****/
/* Converts given string to lowercase. */
static std::string tolower(const std::string& s) {
    std::stringstream sstr;
    for (uint32_t i = 0; i < s.size(); i++) {
        sstr << (char) tolower((int) s[i]);
    }
    return sstr.str();
}





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



/* Given a string filename/filepath, deduces any of the possible TextureFormats from it. Will throw errors if no such format is found. */
TextureFormat TextureSystem::deduce_format(const std::string& path) {
    // Simply look at the end of the string to find the extensions
    if (path.size() >= 4 && tolower(path.substr(path.size() - 4)) == ".png") {
        return TextureFormat::png;
    } else if ((path.size() >= 4 && tolower(path.substr(path.size() - 4)) == ".jpg") || (path.size() >= 5 && tolower(path.substr(path.size() - 5)) == ".jpeg")) {
        return TextureFormat::jpg;
    } else {
        logger.fatalc(TextureSystem::channel, "Could not deduce texture format from path '", path, "'");
    }
}



/* Loads a new texture from the given file with the given format into a Texture struct and returns that. */
Texture TextureSystem::load_texture(const std::string& path, TextureFormat format) {
    // Change the given path in a full path
    std::string fullpath = get_executable_path() + (path.size() > 0 && (path[0] == '/' || path[0] == '\\') ? "" : "/") + path;

    // Load the texture according to the given format
    Texture texture{};
    switch (format) {
        case TextureFormat::png:
            // Use the load function from the pngloader
            logger.logc(Verbosity::details, TextureSystem::channel, "Loading '", fullpath, "' as .png file...");
            load_png_texture(this->memory_manager, texture, fullpath);
            break;

        case TextureFormat::jpg:
            // Use the load function from the jpgloader
            logger.logc(Verbosity::details, TextureSystem::channel, "Loading '", fullpath, "' as .jpg/.jpeg file...");
            load_jpg_texture(this->memory_manager, texture, fullpath);
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
    return texture;
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
