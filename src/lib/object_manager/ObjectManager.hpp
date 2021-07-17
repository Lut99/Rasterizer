/* OBJECT MANAGER.hpp
 *   by Lut99
 *
 * Created:
 *   17/07/2021, 14:23:43
 * Last edited:
 *   17/07/2021, 14:23:43
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ObjectManager class, which combines models and textures
 *   into objects, which can be updated, applied physics to, etc.
**/

#ifndef OBJECTS_OBJECT_MANAGER_HPP
#define OBJECTS_OBJECT_MANAGER_HPP

#include "glm/glm.hpp"
#include "window/Window.hpp"
#include "model_manager/ModelManager.hpp"
#include "tools/Array.hpp"

#include "objects/Object.hpp"

namespace Rasterizer::Objects {
    /* The ObjectManager class, which combines textures and models into moveable and manageable objects. */
    class ObjectManager {
    public:
        /* The Window from which we read user input. */
        Window& window;
        /* The ModelManager used to load models. */
        Models::ModelManager& model_manager;
    
    private:
        /* List of the internally loaded objects. */
        Tools::Array<ECS::Object*> objects;

    public:
        /* Constructor for the ObjectManager class, which takes a window to take user input from and a ModelManager to load models with. */
        ObjectManager(Window& window, Models::ModelManager& model_manager);
        /* Copy constructor for the ObjectManager class. */
        ObjectManager(const ObjectManager& other);
        /* Move constructor for the ObjectManager class. */
        ObjectManager(ObjectManager&& other);
        /* Destructor for the ObjectManager class. */
        ~ObjectManager();

        /* Spawns in the player (i.e., camera) at the given location and looking at the given point. */
        void spawn_player(const glm::vec3& pos, const glm::vec3& lookat);

        /* Creates a new object using the given model at the given location. */
        void add_object(const glm::vec3& pos, const std::string& model_path, Models::ModelFormat model_format = Models::ModelFormat::obj);

        /* Copy assignment operator for the ObjectManager class. */
        inline ObjectManager& operator=(const ObjectManager& other) { return *this = ObjectManager(other); }
        /* Move assignment operator for the ObjectManager class. */
        inline ObjectManager& operator=(ObjectManager&& other) { if (this != &other) { swap(*this, other); } return *this; }
        /* Swap operator for the ObjectManager class. */
        friend void swap(ObjectManager& om1, ObjectManager& om2);

    };

    /* Swap operator for the ObjectManager class. */
    void swap(ObjectManager& om1, ObjectManager& om2);

};

#endif
