/* MAIN.cpp
 *   by Lut99
 *
 * Created:
 *   11/06/2021, 18:03:12
 * Last edited:
 *   11/06/2021, 18:03:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Entry point to the rasterizer executable
**/

#include <chrono>
#include <GLFW/glfw3.h>

#include "tools/CppDebugger.hpp"
#include "vulkan/engine/RenderEngine.hpp"

using namespace std;
using namespace Rasterizer;
using namespace CppDebugger::SeverityValues;


/* Entry point to the program. */
int main(int argc, const char** argv) {
    DSTART("main"); DENTER("main");

    // Print some nice entry message
    DLOG(auxillary, "");
    DLOG(auxillary, "<<< RASTERIZER >>>");
    DLOG(auxillary, "");

    // Prepare the GLFW library
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Get the window
    GLFWwindow* glfw_window = glfwCreateWindow(800, 600, "Rasterizer", NULL, NULL);

    // Initialize the engine
    Vulkan::RenderEngine render_engine(glfw_window);
    DLOG(auxillary, "");

    // Do the render
    uint32_t fps = 0;
    DLOG(info, "Entering game loop...");
    chrono::system_clock::time_point last_fps_update = chrono::system_clock::now();
    while (render_engine.open()) {
        render_engine.loop();

        // Keep track of the fps
        ++fps;
        if (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - last_fps_update).count() >= 1000) {
            // Reset the timer
            last_fps_update += chrono::milliseconds(1000);

            // Show the FPS
            glfwSetWindowTitle(glfw_window, ("Rasterizer (FPS: " + std::to_string(fps) + ")").c_str());
            fps = 0;
        }
    }

    // Wait for the GPU to be idle before we stop
    DLOG(info, "Cleaning up...");
    render_engine.wait_for_idle();

    // We're done
    glfwTerminate();
    DRETURN 0;
}
