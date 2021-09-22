/* OBJ LOADER.cpp
 *   by Lut99
 *
 * Created:
 *   22/09/2021, 14:51:31
 * Last edited:
 *   22/09/2021, 14:51:31
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a function that can load .obj files and associated .mtl
 *   files. Uses the assimp library to actually parse the files, and then
 *   converts its format to the one we wanna see.
**/

#include <fstream>
#include <cstring>
#include <cerrno>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "rendering/auxillary/Index.hpp"
#include "rendering/auxillary/Vertex.hpp"

#include "tiny_obj_loader.h"
#include "ObjLoader.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Models;


/***** CONSTANTS *****/
/* Channel for the ObjLoader. */
static constexpr const char* channel = "ObjLoader";





/***** HELPER FUNCTIONS *****/
/* Given the actual data and a vertex/normal/texel index triplet, insert the vertex in the Model data and return its index. */
static Rendering::index_t store_vertex(Tools::Array<Rendering::Vertex>& vertices, std::unordered_map<glm::uvec3, uint32_t>& vertex_map, const tinyobj::attrib_t& data, int vertex_index, int normal_index, int texel_index) {
    // Wrap the indices in a vector
    glm::uvec3 hash_index(static_cast<uint32_t>(vertex_index), static_cast<uint32_t>(normal_index), static_cast<uint32_t>(texel_index));

    // See if we already have this index pair somewhere
    std::unordered_map<glm::uvec3, uint32_t>::iterator iter = vertex_map.find(hash_index);
    if (iter != vertex_map.end()) {
        // Return the index of that pair
        return static_cast<Rendering::index_t>((*iter).second);
    } else {
        // Otherwise, generate the index & the Vertex
        uint32_t index = vertices.size();
        Rendering::Vertex vertex(
            glm::vec3(
                data.vertices[3 * vertex_index    ],
                data.vertices[3 * vertex_index + 1],
                data.vertices[3 * vertex_index + 2]
            ),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec2(
                data.texcoords[2 * texel_index    ],
                data.texcoords[2 * texel_index + 1]
            )
        );

        // Update the list of vertices (resizing more optimally) and the map
        while (vertices.size() >= vertices.capacity()) { vertices.reserve(2 * vertices.capacity()); }
        vertices.push_back(std::move(vertex));
        vertex_map.insert({ hash_index, index });

        // Return the new index
        return static_cast<Rendering::index_t>(index);
    }

}





/***** LIBRARY FUNCTIONS *****/
/* Loads the file at the given path as a .obj file, and populates the given model data from it. Uses the assimp library for most of the work. */
void Models::load_obj_model(Rendering::MemoryManager& memory_manager, Materials::MaterialSystem& material_system, ECS::Model& model, const std::string& path) {
    // Load the model
    tinyobj::attrib_t data;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    if (tinyobj::LoadObj(&data, &shapes, &materials, &error, path.c_str(), (get_executable_path() + "/data/materials").c_str()) != true) {
        // Show the error
        logger.fatalc(channel, "Could not parse input file '", path, "' as .obj file: ", error);
    }

    // Create the materials we parsed, if any, and map them to the local material indices
    std::unordered_map<int, Materials::material_t> material_collection;
    for (size_t i = 0; i < materials.size(); i++) {
        /* TBD */
    }

    // Go through the meshes to collect the data
    Tools::Array<Rendering::Vertex> vertices(16);
    std::unordered_map<glm::uvec3, uint32_t> vertex_map;
    Tools::Array<Rendering::index_t> mesh_indices;
    for (size_t i = 0; i < shapes.size(); i++) {
        // Each shape is what we call a mesh, so initialize it with the correct name and the material index for this mesh
        ECS::Mesh mesh{};
        mesh.name = shapes[i].name;
        mesh.material = shapes[i].mesh.material_ids.size() > 0 ? material_collection.at(shapes[i].mesh.material_ids[0]) : Materials::DefaultMaterial;

        // Populate the list of indices
        for (size_t j = 0; j < shapes[i].mesh.indices.size(); j++) {
            // Find a unique vector/normal/texel pair from the coordinates and map it to our indices
            const tinyobj::index_t& obj_indices = shapes[i].mesh.indices[j];
            Rendering::index_t index = store_vertex(vertices, vertex_map, data, obj_indices.vertex_index, obj_indices.normal_index, obj_indices.texcoord_index);

            // Insert the engine's index into the mesh's list
            while (mesh_indices.size() >= mesh_indices.capacity()) { mesh_indices.reserve(2 * mesh_indices.capacity()); }
            mesh_indices.push_back(index);
        }

        // We can already send the list of indices to the GPU.
        mesh.n_indices = mesh_indices.size();
        mesh.indices = memory_manager.draw_pool.allocate(mesh.n_indices * sizeof(Rendering::index_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        // Prepare a stage buffer to use
        Rendering::Buffer* stage = memory_manager.stage_pool.allocate(mesh.n_indices * sizeof(Rendering::index_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        // Use the stage buffer to populate the main buffer
        mesh.indices->set((void*) mesh_indices.rdata(), mesh.n_indices * sizeof(Rendering::index_t), stage, memory_manager.copy_cmd);
        // We can free the buffer again
        memory_manager.stage_pool.free(stage);

        // The mesh is now complete, so add it to the model
        model.meshes.push_back(std::move(mesh));
    }

    // With a global collection of vertices complete, it's time to send them to the GPU
    model.n_vertices = vertices.size();
    model.vertices = memory_manager.draw_pool.allocate(model.n_vertices * sizeof(Rendering::Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    // Prepare a stage buffer to use
    Rendering::Buffer* stage = memory_manager.stage_pool.allocate(model.n_vertices * sizeof(Rendering::Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    // Use the stage buffer to populate the main buffer
    mesh.indices->set((void*) vertices.rdata(), model.n_vertices * sizeof(Rendering::Vertex), stage, memory_manager.copy_cmd);
    // We can free the buffer again
    memory_manager.stage_pool.free(stage);

    // And with that, we've loaded the model
}
