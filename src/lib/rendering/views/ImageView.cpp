/* IMAGE VIEW.cpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:15:51
 * Last edited:
 *   25/08/2021, 16:15:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ImageView class, which wraps a single ImageView. Is
 *   managed by the ImageViewPool, though.
**/

#include "tools/Logger.hpp"

#include "ImageView.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** IMAGEVIEW CLASS *****/
/* Constructor for the ImageView class, which takes the ImageView to wrap, the image the view wraps and the view's format. */
ImageView::ImageView(VkImageView vk_view, VkImage image, VkFormat view_format) :
    vk_view(vk_view),
    init_data(InitData({ image, view_format }))
{}

/* Private destructor for the ImageView class. */
ImageView::~ImageView() {}
