/* INDEXED VERTEX ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   29/06/2021, 16:30:05
 * Last edited:
 *   29/06/2021, 16:30:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the IndexedVertexArray class, which is a list of vertices -
 *   but then indexed to save memory. Is created to be used with Vulkan.
**/

#ifndef LOADERS_INDEXED_VERTEX_ARRAY_HPP
#define LOADERS_INDEXED_VERTEX_ARRAY_HPP

#include "tools/Array.hpp"
#include "Vertex.hpp"

namespace Rasterizer::Loaders {
    /* The type that we use to define the indices. */
    using index_t = uint32_t;

    /* The IndexedVertexArray class, which abstracts a list of indices and a list of vertices to a single list. */
    class IndexedVertexArray {
    public:
        static const constexpr Tools::array_size_t initial_size = 10;

    private:
        /* The list of vertices. */
        Tools::Array<Loaders::Vertex> vertex_list;
        /* The indices to the vertices. */
        Tools::Array<index_t> index_list;

    public:
        /* Default constructor for the IndexedVertexArray class. */
        IndexedVertexArray();
        /* Constructor for the IndexedVertexArray class, which already takes a list of vertices and a list of indices. */
        IndexedVertexArray(const Tools::Array<Loaders::Vertex>& vertices, const Tools::Array<index_t>& indices);

        /* Appends the given IndexedVertexArray to this one, and returns it as a new one. Does not check the complete list for duplicates. */
        inline IndexedVertexArray operator+(const IndexedVertexArray& other) const { return IndexedVertexArray(this->vertex_list + other.vertex_list, this->index_list + other.index_list); }
        /* Appends the given IndexedVertexArray to this one. Does not check the complete list for duplicates. */
        IndexedVertexArray& operator+=(const IndexedVertexArray& other);

        /* Returns the Vertex at the given position. Note that the index is the position in the list, not the index as seen in the index list. */
        inline Vertex& operator[](Tools::array_size_t index) { return this->vertex_list[this->index_list[index]]; }
        /* Returns the Vertex at the given position. Note that the index is the position in the list, not the index as seen in the index list. */
        inline const Vertex& operator[](Tools::array_size_t index) const { return this->vertex_list[this->index_list[index]]; }

        /* Pushes a new vertex to the IndexedVertexArray and returns its index. */
        inline index_t push_vertex(const Loaders::Vertex& vertex) { this->vertex_list.push_back(vertex); return this->vertex_list.size() - 1; }
        /* Pushes a new index entry to the IndexedVertexArray. Will complain if it does not yet exist. */
        inline void push_index(index_t index) { this->index_list.push_back(index); }

        /* Scans the list of vertices, and removes all duplicate ones. The indices for the duplicate ones will be replaced with a single one, conserving space. */
        void prune();

        /* Reserves space for N new vertices in the IndexedVertexArray. More specifically, resizes both of the internal lists to the given number, throwing away overflowing ones - unless the number of the vertex list is different. New elements will be left unitialized, just like the sizes of the internal array. */
        void reserve(Tools::array_size_t new_capacity, Tools::array_size_t vertex_capacity = std::numeric_limits<Tools::array_size_t>::max());
        /* Reserves space for N new vertices in the IndexedVertexArray. More specifically, resizes both of the internal lists to the given number, throwing away overflowing ones - unless the number of the vertex list is different. New elements will be default constructed, and the size of the arrays will adapt too. */
        void resize(Tools::array_size_t new_capacity, Tools::array_size_t vertex_capacity = std::numeric_limits<Tools::array_size_t>::max());

        /* Returns the number of indices in the IndexedVertexArray. */
        inline Tools::array_size_t size() const { return this->index_list.size(); }
        /* Returns the capacity for the indices in the IndexedVertexArray. */
        inline Tools::array_size_t capacity() const { return this->index_list.capacity(); }
        /* Provides access to the array of vertices. */
        inline const Tools::Array<Loaders::Vertex>& vertices() const { return this->vertex_list; }
        /* Provides access to the array of indices. */
        inline const Tools::Array<index_t>& indices() const { return this->index_list; }

    };
}

#endif
