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

#include "window/Window.hpp"

#include "render_engine/instance/Instance.hpp"
#include "render_engine/gpu/Surface.hpp"
#include "render_engine/gpu/GPU.hpp"
#include "render_engine/memory/MemoryPool.hpp"
#include "render_engine/RenderEngine.hpp"

#include "model_manager/ModelManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace CppDebugger::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Returns a Tools::Array with the required extensions for GLFW. */
static Tools::Array<const char*> get_glfw_extensions() {
    DENTER("get_glfw_extensions");

    // We first collect a list of GLFW extensions
    uint32_t n_extensions = 0;
    const char** raw_extensions = glfwGetRequiredInstanceExtensions(&n_extensions);

    // Return them as an array
    DRETURN Tools::Array<const char*>(raw_extensions, n_extensions);
}





/***** ENTRY POINT *****/
int main(int argc, const char** argv) {
    DSTART("main"); DENTER("main");

    // Print some nice entry message
    DLOG(auxillary, "");
    DLOG(auxillary, "<<< RASTERIZER >>>");
    DLOG(auxillary, "");

    // Initialize the GLFW library
    DLOG(info, "Initializing GLFW...");
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Prepare the Vulkan instance first
    Rendering::Instance instance(Rendering::instance_extensions + get_glfw_extensions());

    // Use that to prepare the Window class
    Window window(instance, "Rasterizer", 800, 600);

    // Investigate the memory type we'll need
    uint32_t draw_type = Rendering::MemoryPool::select_memory_type(window.gpu(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    uint32_t stage_type = Rendering::MemoryPool::select_memory_type(window.gpu(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    // Initialize the pools we'll need
    Rendering::CommandPool copy_pool(window.gpu(), window.gpu().queue_info().memory(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    Rendering::MemoryPool draw_pool(window.gpu(), draw_type, 1024 * 1024 * 1024, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    Rendering::MemoryPool stage_pool(window.gpu(), stage_type, 1024 * 1024 * 1024, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    // Use those to initialize the model manager
    Models::ModelManager model_manager(copy_pool, draw_pool, stage_pool);
    model_manager.load_model("triangle", Models::ModelFormat::triangle);

    // Initialize the engine
    Rendering::RenderEngine render_engine(window, model_manager);
    DLOG(auxillary, "");

    // Do the render
    uint32_t fps = 0;
    DLOG(info, "Entering game loop...");
    chrono::system_clock::time_point last_fps_update = chrono::system_clock::now();
    bool busy = true;
    while (busy) {
        // Run the render engine
        busy = render_engine.loop();

        // Keep track of the fps
        ++fps;
        if (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - last_fps_update).count() >= 1000) {
            // Reset the timer
            last_fps_update += chrono::milliseconds(1000);

            // Show the FPS
            window.set_title("Rasterizer (FPS: " + std::to_string(fps) + ")");
            fps = 0;

            // Add another model???
            if (model_manager.contains("triangle")) {
                model_manager.unload_model("triangle");
                render_engine.refresh();
            }
        }
    }

    // Wait for the GPU to be idle before we stop
    DLOG(info, "Cleaning up...");
    render_engine.wait_for_idle();

    // We're done
    glfwTerminate();
    DRETURN 0;
}
