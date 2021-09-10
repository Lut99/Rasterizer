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
        /* Channel name for the Sampler class. */
        static constexpr const char* channel = "Sampler";

        /* The GPU where the Sampler lives. */
        const Rendering::GPU& gpu;

    private:
        /* The VkSampler object we wrap. */
        VkSampler vk_sampler;

        /* The linear mode for the sampler. */
        VkFilter vk_linear_mode;
        /* Whether or not to enable anisotropy. */
        VkBool32 vk_enable_anisotropy;
        /* The maximum level of anisotropy (if enabled). */
        float vk_max_anisotropy_level;

    public:
        /* Constructor for the Sampler class, which takes the GPU where the Sampler will live, what kind of filtering to use and whether anisotropy is enabled. */
        Sampler(const Rendering::GPU& gpu, VkFilter filter, VkBool32 enable_anisotropy);
        /* Copy constructor for the Sampler class. */
        Sampler(const Sampler& other);
        /* Move constructor for the Sampler class. */
        Sampler(Sampler&& other);
        /* Destructor for the Sampler class. */
        ~Sampler();

        /* Explicitly returns the internal Sampler object. */
        inline const VkSampler& sampler() const { return this->vk_sampler; }
        /* Implicitly returns the internal Sampler object. */
        inline operator const VkSampler&() const { return this->vk_sampler; }

        /* Copy assignment operator for the Sampler class. */
        inline Sampler& operator=(const Sampler& other) { return *this = Sampler(other); }
        /* Move assignment operator for the Sampler class. */
        inline Sampler& operator=(Sampler&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Sampler class. */
        friend void swap(Sampler& s1, Sampler& s2);

    };

    /* Swap operator for the Sampler class. */
    void swap(Sampler& s1, Sampler& s2);

}

#endif
