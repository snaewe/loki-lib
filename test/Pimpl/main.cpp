////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header:

#ifdef _MSC_VER
#pragma warning (disable: 4512)
#endif


#define LOKI_INHERITED_PIMPL_NAME d
#define LOKI_INHERITED_RIMPL_NAME d

#include "type.h"

#include <loki/PimplDef.h>

#include <loki/SafeFormat.h>



/////////////////////////////////////////
// Definition of Impl<A>
/////////////////////////////////////////
namespace Loki // gcc!!
{
    template<>
    struct Impl<A> : public SmallObject<> // inherit SmallObj for speed up
    {
        Impl() : data(0) {Printf("A created\n");}
        ~Impl(){Printf("A destroyed, data=%d\n")(data);}
        int data;
    };
}
/////////////////////////////////////////
// class A definition
/////////////////////////////////////////
A::A()
{}

void A::foo()
{
    (*d).data = 1;
    d->data = 111;
}


/////////////////////////////////////////
// Definition  of Impl<B>
/////////////////////////////////////////
namespace Loki // gcc!!
{
    template<>
    struct Impl<B> : public SmallObject<> // inherit SmallObj for speed up
    {
        Impl() : data(0) {Printf("B created\n");}
        ~Impl(){Printf("B destroyed, data=%d\n")(data);}
        int data;
    };
}
/////////////////////////////////////////
// class B definition
/////////////////////////////////////////
B::B() : Loki::Pimpl<B>::Owner()
{}

void B::foo()
{
    (*d).data = 2;
    d->data = 222;
}


/////////////////////////////////////////
// Definition  of Impl<C>
/////////////////////////////////////////
namespace Loki // gcc!!
{
    template<>
    struct Impl<C> : public SmallObject<> // inherit SmallObj for speed up
    {
        Impl(): data(0) {Printf("C created\n");}
        ~Impl(){Printf("C destroyed, data=%d\n")(data);}
        int data;
    };
}
/////////////////////////////////////////
// class C definition
/////////////////////////////////////////
C::C() : d(rlife)
{}

void C::foo()
{
    d.data = 333;
}



/////////////////////////////////////////
// Definition  of Impl<D>
/////////////////////////////////////////
namespace Loki // gcc!!
{
    template<>
    struct Impl<D> : public SmallObject<> // inherit SmallObj for speed up
    {
        Impl(): data(0) {Printf("D created\n");}
        ~Impl(){Printf("D destroyed, data=%d\n")(data);}
        int data;
    };
}
/////////////////////////////////////////
// class D definition
/////////////////////////////////////////
D::D() : Loki::Rimpl<D>::Owner()
{}

void D::foo()
{
    d.data = 444;
}


/////////////////////////////////////////
// main
/////////////////////////////////////////
void test_more();

//#define MSVC_DETECT_MEMORY_LEAKS
#ifdef MSVC_DETECT_MEMORY_LEAKS 

#include <crtdbg.h>
#include <cassert>

void heap_debug()
{
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn on leak-checking bit
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

	// Turn off CRT block checking bit
	tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

	// Set flag to the new value
	_CrtSetDbgFlag( tmpFlag );

}

#else

void heap_debug()
{
}

#endif



int main()
{
	heap_debug();

    A* a = new A;
    B* b = new B;
    C* c = new C;
    D* d = new D;
    a->foo();
    b->foo();
    c->foo();
    d->foo();
    delete a;
    delete b;
    delete c;
    delete d;

    test_more();

#if defined(__BORLANDC__) || defined(_MSC_VER)
    system("PAUSE");
#endif

    return 0;
}


////////////////////
// more test code
////////////////////



P1::P1(){d->data = 1;}
P2::P2(){d->data = 2;}
P3::P3(){d->data = 3;}
P4::P4(){d->data = 4;}
P5::P5(){d->data = 5;}

R1::R1(){d.data = 11;}
R2::R2(){d.data = 22;}
R3::R3(){d.data = 33;}
R4::R4():d(t){d.data = 44;}
R5::R5():d(t){d.data = 55;}
R6::R6():d(t){d.data = 66;}

void test_more()
{
    Loki::Printf("\nmore tests:\n");

    P1* p1 =  new P1;
    P2* p2 =  new P2;
    P3* p3 =  new P3;
    P4* p4 =  new P4;
    P5* p5 =  new P5;

    R1* r1 =  new R1;
    R2* r2 =  new R2;
    R3* r3 =  new R3;
    R4* r4 =  new R4;
    R5* r5 =  new R5;
    R6* r6 =  new R6;
    

    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete p5;
    
    delete r1;
    delete r2;
    delete r3;
    delete r4;
    delete r5;
    delete r6;
}
