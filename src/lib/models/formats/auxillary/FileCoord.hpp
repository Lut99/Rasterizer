/* FILE COORD.hpp
 *   by Lut99
 *
 * Created:
 *   04/07/2021, 17:44:19
 * Last edited:
 *   04/07/2021, 17:44:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains a simple helper struct for (line, col) pair coordinates.
**/

#ifndef MODELS_FILE_COORD_HPP
#define MODELS_FILE_COORD_HPP

namespace Rasterizer::Models {
    /* A simple helper for (line, col) coordinates in some file. */
    struct FileCoord {
        /* The line number. */
        size_t line;
        /* The column number. */
        size_t col;
    };

}

#endif
