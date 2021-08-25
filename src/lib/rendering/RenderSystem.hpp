/* RENDER SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   20/07/2021, 15:10:33
 * Last edited:
 *   20/07/2021, 15:10:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines the rendering system, which is capable of showing the entities
 *   that have a Render component on the screen. Also uses the Transform
 *   component to decide where to place the entity.
**/

#ifndef RENDERING_RENDER_SYSTEM_HPP
#define RENDERING_RENDER_SYSTEM_HPP

#include "ecs/EntityManager.hpp"
#include "window/Window.hpp"
#include "memory/MemoryManager.hpp"
#include "models/ModelSystem.hpp"
#include "textures/TextureSystem.hpp"

#include "depthtesting/DepthStencil.hpp"
#include "swapchain/Framebuffer.hpp"

#include "descriptors/DescriptorSetLayout.hpp"
#include "descriptors/DescriptorSet.hpp"

#include "pipeline/Shader.hpp"
#include "renderpass/RenderPass.hpp"
#include "pipeline/Pipeline.hpp"

#include "synchronization/Semaphore.hpp"
#include "synchronization/Fence.hpp"

namespace Rasterizer::Rendering {
    /* The RenderSystem class, which is in charge of rendering the renderable entities in the EntityManager. */
    class RenderSystem {
    public:
        /* Channel name for the RenderSystem class. */
        static constexpr const char* channel = "RenderSystem";
        /* The maximum number of frames in flight we allow. */
        static const constexpr uint32_t max_frames_in_flight = 2;

        /* The Window which we render to. */
        Window& window;
        /* The MemoryManager that contains the pools we might need. */
        MemoryManager& memory_manager;
        /* The ModelSystem which we use to schedule the model buffers with. */
        const Models::ModelSystem& model_system;
        /* The TextureSystem which we use to schedule the texture buffers with. */
        const Textures::TextureSystem& texture_system;

    private:
        /* Descriptor set layout for the textures. */
        Rendering::DescriptorSetLayout texture_layout;
        /* Descriptor sets for the textures (we have one per swapchain image). */
        Tools::Array<Rendering::DescriptorSet*> texture_sets;

        /* The depth stencil we attach to the pipeline. */
        Rendering::DepthStencil depth_stencil;
        /* List of framebuffers we'll draw to. */
        Tools::Array<Rendering::Framebuffer> framebuffers;
        
        /* The vertex shader we use. */
        Rendering::Shader vertex_shader;
        /* The fragment shader we use. */
        Rendering::Shader fragment_shader;
        
        /* The render pass which we use to draw. */
        Rendering::RenderPass render_pass;
        /* The graphics pipeline we use to render. */
        Rendering::Pipeline pipeline;

        /* Handles for the command buffer to use. */
        Tools::Array<CommandBuffer*> draw_cmds;

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
    
    private:
        /* Private helper function that resizes all required structures for a new window size. */
        void _resize();

    public:
        /* Constructor for the RenderSystem, which takes a window, a memory manager to render (to and draw memory from, respectively), a model system to schedule the model buffers with and a texture system to schedule texture images with. */
        RenderSystem(Window& window, MemoryManager& memory_manager, const Models::ModelSystem& model_system, const Textures::TextureSystem& texture_system);
        /* Copy constructor for the RenderSystem class. */
        RenderSystem(const RenderSystem& other);
        /* Move constructor for the RenderSystem class. */
        RenderSystem(RenderSystem&& other);
        /* Destructor for the RenderSystem class. */
        ~RenderSystem();

        /* Runs a single iteration of the game loop. Returns whether or not the RenderSystem is asked to close the window (false) or not (true). */
        bool render_frame(const ECS::EntityManager& entity_manager);

        /* Copy assignment operator for the RenderSystem class. */
        inline RenderSystem& operator=(const RenderSystem& other) { return *this = RenderSystem(other); }
        /* Move assignment operator for the RenderSystem class. */
        inline RenderSystem& operator=(RenderSystem&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the RenderSystem class. */
        friend void swap(RenderSystem& rs1, RenderSystem& rs2);

    };

    /* Swap operator for the RenderSystem class. */
    void swap(RenderSystem& rs1, RenderSystem& rs2);

}

#endif
