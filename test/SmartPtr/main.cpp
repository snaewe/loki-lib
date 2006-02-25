////////////////////////////////////////////////////////////////////////////////
// Test program for The Loki Library
// Copyright (c) 2006 Richard Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The authors make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$

// ----------------------------------------------------------------------------

#include <loki/SmartPtr.h>

#include <iostream>


// ----------------------------------------------------------------------------

using namespace std;
using namespace Loki;


// ----------------------------------------------------------------------------

class BaseClass
{
public:
    BaseClass( void )
    {
        s_constructions++;
    }

    virtual ~BaseClass( void )
    {
        s_destructions++;
    }

    // These 2 functions are so we can pretend we have a COM object.
    void AddRef( void ) {}
    void Release( void ) {}

    // This function is used only for the DeepCopy policy.
    virtual BaseClass * Clone( void ) const
    {
        return new BaseClass();
    }

    static bool AllDestroyed( void )
    {
        return ( s_constructions == s_destructions );
    }

    static bool ExtraConstructions( void )
    {
        return ( s_constructions > s_destructions );
    }

    static bool ExtraDestructions( void )
    {
        return ( s_constructions < s_destructions );
    }

private:
    /// Not implemented.
    BaseClass( const BaseClass & );
    /// Not implemented.
    BaseClass & operator = ( const BaseClass & );

    static unsigned int s_constructions;
    static unsigned int s_destructions;
};

unsigned int BaseClass::s_constructions = 0;
unsigned int BaseClass::s_destructions = 0;


// ----------------------------------------------------------------------------

class PublicSubClass : public BaseClass
{
public:
    // This function is used only for the DeepCopy policy.
    virtual BaseClass * Clone( void ) const
    {
        return new BaseClass();
    }
};

// ----------------------------------------------------------------------------

class PrivateSubClass : private BaseClass
{
public:
    // This function is used only for the DeepCopy policy.
    virtual BaseClass * Clone( void ) const
    {
        return new BaseClass();
    }
};

// ----------------------------------------------------------------------------

/// @note Used for testing most policies.
typedef Loki::SmartPtr< BaseClass, RefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr;

/// @note These 3 are used for testing const policies.
typedef Loki::SmartPtr< const BaseClass, RefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    ConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr;

typedef Loki::SmartPtr< const BaseClass, RefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, PropagateConst >
    ConstBase_RefCount_NoConvert_Assert_Propagate_ptr;

typedef Loki::SmartPtr< BaseClass, RefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, PropagateConst >
    NonConstBase_RefCount_NoConvert_Assert_Propagate_ptr;


/// @note These 5 are used for testing ownership policies.
typedef Loki::SmartPtr< BaseClass, COMRefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr;

typedef Loki::SmartPtr< BaseClass, RefLinked, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr;

typedef Loki::SmartPtr< BaseClass, DeepCopy, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr;

typedef Loki::SmartPtr< BaseClass, DestructiveCopy, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr;

typedef Loki::SmartPtr< BaseClass, NoCopy, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr;

/// @note These 2 are used for testing inheritance.
typedef Loki::SmartPtr< PublicSubClass, RefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    PublicSub_RefCount_NoConvert_Assert_DontPropagate_ptr;

typedef Loki::SmartPtr< PrivateSubClass, RefCounted, DisallowConversion,
    AssertCheck, DefaultSPStorage, DontPropagateConst >
    PrivateSub_RefCount_NoConvert_Assert_DontPropagate_ptr;


// ----------------------------------------------------------------------------

void DoConstConversionTests( void )
{

    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p1;
    ConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p2( p1 );
    ConstBase_RefCount_NoConvert_Assert_Propagate_ptr p3( p1 );
    NonConstBase_RefCount_NoConvert_Assert_Propagate_ptr p4( p1 );

//  p1 = p2;  // illegal! converts const to non-const.
//  p1 = p3;  // illegal! converts const to non-const.
    p1 = p4;  // legal, but dubious. Changes const-propagation policy.
    p2 = p1;  // legal.  natural const conversion.
    p2 = p3;  // legal, but dubious. Changes const-propagation policy.
    p2 = p4;  // legal, but dubious. Changes const-propagation policy.
    p3 = p1;  // legal, but dubious. Changes const-propagation policy.
    p3 = p2;  // legal, but dubious. Changes const-propagation policy.
    p3 = p4;  // legal.  natural const conversion.
    p4 = p1;  // legal, but dubious. Changes const-propagation policy.
//  p4 = p2;  // illegal! converts const to non-const.
//  p4 = p3;  // illegal! converts const to non-const.
}

// ----------------------------------------------------------------------------

void DoOwnershipConversionTests( void )
{
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p1;
    NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p2;
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p3;
    NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p4( new BaseClass );
    NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p5;
    NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p6;

    // legal constructions.  Each should allow copy with same policies.
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p7( p1 );
    NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p8( p2 );
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p9( p3 );
    NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p10( p4 );
    NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p11( p5 );

    // illegal construction!  Can't copy anything with NoCopy policy.
    // NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p12( p6 );

    // illegal constructions!  Can't convert from one ownership policy to another.
//  NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p13( p2 );
//  NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p14( p3 );
//  NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p15( p4 );
//  NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p16( p5 );
//  NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p17( p6 );

    // illegal constructions!  Can't convert from one ownership policy to another.
//  NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p18( p1 );
//  NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p19( p3 );
//  NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p20( p4 );
//  NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p21( p5 );
//  NonConstBase_ComRef_NoConvert_Assert_DontPropagate_ptr p22( p6 );

    // illegal constructions!  Can't convert from one ownership policy to another.
//  NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p23( p1 );
//  NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p24( p2 );
//  NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p25( p4 );
//  NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p26( p5 );
//  NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p27( p6 );

    // illegal constructions!  Can't convert from one ownership policy to another.
//  NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p28( p1 );
//  NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p29( p2 );
//  NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p30( p3 );
//  NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p31( p5 );
//  NonConstBase_DeepCopy_NoConvert_Assert_DontPropagate_ptr p32( p6 );

    // illegal constructions!  Can't convert from one ownership policy to another.
//  NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p33( p1 );
//  NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p34( p2 );
//  NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p35( p3 );
//  NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p36( p4 );
//  NonConstBase_KillCopy_NoConvert_Assert_DontPropagate_ptr p37( p6 );

    // illegal constructions!  Can't convert from one ownership policy to another.
//  NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p38( p1 );
//  NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p39( p2 );
//  NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p40( p3 );
//  NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p41( p4 );
//  NonConstBase_NoCopy_NoConvert_Assert_DontPropagate_ptr p42( p5 );

    // illegal assignements!  Can't convert from one ownership policy to another.
//  p1 = p2;
//  p1 = p3;
//  p1 = p4;
//  p1 = p5;
//  p1 = p6;
//  p2 = p1;
//  p2 = p3;
//  p2 = p4;
//  p2 = p5;
//  p2 = p6;
//  p3 = p1;
//  p3 = p2;
//  p3 = p4;
//  p3 = p5;
//  p3 = p6;
//  p4 = p1;
//  p4 = p2;
//  p4 = p3;
//  p4 = p5;
//  p4 = p6;
//  p5 = p1;
//  p5 = p2;
//  p5 = p3;
//  p5 = p4;
//  p5 = p6;
//  p6 = p1;
//  p6 = p2;
//  p6 = p3;
//  p6 = p4;
//  p6 = p5;
}

// ----------------------------------------------------------------------------

void DoInheritanceConversionTests( void )
{
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p1;
    PublicSub_RefCount_NoConvert_Assert_DontPropagate_ptr p2;
    PrivateSub_RefCount_NoConvert_Assert_DontPropagate_ptr p3;

    p1 = p2;  // legal.  Cast to public base class allowed.
    assert( p1 == p2 );
//  p1 = p3;  // illegal!  Can't assign pointer since base class is private.
//  p2 = p1;  // illegal!  Can't do cast to derived class in pointer assignment.
//  p2 = p3;  // illegal!  Can't assign when types are unrelated.
//  p3 = p1;  // illegal!  Can't do cast to derived class in pointer assignment.
//  p3 = p2;  // illegal!  Can't assign when types are unrelated.

    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p4( p2 );
    assert( p4 == p1 );
    assert( p4 == p2 );
    // These copy-constructions are illegal for reasons shown above.
//  NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p5( p3 );
//  PublicSub_RefCount_NoConvert_Assert_DontPropagate_ptr p6( p1 );
//  PublicSub_RefCount_NoConvert_Assert_DontPropagate_ptr p7( p3 );
//  PrivateSub_RefCount_NoConvert_Assert_DontPropagate_ptr p8( p1 );
//  PrivateSub_RefCount_NoConvert_Assert_DontPropagate_ptr p9( p2 );
}

// ----------------------------------------------------------------------------

void DoRefCountSwapTests( void )
{
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p1( new BaseClass );
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p2( new BaseClass );

    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p3( p1 );
    NonConstBase_RefCount_NoConvert_Assert_DontPropagate_ptr p4( p2 );

    // p1 == p3    and    p2 == p4
    assert( p1 == p3 );
    assert( p1 != p2 );
    assert( p1 != p4 );
    assert( p2 == p4 );
    assert( p2 != p3 );
    assert( p2 != p1 );

    // p1 == p4    and    p2 == p3
    p3.Swap( p4 );
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );

    // p1 == p3    and    p2 == p4
    p3.Swap( p4 );
    assert( p1 == p3 );
    assert( p1 != p2 );
    assert( p1 != p4 );
    assert( p2 == p4 );
    assert( p2 != p3 );
    assert( p2 != p1 );

    // p2 == p3    and    p1 == p4
    p1.Swap( p2 );
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );

    // p2 == p3    and    p1 == p4
    p1.Swap( p1 );
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );

    // p2 == p3    and    p4 == p1
    p1.Swap( p4 );
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
}

// ----------------------------------------------------------------------------

void DoRefLinkSwapTests( void )
{
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p1( new BaseClass );
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p2( new BaseClass );

    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p3( p1 );
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p4( p2 );

    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p5;
    NonConstBase_RefLink_NoConvert_Assert_DontPropagate_ptr p6( new BaseClass );

    // p1 <---> p3    and    p2 <---> p4   and   p5   and   p6
    assert( p1 == p3 );
    assert( p1 != p2 );
    assert( p1 != p4 );
    assert( p2 == p4 );
    assert( p2 != p3 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p3.Swap( p4 );  // p1 <---> p4    and    p2 <---> p3   and   p5   and   p6
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p3.Swap( p4 );   // p1 <---> p3    and    p2 <---> p4   and   p5   and   p6
    assert( p1 == p3 );
    assert( p1 != p2 );
    assert( p1 != p4 );
    assert( p2 == p4 );
    assert( p2 != p3 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p1.Swap( p2 );  // p2 <---> p3    and    p1 <---> p4   and   p5   and   p6
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p1.Swap( p1 );  // p2 <---> p3    and    p1 <---> p4   and   p5   and   p6
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p1.Swap( p4 );  // p2 <---> p3    and    p4 <---> p1   and   p5   and   p6
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p4.Swap( p1 );  // p2 <---> p3    and    p4 <---> p1   and   p5   and   p6
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p5.Swap( p5 );  // p2 <---> p3    and    p4 <---> p1   and   p5   and   p6
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );

    p5.Swap( p1 );  // p2 <---> p3    and    p4 <---> p5   and   p1   and   p6
    assert( p5 == p4 );
    assert( p5 != p2 );
    assert( p5 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p5 );
    assert( p1 != p5 );
    assert( p2 != p1 );
    assert( p3 != p1 );
    assert( p4 != p1 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p6.Swap( p1 );  // p2 <---> p3    and    p4 <---> p5   and   p1   and   p6
    assert( p5 == p4 );
    assert( p5 != p2 );
    assert( p5 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p5 );
    assert( p1 != p5 );
    assert( p2 != p1 );
    assert( p3 != p1 );
    assert( p4 != p1 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );
    assert( p6 == p6 );

    p5.Swap( p1 );  // p2 <---> p3    and    p4 <---> p1   and   p5   and   p6
    assert( p1 == p4 );
    assert( p1 != p2 );
    assert( p1 != p3 );
    assert( p2 == p3 );
    assert( p2 != p4 );
    assert( p2 != p1 );
    assert( p1 != p5 );
    assert( p2 != p5 );
    assert( p3 != p5 );
    assert( p4 != p5 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );

    p6 = p2;  // p6 <---> p2 <---> p3    and    p4 <---> p1   and   p5
    assert( p6 == p2 );
    assert( p6 == p3 );
    assert( p2 == p3 );
    assert( p1 == p4 );
    assert( p5 != p3 );
    assert( p2 != p4 );
    assert( p2 != p5 );
    assert( p1 != p5 );
    assert( p2 != p1 );
    assert( p3 != p1 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );
    assert( p6 == p6 );

    p5 = p3;
    assert( p6 == p5 );
    assert( p6 == p2 );
    assert( p6 == p3 );
    assert( p5 == p3 );
    assert( p2 == p3 );
    assert( p1 == p4 );
    assert( p2 != p4 );
    assert( p1 != p5 );
    assert( p2 != p1 );
    assert( p3 != p1 );
    assert( p1 == p1 );
    assert( p2 == p2 );
    assert( p3 == p3 );
    assert( p4 == p4 );
    assert( p5 == p5 );
    assert( p6 == p6 );
}

// ----------------------------------------------------------------------------

int main( unsigned int , const char * [] )
{

    DoRefCountSwapTests();
    DoRefLinkSwapTests();
    DoConstConversionTests();
    DoOwnershipConversionTests();
    DoInheritanceConversionTests();

    // Check that nothing was leaked.
    assert( BaseClass::AllDestroyed() );
    assert( !BaseClass::ExtraConstructions() );

    // Check that no destructor called too often.
    assert( !BaseClass::ExtraDestructions() );

    return 0;
}

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.1  2006/02/25 01:53:20  rich_sposato
// Added test project for Loki::SmartPtr class.
//
