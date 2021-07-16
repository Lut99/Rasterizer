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


/***** WINDOW CLASS *****/
/* Constructor for the Window class, which takes the Vulkan instance to create the surface, GPU and swapchain with, the title and the size for the window. */
Window::Window(const Rendering::Instance& instance, const std::string& title, uint32_t width, uint32_t height) :
    instance(instance),

    t(title),
    w(width),
    h(height),

    should_resize(false),
    should_close(false)
{
    DENTER("Window::Window");
    DLOG(info, "Initializing Window class...");
    DINDENT;

    DLOG(info, "Initializing GLFW window...");
    // Get the window
    this->glfw_window = glfwCreateWindow(this->w, this->h, this->t.c_str(), NULL, NULL);

    // Set the mouse input mode
    glfwSetInputMode(this->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(this->glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // Pass ourselves as data to the GLFW window s.t. we can keep track of resizes
    glfwSetWindowUserPointer(this->glfw_window, (void*) this);
    glfwSetFramebufferSizeCallback(this->glfw_window, Window::glfw_resize_callback);
    glfwSetKeyCallback(this->glfw_window, Window::glfw_key_callback);
    glfwSetCursorPosCallback(this->glfw_window, Window::glfw_cursor_callback);

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

    old_mouse_pos(other.old_mouse_pos),
    new_mouse_pos(other.new_mouse_pos),
    mouse_callbacks(other.mouse_callbacks),

    should_resize(other.should_resize),
    should_close(other.should_close)
{
    DENTER("Window::Window::copy");

    // First, copy the glfw window
    this->glfw_window = glfwCreateWindow(this->w, this->h, this->t.c_str(), NULL, NULL);

    // Set the mouse input mode
    glfwSetInputMode(this->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(this->glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // Pass ourselves as data to the GLFW window s.t. we can keep track of resizes
    glfwSetWindowUserPointer(this->glfw_window, (void*) this);
    glfwSetFramebufferSizeCallback(this->glfw_window, Window::glfw_resize_callback);
    glfwSetKeyCallback(this->glfw_window, Window::glfw_key_callback);
    glfwSetCursorPosCallback(this->glfw_window, Window::glfw_cursor_callback);

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

    old_mouse_pos(other.old_mouse_pos),
    new_mouse_pos(other.new_mouse_pos),
    mouse_callbacks(other.mouse_callbacks),

    should_resize(other.should_resize),
    should_close(other.should_close)
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
    }

    DDEDENT;
    DLEAVE;
}



/* Callback for the GLFW window resize. */
void Window::glfw_resize_callback(GLFWwindow* glfw_window, int width, int height) {
    DENTER("Window::glfw_resize_callback");

    // First, get the window back
    Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);
    // Mark that we need to resize at the new opportunity
    window->should_resize = true;

    // Done
    DRETURN;
}

/* Callback for GLFW window key events. */
void Window::glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    DENTER("Window::glfw_key_callback");

    // First, get the window back
    Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);

    // Check if escape was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        window->should_close = true;
    }

    // Done
    DRETURN;
}

/* Callback for GLFW window cursor move events. */
void Window::glfw_cursor_callback(GLFWwindow* glfw_window, double x, double y) {
    DENTER("Window::glfw_cursor_callback");

    // First, get the window back
    Window* window = (Window*) glfwGetWindowUserPointer(glfw_window);

    // Store the new x and y position
    window->old_mouse_pos = window->new_mouse_pos;
    window->new_mouse_pos.x = (float) x;
    window->new_mouse_pos.y = (float) y;

    // Call the relevant callbacks
    for (uint32_t i = 0; i < window->mouse_callbacks.size(); i++) {
        window->mouse_callbacks[i].func(window->mouse_callbacks[i].extra_data, window->new_mouse_pos, window->old_mouse_pos);
    }

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
    DRETURN !glfwWindowShouldClose(this->glfw_window) && !this->should_close;
}



/* Registers the given function as a new mouse callback. Optionally, some extra datapoint or object can be given that can be accessed during the callback. */
void Window::register_mouse_callback(void (*callback)(void*, const glm::vec2&, const glm::vec2&), void* extra_data) {
    DENTER("Window::register_mouse_callback");

    // Simply add to the list
    this->mouse_callbacks.push_back({ callback, extra_data });

    // Done
    DRETURN;
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

    swap(w1.old_mouse_pos, w2.old_mouse_pos);
    swap(w1.new_mouse_pos, w2.new_mouse_pos);
    swap(w1.mouse_callbacks, w2.mouse_callbacks);
    
    swap(w1.should_resize, w2.should_resize);
    swap(w1.should_close, w2.should_close);

    // Done
    DRETURN;
}

