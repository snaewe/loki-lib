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
    template<typename Head, typename Tail>
    static void Print(GenLinearHierarchy<Typelist<Head, Tail>, Unit, Root> &a, Order<0>)
    {
        printf("%s\n", typeid(a).name());

        Unit<Head, GenLinearHierarchy<Tail, Unit, Root> > &base1 = a;
        printf("%s\n", typeid(base1).name());
    
        GenLinearHierarchy<Tail, Unit, Root> &base2 = base1;
        Print(base2);
    }

    template<typename Head>
    static void Print(GenLinearHierarchy<Typelist<Head, NullType>, Unit, Root> &a, Order<1>)
    {
        printf("%s\n", typeid(a).name());

        Unit<Head, Root> &base1 = a;

        printf("%s\n", typeid(base1).name());
    
        Root &base2 = base1; (void)base2;

        printf("%s\n", typeid(base2).name());
    }

public:
    template<typename Head, typename Tail>
    static void Print(GenLinearHierarchy<Typelist<Head, Tail>, Unit, Root> &a)
    {
        Print(a, Order<1>());
    }
};

template<typename, class Base> 
class TestUnit : public Base {};

class TestRoot {};

template<int> struct A {};
template<int> struct B {};

int
main()
{
    typedef TYPELIST_20(A<1>, A<2>, A<3>, A<4>, A<5>, A<6>, A<7>, A<8>, A<9>, A<10>,
                        B<1>, B<2>, B<3>, B<4>, B<5>, B<6>, B<7>, B<8>, B<9>, B<10>) TestList_t;

    GenLinearHierarchy<TestList_t, TestUnit, TestRoot> TestGenLinearHierarchy;

    PrintGenLinearHierarchy<TestUnit, TestRoot>::Print(TestGenLinearHierarchy);
}


