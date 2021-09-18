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
    public:
        /* Whether or not to enable depthtesting. */
        VkBool32 enabled;
        /* What kind of operation to perform when comparing old pixels with new ones. */
        VkCompareOp compare_op;
    
    public:
        /* Default constructor for the DepthTesting class. */
        DepthTesting();
        /* Constructor for the DepthTesting class, which takes whether to enable depthtesting or not and the compare operation for when a new fragment needs to be tested for depth. */
        DepthTesting(VkBool32 enabled, VkCompareOp compare_op);

        /* Returns a VkPipelineDepthStencilStateCreateInfo populated with the internal properties. */
        VkPipelineDepthStencilStateCreateInfo get_info() const;

    };

}

#endif
