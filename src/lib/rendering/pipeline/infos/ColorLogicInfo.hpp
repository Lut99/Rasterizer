/* COLOR LOGIC INFO.hpp
 *   by Lut99
 *
 * Created:
 *   19/09/2021, 15:06:04
 * Last edited:
 *   19/09/2021, 15:06:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorLogicInfo, which bundles a
 *   VkPipelineColorBlendStateCreateInfo struct and the structs it needs to
 *   exist in one place.
**/

#ifndef RENDERING_COLOR_LOGIC_INFO_HPP
#define RENDERING_COLOR_LOGIC_INFO_HPP

#include <vulkan/vulkan.h>
#include "../properties/ColorLogic.hpp"

namespace Makma3D::Rendering {
    /* The ColorLogicInfo class, which wraps a VkPipelineColorBlendStateCreateInfo and associated memory. */
    class ColorLogicInfo {
    private:
        /* The array of VkPipelineColorBlendAttachmentState structs that we need for the main struct. */
        VkPipelineColorBlendAttachmentState* vk_color_blend_states;
        /* The number of color blend states in the array. */
        uint32_t vk_color_blend_states_size;

        /* The main VkPipelineColorBlendStateCreateInfo struct we wrap. */
        VkPipelineColorBlendStateCreateInfo vk_color_blend_state_info;

    public:
        /* Constructor for the ColorLogicInfo class, which takes a normal ColorLogic object to initialize itself with. */
        ColorLogicInfo(const Rendering::ColorLogic& color_logic);
        /* Copy constructor for the ColorLogicInfo class. */
        ColorLogicInfo(const ColorLogicInfo& other);
        /* Move constructor for the ColorLogicInfo class. */
        ColorLogicInfo(ColorLogicInfo&& other);
        /* Destructor for the ColorLogicInfo class. */
        ~ColorLogicInfo();

        /* Expliticly returns the internal VkPipelineColorBlendStateCreateInfo object. */
        inline const VkPipelineColorBlendStateCreateInfo& vulkan() const { return this->vk_color_blend_state_info; }
        /* Implicitly returns the internal VkPipelineColorBlendStateCreateInfo object. */
        inline operator const VkPipelineColorBlendStateCreateInfo&() const { return this->vk_color_blend_state_info; }

        /* Copy assignment operator for the ColorLogicInfo class. */
        inline ColorLogicInfo& operator=(const ColorLogicInfo& other) { return *this = ColorLogicInfo(other); }
        /* Move assignment operator for the ColorLogicInfo class. */
        inline ColorLogicInfo& operator=(ColorLogicInfo&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ColorLogicInfo class. */
        friend void swap(ColorLogicInfo& cli1, ColorLogicInfo& cli2);

    };
    
    /* Swap operator for the ColorLogicInfo class. */
    void swap(ColorLogicInfo& cli1, ColorLogicInfo& cli2);
}

#endif
