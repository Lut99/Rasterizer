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

#include "tools/CppDebugger.hpp"

#include "TextureSystem.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Textures;
using namespace CppDebugger::SeverityValues;


/***** TEXTURESYSTEM CLASS *****/
/* Constructor for the TextureSystem class, which takes a reference to the MemoryManager from which we allocate buffers and images and junk. */
TextureSystem::TextureSystem(Rendering::MemoryManager& memory_manager) :
    memory_manager(memory_manager)
{}

/* Copy constructor for the TextureSystem class. */
TextureSystem::TextureSystem(const TextureSystem& other) :
    memory_manager(other.memory_manager)
{}

/* Move constructor for the TextureSystem class. */
TextureSystem::TextureSystem(TextureSystem&& other) :
    memory_manager(std::move(other.memory_manager))
{}

/* Destructor for the TextureSystem class. */
TextureSystem::~TextureSystem() {}



/* Loads a new texture for the given entity using the given format. */
void TextureSystem::load_texture(ECS::EntityManager& entity_manager, entity_t entity, const std::string& path, TextureFormat format = TextureFormat::png) {
    DENTER("Textures::TextureSystem::load_texture");
    DLOG(info, "Loading texture for entity " + std::to_string(entity) + "...");
    DINDENT;

    // Get the entity's component
    ECS::Texture& texture = entity_manager.get_component<ECS::Texture>(entity);

    // Load the model according to the given format
    switch (format) {
        case TextureFormat::png:
            // Use the load function from the modelloader
            DLOG(info, "Loading '" + path + "' as .png file...");
            // load_obj_model(this->memory_manager, meshes, path);
            break;

        default:
            DLOG(fatal, "Unsupported texture format '" + texture_format_names[(int) format] + "'");
            break;

    }

    // Done
    DDEDENT;
    DRETURN;
}

/* Unloads the texture loaded for the given entity. */
void TextureSystem::unload_texture(ECS::EntityManager& entity_manager, entity_t entity) {
    DENTER("Textures::TextureSystem::unload_texture");
    DLOG(info, "Deallocating texture for entity " + std::to_string(entity) + "...");
    DINDENT;

    // Try to get the entity's texture
    ECS::Texture& texture = entity_manager.get_component<ECS::Texture>(entity);
    
    // Deallocate the image
    this->memory_manager.draw_pool.deallocate(texture.image_h);

    // Done
    DDEDENT;
    DRETURN;
}



/* Binds the model-related buffers and junk for the given mesh component to the given command buffer. */
void TextureSystem::schedule(const ECS::Texture& entity_texture, const Rendering::CommandBuffer& draw_cmd) const {
    DENTER("Textures::TextureSystem::schedule");



    DRETURN;
}



/* Swap operator for the TextureSystem class. */
void Rasterizer::Textures::swap(TextureSystem& ts1, TextureSystem& ts2) {
    DENTER("Textures::swap(TextureSystem)");

    #ifndef NDEBUG
    if (&ts1.memory_manager != &ts2.memory_manager) {
        DLOG(fatal, "Cannot swap texture systems with different memory managers.");
    }
    #endif

    // Simply swap it all
    using std::swap;

    // Done
    DRETURN;
}
