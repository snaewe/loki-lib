#include <cstdio>
#include <typeinfo>
#include "HierarchyGenerators.h"

using namespace ::Loki;

//
// Helper class for simulating (ranking) partial ordering
//
template<unsigned I>
struct Order : Order<I - 1> {};

template<>
struct Order<0> {};

template<template<typename> class Unit>
class PrintGenScatterHierarchy
{    
    //
    // The second parameter is not needed when using compilers that support partial ordering
    // VC7 doesn't support that feature - this is an example of simulating that feature in VC7
    //
    template<typename Head, typename Tail>
    static void Print(GenScatterHierarchy<Typelist<Head, Tail>, Unit> &a, Order<1>)
    {
        printf("%s\n", typeid(a).name());

        GenScatterHierarchy<Head, Unit> &left = a;
        Print(left);

        GenScatterHierarchy<Tail, Unit> &right = a;
        Print(right);
    }

    template<typename T>
    static void Print(GenScatterHierarchy<T, Unit> &a, Order<0>)
    {
        printf("%s\n", typeid(a).name());

        Unit<T> &left = a;
        (void)left;

        printf("%s\n", typeid(left).name());
    }

    static void Print(GenScatterHierarchy<NullType, Unit> &a, Order<1>)
    {
        (void)a;
        printf("%s\n", typeid(a).name());
    }

public:
    template<typename T>
    static void Print(GenScatterHierarchy<T, Unit> &a)
    {
        Print(a, Order<1>());
    }
    
};

template<typename> class TestUnit {};
template<int> struct A {};
template<int> struct B {};

int
main()
{
    typedef TYPELIST_20(A<1>, A<2>, A<3>, A<4>, A<5>, A<6>, A<7>, A<8>, A<9>, A<10>,
                        B<1>, B<2>, B<3>, B<4>, B<5>, B<6>, B<7>, B<8>, B<9>, B<10>) TestList_t;

    GenScatterHierarchy<TestList_t, TestUnit> TestGenScatterHierarchy;

    PrintGenScatterHierarchy<TestUnit>::Print(TestGenScatterHierarchy);
}


