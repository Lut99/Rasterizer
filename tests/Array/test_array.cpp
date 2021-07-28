/* TEST ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:06:03 PM
 * Last edited:
 *   1/13/2021, 4:57:16 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that tests the Array class.
**/

#include <ctime>
#include <cstddef>

#define FULLCOMPILE
#include "Tools/Array.hpp"

using namespace std;
using namespace Tools;

// Function that tests initialization & destruction of the Array class
extern bool test_initialization();
// Function that tests consistency for the Array class
extern bool test_consistancy();
// Function that tests the correctness of reserve() and resize() of the Array class
extern bool test_resize();
// Function that tests the correctness of push_back(), pop_back() and erase() of the Array class
extern bool test_push_erase();
// Function that tests the correctness of T's without all constructors
extern bool test_constructors();

int main() {
    // Seed the random seed
    srand((unsigned int) time(0));

    if (!test_initialization()) {
        return EXIT_FAILURE;
    }
    if (!test_consistancy()) {
        return EXIT_FAILURE;
    }
    if (!test_resize()) {
        return EXIT_FAILURE;
    }
    if (!test_push_erase()) {
        return EXIT_FAILURE;
    }
    if (!test_constructors()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
