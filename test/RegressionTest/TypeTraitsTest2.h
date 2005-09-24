// Loki TypeTraits test program
// Kalle Rutanen
// 9.9.2005
//
// Some type sets
// --------------
//
// [] for optional
// ^ for repetition
// T for any type
//
// The set of normal pointers:
//
// G = T* [const] [volatile] [&]
//
// The set of member variable pointers:
//
// H = T A::* [const] [volatile] [&]
//
// The set of function pointers:
//
// F = T (* [const] [volatile] [&])([P1, P2, ---, Pn] + [...]])
//
// The set of member function pointers:
//
// I = T (A::* [const] [volatile] [&])([P1, P2, ---, Pn] [...]]) [const] [volatile]
//    
// Repetition
// ----------
//
// With pointers, we have an infinite number of possibilities. We can have
// for example the type 
// "const int* volatile*** const volatile**const*volatile*volatile"
// Naturally we can't test for every possible situation. In this case
// we shall only test the first level of this repetition and rely on induction.
//
// Reference &
// -----------
//
// A problem is how the reference should affect the type, in particular:
//
// Is "T*&" a pointer ?
// Is "T const&" const?
// Is "int&" integral?
// etc..
//
// The philosophy for solving these situations is: 
// "a type is what it does". By this philosophy,
// all of the questions above are answered affirmitively.
//
// Dealing with the number of possibilities
// ----------------------------------------
//
// For every test, types are separated into groups: types
// in the same groups behave the same kind with respect to the tested
// property. Thus we have to test only one of types in group.
//
// The number of tests is important to keep low for two reasons:
// First, the test program can come up too long, and as such contain
// errors itself. Second, compilers have limitations, which quite certainly
// raise internal errors during compilations due to a extensive use of the
// precompiler.
//
// Types that should not be forgotten
// ----------------------------------
//
// bool, char, wchar_t 
// signed char, signed short, signed int, signed long
// unsigned char, unsigned short, unsigned int, unsigned long
// T* (normal pointers)
// T (*)() (non-parametric function pointers)
// T (*)(int, float) (n-parametric function pointers)
// T (*)(int, float, ...) (variable-parametric function pointers)
// T A::* (member variable pointers)
// T A::(*)() (non-parametric member function pointers)
// T A::(*)(int, float) (n-parametric member function pointers)
// T A::(*)(int, float, ...) (variable-parametric member function pointers)
// T A::(*)() const volatile 
// (non-parametric cv-qualified member function pointers)
// T A::(*)(int, float) const volatile 
// (n-parametric cv-qualified member function pointers)
// T A::(*)(int, float, ...) const volatile 
// (variable-parametric cv-qualified member function pointers)
// T() (non-parametric functions)
// T(int, float) (n-parametric functions)
// T(int, float, ...) (variable-parametric functions)
//
// All types listed above with cv-qualifiers
// References to every type listed above.
// void
//
// IMPORTANT:
// 1) char != signed char && char != unsigned char
// 2) cv-qualifiers in the end of a member function does not
// affect the cv-qualifiers of the pointer type. However, they
// do affect the overall type. (void (A::*)f() const != void (A::*)f())
// 3) The ellipsis "..." does not correspond to any explicit parameter list
// such as () or (int, float)
// 4) You can have a reference to a function, for example: 
// int (&)(int, float)

#ifndef TYPETRAITSTEST2_H
#define TYPETRAITSTEST2_H

#include <iostream>
#include <string>
#include <typeinfo>

#define ENABLE_ADDITIONAL_TYPETRAITS


#include <loki/TypeTraits.h>

// Macros for testing isX variables

#define TEST_CONDITION(cond, answer) \
    testCondition(cond, answer, #cond);

#define TEST_CONDITION_HELP(type, cond, answer) \
    TEST_CONDITION(Loki::TypeTraits<type>::cond, answer)

#define PASS(type) \
    TEST_CONDITION_HELP(type, CONDITION, true)

#define FAIL(type) \
    TEST_CONDITION_HELP(type, CONDITION, false)

// Macros for testing xType types

#define TEST_TYPE_HELP2(type, rightType, cond) \
    { \
    const std::type_info& info = typeid(Loki::TypeTraits<type>::cond); \
    bool testResult = Loki::IsSameType<rightType, Loki::TypeTraits<type>::cond>::value; \
    testType(testResult, #cond, #type, info.name(), #rightType); \
    }

// This macro is needed as an intermediate step to print CONDITION right
// Could be a bug...
#define TEST_TYPE_HELP(type, rightType, cond) \
    TEST_TYPE_HELP2(type, rightType, cond)

#define TEST_TYPE(type, rightType) \
    TEST_TYPE_HELP(type, rightType, CONDITION)

///////////////////////////////////////////////////////////////////////////////
// TypeTraitsTest2
///////////////////////////////////////////////////////////////////////////////

class TypeTraitsTest2: public Test
{
public:
    TypeTraitsTest2() 
        : Test("TypeTraits.h"),
        testedConditions_(0),
        erroneousConditions_(0)
    {
    }

    virtual void execute(TestResult &result);

private:
    class A{};

    void testCount(bool result);
    void testType(bool result, std::string conditionText, 
        std::string typeText,
        std::string deducedTypeText, 
        std::string rightTypeText);
    void testCondition(bool result, bool answer, std::string text);

    void testIntegral();
    void testFloat();
    void testConst();
    void testVolatile();
    void testPointer();
    void testReference();
    void testMemberPointer();
    void testFunctionPointer();
    void testMemberFunctionPointer();
    void testFunction();

    void testParameterType();
    void testReferredType();
    void testPointeeType();

    unsigned int testedConditions_;
    unsigned int erroneousConditions_;

} typeTraitsTest2;

inline void TypeTraitsTest2::execute(TestResult &result)
{
    printName(result);

    testIntegral();
    testFloat();
    testConst();
    testVolatile();
    testPointer();
    testReference();
    testMemberPointer();
    testFunctionPointer();
    testMemberFunctionPointer();
    testFunction();
    testParameterType();
    testReferredType();
    testPointeeType();

    bool r = erroneousConditions_ == 0;

    std::cout << "Tested " << testedConditions_ 
        << " conditions of which " << erroneousConditions_ 
        << " were erroneous" << std::endl;

    testAssert("TypeTraits2",r,result);

    std::cout << std::endl;
}

inline void TypeTraitsTest2::testCount(bool result)
{
    if (!result)
    {
        ++erroneousConditions_;
    }

    ++testedConditions_;
}

inline void TypeTraitsTest2::testType(bool result, std::string conditionText, 
                              std::string typeText,
                              std::string deducedTypeText, 
                              std::string rightTypeText)
{
    testCount(result);

    if (!result)
    {
        std::cout << "TypeTraits<" << typeText << ">::" 
            << conditionText << " -> " << deducedTypeText << std::endl;
        std::cout << "    Expected " << rightTypeText << std::endl;
    }
}

inline void TypeTraitsTest2::testCondition(bool result, bool answer, std::string text)
{
    testCount(result == answer);

    if (result != answer)
    {
        std::cout << text << " == " << result << std::endl;
    }
}

inline void TypeTraitsTest2::testIntegral()
{
#undef CONDITION
#define CONDITION isIntegral

    PASS(bool);
    PASS(char);
    PASS(wchar_t);

    PASS(signed char);
    PASS(signed short int);
    PASS(signed long int);

    PASS(unsigned char);
    PASS(unsigned short int);
    PASS(unsigned long int);

    PASS(unsigned long int const volatile);
    PASS(unsigned long int const volatile&);

    FAIL(float);
    FAIL(A);
    FAIL(int*);
    FAIL(int A::*);
    FAIL(int(*)());
    FAIL(int(*)(int, float, ...));
    FAIL(int (A::*)());
    FAIL(int (A::*)(int, float, ...));
}

inline void TypeTraitsTest2::testFloat()
{
#undef CONDITION
#define CONDITION isFloat

    PASS(float);
    PASS(double);
    PASS(long double);

    PASS(long double const volatile);
    PASS(long double const volatile&);

    FAIL(int);
    FAIL(void);
    FAIL(A);
    FAIL(float*);
    FAIL(float A::*);
    FAIL(float(*)());
    FAIL(float(*)(int, float, ...));
    FAIL(float (A::*)());
    FAIL(float (A::*)(int, float, ...));
}

inline void TypeTraitsTest2::testConst()
{
#undef CONDITION
#define CONDITION isConst

    PASS(const volatile int);
    PASS(int* const volatile);
    PASS(int (* const volatile)());
    PASS(int (* const volatile)(int, float, ...));
    PASS(int A::* const volatile);
    PASS(int (A::* const volatile)());
    PASS(int (A::* const volatile)(int, float, ...));
    PASS(const volatile int&);

    FAIL(void);
    FAIL(int);
    FAIL(A);
    FAIL(const int*);
    FAIL(const int A::*);
    FAIL(const int(*)());
    FAIL(const int(*)(int, float, ...));
    FAIL(const int (A::*)() const);
    FAIL(const int (A::*)(int, float, ...) const);
}

inline void TypeTraitsTest2::testVolatile()
{
#undef CONDITION
#define CONDITION isVolatile

    PASS(const volatile int);
    PASS(int* const volatile);
    PASS(int (* const volatile)());
    PASS(int (* const volatile)(int, float, ...));
    PASS(int A::* const volatile);
    PASS(int (A::* const volatile)());
    PASS(int (A::* const volatile)(int, float, ...));
    PASS(const volatile int&);

    FAIL(void);
    FAIL(int);
    FAIL(A);
    FAIL(volatile int*);
    FAIL(volatile int A::*);
    FAIL(volatile int(*)());
    FAIL(volatile int (A::*)() volatile);
    FAIL(volatile int(*)(int, float, ...));
    FAIL(volatile int (A::*)(int, float, ...) volatile);
    FAIL(volatile int(*)(int, float));
    FAIL(volatile int (A::*)(int, float) volatile);
}

inline void TypeTraitsTest2::testReference()
{
#undef CONDITION
#define CONDITION isReference

    PASS(const volatile int&);
    PASS(void (*const volatile&)(int, float));
    PASS(void (&)(int, float));
    PASS(void (*const volatile&)(int, float, ...));
    PASS(void (&)());
    PASS(void (&)(int, float, ...));

    FAIL(int);
    FAIL(void);
}

inline void TypeTraitsTest2::testPointer()
{
#undef CONDITION
#define CONDITION isPointer

    PASS(int* const volatile);
    PASS(void (* const volatile)());
    PASS(void (* const volatile)(int, float));
    PASS(void (* const volatile)(int, float, ...));
    

    PASS(void (A::* const volatile)() const volatile);
    PASS(void (A::* const volatile)(int, float) const volatile);
    PASS(void (A::* const volatile)(int, float, ...) const volatile);
    PASS(int A::* const volatile);

    
    PASS(const volatile int** const volatile ** const * const volatile);
    PASS(int* const volatile&);

    FAIL(int);
    FAIL(int&);
    FAIL(void);
}

inline void TypeTraitsTest2::testMemberPointer()
{
#undef CONDITION
#define CONDITION isMemberPointer

    PASS(int A::* const volatile);
    PASS(const volatile int A::* const volatile);
    
    
    // Pointer to member functions
    PASS(int (A::* )() const);
    PASS(int (A::* const volatile)() const volatile);
    PASS(int (A::* const volatile)(int, float) const volatile);
    PASS(int (A::* const volatile&)(int, float) const volatile);
    PASS(int (A::* const volatile)(int, float, ...) const volatile);
    PASS(int (A::* const volatile&)(int, float, ...) const volatile);
    
    
    FAIL(int);
    FAIL(void);
    FAIL(void*);
    FAIL(void(*)());
}

inline void TypeTraitsTest2::testFunctionPointer()
{
#ifdef ENABLE_ADDITIONAL_TYPETRAITS
#undef CONDITION
#define CONDITION isFunctionPointer

 	PASS(void (*)());
    FAIL(void (A::* const volatile)());
    FAIL(void (A::* const volatile)() const);
    FAIL(void (A::* const volatile)() volatile);
    FAIL(void (A::* const volatile)() const volatile);
    FAIL(void (A::* const volatile&)() const volatile);
    PASS(void (* const volatile)());
    PASS(void (* const volatile&)());
    
    PASS(void (* const volatile&)(int, float));
    FAIL(void (A::* const volatile&)(int, float) const volatile);

    PASS(void (* const volatile&)(int, float, ...));
    FAIL(void (A::* const volatile&)(int, float, ...) const volatile);

    FAIL(void);
    FAIL(void*);
    FAIL(int A::*);
    
#endif
}

inline void TypeTraitsTest2::testMemberFunctionPointer()
{
#ifdef ENABLE_ADDITIONAL_TYPETRAITS
#undef CONDITION
#define CONDITION isMemberFunctionPointer

    PASS(void (A::* const volatile)());
    PASS(void (A::* const volatile)() const);
    PASS(void (A::* const volatile)() volatile);
    PASS(void (A::* const volatile)() const volatile);
    PASS(void (A::* const volatile&)() const volatile);
    FAIL(void (* const volatile)());
    FAIL(void (* const volatile&)());
    
    FAIL(void (* const volatile&)(int, float));
    PASS(void (A::* const volatile&)(int, float) const volatile);

    FAIL(void (* const volatile&)(int, float, ...));
    PASS(void (A::* const volatile&)(int, float, ...) const volatile);

    FAIL(void);
    FAIL(void*);
    FAIL(int A::*);
#endif
}

inline void TypeTraitsTest2::testFunction()
{
#ifdef ENABLE_ADDITIONAL_TYPETRAITS
#undef CONDITION
#define CONDITION isFunction

    PASS(void());
    PASS(void(int, float, ...));
    PASS(void(&)(int, float, ...));
    PASS(void(int, float));
    PASS(void(&)());
    PASS(void(&)(int, float));
    PASS(int(&)(int, float));
#endif
}

inline void TypeTraitsTest2::testParameterType()
{
#undef CONDITION
#define CONDITION ParameterType

    TEST_TYPE(int, int);
    TEST_TYPE(const volatile int, const volatile int);
    TEST_TYPE(const volatile int&, const volatile int&);
    TEST_TYPE(void, Loki::NullType);
    TEST_TYPE(void*, void*);
    TEST_TYPE(void* const volatile, void* const volatile);
    TEST_TYPE(void* const volatile&, void* const volatile&);
    TEST_TYPE(const volatile A, const volatile A&);
    TEST_TYPE(A&, A&);
    
    TEST_TYPE(A, const A&);  
}

inline void TypeTraitsTest2::testReferredType()
{
#undef CONDITION
#define CONDITION ReferredType

    TEST_TYPE(const volatile int&, const volatile int);
    TEST_TYPE(void* const volatile&, void* const volatile);
    TEST_TYPE(void (&)(), void());
    TEST_TYPE(void (&)(int, float, ...), void(int, float, ...));
    TEST_TYPE(int (*)(), int (*)());
    TEST_TYPE(int (*&)(int, float, ...), int (*)(int, float, ...));
    TEST_TYPE(int (A::*&)() const volatile, int (A::*)() const volatile);
    TEST_TYPE(int (A::*&)(int, float, ...) const volatile, int (A::*)(int, float, ...) const volatile);
}

inline void TypeTraitsTest2::testPointeeType()
{
#undef CONDITION
#define CONDITION PointeeType

    TEST_TYPE(void, Loki::NullType);
    TEST_TYPE(void*, void);
    TEST_TYPE(const volatile int*, const volatile int);
      
    TEST_TYPE(
        const volatile int* const volatile&, 
        const volatile int);
    
    TEST_TYPE(void(*)(int, float), void(int, float));
    TEST_TYPE(void(int, float), Loki::NullType);
    TEST_TYPE(void(*&)(int, float), void(int, float));
}

#endif
