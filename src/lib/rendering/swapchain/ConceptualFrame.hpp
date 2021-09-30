/* CONCEPTUAL FRAME.hpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 18:53:40
 * Last edited:
 *   9/20/2021, 9:14:26 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a class that reprents a ConceptualFrame to which we can
 *   render. Builds upon a SwapchainFrame.
**/

#ifndef RENDERING_CONCEPTUAL_FRAME_HPP
#define RENDERING_CONCEPTUAL_FRAME_HPP

#include "tools/Array.hpp"
#include "ecs/components/Model.hpp"
#include "models/ModelSystem.hpp"
#include "materials/variants/simple/Simple.hpp"
#include "materials/variants/simple_coloured/SimpleColoured.hpp"
#include "materials/variants/simple_textured/SimpleTextured.hpp"

#include "../data/MaterialData.hpp"
#include "../data/EntityData.hpp"
#include "../memory_manager/MemoryManager.hpp"
#include "../memory/LinearMemoryPool.hpp"
#include "../memory/Buffer.hpp"
#include "../commandbuffers/CommandBuffer.hpp"
#include "../descriptors/DescriptorSetLayout.hpp"
#include "../descriptors/DescriptorPool.hpp"
#include "../descriptors/DescriptorSet.hpp"
#include "../pipeline/Pipeline.hpp"
#include "../synchronization/Semaphore.hpp"
#include "../synchronization/Fence.hpp"

#include "SwapchainFrame.hpp"

namespace Makma3D::Rendering {
    /* The ConceptualFrame class, which wraps around (different) SwapchainFrames to be able to render linearly to the swapchain. */
    class ConceptualFrame {
    public:
        /* The logger channel name for the ConceptualFrame class. */
        static constexpr const char* channel = "ConceptualFrame";

        /* The MemoryManager from which the ConceptualFrame draws memory resources. */
        Rendering::MemoryManager& memory_manager;

    private:
        /* The SwapchainFrame we wrap. */
        Rendering::SwapchainFrame* swapchain_frame;
        /* Temporary staging buffer for uploading stuff to the GPU. */
        Rendering::Buffer* stage_buffer;
        /* Pipeline bound to the ConceptualFrame for a single render pass. */
        const Rendering::Pipeline* pipeline;

        /* Declare the FrameManager as a friend. */
        friend class FrameManager;

    public:
        /* Descriptor set layout for the global descriptor. */
        Rendering::DescriptorSetLayout global_layout;
        /* Descriptor set layout for the per-material descriptors. */
        Rendering::DescriptorSetLayout material_layout;
        /* Descriptor set layout for the per-object descriptors. */
        Rendering::DescriptorSetLayout entity_layout;

        /* Command buffer for drawing to this Frame. */
        Rendering::CommandBuffer* draw_cmd;
        /* Memory pool for creating buffers cheaply each Frame. */
        Rendering::LinearMemoryPool* memory_pool;
        /* Descriptor pool for all descriptors in this frame. */
        Rendering::DescriptorPool* descriptor_pool;
        
        /* Global descriptor set for this frame. */
        Rendering::DescriptorSet* global_set;
        /* Global camera buffer for this frame. */
        Rendering::Buffer* camera_buffer;
        
        /* Maps material IDs to material indices into the arrays. */
        std::unordered_map<const Materials::Material*, uint32_t> material_index_map;
        /* Descriptors for all materials drawn with this buffer. */
        Tools::Array<Rendering::DescriptorSet*> material_sets;
        /* Buffers for all materials. */
        Tools::Array<Rendering::Buffer*> material_buffers;

        /* Maps entity IDs to entity indices into the arrays. */
        std::unordered_map<ECS::entity_t, uint32_t> entity_index_map;
        /* Descriptors for all entities drawn with this buffer. */
        Tools::Array<Rendering::DescriptorSet*> entity_sets;
        /* Buffers for all entities. */
        Tools::Array<Rendering::Buffer*> entity_buffers;

        /* Semaphore that signals when the image is ready to be rendered to. */
        Rendering::Semaphore image_ready_semaphore;
        /* Semaphore that signals when the image is done being rendered to, i.e., ready to be presented. */
        Rendering::Semaphore render_ready_semaphore;
        /* Fence that is used to prevent the CPU from scheduling this frame again if it's still in flight. */
        Rendering::Fence in_flight_fence;

    public:
        /* Constructor for the ConceptualFrame class, which takes a MemoryManager to be able to draw games, a descriptor set layout for the global descriptor, a descriptor set layout for per-material descriptors and a descriptor set layout for the per-entity descriptors. */
        ConceptualFrame(Rendering::MemoryManager& memory_manager, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& material_layout, const Rendering::DescriptorSetLayout& entity_layout);
        /* Copy constructor for the ConceptualFrame class, which is deleted. */
        ConceptualFrame(const ConceptualFrame& other) = delete;
        /* Move constructor for the ConceptualFrame class. */
        ConceptualFrame(ConceptualFrame&& other);
        /* Destructor for the ConceptualFrame class. */
        ~ConceptualFrame();

        /* Prepares rendering the frame as new by throwing out old data preparing to render at most the given number of objects with at least the given number of materials different materials. */
        void prepare_render(uint32_t n_materials, uint32_t n_objects);

        /* Populates the internal camera buffer with the given projection and view matrices. */
        void upload_camera_data(const glm::mat4& proj_matrix, const glm::mat4& view_matrix);
        /* Uploads the given material to the GPU. What precisely will be uploaded is, of course, material dependent. */
        void upload_material_data(const Materials::Material* material);
        /* Uploads entity data for the given entity to its buffer and its descriptor set. */
        void upload_entity_data(ECS::entity_t entity, const Rendering::EntityData& entity_data);

        /* Starts to schedule the render pass associated with the wrapped SwapchainFrame on the internal draw queue. */
        void schedule_start();
        /* Binds the given pipeline on the internal draw command queue. */
        void schedule_pipeline(const Rendering::Pipeline* pipeline);
        /* Schedules frame-global descriptors on the internal draw queue (i.e., binds the camera data and the global descriptor). */
        void schedule_global();
        /* Schedules the stuff for the given material. Does have to have its data uploaded first, of course. */
        void schedule_material(const Materials::Material* material);
        /* Schedules the given entity's descriptor set on the internal draw queue. */
        void schedule_entity(ECS::entity_t entity);
        /* Binds the given vertex buffer to the internal draw queue. */
        void schedule_vertex_buffer(const Rendering::Buffer* vertex_buffer);
        /* Schedules a draw command for the given index buffer (with the given number of indices) on the internal draw queue. */
        void schedule_draw(const Rendering::Buffer* index_buffer, uint32_t n_indices);
        /* Stops scheduling by stopping the render pass associated with the wrapped SwapchainFrame. Then also stops the command buffer itself. */
        void schedule_stop();

        /* "Renders" the frame by sending the internal draw queue to the given device queue. */
        void submit(const VkQueue& vk_queue);

        /* Returns the index of the internal frame. */
        inline uint32_t index() const { return this->swapchain_frame->index(); }

        /* Copy assignment operator for the ConceptualFrame class, which is deleted. */
        ConceptualFrame& operator=(const ConceptualFrame& other) = delete;
        /* Move assignment operator for the ConceptualFrame class. */
        inline ConceptualFrame& operator=(ConceptualFrame&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ConceptualFrame class. */
        friend void swap(ConceptualFrame& cf1, ConceptualFrame& cf2);

    };

    /* Swap operator for the ConceptualFrame class. */
    void swap(ConceptualFrame& cf1, ConceptualFrame& cf2);

}

#endif
