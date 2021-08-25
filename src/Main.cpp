/* MAIN.cpp
 *   by Lut99
 *
 * Created:
 *   11/06/2021, 18:03:12
 * Last edited:
 *   07/08/2021, 23:05:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Entry point to the rasterizer executable
**/

#include <string>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>

#include "tools/Logger.hpp"
#include "tools/Common.hpp"

#include "window/Window.hpp"

#include "world/WorldSystem.hpp"

#include "models/ModelSystem.hpp"
#include "textures/TextureSystem.hpp"

#include "rendering/instance/Instance.hpp"
#include "rendering/memory/MemoryManager.hpp"
#include "rendering/RenderSystem.hpp"

#include "ecs/EntityManager.hpp"

using namespace std;
using namespace Rasterizer;


/***** HELPER STRUCTS *****/
struct Options {
    /* The number of bytes to allocate on device local memory. */
    VkDeviceSize local_memory_size;
    /* The number of bytes to allocate on host visible memory. */
    VkDeviceSize visible_memory_size;

    /* Default constructor for the Options class, which sets everything to default. */
    Options() :
        local_memory_size(1024 * 1024 * 1024),
        visible_memory_size(100 * 1024 * 1024)
    {}
};





/***** HELPER FUNCTIONS *****/
/* Returns a Tools::Array with the required extensions for GLFW. */
static Tools::Array<const char*> get_glfw_extensions() {
    // We first collect a list of GLFW extensions
    uint32_t n_extensions = 0;
    const char** raw_extensions = glfwGetRequiredInstanceExtensions(&n_extensions);

    // Return them as an array
    return Tools::Array<const char*>(raw_extensions, n_extensions);
}

/* Prints the usage string. */
static void print_usage(std::ostream& os, const std::string& filename) {
    os << "Usage: " << filename << " [options]" << endl;
}

/* Prints the help string. */
static void print_help(std::ostream& os, const std::string& filename) {
    print_usage(os, filename);

    os << endl;
    os << "Options:" << endl;
    os << "     --local <bytes> : The number of bytes we reserve in local device memory." << endl;
    os << "     --visible <bytes> : The number of bytes we reserve in host visible device memory." << endl;
    os << endl;
}

/* Parses the given arguments, populating the given Settings struct. */
static void parse_args(Options& opts, int argc, const char** argv) {
    // Start parsin'
    bool accept_options = true;
    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];
        
        // Check if it begins with a stripe
        if (accept_options && arg[0] == '-') {
            // It's either shortoption or a long option
            if (arg[1] == '-') {
                // Longoption
                std::string option = arg + 2;
                if (option[0] == '\0') {
                    // It's the empty option; from now on, no more options
                    accept_options = false;
                    continue;
                
                } else if (option == "local" || option.substr(0, 6) == "local=") {
                    // Either take the next one or split
                    std::string value;
                    if (option.size() > 5 && option[5] == '=') {
                        value = option.substr(6);
                    } else if (i < argc - 1) {
                        value = argv[++i];
                    } else {
                        cerr << "Missing value for option '" << arg << "'.";
                    }

                    // Try to parse as a byte format
                    VkDeviceSize ivalue;
                    try {
                        ivalue = Tools::string_to_bytes(value);
                    } catch (std::invalid_argument& e) {
                        cerr << e.what() << endl;
                        exit(EXIT_FAILURE);
                    } catch (std::out_of_range& e) {
                        cerr << e.what() << endl;
                        exit(EXIT_FAILURE);
                    }

                    // Set in the settings
                    opts.local_memory_size = ivalue;
                
                } else if (option == "visible" || option.substr(0, 8) == "visible=") {
                    // Either take the next one or split
                    std::string value;
                    if (option.size() > 7 && option[7] == '=') {
                        value = option.substr(8);
                    } else if (i < argc - 1) {
                        value = argv[++i];
                    } else {
                        cerr << "Missing value for option '" << arg << "'.";
                    }

                    // Try to parse as a byte format
                    VkDeviceSize ivalue;
                    try {
                        ivalue = Tools::string_to_bytes(value);
                    } catch (std::invalid_argument& e) {
                        cerr << e.what() << endl;
                        exit(EXIT_FAILURE);
                    } catch (std::out_of_range& e) {
                        cerr << e.what() << endl;
                        exit(EXIT_FAILURE);
                    }

                    // Set in the settings
                    opts.visible_memory_size = ivalue;
                    
                } else if (option == "help") {
                    // Print the help string!
                    print_help(cout, argv[0]);
                    exit(EXIT_SUCCESS);

                } else {
                    // Iwwegal option
                    cerr << "Unknown option '" << option << "'." << endl;
                    cerr << "Use '" << argv[0] << " -h' to see all options." << endl;
                    exit(EXIT_FAILURE);

                }

            } else {
                switch(arg[2]) {
                    case 'h':
                        // Print the help string!
                        print_help(cout, argv[0]);
                        exit(EXIT_SUCCESS);
                    
                    default:
                        // Iwwegal option
                        cerr << "Unknown option '" << arg[2] << "'." << endl;
                        cerr << "Use '" << argv[0] << " -h' to see all options." << endl;
                        exit(EXIT_FAILURE);
                }
            }
        } else {
            // Parse as positional value, but we have none of those so simply ignore
            continue;

        }
    }
}





/***** ENTRY POINT *****/
int main(int argc, const char** argv) {
    try {
        // Parse the arguments
        Options opts;
        parse_args(opts, argc, argv);
        
        // Update the logger with the correct verbosity
        logger.set_verbosity(Verbosity::debug);

        // Indicate that we're starting
        logger.logc(Verbosity::important, "main", "Starting Rasterizer on ", logger.get_start_time());
        logger.logc(Verbosity::important, "main", "Initializing Rasterizer...");

        // Initialize the GLFW library
        logger.logc(Verbosity::important, "main", "Initializing GLFW...");
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Prepare the Vulkan instance first
        Rendering::Instance instance(Rendering::instance_extensions + get_glfw_extensions());

        // Use that to prepare the Window class
        uint32_t width = 800, height = 600;
        Window window(instance, "Rasterizer", width, height);
        // Prepare the memory manager
        Rendering::MemoryManager memory_manager(window.gpu(), opts.local_memory_size, opts.visible_memory_size);
        // Initialize the WorldSystem
        World::WorldSystem world_system;
        // Initialize the ModelSystem
        Models::ModelSystem model_system(memory_manager);
        // Initialize the TextureSystem
        Textures::TextureSystem texture_system(memory_manager);
        // Initialize the RenderSystem
        Rendering::RenderSystem render_system(window, memory_manager, model_system, texture_system);
        // Initialize the entity manager
        ECS::EntityManager entity_manager;

        // // Prepare a renderable entity
        // entity_t square1 = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
        // world_system.set(entity_manager, square1, { 0.0, 0.0, 0.5 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 });
        // model_system.load_model(entity_manager, square1, "", Models::ModelFormat::square);

        // // Prepare another renderable entity
        // entity_t triangle = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
        // world_system.set(entity_manager, triangle, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 });
        // model_system.load_model(entity_manager, triangle, "", Models::ModelFosin(glm::radians(transform.rotation.z)) * cos(glm::radians(transform.rotation.y));rmat::triangle);

        // // Prepare a final renderable entity
        // entity_t square2 = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::mesh);
        // world_system.set(entity_manager, square2, { 0.0, 0.0, -0.5 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 });
        // model_system.load_model(entity_manager, square2, "", Models::ModelFormat::square);

        // Prepare the camera
        entity_t cam = entity_manager.add(ECS::ComponentFlags::camera | ECS::ComponentFlags::controllable | ECS::ComponentFlags::transform);
        world_system.set_cam(entity_manager, cam, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, 45, (float) width / (float) height);
        world_system.set_controllable(entity_manager, cam, 1.0f, 10.0f);

        // Prepare the teddy bear
        entity_t obj = entity_manager.add(ECS::ComponentFlags::transform | ECS::ComponentFlags::meshes | ECS::ComponentFlags::texture);
        world_system.set(entity_manager, obj, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 1, 1, 1 });
        // world_system.set(entity_manager, obj, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.5, 0.5, 0.5 });
        // world_system.set(entity_manager, obj, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.03, 0.03, 0.03 });
        // model_system.load_model(entity_manager, obj, "F:\\Downloads\\Kenney Game Assets (version 41)\\3D assets\\Fantasy Town Kit\\Models\\OBJ format\\watermill.obj", Models::ModelFormat::obj);
        // model_system.load_model(entity_manager, obj, "src/lib/models/formats/obj/pegleg/test.obj", Models::ModelFormat::obj);
        // model_system.load_model(entity_manager, obj, "bin/models/teddy.obj", Models::ModelFormat::obj);
        model_system.load_model(entity_manager, obj, "square", Models::ModelFormat::square);
        texture_system.load_texture(entity_manager, obj, "bin/textures/man.png", Textures::TextureFormat::png);

        // Do the render
        uint32_t fps = 0;
        logger.logc(Verbosity::important, "main", "Done initializing, entering game loop...");
        chrono::system_clock::time_point last_fps_update = chrono::system_clock::now();
        bool busy = true;
        uint32_t count = 0;
        while (busy) {
            // Run the render engine
            busy = render_system.render_frame(entity_manager);
            // Update the world
            world_system.update(entity_manager, window);

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
        logger.logc(Verbosity::important, "main", "Cleaning up...");
        window.gpu().wait_for_idle();

        // We're done
        glfwTerminate();
        return EXIT_SUCCESS;

    } catch (std::exception& e) {
        // Otherwise, print the error and return
        logger.error(e.what());
        return EXIT_FAILURE;
    }
}
