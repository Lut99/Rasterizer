/* SIMPLE TEXTURED.hpp
 *   by Lut99
 *
 * Created:
 *   29/09/2021, 14:07:24
 * Last edited:
 *   29/09/2021, 14:07:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SimpleTextured class, which carries data needed for a
 *   material with a texture and no lighting. Extends from the base
 *   Material class, and is thus managed by the MaterialPool.
**/

#ifndef MATERIALS_SIMPLE_TEXTURED_HPP
#define MATERIALS_SIMPLE_TEXTURED_HPP

#include "../../Material.hpp"
#include "../../textures/Texture.hpp"

namespace Makma3D::Materials {
    /* The SimpleTextured class, which carries information needed for the similarly named material. */
    class SimpleTextured: public Materials::Material {
    public:
        /* The texture of the SimpleTextured material. */
        const Materials::Texture* texture;

    private:
        /* Constructor for the SimpleTextured class, which takes its name and the texture it is supposed to have. */
        SimpleTextured(const std::string& name, const Materials::Texture* texture);
        /* Destructor for the SimpleTextured class, which is virtual and private. */
        virtual ~SimpleTextured();

        /* Declare the MaterialPool as our friend. */
        friend class MaterialPool;
    
    public:
        /* Copy constructor for the SimpleTextured class, which is deleted. */
        SimpleTextured(const SimpleTextured& other) = delete;
        /* Move constructor for the SimpleTextured class, which is deleted. */
        SimpleTextured(SimpleTextured&& other) = delete;

        // /* Returns the VkImage that carries the texture's data. */
        // inline const VkImage& image() const { return this->texture->image(); }
        // /* Returns the VkImageView needed to access the texture's data. */
        // inline const VkImageView& view() const { return this->texture->view(); }
        // /* Returns the VkSampler needed to access the texture's data in the shader. */
        // inline const VkSampler& sampler() const { return this->texture->sampler(); }

        /* Copy assignment operator for the SimpleTextured class, which is deleted. */
        SimpleTextured& operator=(const SimpleTextured& other) = delete;
        /* Move assignment operator for the SimpleTextured class, which is deleted. */
        SimpleTextured& operator=(SimpleTextured&& other) = delete;

    };
}

#endif
