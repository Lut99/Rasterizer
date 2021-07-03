/* WINDOW.cpp
 *   by Lut99
 *
 * Created:
 *   02/07/2021, 13:44:58
 * Last edited:
 *   02/07/2021, 13:44:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Window class, which manages the GLFW window and bundles
 *   Vulkan instance, gpu, surface and swapchain together in one place.
**/

#include "tools/CppDebugger.hpp"

#include "Window.hpp"

using namespace std;
using namespace Rasterizer;
using namespace CppDebugger::SeverityValues;


/***** GLOBALS *****/
/* Keeps track of how many classes use the GLFW library; the first one should initialize it, the last one should destroy it. */
static uint32_t n_windows = 0;





/***** WINDOW CLASS *****/
/* Constructor for the Window class, which takes the Vulkan instance to create the surface, GPU and swapchain with, the title and the size for the window. */
Window::Window(const Rendering::Instance& instance, const std::string& title, uint32_t width, uint32_t height) :
    instance(instance),

    t(title),
    w(width),
    h(height),

    should_resize(false)
{
    DENTER("Window::Window");
    DLOG(info, "Initializing Window class...");
    DINDENT;

    DLOG(info, "Initializing GLFW window...");
    if (n_windows == 0) {
        DINDENT; DLOG(auxillary, "Initializing GLFW library..."); DDEDENT;
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }
    // Get the window
    this->glfw_window = glfwCreateWindow(this->w, this->h, this->t.c_str(), NULL, NULL);
    ++n_windows;

    // Pass ourselves as data to the GLFW window s.t. we can keep track of resizes
    glfwSetWindowUserPointer(this->glfw_window, (void*) this);
    glfwSetFramebufferSizeCallback(this->glfw_window, Window::glfw_resize_callback);

    // Initialize the other classes
    this->rendering_surface = new Rendering::Surface(this->instance, this->glfw_window);
    this->rendering_gpu = new Rendering::GPU(this->instance, *this->rendering_surface, { 1, 1, 1, 1 }, Rendering::device_extensions);
    this->rendering_swapchain = new Rendering::Swapchain(*this->rendering_gpu, this->glfw_window, *this->rendering_surface);

    // Done
    DDEDENT;
    DLEAVE;
}

/* Copy constructor for the Window class, which is deleted. */
Window::Window(const Window& other) :
    instance(other.instance),

    t(other.t),
    w(other.w),
    h(other.h),
    rw(other.rw),
    rh(other.rh),

    should_resize(other.should_resize)
{
    DENTER("Window::Window::copy");

    // First, copy the glfw window
    this->glfw_window = glfwCreateWindow(this->w, this->h, this->t.c_str(), NULL, NULL);
    ++n_windows;

    // Pass ourselves as data to the GLFW window s.t. we can keep track of resizes
    glfwSetWindowUserPointer(this->glfw_window, (void*) this);
    glfwSetFramebufferSizeCallback(this->glfw_window, Window::glfw_resize_callback);

    // Also copy the other classes
    this->rendering_surface = new Rendering::Surface(this->instance, this->glfw_window);
    this->rendering_gpu = new Rendering::GPU(this->instance, *this->rendering_surface, { 1, 1, 1, 1 }, Rendering::device_extensions);
    this->rendering_swapchain = new Rendering::Swapchain(*this->rendering_gpu, this->glfw_window, *this->rendering_surface);

    DLEAVE;
}

/* Move constructor for the Window class. */
Window::Window(Window&& other) :
    instance(other.instance),

    glfw_window(other.glfw_window),
    rendering_surface(other.rendering_surface),
    rendering_gpu(other.rendering_gpu),
    rendering_swapchain(other.rendering_swapchain),

    t(other.t),
    w(other.w),
    h(other.h),
    rw(other.rw),
    rh(other.rh),

    should_resize(other.should_resize)
{
    // Set the deallocatable stuff to nullptr to avoid them, well, deallocating
    other.glfw_window = nullptr;
    other.rendering_surface = nullptr;
    other.rendering_gpu = nullptr;
    other.rendering_swapchain = nullptr;
}

/* Destructor for the Window class. */
Window::~Window() {
    DENTER("Window::~Window");
    DLOG(info, "Cleaning Window object...");
    DINDENT;

    // Destroy the three vulkan objects if needed
    if (this->rendering_swapchain != nullptr) {
        delete this->rendering_swapchain;
    }
    if (this->rendering_gpu != nullptr) {
        delete this->rendering_gpu;
    }
    if (this->rendering_surface != nullptr) {
        delete this->rendering_surface;
    }

    // Also destroy the window
    if (this->glfw_window != nullptr) {
        DLOG(auxillary, "Destroying GLFW window...");
        glfwDestroyWindow(this->glfw_window);

        // If we're the last window, also destroy the library
        --n_windows;
        if (n_windows == 0) {
            DINDENT; DLOG(auxillary, "Terminating GLFW library..."); DDEDENT;
            glfwTerminate();
        }
    }

    DDEDENT;
    DLEAVE;
}



/* Callback for the GLFW window resize. */
void Window::glfw_resize_callback(GLFWwindow* glfw_window, int width, int height) {
    DENTER("glfw_resize_callback");

    // First, get the RenderEngine back
    Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);
    // Mark that we need to resize at the new opportunity
    window->should_resize = true;

    // Done
    DRETURN;
}



/* Resizes the window to the new size of the GLFWwindow. */
void Window::resize() {
    DENTER("Window::resize");

    // If the user minimized the application, then we shall wait until the window has a size again
    int width = 0, height = 0;
    glfwGetFramebufferSize(this->glfw_window, &width, &height);
    if (width == 0 || height == 0) {
        DLOG(info, "Window minimized");
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(this->glfw_window, &width, &height);
            // Use the blocking event call to let the thread sleepy sleepy
            glfwWaitEvents();
        }
    }

    // Wait until the device is idle
    this->rendering_gpu->wait_for_idle();

    // Then, resize the swapchain (which also updates the image views)
    this->rendering_gpu->refresh_swapchain_info();
    this->rendering_swapchain->resize(this->glfw_window);

    // Done as far as the window is concerned
    this->should_resize = false;
    DRETURN;
}

/* Resizes the window to the given size. */
void Window::resize(uint32_t new_width, uint32_t new_height) {
    DENTER("Window::resize(width, height)");

    // If the user minimized the application, then we shall wait until the window has a size again
    int width = 0, height = 0;
    glfwGetFramebufferSize(this->glfw_window, &width, &height);
    if (width == 0 || height == 0) {
        DLOG(info, "Window minimized");
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(this->glfw_window, &width, &height);
            // Use the blocking event call to let the thread sleepy sleepy
            glfwWaitEvents();
        }
    }

    // Wait until the device is idle
    this->rendering_gpu->wait_for_idle();

    // Then, resize the swapchain (which also updates the image views)
    this->rendering_gpu->refresh_swapchain_info();
    this->rendering_swapchain->resize(new_width, new_height);

    // Done as far as the window is concerned
    this->should_resize = false;
    DRETURN;
}



/* Runs window events. Returns whether or not the window can remain open (true) or should close due to user interaction (false). */
bool Window::loop() const {
    DENTER("Window::loop");

    // First, poll the GLFW events
    glfwPollEvents();

    // Next, return if the window should close
    DRETURN !glfwWindowShouldClose(this->glfw_window);
}



/* Swap operator for the Window class. */
void Rasterizer::swap(Window& w1, Window& w2) {
    DENTER("Rasterizer::swap(Window)");

    #ifndef NDEBUG
    if (w1.instance != w2.instance) {
        DLOG(fatal, "Cannot swap windows with different instances.");
    }
    #endif

    // Simply swap everything
    using std::swap;
    swap(w1.glfw_window, w2.glfw_window);
    swap(w1.rendering_surface, w2.rendering_surface);
    swap(w1.rendering_gpu, w2.rendering_gpu);
    swap(w1.rendering_swapchain, w2.rendering_swapchain);

    swap(w1.t, w2.t);
    swap(w1.w, w2.w);
    swap(w1.h, w2.h);
    swap(w1.rw, w2.rw);
    swap(w1.rh, w2.rh);
    
    swap(w1.should_resize, w2.should_resize);

    // Done
    DRETURN;
}

