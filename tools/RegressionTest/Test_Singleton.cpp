#include <Loki\Singleton.h>

#include <iostream>
using std::cout;
using std::endl;

using namespace ::Loki;


namespace
{
	struct A { ~A(){} };
	struct B { ~B(){} };
	struct C { ~C(){} };

	unsigned int GetLongevity(C*)
	{
		return 1;
	}
}//ns anon

extern void Test_Singleton()
{
    cout << "Testing Loki\\Singleton" << endl;
    typedef SingletonHolder<A> SingletonHolder1_t;
    typedef SingletonHolder<B, CreateStatic> SingletonHolder2_t;
    typedef SingletonHolder<C, CreateUsingNew, SingletonWithLongevity> SingletonHolder3_t;

    A& p = SingletonHolder1_t::Instance();
    SingletonHolder2_t::Instance();
    SingletonHolder3_t::Instance();
    cout << endl << endl;
}

