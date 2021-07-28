/* PUSH ERASE.cpp
 *   by Lut99
 *
 * Created:
 *   12/23/2020, 5:31:23 PM
 * Last edited:
 *   1/13/2021, 4:22:47 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Checks the push_back() and erase() functions of the Array class.
**/

#include <iostream>
#include <cstddef>
#include <cstring>

#define FULLCOMPILE
#include "tools/Array.hpp"
#include "common.hpp"

using namespace std;
using namespace Tools;


/***** TESTS *****/
/* Function that tests if the Array's push_back() works for an easy case (array with pre-reserved size). */
bool test_reserved_push() {
    TESTCASE("reserved push")

    // Get us a new array with an initial size
    Array<hard_t> test(3);
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    if (test.size() != 3) {
        ERROR("Pushing to reserved array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < 3; i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to reserved array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for a slightly harder case (non-reserved Array). */
bool test_nonreserved_push() {
    TESTCASE("non-reserved push")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    if (test.size() != 3) {
        ERROR("Pushing to non-reserved array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < 3; i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to non-reserved array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's pop() works for an empty array. */
bool test_empty_pop() {
    TESTCASE("empty pop")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.pop_back();
    if (test.size() != 0) {
        ERROR("Popping from empty array failed; incorrect size: expected " + std::to_string(0) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Add three elements again
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    if (test.size() != 3) {
        ERROR("Pushing to popped array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < 3; i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to popped array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's pop() works for a non-empty array. */
bool test_nonempty_pop() {
    TESTCASE("non-empty pop")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    test.push_back(hard_t(4));
    test.pop_back();
    if (test.size() != 3) {
        ERROR("Popping from non-empty array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
             ERROR("Popping from non-empty array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    // Add three elements again
    test.push_back(hard_t(4));
    test.push_back(hard_t(5));
    test.push_back(hard_t(6));
    if (test.size() != 6) {
        ERROR("Pushing to popped array failed; incorrect size: expected " + std::to_string(6) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to popped array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's erase() works for an empty array. */
bool test_empty_erase() {
    TESTCASE("empty erase")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.erase(0);
    if (test.size() != 0) {
        ERROR("Erasing from non-empty array failed; incorrect size: expected " + std::to_string(0) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Add three elements again
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    if (test.size() != 3) {
        ERROR("Pushing to erased array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to erased array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's erase() works for an empty array. */
bool test_nonempty_erase() {
    TESTCASE("non-empty erase")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    test.erase(1);
    if (test.size() != 3) {
        ERROR("Erasing from non-empty array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }
    
    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
             ERROR("Erasing from non-empty array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    // Add three elements again
    test.push_back(hard_t(4));
    test.push_back(hard_t(5));
    test.push_back(hard_t(6));
    if (test.size() != 6) {
        ERROR("Pushing to erased array failed; incorrect size: expected " + std::to_string(6) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to erased array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    // Remove the first, then the last elements
    test.erase(0);
    test.erase(test.size() - 1);
    if (test.size() != 4) {
        ERROR("Pushing to very erased array failed; incorrect size: expected " + std::to_string(4) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 2) {
             ERROR("Pushing to very erased array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 2) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's erase() works for an empty array. */
bool test_range_erase() {
    TESTCASE("range erase")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(2));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    test.erase(1, 2);
    if (test.size() != 3) {
        ERROR("Erasing from non-empty array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }
    
    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
            ERROR("Erasing from non-empty array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    // Add three elements again
    test.push_back(hard_t(4));
    test.push_back(hard_t(5));
    test.push_back(hard_t(6));
    if (test.size() != 6) {
        ERROR("Pushing to erased array failed; incorrect size: expected " + std::to_string(6) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to erased array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    // Remove the first two, then the last two elements
    test.erase(0, 1);
    test.erase(test.size() - 2, test.size() - 1);
    if (test.size() != 2) {
        ERROR("Pushing to very erased array failed; incorrect size: expected " + std::to_string(4) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (array_size_t i = 0; i < test.size(); i++) {
        if (*test[i] != i + 3) {
             ERROR("Pushing to very erased array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 3) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_push_erase() {
    TESTRUN("Array push_back(), pop_back() and erase()");

    if (!test_reserved_push()) {
        ENDRUN(false);
    }
    if (!test_nonreserved_push()) {
        ENDRUN(false);
    }
    if (!test_empty_pop()) {
        ENDRUN(false);
    }
    if (!test_nonempty_pop()) {
        ENDRUN(false);
    }
    if (!test_empty_erase()) {
        ENDRUN(false);
    }
    if (!test_nonempty_erase()) {
        ENDRUN(false);
    }
    if (!test_range_erase()) {
        ENDRUN(false);
    }

    ENDRUN(true);
}
