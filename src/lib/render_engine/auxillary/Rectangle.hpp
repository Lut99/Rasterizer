/* RECTANGLE.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 13:01:22
 * Last edited:
 *   20/06/2021, 13:01:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a very simple Rectangle struct, that is used to more easily
 *   pass rect information to the Vulkan lib.
**/

#ifndef RENDERING_RECTANGLE_HPP
#define RENDERING_RECTANGLE_HPP

#include <vulkan/vulkan.h>

namespace Rasterizer::Rendering {
    /* A very simple Rectangle struct, mostly used to pass the four floats needed to describe one. */
    class Rectangle {
    public:
        /* The x coordinate of the top-left corner of the Rectangle. */
        float x;
        /* The y coordinate of the top-left corner of the Rectangle. */
        float y;
        /* The width of the rectangle, in pixels. */
        float w;
        /* The height of the rectangle, in pixels. */
        float h;


        /* Constructor for the Rectangle class, which takes the four floats. */
        Rectangle(float x, float y, float w, float h);
        /* Constructor for the Rectangle class, which takes the position as two floats but the size as a VkExtent2D. */
        Rectangle(float x, float y, const VkExtent2D& vk_extent);
        /* Constructor for the Rectangle class, which takes the position as a VkOffset2D but the size as two floats. */
        Rectangle(const VkOffset2D& vk_offset, float w, float h);
        /* Constructor for the Rectangle class, which takes the position and offset as a VkOffset2D and VkExtent2D, respectively. */
        Rectangle(const VkOffset2D& vk_offset, const VkExtent2D& vk_extent);

        /* Returns the offset of the rectangle as a VkOffset2D struct. */
        inline VkOffset2D offset() const { return {(int32_t) this->x, (int32_t) this->y}; }
        /* Returns the offset of the rectangle as a VkExtent2D struct. */
        inline VkExtent2D extent() const { return {(uint32_t) this->w, (uint32_t) this->h}; }
    };
}

#endif
