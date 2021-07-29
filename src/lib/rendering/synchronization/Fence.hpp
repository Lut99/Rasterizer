/* FENCE.hpp
 *   by Lut99
 *
 * Created:
 *   28/06/2021, 16:57:12
 * Last edited:
 *   28/06/2021, 16:57:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that wraps a VkFence object.
**/

#ifndef RENDERING_FENCE_HPP
#define RENDERING_FENCE_HPP

#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

namespace Rasterizer::Rendering {
    /* The Fence class, which wraps a VkFence object and manages its memory. */
    class Fence {
    public:
        /* The GPU where the semaphore lives. */
        const Rendering::GPU& gpu;

    private:
        /* The VkFence object we wrap. */
        VkFence vk_fence;
        /* The flags used to create the fence. */
        VkFenceCreateFlags vk_create_flags;
    
    public:
        /* Constructor for the Fence class, which takes completely nothing! (except for the GPU where it lives, obviously, and optionally some flags) */
        Fence(const Rendering::GPU& gpu, VkFenceCreateFlags create_flags = 0);
        /* Copy constructor for the Fence class. */
        Fence(const Fence& other);
        /* Move constructor for the Fence class. */
        Fence(Fence&& other);
        /* Destructor for the Fence class. */
        ~Fence();

        /* Expliticly returns the internal VkFence object. */
        inline const VkFence& fence() const { return this->vk_fence; }
        /* Implicitly returns the internal VkFence object. */
        inline operator VkFence() const { return this->vk_fence; }

        /* Copy assignment operator for the Fence class. */
        inline Fence& operator=(const Fence& other) { return *this = Fence(other); }
        /* Move assignment operator for the Fence class. */
        inline Fence& operator=(Fence&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Fence class. */
        friend void swap(Fence& s1, Fence& s2);

    };

    /* Swap operator for the Fence class. */
    void swap(Fence& f1, Fence& f2);
}

#endif
