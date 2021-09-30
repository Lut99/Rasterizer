/* TEXTURE.hpp
 *   by Lut99
 *
 * Created:
 *   10/09/2021, 14:09:08
 * Last edited:
 *   10/09/2021, 14:09:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Texture class, which is a TexturePool-managed object that
 *   contains the data needed to render a single texture.
**/

#ifndef MATERIALS_TEXTURE_HPP
#define MATERIALS_TEXTURE_HPP

#include <string>
#include <vulkan/vulkan.h>

#include "rendering/memory/Image.hpp"

namespace Makma3D::Materials {
    /* The Texture class, which carries the data needed to render a texture. */
    class Texture {
    private:
        /* The image we loaded that contains the texture itself. */
        Rendering::Image* _image;

        /* An VkImageView used to interact with the Image. */
        VkImageView _view;
        /* The VkSampler used to access the image from a shader. */
        VkSampler _sampler;

        /* The name/ID of the texture. Used only to identify this Texture's uniqueness and for debugging purposes. */
        std::string _name;


        /* Constructor for the Texture class, which takes the Image, ImageView and Sampler to load and the name/ID of the Texture. */
        Texture(Rendering::Image* image, const VkImageView& view, const VkSampler& sampler, const std::string& name);
        /* Destructor for the Texture class. */
        ~Texture();

        /* Declare the TexturePool as our fwiend. */
        friend class TexturePool;
    
    public:
        /* Copy constructor for the Texture class, which is deleted. */
        Texture(const Texture& other) = delete;
        /* Move constructor for the Texture class, which is deleted. */
        Texture(Texture&& other) = delete;

        

        /* Explicitly returns the layout of the internal Image. */
        inline const VkImageLayout& layout() const { return this->_image->layout(); }
        /* Explicitly returns the internal VkImage object. */
        inline const VkImage& image() const { return this->_image->vulkan(); }
        /* Explicitly returns the internal VkImageView object. */
        inline const VkImageView& view() const { return this->_view; }
        /* Explicitly returns the internal VkSampler object. */
        inline const VkSampler& sampler() const { return this->_sampler; }
        /* Returns the Texture's name/ID. */
        inline const std::string& name() const { return this->_name; }

        /* Copy assignment operator for the Texture class, which is deleted. */
        Texture& operator=(const Texture& other) = delete;
        /* Move assignment operator for the Texture class, which is deleted. */
        Texture& operator=(Texture&& other) = delete;

    };

}

#endif
