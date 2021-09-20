/* STREAM OPERATORS.hpp
 *   by Lut99
 *
 * Created:
 *   20/09/2021, 13:59:19
 * Last edited:
 *   20/09/2021, 13:59:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains extra stream operators for library functions.
**/

#ifndef TOOLS_STREAM_OPERATORS_HPP
#define TOOLS_STREAM_OPERATORS_HPP

#include <chrono>
#include <ostream>
#include "glm/glm.hpp"
#include "date.h"

namespace Tools {
    /* The stream operator for a glm::vec3. */
    inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) { return os << '[' << v.x << ' ' << v.y << ' ' << v.z << ']'; }
    /* The stream operator for a glm::vec4. */
    inline std::ostream& operator<<(std::ostream& os, const glm::vec4& v) { return os << '[' << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << ']'; }
    /* The stream operator for time points. */
    inline std::ostream& operator<<(std::ostream& os, const std::chrono::system_clock::time_point& t) {
        return date::operator<<(os, t);
    }
}

#endif
