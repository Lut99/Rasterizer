/* CONCEPTUAL FRAME.hpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 18:53:40
 * Last edited:
 *   08/09/2021, 18:53:40
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
#include "ecs/components/Meshes.hpp"
#include "models/ModelSystem.hpp"

#include "../data/ObjectData.hpp"
#include "../memory/MemoryManager.hpp"
#include "../memory/Buffer.hpp"
#include "../commandbuffers/CommandBuffer.hpp"
#include "../descriptors/DescriptorSetLayout.hpp"
#include "../descriptors/DescriptorPool.hpp"
#include "../descriptors/DescriptorSet.hpp"
#include "../pipeline/Pipeline.hpp"
#include "../synchronization/Semaphore.hpp"
#include "../synchronization/Fence.hpp"

#include "SwapchainFrame.hpp"

namespace Rasterizer::Rendering {
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
        /* Descriptor set layout for the per-object descriptors. */
        Rendering::DescriptorSetLayout object_layout;

        /* Command buffer for drawing to this Frame. */
        Rendering::CommandBuffer* draw_cmd;
        /* Descriptor pool for all descriptors in this frame. */
        Rendering::DescriptorPool* descriptor_pool;
        
        /* Global descriptor set for this frame. */
        Rendering::DescriptorSet* global_set;
        /* Global camera buffer for this frame. */
        Rendering::Buffer* camera_buffer;

        /* Descriptors for all objects drawn with this buffer. */
        Tools::Array<Rendering::DescriptorSet*> object_sets;
        /* Buffers for all objects. */
        Tools::Array<Rendering::Buffer*> object_buffers;

        /* Semaphore that signals when the image is ready to be rendered to. */
        Rendering::Semaphore image_ready_semaphore;
        /* Semaphore that signals when the image is done being rendered to, i.e., ready to be presented. */
        Rendering::Semaphore render_ready_semaphore;
        /* Fence that is used to prevent the CPU from scheduling this frame again if it's still in flight. */
        Rendering::Fence in_flight_fence;

    public:
        /* Constructor for the ConceptualFrame class, which takes a MemoryManager to be able to draw games, a descriptor set layout for the global descriptor and a descriptor set layout for the per-object descriptors. */
        ConceptualFrame(Rendering::MemoryManager& memory_manager, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& object_layout);
        /* Copy constructor for the ConceptualFrame class, which is deleted. */
        ConceptualFrame(const ConceptualFrame& other) = delete;
        /* Move constructor for the ConceptualFrame class. */
        ConceptualFrame(ConceptualFrame&& other);
        /* Destructor for the ConceptualFrame class. */
        ~ConceptualFrame();

        /* Prepares rendering the frame as new by throwing out old data preparing to render at most the given number of objects. */
        void prepare_render(uint32_t n_objects);

        /* Populates the internal camera buffer with the given projection and view matrices. */
        void upload_camera_data(const glm::mat4& proj_matrix, const glm::mat4& view_matrix);
        /* Uploads object data for the given object to its buffer. */
        void upload_object_data(uint32_t object_index, const Rendering::ObjectData& object_data);

        /* Starts to schedule the render pass associated with the wrapped SwapchainFrame on the internal draw queue, followed by binding the given pipeline. */
        void schedule_start(const Rendering::Pipeline& pipeline);
        /* Schedules frame-global descriptors on the internal draw queue (i.e., binds the camera data and the global descriptor). */
        void schedule_global();
        /* Schedules the given object's buffer (and thus descriptor set) on the internal draw queue. */
        void schedule_object(uint32_t object_index);
        /* Schedules a draw command for the given mesh on the internal draw queue. */
        void schedule_draw(const Models::ModelSystem& model_system, const ECS::Mesh& mesh);
        /* Stops scheduling by stopping the render pass associated with the wrapped SwapchainFrame. Then also stops the command buffer itself. */
        void schedule_stop();

        /* "Renders" the frame by sending the internal draw queue to the given device queue. */
        void submit(const VkQueue& vk_queue);

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
