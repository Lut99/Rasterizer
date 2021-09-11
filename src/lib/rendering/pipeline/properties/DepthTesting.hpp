/* DEPTH TESTING.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 17:24:32
 * Last edited:
 *   11/09/2021, 17:24:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DepthTesting class, which describes the properties of the
 *   depthtesting part of a Pipeline.
**/

#ifndef RENDERING_DEPTH_TESTING_HPP
#define RENDERING_DEPTH_TESTING_HPP

#include <vulkan/vulkan.h>

#include <type_traits>

namespace Makma3D::Rendering {
    /* The DepthTesting class, which groups a pipeline's depthtesting-related properties. */
    class DepthTesting {
    private:
        /* The VkPipelineDepthStencilStateCreateInfo that we wrap. */
        VkPipelineDepthStencilStateCreateInfo vk_depth_stencil_state;
    
    public:
        /* Constructor for the DepthTesting class, which takes that it is enabled (true_type) and the compare operation for when a new fragment needs to be tested for depth. */
        DepthTesting(const std::true_type&, VkCompareOp compare_op);
        /* Constructor for the DepthTesting class, which takes that it is disabled. */
        DepthTesting(const std::false_type&);

        /* Explicitly returns the internal VkPipelineDepthStencilStateCreateInfo object. */
        inline const VkPipelineDepthStencilStateCreateInfo& info() const { return this->vk_depth_stencil_state; }
        /* Implicitly returns the internal VkPipelineDepthStencilStateCreateInfo object. */
        inline operator const VkPipelineDepthStencilStateCreateInfo&() const { return this->vk_depth_stencil_state; }

    };

}

#endif
