/* INITIALIZATION.cpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:37:41 PM
 * Last edited:
 *   12/23/2020, 5:19:39 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that tests the intitializations part of the Array.
**/

#include <iostream>
#include <cstddef>

#define FULLCOMPILE
#include "tools/Array.hpp"
#include "common.hpp"

using namespace std;
using namespace Tools;


/***** TESTS *****/
/* First test, that checks if the default constructor works. */
static bool test_default_constructor() {
    TESTCASE("default constructor");

    // Try each type, with a success message
    Array<easy_t> test1;
    cout << endl << "      Easy case success" << endl;
    Array<medium_t> test2;
    cout << "      Medium case success" << endl;
    Array<medium_hard_t> test3;
    cout << "      Medium hard case success" << endl;
    Array<hard_t> test4;
    cout << "      Hard case success" << endl;
    Array<super_hard_t> test5;
    cout << "      Super hard case success" << endl;
    Array<extreme_t> test6;
    cout << "      Extreme case success" << endl;
    Array<super_extreme_t> test7;
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}

/* Second test, that tests initialization and destruction with the initial size constructor. */
static bool test_initial_size_constructor() {
    TESTCASE("initial size constructor");

    // Try each type, with a success message
    Array<easy_t> test1(rand() % 2048);
    cout << endl << "      Easy case success" << endl;
    Array<medium_t> test2(rand() % 2048);
    cout << "      Medium case success" << endl;
    Array<medium_hard_t> test3(rand() % 2048);
    cout << "      Medium hard case success" << endl;
    Array<hard_t> test4(rand() % 2048);
    cout << "      Hard case success" << endl;
    Array<super_hard_t> test5(rand() % 2048);
    cout << "      Super hard case success" << endl;
    Array<extreme_t> test6(rand() % 2048);
    cout << "      Extreme case success" << endl;
    Array<super_extreme_t> test7(rand() % 2048);
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}

/* Third test, that tests initialization and destruction with the initializer list constructor. */
static bool test_initializer_list_constructor() {
    TESTCASE("initializer list constructor");

    // Try each type (both empty , one element and with more elements), with a success message
    Array<easy_t> test1a = {};
    Array<easy_t> test1b = { rand() };
    Array<easy_t> test1c = { rand(), rand() };
    cout << endl << "      Easy case success" << endl;

    Array<medium_t> test2a = {};
    Array<medium_t> test2b = { medium_t(rand()) };
    Array<medium_t> test2c = { medium_t(rand()), medium_t(rand()) };
    cout << "      Medium case success" << endl;

    Array<medium_hard_t> test3a = {};
    Array<medium_hard_t> test3b = { medium_hard_t(medium_t(rand())) };
    Array<medium_hard_t> test3c = { medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())) };
    cout << "      Medium hard case success" << endl;

    Array<hard_t> test4a = {};
    Array<hard_t> test4b = { hard_t(rand()) };
    Array<hard_t> test4c = { hard_t(rand()), hard_t(rand()) };
    cout << "      Hard case success" << endl;

    Array<super_hard_t> test5a = {};
    Array<super_hard_t> test5b = { super_hard_t(hard_t(rand())) };
    Array<super_hard_t> test5c = { super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())) };
    cout << "      Super hard case success" << endl;

    Array<extreme_t> test6a = {};
    Array<extreme_t> test6b = { extreme_t(rand(), medium_t(rand())) };
    Array<extreme_t> test6c = { extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())) };
    cout << "      Extreme case success" << endl;

    Array<super_extreme_t> test7a = {};
    Array<super_extreme_t> test7b = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    Array<super_extreme_t> test7c = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}

/* Third test, that tests initialization and destruction with the initializer list constructor. */
static bool test_c_array_initializer() {
    TESTCASE("C-style array constructor");

    // Try each type (both empty , one element and with more elements), with a success message
    easy_t test1b_val[] = { rand() };
    easy_t test1c_val[] = { rand(), rand() };
    Array<easy_t> test1a(nullptr, 0);
    Array<easy_t> test1b(test1b_val, 1);
    Array<easy_t> test1c(test1c_val, 2);
    cout << endl << "      Easy case success" << endl;

    medium_t test2b_val[] = { medium_t(rand()) };
    medium_t test2c_val[] = { medium_t(rand()), medium_t(rand()) };
    Array<medium_t> test2a(nullptr, 0);
    Array<medium_t> test2b(test2b_val, 1);
    Array<medium_t> test2c(test2c_val, 2);
    cout << "      Medium case success" << endl;

    medium_hard_t test3b_val[] = { medium_hard_t(medium_t(rand())) };
    medium_hard_t test3c_val[] = { medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())) };
    Array<medium_hard_t> test3a(nullptr, 0);
    Array<medium_hard_t> test3b(test3b_val, 1);
    Array<medium_hard_t> test3c(test3c_val, 2);
    cout << "      Medium hard case success" << endl;

    hard_t test4b_val[] = { hard_t(rand()) };
    hard_t test4c_val[] = { hard_t(rand()), hard_t(rand()) };
    Array<hard_t> test4a(nullptr, 0);
    Array<hard_t> test4b(test4b_val, 1);
    Array<hard_t> test4c(test4c_val, 2);
    cout << "      Hard case success" << endl;

    super_hard_t test5b_val[] = { super_hard_t(hard_t(rand())) };
    super_hard_t test5c_val[] = { super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())) };
    Array<super_hard_t> test5a(nullptr, 0);
    Array<super_hard_t> test5b(test5b_val, 1);
    Array<super_hard_t> test5c(test5c_val, 2);
    cout << "      Super hard case success" << endl;

    extreme_t test6b_val[] = { extreme_t(rand(), medium_t(rand())) };
    extreme_t test6c_val[] = { extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())) };
    Array<extreme_t> test6a(nullptr, 0);
    Array<extreme_t> test6b(test6b_val, 1);
    Array<extreme_t> test6c(test6c_val, 2);
    cout << "      Extreme case success" << endl;

    super_extreme_t test7b_val[] = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    super_extreme_t test7c_val[] = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    Array<super_extreme_t> test7a(nullptr, 0);
    Array<super_extreme_t> test7b(test7b_val, 1);
    Array<super_extreme_t> test7c(test7c_val, 2);
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}

/* Fourth test, that tests initialization and destruction with the vector constructor. */
static bool test_vector_initializer() {
    TESTCASE("vector constructor");

    // Try each type (both empty , one element and with more elements), with a success message
    std::vector<easy_t> test1a_val({});
    std::vector<easy_t> test1b_val({ rand() });
    std::vector<easy_t> test1c_val({ rand(), rand() });
    Array<easy_t> test1a(test1a_val);
    Array<easy_t> test1b(test1b_val);
    Array<easy_t> test1c(test1c_val);
    cout << endl << "      Easy case success" << endl;

    std::vector<medium_t> test2a_val = {};
    std::vector<medium_t> test2b_val = { medium_t(rand()) };
    std::vector<medium_t> test2c_val = { medium_t(rand()), medium_t(rand()) };
    Array<medium_t> test2a(test2a_val);
    Array<medium_t> test2b(test2b_val);
    Array<medium_t> test2c(test2c_val);
    cout << "      Medium case success" << endl;

    std::vector<medium_hard_t> test3a_val = {};
    std::vector<medium_hard_t> test3b_val = { medium_hard_t(medium_t(rand())) };
    std::vector<medium_hard_t> test3c_val = { medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())) };
    Array<medium_hard_t> test3a(test3a_val);
    Array<medium_hard_t> test3b(test3b_val);
    Array<medium_hard_t> test3c(test3c_val);
    cout << "      Medium hard case success" << endl;

    std::vector<hard_t> test4a_val = {};
    std::vector<hard_t> test4b_val = { hard_t(rand()) };
    std::vector<hard_t> test4c_val = { hard_t(rand()), hard_t(rand()) };
    Array<hard_t> test4a(test4a_val);
    Array<hard_t> test4b(test4b_val);
    Array<hard_t> test4c(test4c_val);
    cout << "      Hard case success" << endl;

    std::vector<super_hard_t> test5a_val = {};
    std::vector<super_hard_t> test5b_val = { super_hard_t(hard_t(rand())) };
    std::vector<super_hard_t> test5c_val = { super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())) };
    Array<super_hard_t> test5a(test5a_val);
    Array<super_hard_t> test5b(test5b_val);
    Array<super_hard_t> test5c(test5c_val);
    cout << "      Super hard case success" << endl;

    std::vector<extreme_t> test6a_val = {};
    std::vector<extreme_t> test6b_val = { extreme_t(rand(), medium_t(rand())) };
    std::vector<extreme_t> test6c_val = { extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())) };
    Array<extreme_t> test6a(test6a_val);
    Array<extreme_t> test6b(test6b_val);
    Array<extreme_t> test6c(test6c_val);
    cout << "      Extreme case success" << endl;

    std::vector<super_extreme_t> test7a_val = {};
    std::vector<super_extreme_t> test7b_val = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    std::vector<super_extreme_t> test7c_val = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    Array<super_extreme_t> test7a(test7a_val);
    Array<super_extreme_t> test7b(test7b_val);
    Array<super_extreme_t> test7c(test7c_val);
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}

/* Fifth test, that tests the copy constructor. */
static bool test_copy_constructor() {
    TESTCASE("copy constructor");

    // Try each type (both empty , one element and with more elements), with a success message
    Array<easy_t> test1a = { rand(), rand(), rand() };
    Array<easy_t> test1b(test1a);
    cout << endl << "      Easy case success" << endl;

    Array<medium_t> test2a = { medium_t(rand()), medium_t(rand()), medium_t(rand()) };
    Array<medium_t> test2b(test2a);
    cout << "      Medium case success" << endl;

    Array<medium_hard_t> test3a = { medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())) };
    Array<medium_hard_t> test3b(test3a);
    cout << "      Medium hard case success" << endl;

    Array<hard_t> test4a = { hard_t(rand()), hard_t(rand()), hard_t(rand()) };
    Array<hard_t> test4b(test4a);
    cout << "      Hard case success" << endl;

    Array<super_hard_t> test5a = { super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())) };
    Array<super_hard_t> test5b(test5a);
    cout << "      Super hard case success" << endl;

    Array<extreme_t> test6a = { extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())) };
    Array<extreme_t> test6b(test6a);
    cout << "      Extreme case success" << endl;

    Array<super_extreme_t> test7a = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    Array<super_extreme_t> test7b(test7a);
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}

/* Sixth test, that tests the move constructor. */
static bool test_move_constructor() {
    TESTCASE("move constructor");

    // Try each type (both empty , one element and with more elements), with a success message
    Array<easy_t> test1a = { rand(), rand(), rand() };
    Array<easy_t> test1b(std::move(test1a));
    cout << endl << "      Easy case success" << endl;

    Array<medium_t> test2a = { medium_t(rand()), medium_t(rand()), medium_t(rand()) };
    Array<medium_t> test2b(std::move(test2a));
    cout << "      Medium case success" << endl;

    Array<medium_hard_t> test3a = { medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())), medium_hard_t(medium_t(rand())) };
    Array<medium_hard_t> test3b(std::move(test3a));
    cout << "      Medium hard case success" << endl;

    Array<hard_t> test4a = { hard_t(rand()), hard_t(rand()), hard_t(rand()) };
    Array<hard_t> test4b(std::move(test4a));
    cout << "      Hard case success" << endl;

    Array<super_hard_t> test5a = { super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())), super_hard_t(hard_t(rand())) };
    Array<super_hard_t> test5b(std::move(test5a));
    cout << "      Super hard case success" << endl;

    Array<extreme_t> test6a = { extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())), extreme_t(rand(), medium_t(rand())) };
    Array<extreme_t> test6b(std::move(test6a));
    cout << "      Extreme case success" << endl;

    Array<super_extreme_t> test7a = { super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))), super_extreme_t(super_hard_t(hard_t(rand())), extreme_t(rand(), medium_t(rand()))) };
    Array<super_extreme_t> test7b(std::move(test7a));
    cout << "      Super extreme case success" << endl;

    cout << "  ";
    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_initialization() {
    TESTRUN("Array initialization / destruction");

    if (!test_default_constructor()) {
        ENDRUN(false);
    }
    if (!test_initial_size_constructor()) {
        ENDRUN(false);
    }
    if (!test_initializer_list_constructor()) {
        ENDRUN(false);
    }
    if (!test_c_array_initializer()) {
        ENDRUN(false);
    }
    if (!test_vector_initializer()) {
        ENDRUN(false);
    }
    if (!test_copy_constructor()) {
        ENDRUN(false);
    }
    if (!test_move_constructor()) {
        ENDRUN(false);
    }

    ENDRUN(true);
}
