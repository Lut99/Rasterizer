/* SAMPLER POOL.cpp
 *   by Lut99
 *
 * Created:
 *   23/09/2021, 14:21:03
 * Last edited:
 *   23/09/2021, 14:21:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The SamplerPool class, which can be used to allocate new image
 *   samplers.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "SamplerPool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


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





/***** SAMPLERPOOL CLASS *****/
/* Constructor for the SamplerPool class, which takes the GPU where the future samplers will live. */
SamplerPool::SamplerPool(const Rendering::GPU& gpu) :
    gpu(gpu)
{}

/* Move constructor for the SamplerPool class. */
SamplerPool::SamplerPool(SamplerPool&& other) :
    gpu(other.gpu),

    samplers(other.samplers)
{
    // The Array's move constructor guarantees that it's empty in the other object
}

/* Destructor for the SamplerPool class. */
SamplerPool::~SamplerPool() {
    if (this->samplers.size() > 0) {
        for (const Rendering::Sampler* sampler : this->samplers) {
            vkDestroySampler(this->gpu, sampler->vk_sampler, nullptr);
            delete sampler;
        }
    }
}



/* Allocates a new Sampler with the given properties. The max anisotropy might be overriden by the hardware if that's lower (which is what happens in the default case). */
Rendering::Sampler* SamplerPool::allocate(VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy = INFINITY) {
    VkResult vk_result;

    // Fetch the maximum allowed anisotropy filter for the chosen device
    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(this->gpu.physical_device(), &gpu_props);
    max_anisotropy = std::min({ max_anisotropy, gpu_props.limits.maxSamplerAnisotropy });

    // Populate the create info for the sampler
    VkSamplerCreateInfo sampler_info;
    populate_sampler_info(sampler_info, filter, enable_anisotropy, max_anisotropy);

    // Create the sampler
    VkSampler vk_sampler;
    if ((vk_result = vkCreateSampler(this->gpu, &sampler_info, nullptr, &vk_sampler)) != VK_SUCCESS) {
        logger.fatalc(SamplerPool::channel, "Could not create sampler: ", vk_error_map[vk_result]);
    }

    // Wrap it in a Sampler and add to the internal list
    Sampler* result = new Sampler(this->gpu, vk_sampler);
    this->samplers.insert(result);

    // Done
    return result;
}

/* Frees the given Sampler. Will throw errors if it isn't allocated with this pool. */
void SamplerPool::free(const Rendering::Sampler* sampler) {
    // Try to find the sampler in the internal list
    std::unordered_set<Rendering::Sampler*>::iterator iter = this->samplers.find(const_cast<Rendering::Sampler*>(sampler));
    if (iter == this->samplers.end()) {
        logger.fatalc(SamplerPool::channel, "Cannot free a Sampler that is not allocated with this pool.");
    }

    // Destroy the actual VkSampler & the pointer
    vkDestroySampler(this->gpu, (*iter)->vk_sampler, nullptr);
    delete *iter;

    // Delete it from the set
    this->samplers.erase(iter);
}



/* Swap operator for the SamplerPool class. */
void Rendering::swap(SamplerPool& sp1, SamplerPool& sp2) {
    #ifndef NDEBUG
    if (sp1.gpu != sp2.gpu) { logger.fatalc(SamplerPool::channel, "Cannot swap sampler pools with different GPUs"); }
    #endif

    using std::swap;

    swap(sp1.samplers, sp2.samplers);
}
