/* IMAGE VIEW POOL.cpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:11:20
 * Last edited:
 *   25/08/2021, 16:11:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Pool that is in charge of managing image views for the Rasterizer.
 *   Unlike the other memory pools, does not actually allocate data on the
 *   GPU, but just manages CPU memory instead.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "ImageViewPool.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** POPULATE FUNCTIONS *****/
/* Populates the given VkImageViewCreateInfo struct. */
static void populate_view_info(VkImageViewCreateInfo& view_info, VkImage vk_image, VkFormat vk_format) {
    // Set to default
    view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    // Set the image and its type (mostly how many dimensions etc)
    view_info.image = vk_image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

    // Set the format of the image view
    view_info.format = vk_format;

    // Set the subresource stuff
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
}





/***** IMAGEVIEWPOOL CLASS *****/
/* Constructor for the ImageViewPool class, which takes a GPU to allocate image views on. */
ImageViewPool::ImageViewPool(const Rendering::GPU& gpu) :
    gpu(gpu)
{
    logger.logc(Verbosity::important, ImageViewPool::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, ImageViewPool::channel, "Init success.");
}

/* Copy constructor for the ImageViewPool class. */
ImageViewPool::ImageViewPool(const ImageViewPool& other) :
    gpu(other.gpu)
{
    logger.logc(Verbosity::debug, ImageViewPool::channel, "Copying...");

    // Don't copy any image views

    logger.logc(Verbosity::debug, ImageViewPool::channel, "Copy success.");
}

/* Move constructor for the ImageViewPool class. */
ImageViewPool::ImageViewPool(ImageViewPool&& other) :
    gpu(other.gpu),
    views(std::move(other.views))
{
    // Clear the other's views
    other.views.clear();
}

/* Destructor for the ImageViewPool class. */
ImageViewPool::~ImageViewPool() {
    logger.logc(Verbosity::important, ImageViewPool::channel, "Cleaning...");

    // Delete the views if neccesary
    if (this->views.size() > 0) {
        logger.logc(Verbosity::details, ImageViewPool::channel, "Destroying image views...");
        for (uint32_t i = 0; i < this->views.size(); i++) {
            delete this->views[i];
        }
    }

    logger.logc(Verbosity::important, ImageViewPool::channel, "Cleaned.");
}



/* Allocates a new image view that offers insight in the given image under the given format. */
ImageView* ImageViewPool::allocate(Rendering::Image* image, VkFormat view_format) {
    // Create the create info for the image view
    VkImageViewCreateInfo view_info;
    populate_view_info(view_info, image->image(), view_format);

    // Create the image view
    VkImageView view;
    VkResult vk_result;
    if ((vk_result = vkCreateImageView(this->gpu, &view_info, nullptr, &view)) != VK_SUCCESS) {
        logger.fatalc(ImageViewPool::channel, "Could not create image view: ", vk_error_map[vk_result]);
    }

    // Create the wrapper and store it internally
    ImageView* result = new ImageView(view, image->image(), view_format);
    this->views.push_back(result);

    // Done
    return result;
}

/* Allocates a new image view that is a copy of the given image view. */
ImageView* ImageViewPool::allocate(const ImageView* view) {
    // Create the create info for the image view
    VkImageViewCreateInfo view_info;
    populate_view_info(view_info, view->init_data.image, view->init_data.view_format);

    // Create the image view
    VkImageView new_view;
    VkResult vk_result;
    if ((vk_result = vkCreateImageView(this->gpu, &view_info, nullptr, &new_view)) != VK_SUCCESS) {
        logger.fatalc(ImageViewPool::channel, "Could not create image view copy: ", vk_error_map[vk_result]);
    }

    // Create the wrapper and store it internally
    ImageView* result = new ImageView(new_view, view->init_data.image, view->init_data.view_format);
    this->views.push_back(result);

    // Done
    return result;
}

/* Deallocates the given ImageView. */
void ImageViewPool::free(const ImageView* view) {
    // Try to remove the pointer from the list
    bool found = false;
    for (uint32_t i = 0; i < this->views.size(); i++) {
        if (this->views[i] == view) {
            this->views.erase(i);
            found = true;
            break;
        }
    }
    if (!found) {
        logger.fatalc(ImageViewPool::channel, "Tried to free ImageView that was not allocated with this pool.");
    }

    // Destroy the vulkan object
    vkDestroyImageView(this->gpu, view->vk_view, nullptr);
    
    // Destroy the pointer itself
    delete view;
}



/* Swap operator for the ImageViewPool class. */
void Rendering::swap(ImageViewPool& ivp1, ImageViewPool& ivp2) {
    #ifndef NDEBUG
    if (ivp1.gpu != ivp2.gpu) { logger.fatalc(ImageViewPool::channel, "Cannot swap image view pools with different GPUs"); }
    #endif

    // Swap everything
    using std::swap;

    swap(ivp1.views, ivp2.views);
}
