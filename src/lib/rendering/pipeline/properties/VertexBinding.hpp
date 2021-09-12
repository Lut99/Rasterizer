/* VERTEX BINDING.hpp
 *   by Lut99
 *
 * Created:
 *   12/09/2021, 14:15:29
 * Last edited:
 *   12/09/2021, 14:15:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the VertexBinding class, which groups together the
 *   information needed to describe a vertex buffer binding.
**/

#ifndef RENDERING_VERTEX_BINDING_HPP
#define RENDERING_VERTEX_BINDING_HPP

#include <vulkan/vulkan.h>

namespace Makma3D::Rendering {
    /* The VertexBinding class, which describes the binding of a single Vertex buffer. */
    class VertexBinding {
    private:
        /* The VkVertexInputBindingDescription struct that we wrap. */
        VkVertexInputBindingDescription vk_binding_description;

    public:
        /* Constructor for the VertexBinding class, which takes the binding index and the size of each Vertex object in this buffer. */
        VertexBinding(uint32_t bind_index, uint32_t vertex_size);

        /* Explicitly returns the internal VkVertexInputBindingDescription object. */
        inline const VkVertexInputBindingDescription& description() const { return this->vk_binding_description; }
        /* Implicitly returns the internal VkVertexInputBindingDescription object. */
        inline operator const VkVertexInputBindingDescription&() const { return this->vk_binding_description; }

    };

}

#endif
