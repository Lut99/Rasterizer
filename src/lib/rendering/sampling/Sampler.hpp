/* SAMPLER.hpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:46:33
 * Last edited:
 *   25/08/2021, 16:46:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Sampler class, which wraps the a VkSampler object. Used
 *   to allow access to textures in shaders.
**/

#ifndef RENDERING_SAMPLER_HPP
#define RENDERING_SAMPLER_HPP

#include <vulkan/vulkan.h>
#include "../gpu/GPU.hpp"

namespace Makma3D::Rendering {
    /* The Sampler class, which allows shaders to sample texture images. */
    class Sampler {
    public:
        /* The GPU where the Sampler lives. */
        const Rendering::GPU& gpu;

    private:
        /* The VkSampler object we wrap. */
        VkSampler vk_sampler;


        /* Constructor for the Sampler class, which takes the GPU where the Sampler lives and the vk_sampler object to wrap. */
        Sampler(const Rendering::GPU& gpu, const VkSampler& vk_sampler);
        /* Destructor for the Sampler class. */
        ~Sampler();

        /* Declare the SamplerPool as our fwiend. */
        friend class SamplerPool;

    public:
        /* Copy constructor for the Sampler class, which is deleted. */
        Sampler(const Sampler& other) = delete;
        /* Move constructor for the Sampler class, which is deleted. */
        Sampler(Sampler&& other) = delete;

        /* Explicitly returns the internal Sampler object. */
        inline const VkSampler& vulkan() const { return this->vk_sampler; }
        /* Implicitly returns the internal Sampler object. */
        inline operator const VkSampler&() const { return this->vk_sampler; }

        /* Copy assignment operator for the Sampler class, which is deleted. */
        Sampler& operator=(const Sampler& other) = delete;
        /* Move assignment operator for the Sampler class, which is deleted. */
        Sampler& operator=(Sampler&& other) = delete;

    };
}

#endif
