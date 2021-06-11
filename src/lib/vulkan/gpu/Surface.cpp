/* SURFACE.cpp
 *   by Lut99
 *
 * Created:
 *   11/06/2021, 15:39:41
 * Last edited:
 *   11/06/2021, 15:39:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Surface class, which wraps a VkSurfaceKHR created from a
 *   GLFW window.
**/

#include "vulkan/ErrorCodes.hpp"
#include "tools/CppDebugger.hpp"

#include "Surface.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;
using namespace CppDebugger::SeverityValues;


/***** SURFACE CLASS *****/
/* Constructor for the Surface class, which takes an instance and the GLFW window to create a surface from. */
Surface::Surface(const Instance& instance, GLFWwindow* glfw_window) :
    instance(instance)
{
    DENTER("Vulkan::Surface::Surface");

    // We can simply use the GLFW constructor for our surface
    VkResult vk_result;
    if ((vk_result = glfwCreateWindowSurface(this->instance, glfw_window, nullptr, &this->vk_surface)) != VK_SUCCESS) {
        DLOG(fatal, "Could not create VkSurface from GLFW window: " + vk_error_map[vk_result]);
    }

    DLEAVE;
}

/* Move constructor for the Surface class. */
Surface::Surface(Surface&& other) :
    instance(other.instance),
    vk_surface(other.vk_surface)
{
    // Mark the swapchain as non-present anymore
    this->vk_surface = nullptr;
}

/* Destructor for the Surface class. */
Surface::~Surface() {
    DENTER("Vulkan::Surface::~Surface");

    // Deallocate the surface if it no longer exists
    if (this->vk_surface != nullptr) {
        vkDestroySurfaceKHR(this->instance, this->vk_surface, nullptr);
    }

    DLEAVE;
}



/* Swap operator for the Surface class. */
void Vulkan::swap(Surface& s1, Surface& s2) {
    DENTER("Vulkan::swap(Surface)");

    #ifndef NDEBUG
    // Check if the instances are actually the same
    if (s1.instance != s2.instance) {
        DLOG(fatal, "Cannot swap surfaces with different instances");
    }
    #endif

    // Swap all fields
    using std::swap;
    swap(s1.vk_surface, s2.vk_surface);

    // Done
    DRETURN;
}
