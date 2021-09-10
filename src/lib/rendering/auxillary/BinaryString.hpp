/* BINARY STRING.hpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 14:13:23
 * Last edited:
 *   28/06/2021, 21:49:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the BinaryString class, which is a small helper class to pass
 *   data or objects as a binary string.
**/

#ifndef RENDERING_BINARY_STRING_HPP
#define RENDERING_BINARY_STRING_HPP

#include <cstdint>

namespace Makma3D::Rendering {
    /* The datatype used to describe the size of the binary string. */
    using binary_size_t = uint32_t;

    /* The BinaryString class, which is used to describe objects as a binary string. */
    class BinaryString {
    public:
        /* The size of the internal string. */
        binary_size_t size;
        /* The string itself, as a void pointer. */
        void* data;


        /* Default constructor for the BinaryString, which sets it to 0. */
        BinaryString();
        /* Constructor for the BinaryString, which takes the size (in bytes) and a pointer to the object. */
        BinaryString(binary_size_t size, void* data);

        /* Factory method for a new BinaryString, which automatically deduces the size based on the given type. */
        template <class T>
        static BinaryString to_string(T* obj);

    };
}

#endif
