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
    public:
        /* The Viewport descriptor of the viewport. */
        VkViewport viewport;
        /* The rectangle describing the scissor. */
        VkRect2D scissor;

    public:
        /* Default constructor for the ViewportTransformation class. */
        ViewportTransformation();
        /* Constructor for the ViewportTransformation class, which takes the coordinates and the extent for the target viewport (which will stretch the result) and the target scissor (which will cutoff the result). */
        ViewportTransformation(VkOffset2D viewport_offset, VkExtent2D viewport_extent, VkOffset2D scissor_offset, VkExtent2D scissor_extent);

        /* Returns a VkPipelineViewportStateCreateInfo struct populated with the internal properties. */
        VkPipelineViewportStateCreateInfo get_info() const;

    };

}

#endif
