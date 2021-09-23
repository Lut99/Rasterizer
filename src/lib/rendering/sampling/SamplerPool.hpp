/* SAMPLER POOL.hpp
 *   by Lut99
 *
 * Created:
 *   23/09/2021, 14:20:59
 * Last edited:
 *   23/09/2021, 14:20:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The SamplerPool class, which can be used to allocate new image
 *   samplers.
**/

#ifndef RENDERING_SAMPLER_POOL_HPP
#define RENDERING_SAMPLER_POOL_HPP

#include <unordered_set>

#include "../gpu/GPU.hpp"

#include "Sampler.hpp"

namespace Makma3D::Rendering {
    /* The SamplerPool class, which allocated and manages image Samplers. */
    class SamplerPool {
    public:
        /* Channel name for the SamplerPool class. */
        static constexpr const char* channel = "SamplerPool";

        /* Reference to the GPU where all samplers will live. */
        const Rendering::GPU& gpu;
    
    private:
        /* The list of Samplers that we allocated. */
        std::unordered_set<Rendering::Sampler*> samplers;

    public:
        /* Constructor for the SamplerPool class, which takes the GPU where the future samplers will live. */
        SamplerPool(const Rendering::GPU& gpu);
        /* Copy constructor for the SamplerPool class, which is deleted. */
        SamplerPool(const SamplerPool& other) = delete;
        /* Move constructor for the SamplerPool class. */
        SamplerPool(SamplerPool&& other);
        /* Destructor for the SamplerPool class. */
        ~SamplerPool();

        /* Allocates a new Sampler with the given properties. The max anisotropy might be overriden by the hardware if that's lower (which is what happens in the default case). */
        Rendering::Sampler* allocate(VkFilter filter, VkBool32 enable_anisotropy, float max_anisotropy = INFINITY);
        /* Frees the given Sampler. Will throw errors if it isn't allocated with this pool. */
        void free(const Rendering::Sampler* sampler);

        /* Copy assignment operator for the SamplerPool class, which is deleted. */
        SamplerPool& operator=(const SamplerPool& other) = delete;
        /* Move assignment operator for the SamplerPool class. */
        inline SamplerPool& operator=(SamplerPool&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the SamplerPool class. */
        friend void swap(SamplerPool& sp1, SamplerPool& sp2);

    };
    
    /* Swap operator for the SamplerPool class. */
    void swap(SamplerPool& sp1, SamplerPool& sp2);

}

#endif
