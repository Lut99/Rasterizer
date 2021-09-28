/* TEXTURE.cpp
 *   by Lut99
 *
 * Created:
 *   28/09/2021, 18:34:07
 * Last edited:
 *   28/09/2021, 18:34:07
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Texture class, which is a TexturePool-managed object that
 *   contains the data needed to render a single texture.
**/

#include "Texture.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Materials;


/***** TEXTURE CLASS *****/
/* Constructor for the Texture class, which takes the Image, ImageView and Sampler to load and the name/ID of the Texture. */
Texture::Texture(Rendering::Image* image, const VkImageView& view, const VkSampler& sampler, const std::string& name) :
    _image(image),
    _view(view),
    _sampler(sampler),
    _name(name)
{}

/* Destructor for the Texture class. */
Texture::~Texture() {}
