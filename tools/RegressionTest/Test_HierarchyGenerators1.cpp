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
    template<typename Head, typename Tail>
    PrintGenScatterHierarchy(GenScatterHierarchy<Typelist<Head, Tail>, Unit> *a, Order<1>)
    {
        printf("%s\n", typeid(*a).name());

        GenScatterHierarchy<Head, Unit> *left = a;
        PrintGenScatterHierarchy<Unit> X1(left);
    
        GenScatterHierarchy<Tail, Unit> *right = a;
        PrintGenScatterHierarchy<Unit> X2(right);
    }

    template<typename T>
    PrintGenScatterHierarchy(GenScatterHierarchy<T, Unit> *a, Order<0>)
    {
        printf("%s\n", typeid(*a).name());

        Unit<T> *left = a;
        (void)left;

        printf("%s\n", typeid(*left).name());
    }

    PrintGenScatterHierarchy(GenScatterHierarchy<NullType, Unit> *a, Order<1>)
    {
        (void)a;
        printf("%s\n", typeid(*a).name());
    }

public:
    template<typename T>
    explicit PrintGenScatterHierarchy(GenScatterHierarchy<T, Unit> *a)
    {
        PrintGenScatterHierarchy<Unit>(a, Order<1>());                
    }
};

template<typename> class TestUnit {};
template<int> struct A {};

int
main()
{
    typedef TYPELIST_10(A<1>, A<2>, A<3>, A<4>, A<5>, A<6>, A<7>, A<8>, A<9>, A<10>) TestList_t;

    GenScatterHierarchy<TestList_t, TestUnit> TestGenScatterHierarchy;

    PrintGenScatterHierarchy<TestUnit> X(&TestGenScatterHierarchy);
}


