/* CONSTRUCTORS.cpp
 *   by Lut99
 *
 * Created:
 *   1/13/2021, 4:05:43 PM
 * Last edited:
 *   7/28/2021, 9:09:17 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File to check how the Array class works for classes that are missing
 *   constructors.
**/

#include <iostream>
#include <cstddef>

#define FULLCOMPILE
#include "tools/Array.hpp"
#include "common.hpp"

using namespace std;
using namespace Tools;


/***** CLASSES *****/
class Value {
public:
    /* The default value that will be pasted everywhere as default. */
    static constexpr int default_value = 42;

private:
    int* value;

public:
    Value() { this->value = new int; *this->value = default_value; }
    Value(int value) { this->value = new int; *this->value = value; }
    Value(const Value& other) { this->value = new int; *this->value = *other.value; }
    Value(Value&& other): value(other.value) { other.value = nullptr; }
    virtual ~Value() { if (this->value != nullptr) { delete this->value; } };

    /* Returns the value. */
    inline int get_value() const { return *this->value; }

};
class NoDefault: public Value {
public:
    NoDefault() = delete;
    NoDefault(int value): Value(value) {}
    NoDefault(const NoDefault& other): Value(other) {}
    NoDefault(NoDefault&& other): Value(std::move(other)) {}
    virtual ~NoDefault() {}

};
class NoCopy: public Value {
public:
    NoCopy(): Value() {}
    NoCopy(int value): Value(value) {}
    NoCopy(const NoCopy& other) = delete;
    NoCopy(NoCopy&& other): Value(std::move(other)) {}
    virtual ~NoCopy() {}
    
};
class NoMove: public Value {
public:
    NoMove(): Value() {}
    NoMove(int value): Value(value) {}
    NoMove(const NoMove& other): Value(other) {}
    NoMove(NoMove&& other) = delete;
    virtual ~NoMove() {}
    
};
class NoDefaultNoCopy: public Value {
public:
    NoDefaultNoCopy() = delete;
    NoDefaultNoCopy(int value): Value(value) {}
    NoDefaultNoCopy(const NoDefaultNoCopy& other) = delete;
    NoDefaultNoCopy(NoDefaultNoCopy&& other): Value(std::move(other)) {}
    virtual ~NoDefaultNoCopy() {}
    
};
class NoCopyNoMove: public Value {
public:
    NoCopyNoMove(): Value() {}
    NoCopyNoMove(int value): Value(value) {}
    NoCopyNoMove(const NoCopyNoMove& other) = delete;
    NoCopyNoMove(NoCopyNoMove&& other) = delete;
    virtual ~NoCopyNoMove() {}

};
class NoDefaultNoMove: public Value {
public:
    NoDefaultNoMove() = delete;
    NoDefaultNoMove(int value): Value(value) {}
    NoDefaultNoMove(const NoDefaultNoMove& other): Value(other) {}
    NoDefaultNoMove(NoDefaultNoMove&& other) = delete;
    virtual ~NoDefaultNoMove() {}
    
};
class NoDefaultNoCopyNoMove: public Value {
public:
    NoDefaultNoCopyNoMove() = delete;
    NoDefaultNoCopyNoMove(int value): Value(value) {}
    NoDefaultNoCopyNoMove(const Value& other) = delete;
    NoDefaultNoCopyNoMove(NoDefaultNoCopyNoMove&& other) = delete;
    virtual ~NoDefaultNoCopyNoMove() {}
    
};


/***** HELPER FUNCTION *****/
template <class T, typename = std::enable_if_t<std::is_base_of<Value, T>::value> >
Array<T> populate(int N) {
    Array<T> result(N);
    for (int i = 0; i < N; i++) {
        result.push_back(T(i));
    }
    return result;
}


/***** TESTS *****/
/* First test, that checks if a missing default constructor works. */
static bool test_nodefault() {
    TESTCASE("without default constructor");

    Array<NoDefault> test = { NoDefault(1), NoDefault(2), NoDefault(3) };
    test.push_back(NoDefault(4));
    Array<NoDefault> test2 = test;
    for (array_size_t i = 0; i < test2.size(); i++) {
        if (test2[i].get_value() != (int) (i + 1)) {
            ERROR("Constructing without default failed; expected " + std::to_string((int) (i + 1)) + ", got " + std::to_string(test2[i].get_value()));
            ENDCASE(false);
        }
    }

    // Also do the through-function test
    Array<NoDefault> test3 = populate<NoDefault>(3);
    for (array_size_t i = 0; i < 3; i++) {
        if (test3[i].get_value() != (int) i) {
            ERROR("Constructing without default through populate failed; expected " + std::to_string(i) + ", got " + std::to_string(test3[i].get_value()));
            ENDCASE(false);
        }
    }

    cout << "  ";
    ENDCASE(true);
}

/* Second test, that checks if a missing copy constructor works. */
static bool test_nocopy() {
    TESTCASE("without copy constructor");

    Array<NoCopy> test(3);
    test.push_back(NoCopy(1));
    test.push_back(NoCopy(2));
    test.push_back(NoCopy(3));
    for (array_size_t i = 0; i < test.size(); i++) {
        if (test[i].get_value() != (int) (i + 1)) {
            ERROR("Constructing without copy failed; expected " + std::to_string((int) (i + 1)) + ", got " + std::to_string(test[i].get_value()));
            ENDCASE(false);
        }
    }

    // Also do the through-function test
    Array<NoCopy> test2 = populate<NoCopy>(3);
    for (array_size_t i = 0; i < 3; i++) {
        if (test2[i].get_value() != (int) i) {
            ERROR("Constructing without copy through populate failed; expected " + std::to_string(i) + ", got " + std::to_string(test2[i].get_value()));
            ENDCASE(false);
        }
    }
    
    test.push_back();
    test.push_back();
    test.push_back();

    cout << "  ";
    ENDCASE(true);
}

/* Third test, that checks if a missing move constructor works. */
static bool test_nomove() {
    TESTCASE("without move constructor");

    NoMove elems[] = { NoMove(1), NoMove(2), NoMove(3) };
    Array<NoMove> test(elems, 3);
    for (array_size_t i = 0; i < test.size(); i++) {
        if (test[i].get_value() != (int) (i + 1)) {
            ERROR("Constructing without copy failed; expected " + std::to_string((int) (i + 1)) + ", got " + std::to_string(test[i].get_value()));
            ENDCASE(false);
        }
    }

    // Also do the through-function test
    Array<NoDefault> test2 = populate<NoDefault>(3);
    for (array_size_t i = 0; i < 3; i++) {
        if (test2[i].get_value() != (int) i) {
            ERROR("Constructing without move through populate failed; expected " + std::to_string(i) + ", got " + std::to_string(test2[i].get_value()));
            ENDCASE(false);
        }
    }

    cout << "  ";
    ENDCASE(true);
}

/* Fourth test, that checks if a missing default & copy constructors works. */
static bool test_nodefaultnocopy() {
    TESTCASE("without default & copy constructors");

    Array<NoDefaultNoCopy> test(3);
    test.push_back(NoDefaultNoCopy(1));
    test.push_back(NoDefaultNoCopy(2));
    test.push_back(NoDefaultNoCopy(3));
    for (array_size_t i = 0; i < test.size(); i++) {
        if (test[i].get_value() != (int) (i + 1)) {
            ERROR("Constructing without copy failed; expected " + std::to_string((int) (i + 1)) + ", got " + std::to_string(test[i].get_value()));
            ENDCASE(false);
        }
    }

    // Also do the through-function test
    Array<NoDefault> test2 = populate<NoDefault>(3);
    for (array_size_t i = 0; i < 3; i++) {
        if (test2[i].get_value() != (int) i) {
            ERROR("Constructing without default & copy through populate failed; expected " + std::to_string(i) + ", got " + std::to_string(test2[i].get_value()));
            ENDCASE(false);
        }
    }

    cout << "  ";
    ENDCASE(true);
}

/* Fifth test, that checks if a missing copy & move constructors works. */
static bool test_nocopynomove() {
    TESTCASE("without copy & move constructors");

    Array<NoCopyNoMove> test(3);
    test.push_back();
    test.push_back();
    test.push_back();
    for (array_size_t i = 0; i < test.size(); i++) {
        if (test[i].get_value() != NoCopyNoMove::default_value) {
            ERROR("Constructing without copy failed; expected " + std::to_string(NoCopyNoMove::default_value) + ", got " + std::to_string(test[i].get_value()));
            ENDCASE(false);
        }
    }

    cout << "  ";
    ENDCASE(true);
}

/* Sixth test, that checks if a missing default & move constructors works. */
static bool test_nodefaultnomove() {
    TESTCASE("without default & move constructors");

    NoDefaultNoMove elems[] = { NoDefaultNoMove(1), NoDefaultNoMove(2), NoDefaultNoMove(3) };
    Array<NoDefaultNoMove> test(elems, 3);
    for (array_size_t i = 0; i < test.size(); i++) {
        if (test[i].get_value() != (int) (i + 1)) {
            ERROR("Constructing without copy failed; expected " + std::to_string((int) (i + 1)) + ", got " + std::to_string(test[i].get_value()));
            ENDCASE(false);
        }
    }

    // Also do the through-function test
    Array<NoDefault> test2 = populate<NoDefault>(3);
    for (array_size_t i = 0; i < 3; i++) {
        if (test2[i].get_value() != (int) i) {
            ERROR("Constructing without default & move through populate failed; expected " + std::to_string(i) + ", got " + std::to_string(test2[i].get_value()));
            ENDCASE(false);
        }
    }

    cout << "  ";
    ENDCASE(true);
}

/* Seventh test, that checks if a missing default, copy & move constructors works. */
static bool test_nodefaultnocopynomove() {
    TESTCASE("without default, copy & move constructors");

    Array<NoDefaultNoCopyNoMove> test;

    cout << "  ";
    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_constructors() {
    TESTRUN("Arrays with missing constructors");

    if (!test_nodefault()) {
        ENDRUN(false);
    }
    if (!test_nocopy()) {
        ENDRUN(false);
    }
    if (!test_nomove()) {
        ENDRUN(false);
    }
    if (!test_nodefaultnocopy()) {
        ENDRUN(false);
    }
    if (!test_nocopynomove()) {
        ENDRUN(false);
    }
    if (!test_nodefaultnomove()) {
        ENDRUN(false);
    }
    if (!test_nodefaultnocopynomove()) {
        ENDRUN(false);
    }

    ENDRUN(true);
}

