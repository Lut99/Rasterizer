/* OBJECT MANAGER.cpp
 *   by Lut99
 *
 * Created:
 *   17/07/2021, 14:23:40
 * Last edited:
 *   17/07/2021, 14:23:40
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ObjectManager class, which combines models and textures
 *   into objects, which can be updated, applied physics to, etc.
**/

#include "tools/CppDebugger.hpp"

#include "ObjectManager.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Objects;
using namespace CppDebugger::SeverityValues;


/***** OBJECTMANAGER CLASS *****/
/* Constructor for the ObjectManager class, which takes a window to take user input from and a ModelManager to load models with. */
ObjectManager::ObjectManager(Window& window, Models::ModelManager& model_manager) :
    window(window),
    model_manager(model_manager)
{}

/* Copy constructor for the ObjectManager class. */
ObjectManager::ObjectManager(const ObjectManager& other) :
    window(other.window),
    model_manager(other.model_manager)
{
    DENTER("Objects::ObjectManager::ObjectManager(copy)");

    // Copy the objects over
    this->objects.reserve(other.objects.size());
    for (uint32_t i = 0; i < other.objects.size(); i++) {
        this->objects.push_back(new ECS::Object(*other.objects[i]));
    }

    // Done
    DLEAVE;
}

/* Move constructor for the ObjectManager class. */
ObjectManager::ObjectManager(ObjectManager&& other) :
    window(other.window),
    model_manager(other.model_manager),
    objects(other.objects)
{
    other.objects.clear();
}

/* Destructor for the ObjectManager class. */
ObjectManager::~ObjectManager() {
    DENTER("Objects::ObjectManager::~ObjectManager");

    for (uint32_t i = 0; i < this->objects.size(); i++) {
        delete this->objects[i];
    }

    DLEAVE;
}



/* Spawns in the player (i.e., camera) at the given location and looking at the given point. */
void ObjectManager::spawn_player(const glm::vec3& pos, const glm::vec3& lookat) {
    DENTER("Objects::ObjectManager::spawn_player");

    

    DRETURN;
}



/* Creates a new object using the given model at the given location. */
void ObjectManager::add_object(const glm::vec3& pos, const std::string& model_path, Models::ModelFormat model_format) {
    DENTER("Objects::ObjectManager::add_object");

    // Load the model
    this->model_manager.load_model(model_path, model_format);

    // Store the object
    this->objects.push_back(new ECS::Object({ pos, model_path }));

    // Done
    DRETURN;
}



/* Swap operator for the ObjectManager class. */
void Objects::swap(ObjectManager& om1, ObjectManager& om2) {
    DENTER("Objects::swap(ObjectManager)");

    #ifndef NDEBUG
    // We only accept same window & model-mananger object managers
    if (&om1.window != &om2.window) {
        DLOG(fatal, "Cannot swap two object managers with different windows.");
    }
    if (&om1.model_manager != &om2.model_manager) {
        DLOG(fatal, "Cannot swap two object managers with different model managers.");
    }
    #endif

    using std::swap;

    swap(om1.objects, om2.objects);

    DRETURN;
}
