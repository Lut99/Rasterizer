/* RENDER SYSTEM.hpp
 *   by Lut99
 *
 * Created:
 *   20/07/2021, 15:10:33
 * Last edited:
 *   9/19/2021, 5:56:40 PM
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

#include "window/Window.hpp"
#include "ecs/EntityManager.hpp"
#include "models/ModelSystem.hpp"

#include "memory_manager/MemoryManager.hpp"

#include "depthtesting/DepthStencil.hpp"

#include "descriptors/DescriptorSetLayout.hpp"

#include "shaders/ShaderPool.hpp"
#include "renderpass/RenderPass.hpp"
#include "pipeline/PipelineCache.hpp"
#include "pipeline/PipelineConstructor.hpp"
#include "pipeline/Pipeline.hpp"

#include "swapchain/FrameManager.hpp"

namespace Makma3D::Rendering {
    /* The RenderSystem class, which is in charge of rendering the renderable entities in the EntityManager. */
    class RenderSystem {
    public:
        /* Channel name for the RenderSystem class. */
        static constexpr const char* channel = "RenderSystem";
        /* The maximum number of frames in flight we allow. */
        static constexpr const uint32_t max_frames_in_flight = 2;
        /* Defines the descriptor set used for engine-global resources (i.e., bound once per frame). */
        static constexpr const uint32_t desc_set_global = 0;
        /* Defines the descriptor set used for per-renderpass resources (i.e., bound once per render pass). */
        static constexpr const uint32_t desc_set_pass = 1;
        /* Defines the descriptor set used for material resources (i.e., bound per kind of material we use). */
        static constexpr const uint32_t desc_set_material = 2;
        /* Defines the descriptor set used for per-object resources (i.e., bound very often). */
        static constexpr const uint32_t desc_set_object = 3;

        /* The Window which we render to. */
        Window& window;
        /* The MemoryManager that contains the pools we might need. */
        MemoryManager& memory_manager;
        /* The ModelSystem which we use to schedule the model buffers with. */
        const Models::ModelSystem& model_system;

    private:
        /* Descriptor set layout for general data, such as the camera information. */
        Rendering::DescriptorSetLayout global_descriptor_layout;
        /* Descriptor set layout for per-material data, such as its colour. */
        Rendering::DescriptorSetLayout material_descriptor_layout;
        /* Descriptor set layout for per-object data, such as its position. */
        Rendering::DescriptorSetLayout object_descriptor_layout;

        /* The depth stencil we attach to the pipeline. */
        Rendering::DepthStencil depth_stencil;

        /* A pool where we draw shaders from. */
        Rendering::ShaderPool shader_pool;

        /* The render pass which we use to draw. */
        Rendering::RenderPass render_pass;

        /* A cache for creating pipelines. */
        Rendering::PipelineCache pipeline_cache;
        /* The constructor we use to (hopefully) efficiently pump out new pipelines. */
        Rendering::PipelineConstructor pipeline_constructor;
        /* The graphics pipelines we use to render, sorted by material types. */
        std::unordered_map<Materials::MaterialType, Rendering::Pipeline*> pipelines;

        /* The FrameManager in charge for giving us frames we can render to. */
        Rendering::FrameManager* frame_manager;

    private:
        /* Private helper function that resizes all required structures for a new window size. */
        void _resize();

    public:
        /* Constructor for the RenderSystem, which takes a window, a memory manager to render (to and draw memory from, respectively) and a model system to schedule the model buffers withh. */
        RenderSystem(Window& window, MemoryManager& memory_manager, const Models::ModelSystem& model_system);
        /* Copy constructor for the RenderSystem class, which is deleted. */
        RenderSystem(const RenderSystem& other) = delete;
        /* Move constructor for the RenderSystem class. */
        RenderSystem(RenderSystem&& other);
        /* Destructor for the RenderSystem class. */
        ~RenderSystem();

        /* Runs a single iteration of the game loop. Returns whether or not the RenderSystem is asked to close the window (false) or not (true). */
        bool render_frame(const ECS::EntityManager& entity_manager);

        /* Copy assignment operator for the RenderSystem class, which is deleted. */
        RenderSystem& operator=(const RenderSystem& other) = delete;
        /* Move assignment operator for the RenderSystem class. */
        inline RenderSystem& operator=(RenderSystem&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the RenderSystem class. */
        friend void swap(RenderSystem& rs1, RenderSystem& rs2);

    };

    /* Swap operator for the RenderSystem class. */
    void swap(RenderSystem& rs1, RenderSystem& rs2);

}

#endif
