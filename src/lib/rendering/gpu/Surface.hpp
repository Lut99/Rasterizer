/* SURFACE.hpp
 *   by Lut99
 *
 * Created:
 *   11/06/2021, 15:39:38
 * Last edited:
 *   11/06/2021, 15:39:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Surface class, which wraps a VkSurfaceKHR created from a
 *   GLFW window.
**/

#ifndef RENDERING_SURFACE_HPP
#define RENDERING_SURFACE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../instance/Instance.hpp"

namespace Rasterizer::Rendering {
    /* The Surface class, which wraps a VkSurfaceKHR and manages its lifetime. */
    class Surface {
    public:
        /* Channel name for the Surface class. */
        static constexpr const char* channel = "Surface";
        /* The Vulkan instance to which the surface is bound. */
        const Instance& instance;
    
    private:
        /* The VkSurfaceKHR object we wrap. */
        VkSurfaceKHR vk_surface;

    public:
        /* Constructor for the Surface class, which takes an instance and the GLFW window to create a surface from. */
        Surface(const Instance& instance, GLFWwindow* glfw_window);
        /* Copy constructor for the Surface class, which is deleted as a window only has one surface at a time. */
        Surface(const Surface& other) = delete;
        /* Move constructor for the Surface class. */
        Surface(Surface&& other);
        /* Destructor for the Surface class. */
        ~Surface();

        /* Expliticly returns the internal VkSurfaceKHR object. */
        inline const VkSurfaceKHR& surface() const { return this->vk_surface; }
        /* Implicitly returns the internal VkSurfaceKHR object. */
        inline operator VkSurfaceKHR() const { return this->vk_surface; }

        /* Copy assignment operator for the Surface class, which is obviously deleted. */
        Surface& operator=(const Surface& other) = delete;
        /* Move assignment operator for the Surface class. */
        inline Surface& operator=(Surface&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Surface class. */
        friend void swap(Surface& s1, Surface& s2);

    };

    /* Swap operator for the Surface class. */
    void swap(Surface& s1, Surface& s2);

}

#endif
