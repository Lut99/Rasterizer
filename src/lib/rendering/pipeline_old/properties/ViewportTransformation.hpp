/* VIEWPORT TRANSFORMATION.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 17:40:24
 * Last edited:
 *   11/09/2021, 17:40:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ViewportTransformation class, which is a collection of
 *   properties for a Pipeline that describe where the target framebuffer
 *   should be filled in exactly.
**/

#ifndef RENDERING_VIEWPORT_TRANSFORMATION_HPP
#define RENDERING_VIEWPORT_TRANSFORMATION_HPP

#include <vulkan/vulkan.h>
#include "glm/glm.hpp"

namespace Makma3D::Rendering {
    /* The ViewportTransformation class, which is a collection of Pipeline properties that determine the resulting viewport's size and cutoff. */
    class ViewportTransformation {
    private:
        /* The Viewport descriptor of the viewport. */
        VkViewport vk_viewport;
        /* The rectangle describing the scissor. */
        VkRect2D vk_scissor;

        /* The VkPipelineViewportStateCreateInfo struct that we wrap. */
        VkPipelineViewportStateCreateInfo vk_viewport_state;
    
    public:
        /* Constructor for the ViewportTransformation class, which takes the coordinates and the extent for the target viewport (which will stretch the result) and the target scissor (which will cutoff the result). */
        ViewportTransformation(VkOffset2D viewport_offset, VkExtent2D viewport_extent, VkOffset2D scissor_offset, VkExtent2D scissor_extent);
        /* Copy constructor for the ViewportTransformation class. */
        ViewportTransformation(const ViewportTransformation& other);
        /* Move constructor for the ViewportTransformation class. */
        ViewportTransformation(ViewportTransformation&& other);
        /* Destructor for the ViewportTransformation class. */
        ~ViewportTransformation();

        /* Explicitly returns the internal VkPipelineViewportStateCreateInfo struct. */
        inline const VkPipelineViewportStateCreateInfo& info() const { return this->vk_viewport_state; }
        /* Implicitly returns the internal VkPipelineViewportStateCreateInfo struct. */
        inline operator const VkPipelineViewportStateCreateInfo&() const { return this->vk_viewport_state; }

        /* Copy assignment operator for the ViewportTransformation class. */
        inline ViewportTransformation& operator=(const ViewportTransformation& other) { return *this = ViewportTransformation(other); }
        /* Move assignment operator for the ViewportTransformation class. */
        inline ViewportTransformation& operator=(ViewportTransformation&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ViewportTransformation class. */
        friend void swap(ViewportTransformation& vt1, ViewportTransformation& vt2);

    };

    /* Swap operator for the ViewportTransformation class. */
    void swap(ViewportTransformation& vt1, ViewportTransformation& vt2);

}

#endif
