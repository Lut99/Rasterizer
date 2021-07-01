/* RENDER ENGINE.hpp
 *   by Lut99
 *
 * Created:
 *   27/06/2021, 15:05:29
 * Last edited:
 *   27/06/2021, 15:05:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains code that runs the Vulkan engine. In particular, sets up the
 *   required Vulkan structures and runs the game loop
 *   iteration-for-iteration.
**/

#ifndef RENDERING_ENGINE_HPP
#define RENDERING_ENGINE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "model_manager/ModelManager.hpp"

#include "render_engine/instance/Instance.hpp"
#include "render_engine/gpu/Surface.hpp"
#include "render_engine/gpu/GPU.hpp"
#include "render_engine/swapchain/Swapchain.hpp"
#include "render_engine/renderpass/RenderPass.hpp"
#include "render_engine/pipeline/Shader.hpp"
#include "render_engine/pipeline/Pipeline.hpp"
#include "render_engine/commandbuffers/CommandPool.hpp"
#include "render_engine/synchronization/Semaphore.hpp"
#include "render_engine/synchronization/Fence.hpp"

#include "tools/Array.hpp"

namespace Rasterizer::Rendering {
    /* The RenderEngine class, which manages the Vulkan rendering engine. */
    class RenderEngine {
    public:
        /* The maximum number of frames in flight we allow. */
        static const constexpr uint32_t max_frames_in_flight = 2;

    private:
        /* The GLFWwindow to which we render. */
        GLFWwindow* glfw_window;

        /* The Vulkan Instance on which we base our entire engine. */
        Rendering::Instance instance;
        /* The surface to which we render. */
        Rendering::Surface surface;
        /* The GPU where we run. */
        Rendering::GPU gpu;
        /* The swapchain to which we shall render. */
        Rendering::Swapchain swapchain;

        /* The vertex shader we use. */
        Rendering::Shader vertex_shader;
        /* The fragment shader we use. */
        Rendering::Shader fragment_shader;

        /* The render pass which we use to draw. */
        Rendering::RenderPass render_pass;
        /* The graphics pipeline we use to render. */
        Rendering::Pipeline pipeline;

        /* The command pool for the draw calls. */
        Rendering::CommandPool draw_cmd_pool;
        /* Handles for the command buffer to use. */
        Tools::Array<CommandBuffer> draw_cmds;

        /* List of framebuffers we'll draw to. */
        Tools::Array<Rendering::Framebuffer> framebuffers;

        /* Contains the semaphores that signal when a new image is available in the swapchain. */
        Tools::Array<Rendering::Semaphore> image_ready_semaphores;
        /* Contains the semaphores that signal when a an image is done being rendered. */
        Tools::Array<Rendering::Semaphore> render_ready_semaphores;
        /* Contains the fences that are used to synchronize the CPU to be sure that we're not already using one of the frames that are in flight. */
        Tools::Array<Rendering::Fence> frame_in_flight_fences;
        /* Contains the fences that are used to avoid using a swapchain image (not a conceptual frame) more than once at a time. Does not physically contain fences, but rather references existing ones from frame_in_flight_fences. */
        Tools::Array<Rendering::Fence*> image_in_flight_fences;

        /* Counter keeping track of which frame we should currently render to. */
        uint32_t current_frame;
        /* Variable that indicates if the RenderEngine should resize or not. */
        bool should_resize;


        /* Callback for the GLFW window resize event. */
        static void glfw_resize_callback(GLFWwindow* glfw_window, int width, int height);
    
    public:
        /* Constructor for the RenderEngine class, which takes a GLFW window to render to and a ModelManager object to load the models to render from. */
        RenderEngine(GLFWwindow* glfw_window, const Models::ModelManager& model_manager);
        /* Copy constructor for the RenderEngine class, which is deleted. */
        RenderEngine(const RenderEngine& other) = delete;

        /* Returns whether or not the render engine should stop due to the window being closed. */
        inline bool open() { return !glfwWindowShouldClose(this->glfw_window); }
        /* Runs a single iteration of the game loop. */
        void loop();
        
        /* Resizes the window to the given size. Note that this is a pretty slow operation, as it requires the device to be idle. */
        void resize(uint32_t new_width, uint32_t new_height);
        /* Resizes the window to the size of the given window. Note that this is a pretty slow operation, as it requires the device to be idle. */
        void resize(GLFWwindow* glfw_window);

        /* Waits for the used GPU to be idle again. */
        inline void wait_for_idle() const { this->gpu.wait_for_idle(); }

        /* Copy assignment operator for the RenderEngine class, which is deleted. */
        RenderEngine& operator=(const RenderEngine& other) = delete;

    };
};

#endif
