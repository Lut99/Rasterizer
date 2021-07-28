/* RESIZE.cpp
 *   by Lut99
 *
 * Created:
 *   12/23/2020, 4:46:17 PM
 * Last edited:
 *   12/23/2020, 5:33:34 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that checks the correctness of Array::reserve() and
 *   Array::resize().
**/

#include <iostream>
#include <cstddef>
#include <cstring>

#define FULLCOMPILE
#include "tools/Array.hpp"
#include "common.hpp"

using namespace std;
using namespace Tools;


/***** HELPER CLASS *****/
/* Has a default constructor. */
template <class T>
class DefaultHeapClass: public HeapClass<T> {
public:
    DefaultHeapClass(): HeapClass<T>(42) {}
};





/***** TESTS *****/
/* Function that tests if the Array's reserve() work for an easy case (empty to larger). */
bool test_reserve_empty() {
    TESTCASE("reserve from empty");

    // Declare a test array & reserve space in it
    Array<hard_t> test;
    test.reserve(3);
    if (test.capacity() != 3) {
        ERROR("Reserving in empty array failed; incorrect capacity: expected " + std::to_string(3) + ", got " + std::to_string(test.capacity()));
        ENDCASE(false);
    }

    // Populate it with values
    new(test.wdata()    ) hard_t(1);
    new(test.wdata() + 1) hard_t(2);
    new(test.wdata(3) + 2) hard_t(3);

    for (array_size_t i = 0; i < test.capacity(); i++) {
        if (*test[i] != i + 1) {
            ERROR("Reserving in empty array failed; element at index " + std::to_string(i) + " is expected: " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for a bit harder case (non-empty to larger). */
bool test_reserve_nonempty() {
    TESTCASE("reserve from non-empty");

    // Declare a test array & reserve space in it
   Array<hard_t> test = { hard_t(1), hard_t(2), hard_t(3) };
    test.reserve(6);
    if (test.capacity() != 6) {
        ERROR("Reserving in non-empty array failed; incorrect capacity: expected " + std::to_string(6) + ", got " + std::to_string(test.capacity()));
        ENDCASE(false);
    }

    // Populate it with values
    new(test.wdata() + 3) hard_t(4);
    new(test.wdata() + 4) hard_t(5);
    new(test.wdata(6) + 5) hard_t(6);

    for (array_size_t i = 0; i < test.capacity(); i++) {
        if (*test[i] != i + 1) {
            ERROR("Reserving in non-empty array failed; element at index " + std::to_string(i) + " is expected: " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for a hard case (downsize). */
bool test_reserve_downsize() {
    TESTCASE("down-sizeing reserve");

    // Declare a test array & reserve space in it
    Array<hard_t> test = { hard_t(1), hard_t(2), hard_t(3), hard_t(4), hard_t(5), hard_t(6) };
    test.reserve(3);
    if (test.capacity() != 3) {
        ERROR("Reserving in larger array failed; incorrect capacity: expected " + std::to_string(3) + ", got " + std::to_string(test.capacity()));
        ENDCASE(false);
    }

    for (array_size_t i = 0; i < test.capacity(); i++) {
        if (*test[i] != i + 1) {
            ERROR("Reserving in larger array failed; element at index " + std::to_string(i) + " is expected: " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for an easy case (empty to larger). */
bool test_resize_empty() {
    TESTCASE("resize from empty");

    // Declare a test array & reserve space in it
    Array<DefaultHeapClass<int>> test;
    test.resize(3);
    if (test.size() != 3) {
        ERROR("Resizeing in empty array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    for (array_size_t i = 0; i < test.capacity(); i++) {
        if (*test[i] != 42) {
            ERROR("Resizeing in empty array failed; element at index " + std::to_string(i) + " is expected: " + std::to_string(42) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for a bit harder case (non-empty to larger). */
bool test_resize_nonempty() {
    TESTCASE("resize from non-empty");

    // Declare a test array & reserve space in it
    Array<DefaultHeapClass<int>> test = { DefaultHeapClass<int>(), DefaultHeapClass<int>(), DefaultHeapClass<int>() };
    test.resize(6);
    if (test.size() != 6) {
        ERROR("Resizeing in non-empty array failed; incorrect capacity: expected " + std::to_string(6) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != 42) {
            ERROR("Resizeing in non-empty array failed; element at index " + std::to_string(i) + " is expected: " + std::to_string(42) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for a hard case (downsize). */
bool test_resize_downsize() {
    TESTCASE("down-sizeing resize");

    // Declare a test array & reserve space in it
    Array<DefaultHeapClass<int>> test = { DefaultHeapClass<int>(), DefaultHeapClass<int>(), DefaultHeapClass<int>(), DefaultHeapClass<int>(), DefaultHeapClass<int>(), DefaultHeapClass<int>() };
    test.resize(3);
    if (test.size() != 3) {
        ERROR("Reserving in larger array failed; incorrect capacity: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != 42) {
            ERROR("Reserving in larger array failed; element at index " + std::to_string(i) + " is expected: " + std::to_string(42) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_resize() {
    TESTRUN("Array reserve() and resize()");

    if (!test_reserve_empty()) {
        ENDRUN(false);
    }
    if (!test_reserve_nonempty()) {
        ENDRUN(false);
    }
    if (!test_reserve_downsize()) {
        ENDRUN(false);
    }
    if (!test_resize_empty()) {
        ENDRUN(false);
    }
    if (!test_resize_nonempty()) {
        ENDRUN(false);
    }
    if (!test_resize_downsize()) {
        ENDRUN(false);
    }


    ENDRUN(true);
}
