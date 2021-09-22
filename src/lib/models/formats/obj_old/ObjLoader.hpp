/* OBJ LOADER.hpp
 *   by Lut99
 *
 * Created:
 *   03/07/2021, 17:37:19
 * Last edited:
 *   9/19/2021, 5:56:32 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the code that loads models from .obj files.
**/

#ifndef MODELS_OBJ_LOADER_HPP
#define MODELS_OBJ_LOADER_HPP

#include "ecs/components/Model.hpp"
#include "materials/MaterialSystem.hpp"
#include "rendering/auxillary/Vertex.hpp"
#include "rendering/auxillary/Index.hpp"
#include "rendering/memory_manager/MemoryManager.hpp"
#include "tools/Array.hpp"

namespace Makma3D::Models {
    /* Loads the file at the given path as a .obj file, and populates the given model data from it. The n_vertices and n_indices are debug counters, to keep track of the total number of vertices and indices loaded.
     *
     * Parsing .obj files has been done with the help of:
     *  - https://en.wikipedia.org/wiki/Wavefront_.obj_file
     *  - http://paulbourke.net/dataformats/obj/
     */
    void load_obj_model(Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system, ECS::Model& model, const std::string& path);

};

#endif
