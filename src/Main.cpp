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

    // Do the render
    while (render_engine.open()) {
        render_engine.loop();
    }

    // Wait for the GPU to be idle before we stop
    render_engine.wait_for_idle();

    // We're done
    DRETURN 0;
}
