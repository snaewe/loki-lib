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

template<template<typename, typename> class Unit, class Root>
class PrintGenLinearHierarchy
{
    template<typename Head>
    PrintGenLinearHierarchy(GenLinearHierarchy<Typelist<Head, NullType>, Unit, Root> *a, Order<1>)
    {
        printf("%s\n", typeid(*a).name());

        Unit<Head, Root> *base1 = a;

        printf("%s\n", typeid(*base1).name());
    
        Root *base2 = base1; (void)base2;

        printf("%s\n", typeid(*base2).name());
    }

    template<typename Head, typename Tail>
    PrintGenLinearHierarchy(GenLinearHierarchy<Typelist<Head, Tail>, Unit, Root> *a, Order<0>)
    {
        printf("%s\n", typeid(*a).name());

        Unit<Head, GenLinearHierarchy<Tail, Unit, Root> > *base1 = a;
        printf("%s\n", typeid(*base1).name());
    
        GenLinearHierarchy<Tail, Unit, Root> *base2 = base1;
        PrintGenLinearHierarchy<Unit, Root> X(base2);
    }

public:
    template<typename Head, typename Tail>
    explicit PrintGenLinearHierarchy(GenLinearHierarchy<Typelist<Head, Tail>, Unit, Root> *a)
    {
        PrintGenLinearHierarchy<Unit, Root>(a, Order<1>());
    }
};

template<typename, class Base> 
class TestUnit : public Base {};

class TestRoot {};

template<int> struct A {};

int
main()
{
    typedef TYPELIST_10(A<1>, A<2>, A<3>, A<4>, A<5>, A<6>, A<7>, A<8>, A<9>, A<10>) TestList_t;

    GenLinearHierarchy<TestList_t, TestUnit, TestRoot> TestGenLinearHierarchy;

    PrintGenLinearHierarchy<TestUnit, TestRoot> X(&TestGenLinearHierarchy);
}


