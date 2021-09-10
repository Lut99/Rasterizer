/* VERTEX.hpp
 *   by Lut99
 *
 * Created:
 *   01/07/2021, 14:35:05
 * Last edited:
 *   01/07/2021, 14:35:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Describes how a single Vertex looks like. Also contains code to create
 *   Vulkan descriptions for itself.
**/

#ifndef MODELS_VERTEX_HPP
#define MODELS_VERTEX_HPP

#include "glm/glm.hpp"
#include <vulkan/vulkan.h>

#include "tools/Array.hpp"

namespace Makma3D::Rendering {
    /* Struct to contain a single Vertex. */
    struct Vertex {
        /* The position of the vertex. */
        glm::vec3 pos;
        /* The colour of the vertex. */
        glm::vec3 colour;
        /* The texture coordinate of the vertex. */
        glm::vec2 texel;

        /* Default constructor for the Vertex struct. */
        Vertex();
        /* Constructor for the Vertex struct, which takes the position and colour. */
        Vertex(const glm::vec3& pos, const glm::vec3& colour);
        /* Constructor for the Vertex struct, which takes the position, colour and texel coordinate. */
        Vertex(const glm::vec3& pos, const glm::vec3& colour, const glm::vec2& texel);

        /* Static function that returns the binding description for the vertex. */
        static VkVertexInputBindingDescription input_binding_description();
        /* Static function that returns the attribute descriptions for the vertex. */
        static Tools::Array<VkVertexInputAttributeDescription> input_attribute_descriptions();

    };

}

#endif
