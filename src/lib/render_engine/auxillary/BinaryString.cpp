/* BINARY STRING.cpp
 *   by Lut99
 *
 * Created:
 *   20/06/2021, 14:13:20
 * Last edited:
 *   28/06/2021, 21:50:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the BinaryString class, which is a small helper class to pass
 *   data or objects as a binary string.
**/

#include "BinaryString.hpp"

using namespace std;
using namespace Rasterizer;
using namespace Rasterizer::Rendering;


/***** BINARYSTRING CLASS *****/
/* Default constructor for the BinaryString, which sets it to 0. */
BinaryString::BinaryString() :
    size(0),
    data(nullptr)
{}

/* Constructor for the BinaryString, which takes the size (in bytes) and a pointer to the object. */
BinaryString::BinaryString(binary_size_t size, void* data) :
    size(size),
    data(data)
{}



/* Factory method for a new BinaryString, which automatically deduces the size based on the given type. */
template <class T>
BinaryString BinaryString::to_string(T* obj) {
    // Simply use sizeof to deduce the size
    return BinaryString(sizeof(T), (void*) obj);
}
