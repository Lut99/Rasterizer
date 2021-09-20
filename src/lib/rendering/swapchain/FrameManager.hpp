/* FRAME MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   08/09/2021, 23:33:27
 * Last edited:
 *   9/20/2021, 9:16:25 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the FrameManager class, which is in charge of retrieving a
 *   conceptual frame from the swapchain.
**/

#ifndef RENDERING_FRAME_MANAGER_HPP
#define RENDERING_FRAME_MANAGER_HPP

#include "tools/Array.hpp"

#include "../memory_manager/MemoryManager.hpp"
#include "../descriptors/DescriptorSetLayout.hpp"
#include "../renderpass/RenderPass.hpp"
#include "../depthtesting/DepthStencil.hpp"

#include "Swapchain.hpp"
#include "SwapchainFrame.hpp"
#include "ConceptualFrame.hpp"

namespace Makma3D::Rendering {
    /* The FrameManager class, which manages and synchronizes swapchain frame access. */
    class FrameManager {
    public:
        /* The channel for the FrameManager. */
        static constexpr const char* channel = "FrameManager";
        /* The maximum number of frames that will be in-flight. */
        static constexpr const uint32_t max_frames_in_flight = 2;

        /* A MemoryManager with which we allocate stuff. */
        Rendering::MemoryManager& memory_manager;
        /* The Swapchain from which we draw frames. */
        const Rendering::Swapchain& swapchain;

    private:
        /* List of SwapchainFrames from which we pick. */
        Tools::Array<Rendering::SwapchainFrame> swapchain_frames;
        /* List of ConceptualFrames which we can return. */
        Tools::Array<Rendering::ConceptualFrame> conceptual_frames;

        /* Index that keeps track of the ConceptualFrame we next intend to return. */
        uint32_t frame_index;

    public:
        /* Constructor for the FrameManager class, which takes a MemoryManager for stuff allocation, a Swapchain to draw images from, a layout for the frame's global descriptor, a layout for the material descriptors and a layout for the frame's per-object descriptors. */
        FrameManager(Rendering::MemoryManager& memory_manager, const Rendering::Swapchain& swapchain, const Rendering::DescriptorSetLayout& global_layout, const Rendering::DescriptorSetLayout& material_layout, const Rendering::DescriptorSetLayout& object_layout);
        /* Copy constructor for the FrameManager class, which is deleted. */
        FrameManager(const FrameManager& other) = delete;
        /* Move constructor for the FrameManager class. */
        FrameManager(FrameManager&& other);
        /* Destructor for the FrameManager class. */
        ~FrameManager();

        /* Resizes the FrameManager by getting all swapchain images again. */
        void resize();

        /* Binds the FrameManager to a render pass and a depth stencil by retrieving the swapchain frames. Must be done at least once. */
        void bind(const Rendering::RenderPass& render_pass, const Rendering::DepthStencil& depth_stencil);
        /* Returns a new ConceptualFrame to which the render system can render. Blocks until any such frame is available. If it returns a nullptr, that means that the swapchain is out of date for some reason. */
        Rendering::ConceptualFrame* get_frame();
        /* Schedules the given frame for presentation once rendering to it has been completed. Returns whether or not the window needs to be resized. */
        bool present_frame(const Rendering::ConceptualFrame* conceptual_frame);

        /* Copy assignment operator for the FrameManager class, which is deleted. */
        FrameManager& operator=(const FrameManager& other) = delete;
        /* Move assignment operator for the FrameManager class. */
        inline FrameManager& operator=(FrameManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the FrameManager class. */
        friend void swap(FrameManager& fm1, FrameManager& fm2);

    };

    /* Swap operator for the FrameManager class. */
    void swap(FrameManager& fm1, FrameManager& fm2);

}

#endif
