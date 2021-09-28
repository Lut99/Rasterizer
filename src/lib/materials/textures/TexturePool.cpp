/* TEXTURE POOL.cpp
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
 *   Contains the TexturePool class, which is responsible for loading and
 *   managing textures. By pooling it, it also tries to minimise data
 *   duplication by avoiding double loads of the same file.
**/

#include <sstream>
#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "rendering/auxillary/ErrorCodes.hpp"

#include "formats/png/PngLoader.hpp"
#include "formats/jpg/JpgLoader.hpp"
#include "TexturePool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** HELPER FUNCTIONS *****/
/* Converts given string to lowercase. */
static std::string tolower(const std::string& s) {
    std::stringstream sstr;
    for (uint32_t i = 0; i < s.size(); i++) {
        sstr << (char) tolower((int) s[i]);
    }
    return sstr.str();
}





/***** POPULATE FUNCTIONS *****/
/* Populates the given VkImageViewCreateInfo struct. */
static void populate_view_info(VkImageViewCreateInfo& view_info, VkImage vk_image, VkFormat vk_format) {
    // Set to default
    view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    // Set the image and its type (mostly how many dimensions etc)
    view_info.image = vk_image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

    // Set the format of the image view
    view_info.format = vk_format;

    // Set the subresource stuff
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
}

/* Populates the given VkSamplerCreateInfo struct. */
static void populate_sampler_info(VkSamplerCreateInfo& sampler_info, VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy_level) {
    // Set to default
    sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    // Set the filtering mode. For the magnified filter, it concerns oversampling: we have more fragments than texels, i.e., our texture resolution is too low. For the minified filter, we look at undersampling, meaning that our image resolution is too high and we thus wonder how to compress (especially in 3D)
    sampler_info.magFilter = filter;
    sampler_info.minFilter = filter;

    // Set the address mode (specified per axis), which determines how to deal with going out of bounds of the image.
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    // Set the anisotropy filtering mode
    sampler_info.anisotropyEnable = enable_anisotropy;
    sampler_info.maxAnisotropy = max_anisotropy_level;

    // Set the border colour for the image in case our address mode is clamp-to-border. Not our case, and thus irrelevant.
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    // Determine whether to use normalized texture coordinates or not. (Tip: you prolly want to say yes to this)
    sampler_info.unnormalizedCoordinates = VK_FALSE;

    // Define a possible operation to do before using the texel values (disabled for now)
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

    // Set the mipmap filter properties (for later)
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;
}





/***** TEXTUREPOOL CLASS *****/
/* Constructor for the TexturePool class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
TexturePool::TexturePool(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager)
{
    logger.logc(Verbosity::important, TexturePool::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, TexturePool::channel, "Init success.");
}

/* Move constructor for the TexturePool class. */
TexturePool::TexturePool(TexturePool&& other) :
    memory_manager(other.memory_manager),
    textures(std::move(other.textures))
{
    this->textures.clear();
}

/* Destructor for the TexturePool class. */
TexturePool::~TexturePool() {
    logger.logc(Verbosity::important, TexturePool::channel, "Cleaning...");

    // Clear all textures
    if (!this->textures.empty()) {
        logger.logc(Verbosity::details, TexturePool::channel, "Cleaning ", this->textures.size(), " textures...");
        for (const auto& p : this->textures) {
            // Free the texture's structures
            vkDestroySampler(this->memory_manager.gpu, p.second->_sampler, nullptr);
            vkDestroyImageView(this->memory_manager.gpu, p.second->_view, nullptr);
            this->memory_manager.draw_pool.free(p.second->_image);

            // Delete the pointer
            delete p.second;
        }
    }

    logger.logc(Verbosity::important, TexturePool::channel, "Cleaned.");
}



/* Given a string filename/filepath, deduces any of the possible TextureFormats from it. Will throw errors if no such format is found. */
TextureFormat TexturePool::deduce_format(const std::string& path) {
    // Simply look at the end of the string to find the extensions
    if (path.size() >= 4 && tolower(path.substr(path.size() - 4)) == ".png") {
        return TextureFormat::png;
    } else if ((path.size() >= 4 && tolower(path.substr(path.size() - 4)) == ".jpg") || (path.size() >= 5 && tolower(path.substr(path.size() - 5)) == ".jpeg")) {
        return TextureFormat::jpg;
    } else {
        logger.fatalc(TexturePool::channel, "Could not deduce texture format from path '", path, "'");
    }
}



/* Allocates a new Texture object, and populates it with the data loaded at the given path as the given format. The filter and by how much to apply anisotropy apply to the texture's sampler. If the given format is "automatic", tries to deduce the format from the file's extension. */
Materials::Texture* TexturePool::allocate(const std::string& path, VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy_level, TextureFormat format) {
    VkResult vk_result;

    // Change the given path to a full path
    std::string fullpath = Tools::merge_paths(get_executable_path(), path);

    // Check if we have already seen this path
    std::unordered_map<std::string, Materials::Texture*>::iterator iter = this->textures.find(fullpath);
    if (iter != this->textures.end()) {
        // Simply return that texture instead
        return (*iter).second;
    }

    // Load the texture we'll need according to the given format
    Rendering::Image* image;
    switch(format) {
        case TextureFormat::automatic:
            // Retry the function with the found format
            return this->allocate(path, filter, enable_anisotropy, max_anisotropy_level, TexturePool::deduce_format(path));

        case TextureFormat::png:
            logger.logc(Verbosity::details, TexturePool::channel, "Loading '", fullpath, "' as .png file...");
            image = load_png_texture(this->memory_manager, fullpath);
            break;

        case TextureFormat::jpg:
            // Use the load function from the jpgloader
            logger.logc(Verbosity::details, TexturePool::channel, "Loading '", fullpath, "' as .jpg/.jpeg file...");
            image = load_jpg_texture(this->memory_manager, fullpath);
            break;

        default:
            logger.fatalc(TexturePool::channel, "Unsupported texture format '", texture_format_names[(int) format], '\'');

    }

    // Next, create an image view for this image
    VkImageViewCreateInfo view_info;
    populate_view_info(view_info, image->vulkan(), image->format());
    VkImageView view;
    if ((vk_result = vkCreateImageView(this->memory_manager.gpu, &view_info, nullptr, &view)) != VK_SUCCESS) {
        logger.fatalc(TexturePool::channel, "Cannot create image view: ", Rendering::vk_error_map[vk_result]);
    }

    // Then, create the Sampler
    VkSamplerCreateInfo sampler_info;
    populate_sampler_info(sampler_info, VK_FILTER_LINEAR, VK_TRUE, INFINITY);
    VkSampler sampler;
    if ((vk_result = vkCreateSampler(this->memory_manager.gpu, &sampler_info, nullptr, &sampler)) != VK_SUCCESS) {
        logger.fatalc(TexturePool::channel, "Cannot create sampler: ", Rendering::vk_error_map[vk_result]);
    }

    // We can now create the Texture wrapper
    Texture* result = new Texture(image, view, sampler, fullpath);
    this->textures.insert({ fullpath, result });

    // Done
    return result;
}

/* Destroys the given texture, freeing memory again. */
void TexturePool::free(const Materials::Texture* texture) {
    // First, check if this texture is allocated with this pool
    std::unordered_map<std::string, Materials::Texture*>::iterator iter = this->textures.find(texture->_name);
    if (iter == this->textures.end()) {
        logger.fatalc(TexturePool::channel, "Cannot free Texture '", texture->_name, "' that wasn't allocated with this pool.");
    }

    // Free the texture's structures
    vkDestroySampler(this->memory_manager.gpu, texture->_sampler, nullptr);
    vkDestroyImageView(this->memory_manager.gpu, texture->_view, nullptr);
    this->memory_manager.draw_pool.free(texture->_image);

    // Delete the pointer
    delete texture;

    // Remove it from the list
    this->textures.erase(iter);
}



/* Swap operator for the TexturePool class. */
void Materials::swap(TexturePool& tp1, TexturePool& tp2) {
    #ifndef NDEBUG
    if (&tp1.memory_manager != &tp2.memory_manager) { logger.fatalc(TexturePool::channel, "Cannot swap texture pools with different memory managers."); }
    #endif

    // Simply swap it all
    using std::swap;

    swap(tp1.textures, tp2.textures);
}
