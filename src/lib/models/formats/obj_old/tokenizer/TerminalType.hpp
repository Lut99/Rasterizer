/* TERMINAL TYPE.hpp
 *   by Lut99
 *
 * Created:
 *   21/09/2021, 16:55:54
 * Last edited:
 *   21/09/2021, 16:55:54
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains an enum describing all the possible terminal types in the
 *   Tokenizer.
**/

#ifndef MODELS_OBJ_TERMINAL_TYPE_HPP
#define MODELS_OBJ_TERMINAL_TYPE_HPP

#include <string>

namespace Makma3D::Models::Obj {
    /* Lists all Terminal types. */
    enum class TerminalType {
        /* The undefined symbol. */
        undefined = 0,
        /* The EOF-symbol. */
        eof = 1,

        /* The f keyword, indicating we're talking about a Face. */
        face = 2,
        /* The v keyword, indicating we're talking about a Vertex. */
        vertex = 3,
        /* The vn keyword, indicating we're talking about a vertex normal. */
        normal = 4,
        /* The vt keyword, indicating we're talking about a vertex texture coordinate. */
        texture = 5,
        /* The group keyword, which groups some vertices and faces in, well, a group. */
        group = 6,
        /* The object keyword, which indicates the start of an object. */
        object = 7,
        /* The mtllib keyword, defining a new material. */
        mtllib = 8,
        /* The usemtl keyword, referencing a material defined in another file. */
        usemtl = 9,
        /* The smooth shading keyword, which determines whether it's on or off. */
        smooth = 10,

        /* Signed integer value, only occuring for negative values. */
        sint = 11,
        /* An unsigned integer value. */
        uint = 12,
        /* A floating-point value. */
        decimal = 13,
        /* A combined index of vertex/texture coordinate. */
        v_vt = 14,
        /* A combined index of vertex/texture/normal coordinate. */
        v_vt_vn = 15,
        /* A combined index of vertex/normal coordinate. */
        v_vn = 16,
        /* The name of some external file. */
        filename = 17,
        /* Some name used internally in the obj file structure. */
        name = 18
    };

    /* Converts the given TerminalType to a string. */
    static std::string terminal_type_names[] = {
        "undefined",
        "end-of-file",

        "face",
        "vertex",
        "normal",
        "texture",
        "group",
        "object",
        "mtllib",
        "usemtl",
        "smooth",

        "sint",
        "uint",
        "decimal",
        "v/vt",
        "v/vt/vn",
        "v//vn",
        "filename",
        "name"
    };

}

#endif