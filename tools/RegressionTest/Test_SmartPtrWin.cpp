#include <windows.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "SmartPtr.h"

#if defined(TEST_LOKI_ORIG)
namespace Loki
{
    template
    <
        typename T,
        template <class> class OwnershipPolicy = RefCounted,
        class ConversionPolicy = DisallowConversion,
        template <class> class CheckingPolicy = AssertCheck,
        template <class> class StoragePolicy = DefaultSPStorage
    >
    struct SmartPtrDef
    {
        typedef SmartPtr
        <
            T,
            OwnershipPolicy,
            ConversionPolicy,
            CheckingPolicy,
            StoragePolicy
        >
        type;
    };
} // namespace Loki
#endif // defined(TEST_LOKI_ORIG)

using namespace ::Loki;

struct A 
{ 
    A()          { ++s_instCount; }
    A(const A &) { ++s_instCount; }
    virtual ~A() { --s_instCount; } 
    
    virtual const char *name() const = 0;

    static void AssertInstCount()
    {
        assert(s_instCount == 0);
    }

private:
    static int s_instCount;
};

int A::s_instCount = 0;

struct B : A 
{
    virtual const char *name() const
    {
        return "B";
    }
};

struct C : A 
{
    virtual const char *name() const
    {
        return "C";
    }
};

template<class SmartPtr_A>
struct PrintA
{
    void operator()(const SmartPtr_A &spA) const
    {
        std::cout << spA->name() << ", ";
    }
};

void Test_SmartPtr()
{
#define MyRefCountedMT RefCountedMTAdj<ObjectLevelLockable>::RefCountedMT
//#define MyRefCountedMT RefCountedMTAdj<ClassLevelLockable>::RefCountedMT

    typedef SmartPtrDef<A, MyRefCountedMT>::type SmartPtr_A;
    typedef SmartPtrDef<B, MyRefCountedMT>::type SmartPtr_B;
    typedef SmartPtrDef<C, MyRefCountedMT>::type SmartPtr_C;

#undef MyRefCountedMT

    SmartPtr_B     spB(new B);
    SmartPtr_C     spC(new C);

    SmartPtr_A     spA1(spB);
    SmartPtr_A     spA2(spC);

    std::cout << spA1->name() << std::endl;
    std::cout << spA2->name() << std::endl;

    spA1 = spA2;
    spA2 = spB;

    std::cout << spA1->name() << std::endl;
    
    if (spA2 == spA1)
    {
        std::cout << spA2->name() << std::endl;
    }

    Reset(spA2, new C);
    
    if (spA2)
    {
        std::cout << spA2->name() << std::endl;
    }

    std::vector<SmartPtr_A> vectorA;

    for (unsigned i = 0 ; i < 10 ; ++i)
    {
        if ((i % 3) == 0)
        {
            vectorA.push_back(new B);
        }
        else
        {
            vectorA.push_back(new C);
        }
    }

    for (unsigned i = 0 ; i < 10 ; ++i)
    {
        vectorA.push_back(vectorA[i]);
    }

    std::for_each(vectorA.begin(), vectorA.end(), PrintA<SmartPtr_A>());

    std::cout << std::endl;
}

int main()
{
    Test_SmartPtr();
    A::AssertInstCount();
}

