#include <cstdio>
#include <typeinfo>
#include "Factory.h"
#include "AbstractFactory.h"

template<typename T>
void print_type()
{
    printf("%s\n", typeid(T).name());
}

using namespace ::Loki;

struct A1 {}; struct A2 {}; struct A3 {}; struct A4 {};
struct A5 {}; struct A6 {}; struct A7 {}; struct A8 {};

typedef TL::MakeTypeList<A1, A2, A3, A4, A5, A6, A7, A8>::Result typelist1_t;

struct Abst : AbstractFactory<typelist1_t> {};

int main()
{
    Factory<A1, int>     a;
    CloneFactory<A1>     b;

    typedef AbstractFactory<typelist1_t> abst_t;
    
    struct AA1 : ConcreteFactory<abst_t, OpNewFactoryUnit> {};
    AA1 a1;

    struct AA2 : ConcreteFactory<Abst, OpNewFactoryUnit> {};

    AA2 a2;
}

