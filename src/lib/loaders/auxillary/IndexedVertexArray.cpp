/* INDEXED VERTEX ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 16:30:08
 * Last edited:
 *   29/06/2021, 16:30:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the IndexedVertexArray class, which is a list of vertices -
 *   but then indexed to save memory. Is created to be used with Vulkan.
**/

#include <unordered_map>
#include "tools/CppDebugger.hpp"

#include "IndexedVertexArray.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Loaders;
using namespace CppDebugger::SeverityValues;


/***** INDEXEDVERTEXARRAY CLASS *****/
/* Default constructor for the IndexedVertexArray class. */
IndexedVertexArray::IndexedVertexArray() :
    vertex_list(IndexedVertexArray::initial_size),
    index_list(IndexedVertexArray::initial_size)
{}

/* Constructor for the IndexedVertexArray class, which already takes a list of vertices and a list of indices. */
IndexedVertexArray::IndexedVertexArray(const Tools::Array<Loaders::Vertex>& vertices, const Tools::Array<index_t>& indices) :
    vertex_list(vertices),
    index_list(indices)
{}



/* Appends the given IndexedVertexArray to this one. Does not check the complete list for duplicates. */
IndexedVertexArray& IndexedVertexArray::operator+=(const IndexedVertexArray& other) {
    DENTER("Loaders::IndexedVertexArray::operator+=");

    // Add the other's list to ours
    this->vertex_list += other.vertex_list;
    this->index_list += other.index_list;

    // When done, return
    DRETURN *this;
}



/* Scans the list of vertices, and removes all duplicate ones. The indices for the duplicate ones will be replaced with a single one, conserving space. */
void IndexedVertexArray::prune() {
    DENTER("Loaders::IndexedVertexArray::prune");

    // First, create a map that we'll use to check how unique each vertex is
    std::unordered_map<Vertex, index_t> vertices;

    // Next, loop thru the vertices and collect a unique list
    for (Tools::array_size_t i = 0; i < this->vertex_list.size(); i++) {
        // Check if the vertex already exists in the map
        std::unordered_map<Vertex, index_t>::iterator iter = vertices.find(this->vertex_list[i]);
        if (iter != vertices.end()) { continue; }

        // If it doesn't exist, add it
        vertices.insert(std::make_pair(this->vertex_list[i], i));
    }

    // With the unique list of vertices, replace the list of 

    DRETURN;
}

/* Reserves space for N new vertices in the IndexedVertexArray. More specifically, resizes both of the internal lists to the given number, throwing away overflowing ones - unless the number of the vertex list is different. New elements will be left unitialized, just like the sizes of the internal array. */
void reserve(Tools::array_size_t new_capacity, Tools::array_size_t vertex_capacity = std::numeric_limits<Tools::array_size_t>::max());
/* Reserves space for N new vertices in the IndexedVertexArray. More specifically, resizes both of the internal lists to the given number, throwing away overflowing ones - unless the number of the vertex list is different. New elements will be default constructed, and the size of the arrays will adapt too. */
void resize(Tools::array_size_t new_capacity, Tools::array_size_t vertex_capacity = std::numeric_limits<Tools::array_size_t>::max());
