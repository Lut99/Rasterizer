/* RECTANGLE.cpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 13:01:19
 * Last edited:
 *   20/06/2021, 13:01:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a very simple Rectangle struct, that is used to more easily
 *   pass rect information to the Vulkan lib.
**/

#include "Rectangle.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Vulkan;


/***** RECTANGLE CLASS *****/
/* Constructor for the Rectangle class, which takes the four floats. */
Rectangle::Rectangle(float x, float y, float w, float h) :
    x(x),
    y(y),
    w(w),
    h(h)
{}

/* Constructor for the Rectangle class, which takes the position as two floats but the size as a VkExtent2D. */
Rectangle::Rectangle(float x, float y, const VkExtent2D& vk_extent):
    x(x),
    y(y),
    w(vk_extent.width),
    h(vk_extent.height)
{}

/* Constructor for the Rectangle class, which takes the position as a VkOffset2D but the size as two floats. */
Rectangle::Rectangle(const VkOffset2D& vk_offset, float w, float h):
    x(vk_offset.x),
    y(vk_offset.y),
    w(w),
    h(h)
{}

/* Constructor for the Rectangle class, which takes the position and offset as a VkOffset2D and VkExtent2D, respectively. */
Rectangle::Rectangle(const VkOffset2D& vk_offset, const VkExtent2D& vk_extent):
    x(vk_offset.x),
    y(vk_offset.y),
    w(vk_extent.width),
    h(vk_extent.height)
{}

