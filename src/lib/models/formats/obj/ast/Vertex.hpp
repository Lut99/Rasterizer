/* VERTEX.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 17:25:42
 * Last edited:
 *   21/09/2021, 17:25:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Vertex nonterminal, which represents a single vertex
 *   entry.
**/

#ifndef MODELS_OBJ_AST_VERTEX_HPP
#define MODELS_OBJ_AST_VERTEX_HPP

#include "auxillary/parsers/Nonterminal.hpp"
#include "NonterminalType.hpp"

namespace Makma3D::Models::Obj::AST {
    /* The Vertex AST nonterminal, which assembles the vertex information we need. */
    class Vertex: public Auxillary::Nonterminal<Makma3D::Models::Obj::NonterminalType> {
    public:
        /* The first Vertex value. */
        float x;
        /* The second Vertex value. */
        float y;
        /* The third Vertex value. */
        float z;
        /* The fourth Vertex value, which defaults to 1.0 if it isn't given. */
        float w;

    public:
        /* Constructor for the Vertex class, which takes a DebugInfo so we know where it came from and the three values its supposed to carry. The fourth will be initialized to 1.0. */
        Vertex(float x, float y, float z, const Auxillary::DebugInfo& debug_info): Nonterminal(NonterminalType::vertex, debug_info), x(x), y(y), z(z), w(1.0f) {}
        /* Constructor for the Vertex class, which takes a DebugInfo so we know where it came from and the four values its supposed to carry. */
        Vertex(float x, float y, float z, float w, const Auxillary::DebugInfo& debug_info): Nonterminal(NonterminalType::vertex, debug_info), x(x), y(y), z(z), w(w) {}

        /* Allows the Vertex to be copied polymorphically. */
        virtual Vertex* copy() const { return new Vertex(*this); }

    };
}

#endif
