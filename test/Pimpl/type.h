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

//#define LOKI_DEFAULT_CONSTNESS DeepConstness //default
//#define LOKI_DEFAULT_CONSTNESS FlatConstness

#include <loki/Pimpl.h>


//#define TEST_WITH_BOOST
#ifdef TEST_WITH_BOOST
#include <boost/shared_ptr.hpp>
#endif
#include <loki/SmartPtr.h>
#include <loki/SmallObj.h>

using Loki::Pimpl;
using Loki::Rimpl;


/////////////////////////////////////////
// class A declaration
/////////////////////////////////////////

class A
{
public:
    A();
    void foo();

private:
    Pimpl<A>::Type d;
};


/////////////////////////////////////////
// class B declaration
/////////////////////////////////////////

class B : private Pimpl<B>::Owner
{
public:
    B();
    void foo();
};


/////////////////////////////////////////
// class C declaration
/////////////////////////////////////////

class C
{
public:
    C();
    void foo();

private:
    Rimpl<C>::Init rinit; 
    Rimpl<C>::Type d; 
};


/////////////////////////////////////////
// class D declaration
/////////////////////////////////////////

class D : private Rimpl<D>::Owner
{
public:
    D();
    void foo();
};




////////////////////
// more test code
////////////////////

struct E;

// Pimpl

typedef Loki::PtrImpl
<
    Loki::Impl<E>
>
Pimpl1;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::SmartPtr<Loki::Impl<E> >,
    Loki::DontDeletePimpl
>
Pimpl2;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
#ifdef TEST_WITH_BOOST
    boost::shared_ptr<Loki::Impl<E> >,
#else
    Loki::SmartPtr<Loki::Impl<E> >,
#endif
    Loki::DontDeletePimpl
>
Pimpl3;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::SmartPtr<Loki::Impl<E> >,
    Loki::DontDeletePimpl,
    Loki::DeclaredPimpl
>
Pimpl4;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
#ifdef TEST_WITH_BOOST
    boost::shared_ptr<Loki::Impl<E> >,
#else
    Loki::SmartPtr<Loki::Impl<E> >,
#endif
    Loki::DontDeletePimpl,
    Loki::DeclaredPimpl
>
Pimpl5;

typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::Impl<E>*,
    Loki::AutoDeletePimpl,
    Loki::DeclaredPimpl
>
Pimpl6;


// Rimpl

typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::Impl<E>*,
    Loki::AutoDeletePimpl,
    Loki::InheritedRimpl

>
Rimpl1;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::SmartPtr<Loki::Impl<E> >,
    Loki::DontDeletePimpl,
    Loki::InheritedRimpl
>
Rimpl2;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
#ifdef TEST_WITH_BOOST
    boost::shared_ptr<Loki::Impl<E> >,
#else
    Loki::SmartPtr<Loki::Impl<E> >,
#endif
    Loki::DontDeletePimpl,
    Loki::InheritedRimpl
>
Rimpl3;


typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::Impl<E>*,
    Loki::AutoDeletePimpl,
    Loki::DeclaredRimpl
>
Rimpl4;

typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    Loki::SmartPtr<Loki::Impl<E> >,
    Loki::DontDeletePimpl,
    Loki::DeclaredRimpl
>
Rimpl5;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
#ifdef TEST_WITH_BOOST
    boost::shared_ptr<Loki::Impl<E> >,
#else
    Loki::SmartPtr<Loki::Impl<E> >,
#endif
    Loki::DontDeletePimpl,
    Loki::DeclaredRimpl
>
Rimpl6;



typedef Loki::PtrImpl
<
    Loki::Impl<E>,
    
    //Loki::Impl<E>*,
    Loki::SmartPtr<Loki::Impl<E> >,
    //boost::shared_ptr<Loki::Impl<E> >,
    
    //Loki::AutoDeletePimpl,     // compiler error when used with smart pointers, this is o.k.
    Loki::DontDeletePimpl,

    Loki::DeclaredPimpl
    //Loki::InheritedPimpl,
> 
Pimpl8;


template<class T>
struct R
{
    typedef Loki::Private::AutoPtrHolderChecked
    <
        T,
        T*,
        Loki::AutoDeletePimpl
    >
    Init;
};


struct P1 : private Pimpl1 {P1();void f();void f()const;};
struct P2 : private Pimpl2 {P2();void f();void f()const;};
struct P3 : private Pimpl3 {P3();void f();void f()const;};
struct P4 {Pimpl4 d;        P4();void f();void f()const;};
struct P5 {Pimpl5 d;        P5();void f();void f()const;};
struct P6 {Pimpl6 d;        P6();void f();void f()const;};

struct R1 : private Rimpl1 {R1();void f();void f()const;};
struct R2 : private Rimpl2 {R2();void f();void f()const;};
struct R3 : private Rimpl3 {R3();void f();void f()const;};
struct R4 {R<Rimpl4>::Init rinit; Rimpl4& d; R4();void f();void f()const;};
struct R5 {R<Rimpl5>::Init rinit; Rimpl5& d; R5();void f();void f()const;};
struct R6 {R<Rimpl6>::Init rinit; Rimpl6& d; R6();void f();void f()const;};


