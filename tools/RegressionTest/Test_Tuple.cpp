#include <cstdio>
#include <typeinfo>
#include "HierarchyGenerators.h"

using namespace Loki;

template<typename T>
void PrintType(T &)
{
    printf("%s\n", typeid(T).name());
}

template<unsigned I>
class PrintTuple
{
public:

    template<typename T>
    explicit PrintTuple(const T &x)
    {
        PrintTuple<I - 1> X(x);
        PrintType(Field<I>(x));
    }
};

template<>
class PrintTuple<0>
{
public:

    template<typename T>
    explicit PrintTuple(const T &x)
    {
        PrintType(Field<0>(x));
    }
};


template<typename T>
void PrintTupleFunc()
{
    T   x;

    PrintTuple<TL::Length<typename T::TList>::value - 1> X(x);
}

template<int> struct A {};

int
main()
{
    typedef TYPELIST_11(A<1>, A<1>, int, int, A<1>, A<2>, int[10], int, int *, A<2>, A<3>) typelist_t; 
    typedef Tuple<typelist_t> tuple_t;
    
    PrintTupleFunc<tuple_t>();

    tuple_t X;
    A<3> Y = Field< A<3> >(X);
    
    // without -Za warning C4239: nonstandard extension used
    // this is because VC sometimes binds temporary to non-const reference
    // which leads to selecting the wrong Field function (non-const version)
    // fixed in VC7.1
    Y = Field< A<3> >(tuple_t());

    A<3> &Y1 = Field< A<3> >(X);
    Y1 = Y;
}


