#include <cstdio>
#include "Functor.h"

using namespace Loki;

class A
{
public:
    explicit A(int number)
        : number_(number)
    {}

    int Get() const { return number_; }

private:
    int number_;
};

int foo(int a1, A a2, const A &a3)
{
    return a1 + a2.Get() + a3.Get();
}

int
main()
{
    typedef Functor< int, TYPELIST_3(int, A, const A &) > func1_t;
     
    func1_t fun1(foo);
    func1_t fun2(fun1);

    A   num1(10);
    printf("%d\n", fun1(1, num1, A(100)));

#if defined(_MSC_VER) && _MSC_VER == 1300
    // will not compile with the original Loki although its fine
    printf("%d\n", BindFirst(fun2, 15)(A(24), num1)); 
#else
    A   num2(24);
    printf("%d\n", BindFirst(fun2, 15)(num2, num1)); // will not compile with the original loki
#endif
}


