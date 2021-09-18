/* INPUT ASSEMBLY STATE.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 17:11:24
 * Last edited:
 *   11/09/2021, 17:11:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the InputAssemblyState class, which is a group of properties of
 *   a Pipeline relating to how the vertices are treated.
**/

#ifndef RENDERING_INPUT_ASSEMBLY_HPP
#define RENDERING_INPUT_ASSEMBLY_HPP

#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The InputAssemblyState class, which defines how the Pipeline will treat the input data. */
    class InputAssemblyState {
    public:
        /* The topology we use for the input vertices. */
        VkPrimitiveTopology topology;
        /* Whether or not we can stop a primitive from being drawn halfway by using special vertex values (all one's). */
        VkBool32 restart_enable;
    
    public:
        /* Default constructor for the InputAssemblyState class. */
        InputAssemblyState();
        /* Constructor for the InputAssemblyState class, which takes the topology of the vertices given (i.e., points, lines, triangles, etc) and whether or not a special vertex index is defined that 'restarts' drawing a primitive, i.e., quits drawing a triangle prematurely. Note that any topology other than triangles probably requires GPU extensions. */
        InputAssemblyState(VkPrimitiveTopology topology, VkBool32 restart_enable = VK_FALSE);

        /* Returns a VkPipelineInputAssemblyStateCreateInfo struct containing the settings in this InputAssemblyState. */
        VkPipelineInputAssemblyStateCreateInfo get_info() const;

    };

}

#endif
