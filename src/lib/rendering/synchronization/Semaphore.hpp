/* SEMAPHORE.hpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 15:26:11
 * Last edited:
 *   27/06/2021, 15:26:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Wraps the VkSemaphore class, mostly managing its memory.
**/

#ifndef RENDERING_SEMAPHORE_HPP
#define RENDERING_SEMAPHORE_HPP

#include <vulkan/vulkan.h>

#include "../gpu/GPU.hpp"

namespace Rasterizer::Rendering {
    /* The Semaphore class, which wraps a VkSemaphore object and manages its memory. */
    class Semaphore {
    public:
        /* Channel name for the Semaphore class. */
        static constexpr const char* channel = "Semaphore";

        /* The GPU where the semaphore lives. */
        const Rendering::GPU& gpu;

    private:
        /* The VkSemaphore object we wrap. */
        VkSemaphore vk_semaphore;
    
    public:
        /* Constructor for the Semaphore class, which takes completely nothing! (except for the GPU where it lives, obviously) */
        Semaphore(const Rendering::GPU& gpu);
        /* Copy constructor for the Semaphore class. */
        Semaphore(const Semaphore& other);
        /* Move constructor for the Semaphore class. */
        Semaphore(Semaphore&& other);
        /* Destructor for the Semaphore class. */
        ~Semaphore();

        /* Expliticly returns the internal VkSemaphore object. */
        inline const VkSemaphore& semaphore() const { return this->vk_semaphore; }
        /* Implicitly returns the internal VkSemaphore object. */
        inline operator VkSemaphore() const { return this->vk_semaphore; }

        /* Copy assignment operator for the Semaphore class. */
        inline Semaphore& operator=(const Semaphore& other) { return *this = Semaphore(other); }
        /* Move assignment operator for the Semaphore class. */
        inline Semaphore& operator=(Semaphore&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Semaphore class. */
        friend void swap(Semaphore& s1, Semaphore& s2);

    };

    /* Swap operator for the Semaphore class. */
    void swap(Semaphore& s1, Semaphore& s2);
}

#endif
