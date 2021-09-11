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
    private:
        /* The VkPipelineInputAssemblyStateCreateInfo struct we wrap. */
        VkPipelineInputAssemblyStateCreateInfo vk_input_assembly_state;
    
    public:
        /* Constructor for the InputAssemblyState class, which takes the topology of the vertices given (i.e., points, lines, triangles, etc) and whether or not a special vertex index is defined that 'restarts' drawing a primitive, i.e., quits drawing a triangle prematurely. Note that any topology other than triangles probably requires GPU extensions. */
        InputAssemblyState(VkPrimitiveTopology topology, VkBool32 restart_enable = VK_FALSE);

        /* Explicitly returns the VkPipelineInputAssemblyStateCreateInfo struct we wrap. */
        inline const VkPipelineInputAssemblyStateCreateInfo& info() const { return this->vk_input_assembly_state; }
        /* Implicitly returns the VkPipelineInputAssemblyStateCreateInfo struct we wrap. */
        inline operator const VkPipelineInputAssemblyStateCreateInfo&() const { return this->vk_input_assembly_state; }

    };

}

#endif
