/* CONSISTANCY.cpp
 *   by Lut99
 *
 * Created:
 *   12/23/2020, 4:00:20 PM
 * Last edited:
 *   12/23/2020, 5:18:05 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file tests the consistancy of the array, i.e., if elements are
 *   still the same as they were when we put them in after various
 *   scenario's.
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
/* First test, that checks if the initiliazer list constructor produces useable elements. */
static bool test_list_initializer() {
    TESTCASE("list initializer constructor");

    // Declare all arrays
    Array<easy_t> test1 = { 1, 2, 3 };
    Array<medium_t> test2 = { medium_t(1), medium_t(2), medium_t(3) };
    Array<medium_hard_t> test3 = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    Array<hard_t> test4 = { hard_t(1), hard_t(2), hard_t(3) };
    Array<super_hard_t> test5 = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    Array<extreme_t> test6 = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    Array<super_extreme_t> test7 = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };

    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 1) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 1) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 1) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 1) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 1) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 1) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 1) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 1) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 1) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 1) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Second test, that checks if the C-style array constructor produces useable elements. */
static bool test_c_style_constructor() {
    TESTCASE("C-style constructor");

    // Declare all arrays
    easy_t test1_val[] = { 1, 2, 3 };
    medium_t test2_val[] = { medium_t(1), medium_t(2), medium_t(3) };
    medium_hard_t test3_val[] = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    hard_t test4_val[] = { hard_t(1), hard_t(2), hard_t(3) };
    super_hard_t test5_val[] = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    extreme_t test6_val[] = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    super_extreme_t test7_val[] = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };
    Array<easy_t> test1(test1_val, 3);
    Array<medium_t> test2(test2_val, 3);
    Array<medium_hard_t> test3(test3_val, 3);
    Array<hard_t> test4(test4_val, 3);
    Array<super_hard_t> test5(test5_val, 3);
    Array<extreme_t> test6(test6_val, 3);
    Array<super_extreme_t> test7(test7_val, 3);

    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 1) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 1) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 1) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 1) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 1) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 1) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 1) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 1) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 1) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 1) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Third test, that checks if the vector constructor produces useable elements. */
static bool test_vector_constructor() {
    TESTCASE("vector constructor");

    // Declare all arrays
    std::vector<easy_t> test1_val = { 1, 2, 3 };
    std::vector<medium_t> test2_val = { medium_t(1), medium_t(2), medium_t(3) };
    std::vector<medium_hard_t> test3_val = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    std::vector<hard_t> test4_val = { hard_t(1), hard_t(2), hard_t(3) };
    std::vector<super_hard_t> test5_val = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    std::vector<extreme_t> test6_val = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    std::vector<super_extreme_t> test7_val = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };
    Array<easy_t> test1(test1_val);
    Array<medium_t> test2(test2_val);
    Array<medium_hard_t> test3(test3_val);
    Array<hard_t> test4(test4_val);
    Array<super_hard_t> test5(test5_val);
    Array<extreme_t> test6(test6_val);
    Array<super_extreme_t> test7(test7_val);

    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 1) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 1) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 1) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 1) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 1) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 1) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 1) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 1) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 1) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 1) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Fifth test, that checks the consistency of copied Arrays. */
static bool test_copy_constructor() {
    TESTCASE("copy constructor");

    // Declare all arrays
    Array<easy_t> test1_val = { 1, 2, 3 };
    Array<medium_t> test2_val = { medium_t(1), medium_t(2), medium_t(3) };
    Array<medium_hard_t> test3_val = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    Array<hard_t> test4_val = { hard_t(1), hard_t(2), hard_t(3) };
    Array<super_hard_t> test5_val = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    Array<extreme_t> test6_val = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    Array<super_extreme_t> test7_val = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };
    Array<easy_t> test1(test1_val);
    Array<medium_t> test2(test2_val);
    Array<medium_hard_t> test3(test3_val);
    Array<hard_t> test4(test4_val);
    Array<super_hard_t> test5(test5_val);
    Array<extreme_t> test6(test6_val);
    Array<super_extreme_t> test7(test7_val);

    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 1) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 1) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 1) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 1) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 1) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 1) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 1) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 1) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 1) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 1) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Sixth test, that checks the consistency of moved Arrays. */
static bool test_move_constructor() {
    TESTCASE("move constructor");

    // Declare all arrays
    Array<easy_t> test1_val = { 1, 2, 3 };
    Array<medium_t> test2_val = { medium_t(1), medium_t(2), medium_t(3) };
    Array<medium_hard_t> test3_val = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    Array<hard_t> test4_val = { hard_t(1), hard_t(2), hard_t(3) };
    Array<super_hard_t> test5_val = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    Array<extreme_t> test6_val = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    Array<super_extreme_t> test7_val = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };
    Array<easy_t> test1(std::move(test1_val));
    Array<medium_t> test2(std::move(test2_val));
    Array<medium_hard_t> test3(std::move(test3_val));
    Array<hard_t> test4(std::move(test4_val));
    Array<super_hard_t> test5(std::move(test5_val));
    Array<extreme_t> test6(std::move(test6_val));
    Array<super_extreme_t> test7(std::move(test7_val));

    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 1) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 1) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 1) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 1) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 1) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 1) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 1) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 1) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 1) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 1) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 1) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Seventh test, that checks the consistency after editing the elements using the operator[]. */
static bool test_altered() {
    TESTCASE("altered elements")

    // Declare all arrays
    Array<easy_t> test1 = { 1, 2, 3 };
    Array<medium_t> test2 = { medium_t(1), medium_t(2), medium_t(3) };
    Array<medium_hard_t> test3 = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    Array<hard_t> test4 = { hard_t(1), hard_t(2), hard_t(3) };
    Array<super_hard_t> test5 = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    Array<extreme_t> test6 = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    Array<super_extreme_t> test7 = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };

    // Alter them
    for (int i = 0; i < 3; i++) {
        test1[i] = i + 3;
        *test2[i] = i + 3;
        **test3[i] = i + 3;
        *test4[i] = i + 3;
        **test5[i] = i + 3;
        test6[i].value1() = i + 3;
        *test6[i].value2() = i + 3;
        **test7[i].value1() = i + 3;
        test7[i].value2().value1() = i + 3;
        *test7[i].value2().value2() = i + 3;
    }

    // Check them
    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 3) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 3) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 3) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 3) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 3) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 3) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 3) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 3) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 3) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 3) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Seventh test, that checks the consistency after editing the elements using at. */
static bool test_at_altered() {
    TESTCASE("altered elements (at)")

    // Declare all arrays
    Array<easy_t> test1 = { 1, 2, 3 };
    Array<medium_t> test2 = { medium_t(1), medium_t(2), medium_t(3) };
    Array<medium_hard_t> test3 = { medium_hard_t(medium_t(1)), medium_hard_t(medium_t(2)), medium_hard_t(medium_t(3)) };
    Array<hard_t> test4 = { hard_t(1), hard_t(2), hard_t(3) };
    Array<super_hard_t> test5 = { super_hard_t(hard_t(1)), super_hard_t(hard_t(2)), super_hard_t(hard_t(3)) };
    Array<extreme_t> test6 = { extreme_t(1, medium_t(1)), extreme_t(2, medium_t(2)), extreme_t(3, medium_t(3)) };
    Array<super_extreme_t> test7 = { super_extreme_t(super_hard_t(hard_t(1)), extreme_t(1, medium_t(1))), super_extreme_t(super_hard_t(hard_t(2)), extreme_t(2, medium_t(2))), super_extreme_t(super_hard_t(hard_t(3)), extreme_t(3, medium_t(3))) };

    // Alter them
    for (int i = 0; i < 3; i++) {
        test1.at(i) = i + 3;
        *test2.at(i) = i + 3;
        **test3.at(i) = i + 3;
        *test4.at(i) = i + 3;
        **test5.at(i) = i + 3;
        test6.at(i).value1() = i + 3;
        *test6.at(i).value2() = i + 3;
        **test7.at(i).value1() = i + 3;
        test7.at(i).value2().value1() = i + 3;
        *test7.at(i).value2().value2() = i + 3;
    }

    // Check them
    // Test each of them if they still have the values we like
    for (array_size_t i = 0; i < test1.size(); i++) {
        if (test1[i] != i + 3) {
            ERROR("Easy value " + std::to_string(test1[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (*test2[i] != i + 3) {
            ERROR("Medium value " + std::to_string(*test2[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test3.size(); i++) {
        if (**test3[i] != i + 3) {
            ERROR("Medium hard value " + std::to_string(**test3[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test4.size(); i++) {
        if (*test4[i] != i + 3) {
            ERROR("Hard value " + std::to_string(*test4[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test5.size(); i++) {
        if (**test5[i] != i + 3) {
            ERROR("Super hard value " + std::to_string(**test5[i]) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test6.size(); i++) {
        if (test6[i].value1() != i + 3) {
            ERROR("Extreme value 1 " + std::to_string(test6[i].value1()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
        if (*test6[i].value2() != i + 3) {
            ERROR("Extreme value 2 " + std::to_string(*test6[i].value2()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }
    for (array_size_t i = 0; i < test7.size(); i++) {
        if (**test7[i].value1() != i + 3) {
            ERROR("Super extreme value 1 " + std::to_string(**test7[i].value1()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
        if (test7[i].value2().value1() != i + 3) {
            ERROR("Super extreme value 2 (1) " + std::to_string(test7[i].value2().value1()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
        if (*test7[i].value2().value2() != i + 3) {
            ERROR("Super extreme value 2 (2) " + std::to_string(*test7[i].value2().value2()) + " is incorrect (expected " + std::to_string(i + 3) + ")");
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_consistancy() {
    TESTRUN("Array consistancy");

    if (!test_list_initializer()) {
        ENDRUN(false);
    }
    if (!test_c_style_constructor()) {
        ENDRUN(false);
    }
    if (!test_vector_constructor()) {
        ENDRUN(false);
    }
    if (!test_copy_constructor()) {
        ENDRUN(false);
    }
    if (!test_move_constructor()) {
        ENDRUN(false);
    }
    if (!test_altered()) {
        ENDRUN(false);
    }
    if (!test_at_altered()) {
        ENDRUN(false);
    }

    ENDRUN(true);
}
