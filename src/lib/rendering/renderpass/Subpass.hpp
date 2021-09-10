/* SUBPASS.hpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 13:30:01
 * Last edited:
 *   29/06/2021, 13:30:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Subpass class, which is used internally by the RenderPass
 *   to manage a subpass' memory.
**/

#ifndef RENDERING_SUBPASS_HPP
#define RENDERING_SUBPASS_HPP

#include <vulkan/vulkan.h>

#include "tools/Array.hpp"

namespace Makma3D::Rendering {
    /* The Subpass class, which is used to manage memory around a VkSubpassDescription object. */
    class Subpass {
    private:
        /* The VkSubpassDescription object we wrap. */
        VkSubpassDescription vk_subpass;

        /* The color attachment references for this subpass. */
        VkAttachmentReference* color_attachment_refs;
        /* The number of attachment references. */
        uint32_t n_color_attachments;

        /* The depth attachment reference for this subpass. */
        VkAttachmentReference* depth_attachment_ref;

    public:
        /* Constructor for the Subpass class, which takes a list of attachment index/layout pairs to describe the colour references, an index/layout pair for the depth reference and the bind point in the pipeline (which is usually the default of graphics). */
        Subpass(const Tools::Array<std::pair<uint32_t, VkImageLayout>>& color_attachment_refs, const std::pair<uint32_t, VkImageLayout>& depth_attachment_ref, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);
        /* Copy constructor for the Subpass class. */
        Subpass(const Subpass& other);
        /* Move constructor for the Subpass class. */
        Subpass(Subpass&& other);
        /* Destructor for the Subpass class. */
        ~Subpass();

        /* Explicitly returns the internal VkSubpassDescription object. */
        inline const VkSubpassDescription& subpass() const { return this->vk_subpass; }
        /* Implicitly returns the internal VkSubpassDescription object. */
        inline operator const VkSubpassDescription&() const { return this->vk_subpass; }

        /* Copy assignment operator for the Subpass class. */
        inline Subpass& operator=(const Subpass& other) { return *this = Subpass(other); }
        /* Move assignment operator for the Subpass class. */
        inline Subpass& operator=(Subpass&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the Subpass class. */
        friend void swap(Subpass& s1, Subpass& s2);

    };

    /* Swap operator for the Subpass class. */
    void swap(Subpass& s1, Subpass& s2);
}

#endif
