////////////////////////////////////////////////////////////////////////////////
// Test program for The Loki Library
// Copyright (c) 2008 Richard Sposato
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The authors make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$


// ----------------------------------------------------------------------------


// loki_smart_auto_ptr.cpp Test program for Loki::SmartPtr configured as
// auto_ptr using Colvin-Gibbons trick

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <loki/SmartPtr.h>

#include <memory>
#include <iostream>
#include <cstdlib>


using namespace Loki;
using namespace std;


// ----------------------------------------------------------------------------

class Base
{
public:
    virtual ~Base() {}

    virtual void Hello() { cout << "Base Hello, world!" << endl; }
};

// ----------------------------------------------------------------------------

class Derived: public Base
{
public:
    virtual void Hello() { cout << "Derived Hello, world!" << endl; }
};


// ----------------------------------------------------------------------------

typedef auto_ptr< Base > AutoPtrBase;
typedef auto_ptr< Derived > AutoPtrDerived;

typedef SmartPtr< Base, DestructiveCopy > SmartAutoPtrBase;
typedef SmartPtr< Derived, DestructiveCopy > SmartAutoPtrDerived;


// ----------------------------------------------------------------------------

template <class TPtrBase>
TPtrBase CreateBase()
{
    return TPtrBase( new Base );
}

// ----------------------------------------------------------------------------

template <class TPtrDerived>
TPtrDerived CreateDerived()
{
    return TPtrDerived( new Derived );
}

// ----------------------------------------------------------------------------

template < class TPtrBase >
void UseBase( TPtrBase ptr )
{
    ptr->Hello();
}

// ----------------------------------------------------------------------------

template < class TPtrBase, class TPtrDerived >
void DoTest()
{
    TPtrBase spb0 = CreateBase< TPtrBase >();
    TPtrBase spb1( spb0 );

    UseBase< TPtrBase >( CreateBase< TPtrBase >() );

    TPtrDerived spd1 = CreateDerived< TPtrDerived >();
    TPtrBase spb2( spd1 );

    // NOTE: this does not compile with gcc 4.3.1 also for std::auto_ptr;
    // it should work (Copy-initialization, base-from-derived)
    // according to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/1997/N1128.pdf
    // It does compile with Visual C++ 8 (need to test with 9)
    // UseBase<TPtrBase>(CreateDerived<TPtrDerived>());

    // Quick fix
    UseBase< TPtrBase >( TPtrBase( CreateDerived< TPtrDerived >() ) );
}

// ----------------------------------------------------------------------------

void TryColvinGibbonsTrick( void )
{
    DoTest<AutoPtrBase, AutoPtrDerived>(); // OK
    DoTest<SmartAutoPtrBase, SmartAutoPtrDerived>(); // FIXME: eror:
    // assignment of read-only reference 'val'
}

// ----------------------------------------------------------------------------

