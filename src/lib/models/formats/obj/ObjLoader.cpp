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

#include <sstream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <cmath>

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
/* The gamma correction to apply. */
static const constexpr float gamma = 2.2f;
/* The default vertex colour. */
static const glm::vec3 default_colour(173.0f / 255.0f, 26.0f / 255.0f, 21.0f / 255.0f);





/***** HELPER FUNCTIONS *****/
/* Converts the given colour to a gamma-corrected colour. */
static inline glm::vec3 correct_for_gamma(const glm::vec3& linear_colour) {
    return glm::vec3(
        pow(linear_colour.r, gamma),
        pow(linear_colour.g, gamma),
        pow(linear_colour.b, gamma)
    );
}

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
            correct_for_gamma(default_colour),
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
/* Loads the file at the given path as a .obj file, and populates the given model data from it. Uses the tinyobjloader library for most of the work. */
void Models::load_obj_model(Rendering::MemoryManager& memory_manager, Materials::MaterialPool& material_pool, ECS::Model& model, const std::string& path) {
    // Load the model
    tinyobj::attrib_t data;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    if (tinyobj::LoadObj(&data, &shapes, &materials, &error, path.c_str(), (Tools::merge_paths(get_executable_path(), "data/materials/")).c_str()) != true) {
        // Show the error
        logger.errorc(channel, "Could not parse input file '", path, "' as .obj file:");
        std::stringstream sstr;
        for (size_t i = 0; i < error.size(); i++) {
            if (error[i] == '\n') {
                logger.errorc(channel, sstr.str());
                sstr.str("");
            } else {
                sstr << error[i];
            }
        }
        logger.fatalc(channel, "Cannot continue.");
    }
    if (!error.empty()) {
        std::stringstream sstr;
        for (size_t i = 0; i < error.size(); i++) {
            if (error[i] == '\n') {
                logger.warningc(channel, sstr.str());
                sstr.str("");
            } else {
                sstr << error[i];
            }
        }
    }



    // From the parsed materials, allocate space in the Pool for them and map them in a local map
    std::unordered_map<int, const Materials::Material*> material_map = { { -1, material_pool.default() } };
    for (size_t i = 0; i < materials.size(); i++) {
        // Determine if the material is a texture or not
        const Materials::Material* new_material;
        if (materials[i].diffuse_texname.empty()) {
            new_material = material_pool.allocate_simple_coloured(materials[i].name, correct_for_gamma(glm::vec3(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2])));
        } else {
            new_material = material_pool.allocate_simple_textured(materials[i].name, materials[i].diffuse_texname, VK_FILTER_LINEAR, VK_TRUE);
        }

        // Store the material in our own map
        material_map.insert({ static_cast<int>(i), new_material });        
    }



    // Go through the meshes to collect the data
    model.meshes.reserve_opt(16);
    Tools::Array<Rendering::Vertex> vertices(16);
    std::unordered_map<glm::uvec3, uint32_t> vertex_map;
    std::unordered_map<int, Tools::Array<Rendering::index_t>> cpu_index_lists;
    for (size_t i = 0; i < shapes.size(); i++) {
        // Populate the lists of indices CPU-side, sorting them by material
        uint32_t largest_list_size = 0;
        for (size_t j = 0; j < shapes[i].mesh.indices.size(); j++) {
            // Find a unique vector/normal/texel pair from the coordinates and map it to our indices
            const tinyobj::index_t& obj_indices = shapes[i].mesh.indices[j];
            Rendering::index_t index = store_vertex(vertices, vertex_map, data, obj_indices.vertex_index, obj_indices.normal_index, obj_indices.texcoord_index);

            // Find the correct list of indices to add it to (depending on the mesh)
            int material_id = shapes[i].mesh.material_ids[j / 3];
            std::unordered_map<int, Tools::Array<Rendering::index_t>>::iterator iter = cpu_index_lists.find(material_id);
            if (iter == cpu_index_lists.end()) {
                iter = cpu_index_lists.insert({ material_id, Tools::Array<Rendering::index_t>(16) }).first;
            }

            // Insert our index into the mesh's list
            while ((*iter).second.size() >= (*iter).second.capacity()) { (*iter).second.reserve(2 * (*iter).second.capacity()); }
            (*iter).second.push_back(index);

            // Also keep track of the largest list for the stage buffer
            if ((*iter).second.size() > largest_list_size) { largest_list_size = (*iter).second.size(); }
        }

        // Next, allocate a staging buffer with enough space for all buffers (but not at the same time)
        Rendering::Buffer* stage = memory_manager.stage_pool.allocate(largest_list_size * sizeof(Rendering::index_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        void* stage_map; stage->map(&stage_map);

        // With the list of material-sorted indices, create a new Mesh each
        for (const auto& [ material_id, cpu_indices ] : cpu_index_lists) {
            // Fetch the appropriate material
            const Materials::Material* material = material_map.at(material_id);

            // Create the mesh
            ECS::Mesh mesh{};
            mesh.name = cpu_index_lists.size() == 1 ? shapes[i].name : shapes[i].name + ' ' + material->name();
            mesh.material = material;

            // Create the buffer for this mesh
            mesh.indices = memory_manager.draw_pool.allocate(cpu_indices.size() * sizeof(Rendering::index_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            mesh.n_indices = cpu_indices.size();

            // Populate the buffer with the staging buffer
            memcpy(stage_map, (void*) cpu_indices.rdata(), cpu_indices.size() * sizeof(Rendering::index_t));
            stage->flush();
            stage->copyto(mesh.indices, cpu_indices.size() * sizeof(Rendering::index_t), 0, 0, memory_manager.copy_cmd);

            // Add the Mesh to the model before we're done
            while (model.meshes.size() >= model.meshes.capacity()) { model.meshes.reserve(2 * model.meshes.capacity()); }
            model.meshes.push_back(std::move(mesh));
        }

        // We can free the stage buffer again
        stage->unmap();
        memory_manager.stage_pool.free(stage);

        // Reset the necessary buffers to re-use them next iteration
        cpu_index_lists.clear();
    }



    // With a global collection of vertices complete, it's time to send them to the GPU
    model.n_vertices = vertices.size();
    model.vertices = memory_manager.draw_pool.allocate(model.n_vertices * sizeof(Rendering::Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    // Prepare a stage buffer to use
    Rendering::Buffer* stage = memory_manager.stage_pool.allocate(model.n_vertices * sizeof(Rendering::Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    // Use the stage buffer to populate the main buffer
    model.vertices->set((void*) vertices.rdata(), model.n_vertices * sizeof(Rendering::Vertex), stage, memory_manager.copy_cmd);
    // We can free the buffer again
    memory_manager.stage_pool.free(stage);

    // And with that, we've loaded the model
    model.name = path;
}
