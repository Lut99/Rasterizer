/* TEXTURE SYSTEM.cpp
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:49:36
 * Last edited:
 *   16/08/2021, 11:49:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the TextureSystem class, which is responsible for loading and
 *   managing textures for an entity.
**/

#include "tools/Logger.hpp"

#include "formats/png/PngLoader.hpp"
#include "TextureSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Textures;


/***** TEXTURESYSTEM CLASS *****/
/* Constructor for the TextureSystem class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
TextureSystem::TextureSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager),
    sampler(memory_manager.gpu, VK_FILTER_LINEAR, VK_TRUE)
{
    logger.logc(Verbosity::important, TextureSystem::channel, "Initializing...");

    // Nothing as of yet

    logger.logc(Verbosity::important, TextureSystem::channel, "Init success.");
}

/* Copy constructor for the TextureSystem class. */
TextureSystem::TextureSystem(const TextureSystem& other) :
    memory_manager(other.memory_manager),
    sampler(other.sampler)
{
    logger.logc(Verbosity::debug, TextureSystem::channel, "Copying...");

    // Nothing as of yet

    logger.logc(Verbosity::debug, TextureSystem::channel, "Copy success.");
}

/* Move constructor for the TextureSystem class. */
TextureSystem::TextureSystem(TextureSystem&& other) :
    memory_manager(other.memory_manager),
    sampler(std::move(other.sampler))
{}

/* Destructor for the TextureSystem class. */
TextureSystem::~TextureSystem() {
    logger.logc(Verbosity::important, TextureSystem::channel, "Cleaning...");

    // Nothing as of yet

    logger.logc(Verbosity::important, TextureSystem::channel, "Cleaned.");
}



/* Loads a new texture for the given entity using the given format. */
void TextureSystem::load_texture(ECS::EntityManager& entity_manager, entity_t entity, const std::string& path, TextureFormat format) {
    logger.logc(Verbosity::important, TextureSystem::channel, "Loading texture for entity ", entity, "...");

    // Get the entity's component
    ECS::Texture& texture = entity_manager.get_component<ECS::Texture>(entity);

    // Load the model according to the given format
    switch (format) {
        case TextureFormat::png:
            // Use the load function from the modelloader
            logger.logc(Verbosity::details, TextureSystem::channel, "Loading '", path, "' as .png file...");
            load_png_texture(this->memory_manager, texture, path);
            break;

        default:
            logger.fatalc(TextureSystem::channel, "Unsupported texture format '", texture_format_names[(int) format], '\'');
            break;

    }
    // Do a debug print just for fun
    logger.logc(Verbosity::debug, TextureSystem::channel, "Loaded texture of ", texture.extent.width, 'x', texture.extent.height, " pixels.");

    // Finally, initialize the image view for this image
    texture.view = this->memory_manager.view_pool.allocate(texture.image, VK_FORMAT_R8G8B8A8_SRGB);
}

/* Unloads the texture loaded for the given entity. */
void TextureSystem::unload_texture(ECS::EntityManager& entity_manager, entity_t entity) {
    logger.logc(Verbosity::important, TextureSystem::channel, "Deallocating texture for entity ", entity, "...");

    // Try to get the entity's texture
    ECS::Texture& texture = entity_manager.get_component<ECS::Texture>(entity);
    
    // Deallocate the view
    this->memory_manager.view_pool.free(texture.view);
    // Deallocate the image
    this->memory_manager.draw_pool.free(texture.image);
}



/* Binds the model-related buffers and junk for the given mesh component to the given command buffer. */
void TextureSystem::schedule(const Rendering::CommandBuffer* draw_cmd, const ECS::Texture& entity_texture, const Rendering::DescriptorSet* descriptor_set) const {
    // Schedule the descriptor population
    descriptor_set->bind(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, { make_tuple(entity_texture.view->view(), entity_texture.image->layout(), this->sampler.sampler()) });
}



/* Swap operator for the TextureSystem class. */
void Rasterizer::Textures::swap(TextureSystem& ts1, TextureSystem& ts2) {
    #ifndef NDEBUG
    if (&ts1.memory_manager != &ts2.memory_manager) { logger.fatalc(TextureSystem::channel, "Cannot swap texture systems with different memory managers."); }
    #endif

    // Simply swap it all
    using std::swap;
    swap(ts1.sampler, ts2.sampler);
}
