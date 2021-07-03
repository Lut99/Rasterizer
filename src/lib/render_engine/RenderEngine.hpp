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

#include "window/Window.hpp"

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

        /* The Window object managing and carrying the Instance, Surface, GPU and Swapchain. */
        Window& window;

    private:
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


        /* Private helper function that actually performs the RenderEngine part of a resize. */
        void _resize();
    
    public:
        /* Constructor for the RenderEngine class, which takes a Window to render to, the Vertex binding description and the attribute descriptions. */
        RenderEngine(Window& window, const VkVertexInputBindingDescription& binding_description, const Tools::Array<VkVertexInputAttributeDescription>& attribute_descriptions);
        /* Copy constructor for the RenderEngine class, which is deleted. */
        RenderEngine(const RenderEngine& other) = delete;

        /* Runs a single iteration of the game loop. Returns whether or not the RenderEngine allows the window to continue (true) or not because it's closed (false). */
        bool loop();
        
        /* Resizes the window to the size of the given window. Note that this is a pretty slow operation, as it requires the device to be idle. */
        void resize();
        /* Resizes the window to the given size. Note that this is a pretty slow operation, as it requires the device to be idle. */
        void resize(uint32_t new_width, uint32_t new_height);

        /* Waits for the used GPU to be idle again. */
        inline void wait_for_idle() const { this->window.gpu().wait_for_idle(); }

        /* Copy assignment operator for the RenderEngine class, which is deleted. */
        RenderEngine& operator=(const RenderEngine& other) = delete;

    };
};

#endif