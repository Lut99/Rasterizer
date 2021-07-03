/* WINDOW.hpp
 *   by Lut99
 *
 * Created:
 *   02/07/2021, 13:45:00
 * Last edited:
 *   02/07/2021, 13:45:00
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Window class, which manages the GLFW window and bundles
 *   Vulkan instance, gpu, surface and swapchain together in one place.
**/

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "render_engine/instance/Instance.hpp"
#include "render_engine/gpu/Surface.hpp"
#include "render_engine/gpu/GPU.hpp"
#include "render_engine/swapchain/Swapchain.hpp"

namespace Rasterizer {
    /* The Window class, which manages the GLFW window and which bundles some base Vulkan classes in one place. */
    class Window {
    public:
        /* The Instance for the Window. */
        const Rendering::Instance& instance;

    private:
        /* The GLFWwindow object that we handle. */
        GLFWwindow* glfw_window;
        /* Vulkan's representation of the window. */
        Rendering::Surface* rendering_surface;
        /* The GPU which we use for this window. */
        Rendering::GPU* rendering_gpu;
        /* The Swapchain used to reach the window. */
        Rendering::Swapchain* rendering_swapchain;

        /* The title of the window. */
        std::string t;
        /* The target width of the window. */
        uint32_t w;
        /* The target height of the window. */
        uint32_t h;
        /* The actual width of the window. */
        uint32_t rw;
        /* The actual height of the window. */
        uint32_t rh;

        /* Variable that indicates if the Window wants to resize or not. */
        bool should_resize;


        /* Callback for the GLFW window resize event. */
        static void glfw_resize_callback(GLFWwindow* glfw_window, int width, int height);
    
    public:
        /* Constructor for the Window class, which takes the Vulkan instance to create the surface, GPU and swapchain with, the title and the size for the window. */
        Window(const Rendering::Instance& instance, const std::string& title, uint32_t width, uint32_t height);
        /* Copy constructor for the Window class. */
        Window(const Window& other);
        /* Move constructor for the Window class. */
        Window(Window&& other);
        /* Destructor for the Window class. */
        ~Window();

        /* Updates the title of the window. */
        inline void set_title(const std::string& new_title) { glfwSetWindowTitle(this->glfw_window, new_title.c_str()); }
        /* Resizes the window to the new size of the GLFWwindow. */
        void resize();
        /* Resizes the window to the given size. */
        void resize(uint32_t new_width, uint32_t new_height);

        /* Runs window events. Returns whether or not the window should close. */
        bool loop() const;

        /* Returns the title of the window. */
        inline const std::string& title() const { return this->t; }
        /* Returns the width of the window, in pixels. */
        inline uint32_t width() const { return this->w; }
        /* Returns the height of the window, in pixels. */
        inline uint32_t height() const { return this->h; }
        /* Returns the dimensions of the window as a Vulkan extent, in pixels. */ 
        inline VkExtent2D extent() const { return VkExtent2D({ this->w, this->h }); }
        /* Returns the actual width of the window, in pixels. */
        inline uint32_t real_width() const { return this->rw; }
        /* Returns the actual height of the window, in pixels. */
        inline uint32_t real_height() const { return this->rh; }
        /* Returns the actual dimensions of the window as a Vulkan extent, in pixels. */
        inline VkExtent2D real_extent() const { return VkExtent2D({ this->rw, this->rh }); }

        /* Returns whether or not the window thinks it needs resizing. */
        inline bool needs_resize() const { return this->should_resize; }

        /* Explicitly returns the internal surface object. */
        inline const Rendering::Surface& surface() const { return *this->rendering_surface; }
        /* Explicitly returns the internal GPU object. */
        inline const Rendering::GPU& gpu() const { return *this->rendering_gpu; }
        /* Explicitly returns the internal swapchain object. */
        inline const Rendering::Swapchain& swapchain() const { return *this->rendering_swapchain; }

        /* Expliticitly returns the internal GLFW window object. */
        inline GLFWwindow* window() const { return this->glfw_window; }
        /* Implicitly returns the internal GLFW window object. */
        inline operator GLFWwindow*() const { return this->glfw_window; }

        /* Copy assignment operator for the Window class. */
        inline Window& operator=(const Window& other) { return *this = Window(other); };
        /* Move assignment operator for the Window class. */
        inline Window& operator=(Window&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Window class. */
        friend void swap(Window& w1, Window& w2);

    };

    /* Swap operator for the Window class. */
    void swap(Window& w1, Window& w2);

}

#endif
