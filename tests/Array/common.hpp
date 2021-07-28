/* COMMON.hpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:41:16 PM
 * Last edited:
 *   12/23/2020, 5:19:16 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File with common stuff for all the testfiles.
**/

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <algorithm>

/***** HELPER CLASSES *****/
/* The easiest class; just stores a value. */
template <class T>
class ValueClass {
private:
    /* The value it stores. */
    T value;

public:
    /* Constructor for the ValueClass, which takes the value it should store. */
    ValueClass(const T& value) : value(value) {}
    
    /* Returns a muteable reference to the internal value class. */
    inline T& operator*() { return this->value; }
    /* Returns a constant reference to the internal value class. */
    inline const T& operator*() const { return this->value; }

};

/* A bit harder class: stores a value on the heap. */
template <class T>
class HeapClass {
private:
    /* The value it stores. */
    T* value;

public:
    /* Constructor for the HeapClass, which takes the value it should store. */
    HeapClass(const T& value) : value(new T(value)) {}
    /* Copy constructor for the HeapClass. */
    HeapClass(const HeapClass& other) : value(new T(*other.value)) {}
    /* Move constructor for the HeapClass. */
    HeapClass(HeapClass&& other) : value(other.value) {
        other.value = nullptr;
    }
    /* Destructor for the HeapClass. */
    virtual ~HeapClass() {
        if (this->value != nullptr) { delete this->value; }
    }
    
    /* Returns a muteable reference to the internal value class. */
    inline T& operator*() { return *this->value; }
    /* Returns a constant reference to the internal value class. */
    inline const T& operator*() const { return *this->value; }

    /* Copy assignment operator for the HeapClass. */
    inline HeapClass& operator=(const HeapClass& other) { return *this = HeapClass(other); }
    /* Move assignment operator for the HeapClass. */
    HeapClass& operator=(HeapClass&& other) {
        using std::swap;
        if (this != &other) { swap(this->value, other.value); }
        return *this;
    }
};

/* The hardest class, which implements polymorphism (parent class). */
template <class T>
class ParentClass {
protected:
    /* The value stored in the parent class. */
    T _value1;

public:
    /* Constructor for the ParentClass, which takes the value for the internal value. */
    ParentClass(const T& value) : _value1(value) {}
    /* Declare the destructor as virtual. */
    virtual ~ParentClass() = default;

    /* Returns a muteable reference to the internal value class. */
    inline T& value1() { return this->_value1; }
    /* Returns a constant reference to the internal value class. */
    inline const T& value1() const { return this->_value1; }

};

/* The hardest class, which implements polymorphism (parent class). */
template <class T1, class T2>
class ChildClass : public ParentClass<T1> {
private:
    /* The value stored in the child class (heap-allocated). */
    T2* _value2;

public:
    /* Constructor for the ParentClass, which takes the value for the internal value. */
    ChildClass(const T1& value1, const T2& value2) : 
        ParentClass<T1>(value1),
        _value2(new T2(value2)) 
    {}
    /* Copy constructor for the ChildClass. */
    ChildClass(const ChildClass& other) :
        ParentClass<T1>(other),
        _value2(new T2(*other._value2))
    {}
    /* Move constructor for the ChildClass. */
    ChildClass(ChildClass&& other) :
        ParentClass<T1>(other),
        _value2(other._value2)
    {
        other._value2 = nullptr;
    }
    /* Destructor for the ChildClass. */
    ~ChildClass() { if (this->_value2 != nullptr) { delete this->_value2; } }

    /* Returns a muteable reference to the internal value class. */
    inline T2& value2() { return *this->_value2; }
    /* Returns a constant reference to the internal value class. */
    inline const T2& value2() const { return *this->_value2; }

    /* Copy assignment operator for the ChildClass. */
    inline ChildClass& operator=(const ChildClass& other) { return *this = ChildClass(other); }
    /* Move assignment operator for the ChildClass. */
    ChildClass& operator=(ChildClass&& other) {
        using std::swap;
        if (this != &other) {
            swap(this->_value1, other._value1);
            swap(this->_value2, other._value2);
        }
        return *this;
    }
};





/***** USING *****/
using easy_t = int;
using medium_t = ValueClass<easy_t>;
using medium_hard_t = ValueClass<medium_t>;
using hard_t = HeapClass<int>;
using super_hard_t = HeapClass<hard_t>;
using extreme_t = ChildClass<easy_t, medium_t>;
using super_extreme_t = ChildClass<super_hard_t, extreme_t>;





/***** USEFUL DEFINES *****/
/* Prints the intro for a whole new test run. */
#define TESTRUN(NAME) \
    cout << endl << "TEST RUN for " NAME << endl;
/* Prints the outtro for a whole new test run. */
#define ENDRUN(SUCCESS) \
    cout << "Run: " << ((SUCCESS) ? "\033[32;1mSUCCESS\033[0m" : "\033[31;1mFAIL\033[0m") << endl << endl; \
    return (SUCCESS);
/* Prints the intro for the given test case. */
#define TESTCASE(NAME) \
    cout << " > Testing " NAME "..." << flush;
/* Prints a failure message. */
#define ERROR(MESSAGE) \
    cout << endl << "   \033[31;1mERROR\033[0m: " MESSAGE << endl;
/* Prints the outtro for the given test case. */
#define ENDCASE(SUCCESS) \
    cout << ((SUCCESS) ? " \033[32;1mOK\033[0m" : "   Testcase failed.") << endl; \
    return (SUCCESS);

#endif
