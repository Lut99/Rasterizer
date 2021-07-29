/* TYPENAMES.hpp
 *   by Lut99
 *
 * Created:
 *   18/07/2021, 12:56:59
 * Last edited:
 *   18/07/2021, 12:56:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Provides some templated functions to easily return a sort of
 *   human-readable name of types. Others can be provided by defining
 *   template specializations yourself.
**/

#ifndef TOOLS_TYPENAMES_HPP
#define TOOLS_TYPENAMES_HPP

#include <cstdint>
#include <string>
#include <typeinfo>

namespace Tools {
    /* Returns the typeid(T).name() of the given type. Functions as basecase when no type_name() specialization for the type is available. */
    template <class T>
    inline constexpr const char* type_name() {
        return typeid(T).name();
    }
    /* Wrapper around type_name that returns a string. */
    template <class T>
    inline std::string type_sname() {
        return std::string(type_name<T>());
    }
    
    /* Returns the name of the char* datatype. */
    template <> inline constexpr const char* type_name<char*>() { return "char*"; }
    /* Returns the name of the const char* datatype. */
    template <> inline constexpr const char* type_name<const char*>() { return "const char*"; }
    /* Returns the name of the std::string datatype. */
    template <> inline constexpr const char* type_name<std::string>() { return "std::string"; }

    /* Returns the name of the bool datatype. */
    template <> inline constexpr const char* type_name<bool>() { return "bool"; }
    
    /* Returns the name of the int8_t datatype. */
    template <> inline constexpr const char* type_name<int8_t>() { return "int8_t"; }
    /* Returns the name of the uint8_t datatype. */
    template <> inline constexpr const char* type_name<uint8_t>() { return "uint8_t"; }
    /* Returns the name of the int16_t datatype. */
    template <> inline constexpr const char* type_name<int16_t>() { return "int16_t"; }
    /* Returns the name of the uint16_t datatype. */
    template <> inline constexpr const char* type_name<uint16_t>() { return "uint16_t"; }
    /* Returns the name of the int32_t datatype. */
    template <> inline constexpr const char* type_name<int32_t>() { return "int32_t"; }
    /* Returns the name of the uint32_t datatype. */
    template <> inline constexpr const char* type_name<uint32_t>() { return "uint32_t"; }
    /* Returns the name of the int64_t datatype. */
    template <> inline constexpr const char* type_name<int64_t>() { return "int64_t"; }
    /* Returns the name of the uint64_t datatype. */
    template <> inline constexpr const char* type_name<uint64_t>() { return "uint64_t"; }
}

#endif
