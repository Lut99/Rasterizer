/* OBJ LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:19
 * Last edited:
 *   8/1/2021, 3:48:11 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#ifndef MODELS_OBJ_LOADER_HPP
#define MODELS_OBJ_LOADER_HPP

#include "rendering/auxillary/Vertex.hpp"
#include "rendering/auxillary/Index.hpp"
#include "rendering/memory/MemoryManager.hpp"
#include "ecs/components/Meshes.hpp"
#include "tools/Array.hpp"

namespace Makma3D::Models {
    /* Loads the file at the given path as a .obj file, and populates the given list of meshes from it. The n_vertices and n_indices are debug counters, to keep track of the total number of vertices and indices loaded. */
    void load_obj_model(Rendering::MemoryManager& memory_manager, ECS::Meshes& meshes, const std::string& path);

};

#endif
