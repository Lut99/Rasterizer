/* RASTERIZATION.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:04:49
 * Last edited:
 *   11/09/2021, 18:04:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Rasterization class, which is a collection of properties for
 *   how the Pipeline will rasterizer the vertices.
**/

#ifndef RENDERING_RASTERIZER_HPP
#define RENDERING_RASTERIZER_HPP

#include <type_traits>
#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The Rasterization class, which is a collection of Pipeline-properties that relate to the specific rasterization stage of the pipeline. */
    class Rasterization {
    private:
        /* The VkPipelineRasterizationStateCreateInfo struct that we wrap. */
        VkPipelineRasterizationStateCreateInfo vk_rasterizer_state;
    
    public:
        /* Constructor for the Rasterization class, which takes the rasterizer-specific properties.
         *
         * @param enabled Whether to enable the rasterization stage or not. Technically discards the result if disabled, so the other properties still have to be valid.
         * @param cull_mode Describes how to cull faces that are behind others
         * @param front_face Describes what the front face of an object is (i.e., which side to cull)
         * @param depth_clamp Describes whether to clamp objects that are too far or too near to their plane instead of simply not rendering them. Needs a special GPU feature to enable.
         * @param polygon_mode Describes how to "colour" the given geometry. For example, can be set to fill the whole vertex, or only its lines, etc. Needs a special GPU feature for anything other than FILL.
         * @param line_width The width of the lines that the rasterizer draws. Needs a special GPU feature to grow beyond 1.0f.
         * @param disable_rasterizer If set to TRUE, then discards the output of the rasterizer, disabling it in effect.
         */
        Rasterization(VkBool32 enabled, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_clamp = VK_FALSE, VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, float line_width = 1.0f);

        /* Explicitly returns the internal VkPipelineRasterizationStateCreateInfo object. */
        inline const VkPipelineRasterizationStateCreateInfo& info() const { return this->vk_rasterizer_state; }
        /* Implicitly returns the internal VkPipelineRasterizationStateCreateInfo object. */
        inline operator const VkPipelineRasterizationStateCreateInfo&() const { return this->vk_rasterizer_state; }

    };

}

#endif
