/* OBJ LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   22/09/2021, 14:50:12
 * Last edited:
 *   22/09/2021, 14:50:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a function that can load .obj files and associated .mtl
 *   files. Uses the assimp library to actually parse the files, and then
 *   converts its format to the one we wanna see.
**/

#ifndef MODELS_OBJLOADER_HPP
#define MODELS_OBJLOADER_HPP

#include <string>

#include "ecs/components/Model.hpp"
#include "materials/MaterialSystem.hpp"
#include "rendering/memory_manager/MemoryManager.hpp"

namespace Makma3D::Models {
    /* Loads the file at the given path as a .obj file, and populates the given model data from it. Uses the assimp library for most of the work. */
    void load_obj_model(Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system, ECS::Model& model, const std::string& path);

}

#endif
