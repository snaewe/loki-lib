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
#include "type2.h"

#include <loki/SafeFormat.h>
#include <loki/PimplDef.h>


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
C::C() : d(rinit)
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

/////////////////////////////////////////
// Definition  of Impl<E>
/////////////////////////////////////////
namespace Loki // gcc!!
{
    template<>
    struct Impl<E> : public SmallObject<> // inherit SmallObj for speed up
    {
        Impl() : data(0) {Printf("E created\n");}
        ~Impl(){Printf("E destroyed, data=%d\n")(data);}
        int data;

        void foo()       {Printf("E  foo() \n");}
        void foo() const {Printf("E  foo() const \n");}
    };
}



P1::P1(){d->data = 1;}
P2::P2(){d->data = 2;}
P3::P3(){d->data = 3;}
P4::P4(){d->data = 4;}
P5::P5(){d->data = 5;}
P6::P6(){d->data = 6;}

void P1::f(){d->foo();}
void P2::f(){d->foo();}
void P3::f(){d->foo();}
void P4::f(){d->foo();}
void P5::f(){d->foo();}
void P6::f(){d->foo();}

void P1::f()const{d->foo();}
void P2::f()const{d->foo();}
void P3::f()const{d->foo();}
void P4::f()const{d->foo();}
void P5::f()const{d->foo();}
void P6::f()const{d->foo();}


R1::R1(){d.data = 11;}
R2::R2(){d.data = 22;}
R3::R3(){d.data = 33;}
R4::R4():d(rinit){d.data = 44;}
R5::R5():d(rinit){d.data = 55;}
R6::R6():d(rinit){d.data = 66;}

void R1::f(){d.foo();}
void R2::f(){d.foo();}
void R3::f(){d.foo();}
void R4::f(){d.foo();}
void R5::f(){d.foo();}
void R6::f(){d.foo();}

void R1::f()const{d.foo();}
void R2::f()const{d.foo();}
void R3::f()const{d.foo();}
void R4::f()const{d.foo();}
void R5::f()const{d.foo();}
void R6::f()const{d.foo();}


void test_more()
{
    Loki::Printf("\n\nMore tests:\n");

    Loki::Printf("\nCreating Pimpls\n");
    P1* p1 =  new P1;
    P2* p2 =  new P2;
    P3* p3 =  new P3;
    P4* p4 =  new P4;
    P5* p5 =  new P5;
    P6* p6 =  new P6;

    Loki::Printf("\nConst check\n");
    p1->f();
    p2->f();
    p3->f();
    p4->f();
    p5->f();
    p6->f();

    Loki::Printf("\nDeleting Pimpls\n");
    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete p5;
    delete p6;
    
    Loki::Printf("\nCreating Rimpls\n");
    R1* r1 =  new R1;
    R2* r2 =  new R2;
    R3* r3 =  new R3;
    R4* r4 =  new R4;
    R5* r5 =  new R5;
    R6* r6 =  new R6;

    Loki::Printf("\nConst check\n");
    r1->f();
    r2->f();
    r3->f();
    r4->f();
    r5->f();
    r6->f();

    Loki::Printf("\nDeleting Rimpls\n");
    delete r1;
    delete r2;
    delete r3;
    delete r4;
    delete r5;
    delete r6;

    Loki::Printf("\nCreating Pimpls\n");
    const P1* cp1 =  new P1;
    const P2* cp2 =  new P2;
    const P3* cp3 =  new P3;
    const P4* cp4 =  new P4;
    const P5* cp5 =  new P5;
    const P6* cp6 =  new P6;

    Loki::Printf("\nConst check\n");
    cp1->f();
    cp2->f();
    cp3->f();
    cp4->f();
    cp5->f();
    cp6->f();

    Loki::Printf("\nDeleting Rimpls\n");
    delete cp1;
    delete cp2;
    delete cp3;
    delete cp4;
    delete cp5;
    delete cp6;


    Loki::Printf("\nCreating Rimpls\n");
    const R1* cr1 =  new R1;
    const R2* cr2 =  new R2;
    const R3* cr3 =  new R3;
    const R4* cr4 =  new R4;
    const R5* cr5 =  new R5;
    const R6* cr6 =  new R6;

    Loki::Printf("\nConst check\n");
    cr1->f();
    cr2->f();
    cr3->f();
    cr4->f();
    cr5->f();
    cr6->f();

    Loki::Printf("\nDeleting Rimpls\n");
    delete cr1;
    delete cr2;
    delete cr3;
    delete cr4;
    delete cr5;
    delete cr6;

}
