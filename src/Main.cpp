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
#define _USE_MATH_DEFINES
#include <cmath>
#include <GLFW/glfw3.h>

#include "tools/CppDebugger.hpp"

#include "window/Window.hpp"

#include "world/WorldSystem.hpp"

#include "models/ModelSystem.hpp"

#include "rendering/instance/Instance.hpp"
#include "rendering/memory/MemoryManager.hpp"
#include "rendering/RenderSystem.hpp"

#include "ecs/EntityManager.hpp"

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
    // Prepare the memory manager
    Rendering::MemoryManager memory_manager(window.gpu());
    // Initialize the WorldSystem
    World::WorldSystem world_system;
    // Initialize the ModelSystem
    Models::ModelSystem model_system(memory_manager);
    // Initialize the RenderSystem
    Rendering::RenderSystem render_system(window, memory_manager, model_system);
    // Initialize the entity manager
    ECS::EntityManager entity_manager;

    // // Prepare a renderable entity
    // entity_t square1 = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
    // world_system.set(entity_manager, square1, { 0.0, 0.0, 0.5 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 });
    // model_system.load_model(entity_manager, square1, "", Models::ModelFormat::square);

    // // Prepare another renderable entity
    // entity_t triangle = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
    // world_system.set(entity_manager, triangle, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 });
    // model_system.load_model(entity_manager, triangle, "", Models::ModelFormat::triangle);

    // // Prepare a final renderable entity
    // entity_t square2 = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
    // world_system.set(entity_manager, square2, { 0.0, 0.0, -0.5 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 });
    // model_system.load_model(entity_manager, square2, "", Models::ModelFormat::square);

    // Prepare the teddy bear
    entity_t obj = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
    world_system.set(entity_manager, obj, { 0.0, 0.0, 0.0 }, { 0.75 * M_PI, 0.75 * M_PI, 0.0 }, { 0.03, 0.03, 0.03 });
    // model_system.load_model(entity_manager, obj, "F:\\Downloads\\Kenney Game Assets (version 41)\\3D assets\\Fantasy Town Kit\\Models\\OBJ format\\watermill.obj", Models::ModelFormat::obj);
    model_system.load_model(entity_manager, obj, "src/lib/models/formats/obj/pegleg/test.obj", Models::ModelFormat::obj);

    // Initialize the engine
    DLOG(auxillary, "");

    // Do the render
    uint32_t fps = 0;
    DLOG(info, "Entering game loop...");
    chrono::system_clock::time_point last_fps_update = chrono::system_clock::now();
    bool busy = true;
    uint32_t count = 0;
    while (busy) {
        // Run the render engine
        busy = render_system.render_frame(entity_manager);

        // Keep track of the fps
        ++fps;
        if (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - last_fps_update).count() >= 1000) {
            // Reset the timer
            last_fps_update += chrono::milliseconds(1000);

            // Show the FPS
            window.set_title("Rasterizer (FPS: " + std::to_string(fps) + ")");
            fps = 0;

            // Add another model???
            // if (count == 0) {
            //     model_manager.unload_model("squares");
            //     model_manager.load_model("bin/models/teddy.obj", Models::ModelFormat::obj);
            //     render_engine.refresh();
            //     ++count;
            // }
            // if (count == 0) {
            //     model_manager.unload_model("triangle");
            //     render_engine.refresh();
            //     ++count;
            // } else if (count == 1) {
            //     model_manager.load_model("triangle", Models::ModelFormat::triangle);
            //     render_engine.refresh();
            //     ++count;
            // } else if (count == 2) {
            //     model_manager.unload_model("triangle");
            //     model_manager.load_model("triangle", Models::ModelFormat::triangle);
            //     render_engine.refresh();
            //     ++count;
            // } else if (count == 3) {
            //     model_manager.unload_model("triangle");
            //     model_manager.load_model("bin/models/teddy.obj", Models::ModelFormat::obj);
            //     render_engine.refresh();
            //     ++count;
            // }
        }
    }

    // Wait for the GPU to be idle before we stop
    DLOG(auxillary, "");
    DLOG(info, "Cleaning up...");
    window.gpu().wait_for_idle();

    // We're done
    glfwTerminate();
    DRETURN 0;
}
