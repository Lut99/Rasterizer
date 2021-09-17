/* COLOR LOGIC.hpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 18:38:53
 * Last edited:
 *   11/09/2021, 18:38:53
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ColorLogic class, which describes what the Pipeline
 *   should do with pixels in the target framebuffer(s).
**/

#ifndef RENDERING_COLOR_LOGIC_HPP
#define RENDERING_COLOR_LOGIC_HPP

#include <type_traits>
#include <vulkan/vulkan.h>

#include "tools/Array.hpp"

#include "ColorBlending.hpp"

namespace Makma3D::Rendering {
    /* The ColorLogic class, which describes what to do with pixels already in framebuffer(s). */
    class ColorLogic {
    private:
        /* The list of ColorBlending struct which we used to describe the color blend state for each framebuffer. */
        Tools::Array<Rendering::ColorBlending> color_blend_attachments;
        /* The list of VkPipelineColorBlendAttachmentState structs that are wrapped by the given color blend attachments. */
        Tools::Array<VkPipelineColorBlendAttachmentState> vk_color_blend_attachments;

        /* The VkPipelineColorBlendStateCreateInfo object we wrap. */
        VkPipelineColorBlendStateCreateInfo vk_color_blend_state;

    public:
        /* Constructor for the ColorLogic class, which takes whether or not to enable the logic operation, which operation to apply if it is and the list of per-framebuffer ColorBlending properties. Note that there has to be one ColorBlending struct per framebuffer. */
        ColorLogic(VkBool32 enabled, VkLogicOp logic_op, const Tools::Array<Rendering::ColorBlending>& color_blend_attachments);
        /* Copy constructor for the ColorLogic class. */
        ColorLogic(const ColorLogic& other);
        /* Move constructor for the ColorLogic class. */
        ColorLogic(ColorLogic&& other);
        /* Destructor for the ColorLogic class. */
        ~ColorLogic();

        /* Explicitly returns the internal VkPipelineColorBlendStateCreateInfo object. */
        inline const VkPipelineColorBlendStateCreateInfo& info() const { return this->vk_color_blend_state; }
        /* Implicitly returns the internal VkPipelineColorBlendStateCreateInfo object. */
        inline operator const VkPipelineColorBlendStateCreateInfo&() const { return this->vk_color_blend_state; }

        /* Copy assignment operator for the ColorLogic class. */
        inline ColorLogic& operator=(const ColorLogic& other) { return *this = ColorLogic(other); }
        /* Move assignment operator for the ColorLogic class. */
        inline ColorLogic& operator=(ColorLogic&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ColorLogic class. */
        friend void swap(ColorLogic& cl1, ColorLogic& cl2);

    };

    /* Swap operator for the ColorLogic class. */
    void swap(ColorLogic& cl1, ColorLogic& cl2);

}

#endif
