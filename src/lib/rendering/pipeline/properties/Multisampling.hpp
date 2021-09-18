/* MULTISAMPLING.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:19:03
 * Last edited:
 *   11/09/2021, 18:19:03
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Multisampling class, which describes how a Pipeline
 *   should deal with multisampling.
**/

#ifndef RENDERING_MULTISAMPLING_HPP
#define RENDERING_MULTISAMPLING_HPP

#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The Multisampling class, which is a collection of Pipeline properties that describe how to deal with multisampling. */
    class Multisampling {
    // public:
    public:
        // /* Default constructor for the Multisampling class. */
        // Multisampling();
        /* Constructor for the Multisampling class, which takes nothing as we don't do that for now. */
        Multisampling();

        /* Returns a VkPipelineMultisamplingStateCreateInfo struct based on the internal properties. */
        VkPipelineMultisampleStateCreateInfo get_info() const;

    };

}

#endif
