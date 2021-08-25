/* SAMPLER.cpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:46:36
 * Last edited:
 *   25/08/2021, 16:46:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Sampler class, which wraps the a VkSampler object. Used
 *   to allow access to textures in shaders.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "Sampler.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
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





/***** SAMPLER CLASS *****/
/* Constructor for the Sampler class, which takes the GPU where the Sampler will live, what kind of filtering to use and whether anisotropy is enabled. */
Sampler::Sampler(const Rendering::GPU& gpu, VkFilter filter, VkBool32 enable_anisotropy) :
    gpu(gpu),

    vk_linear_mode(filter),
    vk_enable_anisotropy(enable_anisotropy && this->gpu.supports_anisotropy())
{
    logger.logc(Verbosity::details, Sampler::channel, "Intializing...");

    // Fetch the maximum allowed anisotropy filter for the chosen device
    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(this->gpu.physical_device(), &gpu_props);
    this->vk_max_anisotropy_level = gpu_props.limits.maxSamplerAnisotropy;

    // Populate the create info for the sampler
    VkSamplerCreateInfo sampler_info;
    populate_sampler_info(sampler_info, this->vk_linear_mode, this->vk_enable_anisotropy, this->vk_max_anisotropy_level);

    // Create the sampler
    VkResult vk_result;
    if ((vk_result = vkCreateSampler(this->gpu, &sampler_info, nullptr, &this->vk_sampler)) != VK_SUCCESS) {
        logger.fatalc(Sampler::channel, "Could not create sampler: ", vk_error_map[vk_result]);
    }
    
    logger.logc(Verbosity::details, Sampler::channel, "Init success.");
}

/* Copy constructor for the Sampler class. */
Sampler::Sampler(const Sampler& other) :
    gpu(other.gpu),

    vk_linear_mode(other.vk_linear_mode),
    vk_enable_anisotropy(other.vk_enable_anisotropy),
    vk_max_anisotropy_level(other.vk_max_anisotropy_level)
{
    logger.logc(Verbosity::debug, Sampler::channel, "Copying...");

    // Populate the create info for the sampler
    VkSamplerCreateInfo sampler_info;
    populate_sampler_info(sampler_info, this->vk_linear_mode, this->vk_enable_anisotropy, this->vk_max_anisotropy_level);

    // Create the sampler
    VkResult vk_result;
    if ((vk_result = vkCreateSampler(this->gpu, &sampler_info, nullptr, &this->vk_sampler)) != VK_SUCCESS) {
        logger.fatalc(Sampler::channel, "Could not create sampler copy: ", vk_error_map[vk_result]);
    }
    
    logger.logc(Verbosity::debug, Sampler::channel, "Copy success.");
}

/* Move constructor for the Sampler class. */
Sampler::Sampler(Sampler&& other) :
    gpu(other.gpu),

    vk_sampler(other.vk_sampler),

    vk_linear_mode(other.vk_linear_mode),
    vk_enable_anisotropy(other.vk_enable_anisotropy),
    vk_max_anisotropy_level(other.vk_max_anisotropy_level)
{
    other.vk_sampler = nullptr;
}

/* Destructor for the Sampler class. */
Sampler::~Sampler() {
    logger.logc(Verbosity::details, Sampler::channel, "Cleaning...");

    if (this->vk_sampler != nullptr) {
        logger.logc(Verbosity::details, Sampler::channel, "Destoying VkSampler...");
        vkDestroySampler(this->gpu, this->vk_sampler, nullptr);
    }
    
    logger.logc(Verbosity::details, Sampler::channel, "Cleaned.");
}



/* Swap operator for the Sampler class. */
void Rendering::swap(Sampler& s1, Sampler& s2) {
    #ifndef NDEBUG
    if (s1.gpu != s2.gpu) { logger.fatalc(Sampler::channel, "Cannot swap samplers with different GPUs"); }
    #endif

    using std::swap;

    swap(s1.vk_sampler, s2.vk_sampler);

    swap(s1.vk_linear_mode, s2.vk_linear_mode);
    swap(s1.vk_enable_anisotropy, s2.vk_enable_anisotropy);
    swap(s1.vk_max_anisotropy_level, s2.vk_max_anisotropy_level);
}
