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

// $Id$


// ----------------------------------------------------------------------------

#ifndef LOKI_CLASS_LEVEL_THREADING
#define LOKI_CLASS_LEVEL_THREADING
#endif

#include <loki/StrongPtr.h>

#include <iostream>
#include <cassert>
#include <cstring>

#include "base.h"


// ----------------------------------------------------------------------------

#if !defined( NULL )
    #define nullptr 0
#endif

#if !defined( nullptr )
    #define nullptr NULL
#endif

using namespace std;
using namespace Loki;


// ----------------------------------------------------------------------------

/// Used to check if SmartPtr can be used with a forward-reference.
class Thingy;

typedef Loki::StrongPtr< Thingy, true, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    Thingy_DeleteSingle_ptr;

typedef Loki::StrongPtr< Thingy, true, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteUsingFree, DontPropagateConst >
    Thingy_DeleteUsingFree_ptr;

typedef Loki::StrongPtr< Thingy, true, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteNothing, DontPropagateConst >
    Thingy_DeleteNothing_ptr;


// ----------------------------------------------------------------------------

class Counted
{
public:

    Counted( void ) : m_size( 0 )
    {
        s_constructions++;
    }

    ~Counted( void )
    {
        s_destructions++;
    }

    static inline bool AllDestroyed( void )
    {
        return ( s_constructions == s_destructions );
    }

    static inline bool ExtraConstructions( void )
    {
        return ( s_constructions > s_destructions );
    }

    static inline bool ExtraDestructions( void )
    {
        return ( s_constructions < s_destructions );
    }

    static inline unsigned int GetCtorCount( void )
    {
        return s_constructions;
    }

    static inline unsigned int GetDtorCount( void )
    {
        return s_destructions;
    }

private:
    /// Not implemented.
    Counted( const Counted & );
    /// Not implemented.
    Counted & operator = ( const Counted & );

    static unsigned int s_constructions;
    static unsigned int s_destructions;

    int m_size;
};

unsigned int Counted::s_constructions = 0;
unsigned int Counted::s_destructions  = 0;

typedef ::Loki::StrongPtr< Counted, false > Counted_WeakPtr;
typedef ::Loki::StrongPtr< Counted, true  > Counted_StrongPtr;

typedef ::Loki::StrongPtr< Counted, false, ::Loki::TwoRefLinks > Linked_WeakPtr;
typedef ::Loki::StrongPtr< Counted, true,  ::Loki::TwoRefLinks > Linked_StrongPtr;

typedef ::Loki::StrongPtr< Counted, false, ::Loki::LockableTwoRefCounts >
    Lockable_WeakPtr;
typedef ::Loki::StrongPtr< Counted, true,  ::Loki::LockableTwoRefCounts >
    Lockable_StrongPtr;


// ----------------------------------------------------------------------------

class Earth;
class Moon;

typedef Loki::StrongPtr< Earth, false, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    Earth_WeakPtr;

typedef Loki::StrongPtr< Earth, true, TwoRefCounts, DisallowConversion,
    AssertCheck, AllowReset, DeleteSingle, DontPropagateConst >
    Earth_StrongPtr;

typedef Loki::StrongPtr< Moon, false, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    Moon_WeakPtr;

typedef Loki::StrongPtr< Moon, true, TwoRefCounts, DisallowConversion,
    AssertCheck, AllowReset, DeleteSingle, DontPropagateConst >
    Moon_StrongPtr;


// ----------------------------------------------------------------------------

class Earth
{
public:

    Earth( void ) : m_moon()
    {
        s_constructions++;
    }

    ~Earth( void )
    {
        s_destructions++;
    }

    void SetMoon( const Moon_StrongPtr & p );

    void SetMoon( const Moon_WeakPtr & p );

    static inline bool AllDestroyed( void )
    {
        return ( s_constructions == s_destructions );
    }

    static inline bool ExtraConstructions( void )
    {
        return ( s_constructions > s_destructions );
    }

    static inline bool ExtraDestructions( void )
    {
        return ( s_constructions < s_destructions );
    }

    static inline unsigned int GetCtorCount( void )
    {
        return s_constructions;
    }

    static inline unsigned int GetDtorCount( void )
    {
        return s_destructions;
    }

private:
    /// Not implemented.
    Earth( const Earth & );
    /// Not implemented.
    Earth & operator = ( const Earth & );

    static unsigned int s_constructions;
    static unsigned int s_destructions;

    Moon_WeakPtr m_moon;
};

unsigned int Earth::s_constructions = 0;
unsigned int Earth::s_destructions = 0;

// ----------------------------------------------------------------------------

class Moon
{
public:

    Moon( void ) : m_earth()
    {
        s_constructions++;
    }

    ~Moon( void )
    {
        s_destructions++;
    }

    void SetEarth( const Earth_WeakPtr & p );

    void SetEarth( const Earth_StrongPtr & p );

    static inline bool AllDestroyed( void )
    {
        return ( s_constructions == s_destructions );
    }

    static inline bool ExtraConstructions( void )
    {
        return ( s_constructions > s_destructions );
    }

    static inline bool ExtraDestructions( void )
    {
        return ( s_constructions < s_destructions );
    }

    static inline unsigned int GetCtorCount( void )
    {
        return s_constructions;
    }

    static inline unsigned int GetDtorCount( void )
    {
        return s_destructions;
    }

private:
    /// Not implemented.
    Moon( const Moon & );
    /// Not implemented.
    Moon & operator = ( const Moon & );

    static unsigned int s_constructions;
    static unsigned int s_destructions;
    Earth_WeakPtr m_earth;
};

unsigned int Moon::s_constructions = 0;
unsigned int Moon::s_destructions = 0;

// ----------------------------------------------------------------------------

void Moon::SetEarth( const Earth_WeakPtr & p )
{
    m_earth = p;
}

// ----------------------------------------------------------------------------

void Moon::SetEarth( const Earth_StrongPtr & p )
{
    m_earth = p;
}

// ----------------------------------------------------------------------------

void Earth::SetMoon( const Moon_WeakPtr & p )
{
    m_moon = p;
}

// ----------------------------------------------------------------------------

void Earth::SetMoon( const Moon_StrongPtr & p )
{
    m_moon = p;
}

// ----------------------------------------------------------------------------

typedef Loki::StrongPtr< BaseClass, false, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr;

typedef Loki::StrongPtr< BaseClass, true, TwoRefCounts, DisallowConversion,
    NoCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr;

typedef Loki::StrongPtr< BaseClass, false, TwoRefLinks, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    NonConstBase_WeakLink_NoConvert_Assert_NoPropagate_ptr;

typedef Loki::StrongPtr< BaseClass, true, TwoRefLinks, DisallowConversion,
    NoCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst >
    NonConstBase_StrongLink_NoConvert_NoCheck_NoPropagate_ptr;

typedef Loki::StrongPtr< BaseClass, false, TwoRefCounts, DisallowConversion,
    AssertCheck, AllowReset, DeleteSingle, DontPropagateConst >
    NonConstBase_WeakCount_NoConvert_Assert_Reset_NoPropagate_ptr;

typedef Loki::StrongPtr< BaseClass, true, TwoRefCounts, DisallowConversion,
    NoCheck, AllowReset, DeleteSingle, DontPropagateConst >
    NonConstBase_StrongCount_NoConvert_NoCheck_Reset_NoPropagate_ptr;

/// @note Used for const propagation tests.
typedef Loki::StrongPtr< const BaseClass, true, TwoRefCounts, DisallowConversion,
    NoCheck, CantResetWithStrong, DeleteSingle, PropagateConst >
    ConstBase_StrongCount_NoConvert_NoCheck_Propagate_ptr;

typedef Loki::StrongPtr< const BaseClass, false, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, PropagateConst >
    ConstBase_WeakCount_NoConvert_Assert_Propagate_ptr;

// ----------------------------------------------------------------------------

void DoWeakLeakTest( void )
{
    const unsigned int ctorCount = Counted::GetCtorCount();
    const unsigned int dtorCount = Counted::GetDtorCount();
    assert( Counted::AllDestroyed() );
    (void)ctorCount;
    (void)dtorCount;

    {
        Counted_WeakPtr pWeakInt;
        {
            Counted_StrongPtr pStrongInt( new Counted );
            pWeakInt = pStrongInt;
            assert( Counted::ExtraConstructions() );
            assert( Counted::GetCtorCount() == ctorCount + 1 );
            assert( Counted::GetDtorCount() == dtorCount );
        }
        assert( Counted::AllDestroyed() );
        assert( Counted::GetCtorCount() == ctorCount + 1 );
        assert( Counted::GetDtorCount() == dtorCount + 1 );
    }

    {
        Lockable_WeakPtr pWeakInt;
        {
            Lockable_StrongPtr pStrongInt( new Counted );
            pWeakInt = pStrongInt;
            assert( Counted::ExtraConstructions() );
            assert( Counted::GetCtorCount() == ctorCount + 2 );
            assert( Counted::GetDtorCount() == dtorCount + 1 );
        }
        assert( Counted::AllDestroyed() );
        assert( Counted::GetCtorCount() == ctorCount + 2 );
        assert( Counted::GetDtorCount() == dtorCount + 2 );
    }

    {
        Linked_WeakPtr pWeakInt;
        {
            Linked_StrongPtr pStrongInt( new Counted );
            pWeakInt = pStrongInt;
            assert( Counted::ExtraConstructions() );
            assert( Counted::GetCtorCount() == ctorCount + 3 );
            assert( Counted::GetDtorCount() == dtorCount + 2 );
        }
        assert( Counted::AllDestroyed() );
        assert( Counted::GetCtorCount() == ctorCount + 3 );
        assert( Counted::GetDtorCount() == dtorCount + 3 );
    }
}

// ----------------------------------------------------------------------------

void DoStrongRefCountTests( void )
{

    BaseClass * pNull = NULL; (void)pNull;
    const unsigned int ctorCount = BaseClass::GetCtorCount(); (void)ctorCount;
    const unsigned int dtorCount = BaseClass::GetDtorCount(); (void)dtorCount;
    assert( BaseClass::GetCtorCount() == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w0;
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s0;
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w1( w0 );

        // Copy from weak to strong is available.
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s1( w0 );
        // Assignment from weak to strong is available.
        s1 = w1;

        // Converting from strong to weak is available.
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w2( s0 );
        // Assignment from strong to weak is available.
        w1 = s1;

        assert( !s0 );
        assert( !s1 );
        assert( !w0 );
        assert( !w1 );
        assert( s1 == pNull );
        assert( s0 == pNull );
        assert( w1 == pNull );
        assert( w1 == pNull );
        assert( pNull == s0 );
        assert( pNull == s1 );
        assert( pNull == w0 );
        assert( pNull == w1 );
        assert( s0 == s0 );
        assert( s1 == s1 );
        assert( w0 == w0 );
        assert( w1 == w1 );
        assert( s1 == s0 );
        assert( s0 == s1 );
        assert( w0 == s0 );
        assert( s0 == w0 );
        assert( w0 == w1 );
        assert( w1 == w0 );
        assert( w0 == w1 );
        assert( w1 == s1 );
        assert( s1 == w1 );
        assert( s1 <= s0 );
        assert( s1 >= s0 );
        assert( s0 <= s1 );
        assert( s0 >= s1 );
        assert( w0 <= s0 );
        assert( w0 >= s0 );
        assert( s0 <= w0 );
        assert( s0 >= w0 );
        assert( w1 <= w0 );
        assert( w1 >= w0 );
        assert( w0 <= w1 );
        assert( w0 >= w1 );
        assert( !( s1 < s0 ) );
        assert( !( s1 > s0 ) );
        assert( !( s0 < s1 ) );
        assert( !( s0 > s1 ) );
        assert( !( w0 < s0 ) );
        assert( !( w0 > s0 ) );
        assert( !( s0 < w0 ) );
        assert( !( s0 > w0 ) );
        assert( !( w1 < w0 ) );
        assert( !( w1 > w0 ) );
        assert( !( w0 < w1 ) );
        assert( !( w0 > w1 ) );
        assert( !( w0 < pNull ) );
        assert( !( w0 > pNull ) );
        assert( !( pNull < w0 ) );
        assert( !( pNull > w0 ) );
    }
    assert( ctorCount == BaseClass::GetCtorCount() );
    assert( dtorCount == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w1( new BaseClass );
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w2( new BaseClass );
        assert( w1 != w2 );
        assert( w1 );
        assert( w2 );
        w1 = w2;
        assert( w1 == w2 );
        assert( w1 );
        assert( w2 );
        assert( dtorCount + 1 == BaseClass::GetDtorCount() );
    }
    assert( ctorCount + 2 == BaseClass::GetCtorCount() );
    assert( dtorCount + 2 == BaseClass::GetDtorCount() );

    {
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s1( new BaseClass );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s2( new BaseClass );
        assert( s1 != s2 );
        assert( s1 );
        assert( s2 );
        s1 = s2;
        assert( s1 == s2 );
        assert( s1 );
        assert( s2 );
        assert( dtorCount + 3 == BaseClass::GetDtorCount() );
    }
    assert( ctorCount + 4 == BaseClass::GetCtorCount() );
    assert( dtorCount + 4 == BaseClass::GetDtorCount() );
    assert( BaseClass::GetCtorCount() == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w1( new BaseClass );
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w2( new BaseClass );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s1( w1 );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s2( w2 );

        // prove basic stuff.
        assert( w1 != w2 );
        assert( s1 != s2 );
        assert( s1 == w1 );
        assert( s2 == w2 );
        assert( s1 );
        assert( s2 );
        assert( w1 );
        assert( w2 );

        // prove a weak pointer can be re-assigned to another without affecting
        // any strong co-pointers. and that no objects were released.
        w1 = w2;  // w1 == w2 == s2   s1
        assert( w1 == w2 );
        assert( s1 != s2 );
        assert( s1 != w1 );
        assert( s1 != w2 );
        assert( s2 == w1 );
        assert( w1 );
        assert( w2 );
        assert( s1 );
        assert( s2 );
        assert( dtorCount + 4 == BaseClass::GetDtorCount() );

        // Prove they all point to same thing.
        s1 = s2;  // w1 == w2 == s2 == s1
        // and prove that one of them released the object.
        assert( dtorCount + 5 == BaseClass::GetDtorCount() );
        assert( w1 == w2 );
        assert( s1 == s2 );
        assert( s1 == w1 );
        assert( s1 == w2 );
        assert( s2 == w1 );
        assert( w1 );
        assert( w2 );
        assert( s1 );
        assert( s2 );
    }
    assert( ctorCount + 6 == BaseClass::GetCtorCount() );
    assert( dtorCount + 6 == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w1( new BaseClass );
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w2( new BaseClass );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s1( w1 );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s2( w2 );

        // prove basic stuff.  w1 == s1   w2 == s2
        assert( w1 != w2 );
        assert( s1 != s2 );
        assert( s1 == w1 );
        assert( s2 == w2 );
        assert( s1 );
        assert( s2 );
        assert( w1 );
        assert( w2 );

        // prove a strong pointer can be re-assigned to another weak pointer,
        // and that any weak co-pointers released the object.
        s1 = w2;  // s1 == w2 == s2   w1
        assert( w1 != w2 );
        assert( s1 == s2 );
        assert( s1 != w1 );
        assert( s1 == w2 );
        assert( s2 != w1 );
        assert( !w1 );
        assert( w2 );
        assert( s1 );
        assert( s2 );
        assert( dtorCount + 7 == BaseClass::GetDtorCount() );

        // Prove that when strong pointer is re-assigned, object
        // is not destroyed if another strong co-pointer exists.
        s1 = w1;  // w1 == s1   w2 == s2
        // and prove that none of them released an object.
        assert( dtorCount + 7 == BaseClass::GetDtorCount() );
        assert( w1 != w2 );
        assert( s1 != s2 );
        assert( s1 == w1 );
        assert( s2 == w2 );
        assert( !s1 );
        assert( s2 );
        assert( !w1 );
        assert( w2 );
    }
    assert( ctorCount + 8 == BaseClass::GetCtorCount() );
    assert( dtorCount + 8 == BaseClass::GetDtorCount() );
}

// ----------------------------------------------------------------------------

void DoStrongConstTests( void )
{

    const unsigned int ctorCount = BaseClass::GetCtorCount(); (void)ctorCount;
    const unsigned int dtorCount = BaseClass::GetDtorCount(); (void)dtorCount;
    assert( BaseClass::GetCtorCount() == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w1( new BaseClass );
        const NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w2( w1 );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s1( w1 );
        const NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s2( w2 );

        const BaseClass & cbw1 = *w1;
        cbw1.DoThat();
        const BaseClass & cbw2 = *w2;
        cbw2.DoThat();
        const BaseClass & cbs1 = *s1;
        cbs1.DoThat();
        const BaseClass & cbs2 = *s2;
        cbs2.DoThat();

        BaseClass & bw1 = *w1;
        bw1.DoThat();
        BaseClass & bw2 = *w2;
        bw2.DoThat();
        BaseClass & bs1 = *s1;
        bs1.DoThat();
        BaseClass & bs2 = *s2;
        bs2.DoThat();
    }
    assert( ctorCount + 1 == BaseClass::GetCtorCount() );
    assert( dtorCount + 1 == BaseClass::GetDtorCount() );

    {
        ConstBase_WeakCount_NoConvert_Assert_Propagate_ptr w1( new BaseClass );
        const ConstBase_WeakCount_NoConvert_Assert_Propagate_ptr w2( w1 );
        ConstBase_StrongCount_NoConvert_NoCheck_Propagate_ptr s1( w1 );
        const ConstBase_StrongCount_NoConvert_NoCheck_Propagate_ptr s2( w2 );

        const BaseClass & cbw1 = *w1;
        cbw1.DoThat();
        const BaseClass & cbw2 = *w2;
        cbw2.DoThat();
        const BaseClass & cbs1 = *s1;
        cbs1.DoThat();
        const BaseClass & cbs2 = *s2;
        cbs2.DoThat();

        /** @note These are illegal because constness is propagated by the
         StrongPtr's policy.  Your compiler should produce error messages if
         you attempt to compile these lines.
         */
        //BaseClass & bw1 = *w1;
        //bw1.DoThat();
        //BaseClass & bw2 = *w2;
        //bw2.DoThat();
        //BaseClass & bs1 = *s1;
        //bs1.DoThat();
        //BaseClass & bs2 = *s2;
        //bs2.DoThat();
    }
    assert( ctorCount + 2 == BaseClass::GetCtorCount() );
    assert( dtorCount + 2 == BaseClass::GetDtorCount() );
}

// ----------------------------------------------------------------------------

void DoStrongReleaseTests( void )
{

    BaseClass * pNull = NULL; (void)pNull;
    const unsigned int ctorCount = BaseClass::GetCtorCount(); (void)ctorCount;
    const unsigned int dtorCount = BaseClass::GetDtorCount(); (void)dtorCount;
    assert( BaseClass::GetCtorCount() == BaseClass::GetDtorCount() );

    {
        // These are tests of pointers that don't allow reset or release if
        // there is at least 1 strong pointer.  Ironically, this means that
        // if only 1 strong pointer holds onto an object, and you call Release
        // using that strong pointer, it can't release itself.

        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w1( new BaseClass );
        NonConstBase_WeakCount_NoConvert_Assert_NoPropagate_ptr w2( new BaseClass );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s1( w1 );
        NonConstBase_StrongCount_NoConvert_NoCheck_NoPropagate_ptr s2( w2 );

        // Prove that neither weak nor strong pointers can be
        // released if any co-pointer is strong.
        bool released = ReleaseAll( w2, pNull );
        assert( !released );

        released = ReleaseAll( w1, pNull );
        assert( !released );

        released = ReleaseAll( s1, pNull );
        assert( !released );

        released = ReleaseAll( s2, pNull );
        assert( !released );

        // Prove that weak and strong pointers can be reset only
        // if stored pointer matches parameter pointer - or there
        // are no strong co-pointers.
        bool reset = ResetAll( w2, pNull );
        assert( !reset );

        reset = ResetAll( w1, pNull );
        assert( !reset );

        reset = ResetAll( s1, pNull );
        assert( !reset );

        reset = ResetAll( s2, pNull );
        assert( !reset );

        s2 = pNull;
        assert( dtorCount + 1 == BaseClass::GetDtorCount() );
        reset = ResetAll( w2, pNull );
        assert( reset );

        reset = ResetAll( w1, pNull );
        assert( !reset );

        reset = ResetAll( s1, pNull );
        assert( !reset );

        reset = ResetAll( s2, pNull );
        assert( reset );
    }
    assert( ctorCount + 2 == BaseClass::GetCtorCount() );
    assert( dtorCount + 2 == BaseClass::GetDtorCount() );

    {
        // These are tests of pointers that do allow reset and release even
        // if a strong pointer exists.

        NonConstBase_WeakCount_NoConvert_Assert_Reset_NoPropagate_ptr w1( new BaseClass );
        NonConstBase_StrongCount_NoConvert_NoCheck_Reset_NoPropagate_ptr w2( new BaseClass );
        NonConstBase_WeakCount_NoConvert_Assert_Reset_NoPropagate_ptr s1( w1 );
        NonConstBase_StrongCount_NoConvert_NoCheck_Reset_NoPropagate_ptr s2( w2 );

        BaseClass * thing = NULL;
        bool released = ReleaseAll( w2, thing );
        assert( released );
        assert( NULL != thing );
        delete thing;
        assert( dtorCount + 3 == BaseClass::GetDtorCount() );

        released = ReleaseAll( s1, thing );
        assert( released );
        assert( NULL != thing );
        delete thing;
        assert( dtorCount + 4 == BaseClass::GetDtorCount() );

        released = ReleaseAll( w1, thing );
        assert( released );
        assert( NULL == thing );

        released = ReleaseAll( s2, thing );
        assert( released );
        assert( NULL == thing );

        // Prove that weak and strong pointers can be reset
        // only if stored pointer matches parameter pointer
        // - even if there are strong co-pointers.
        bool reset = ResetAll( w2, pNull );
        assert( reset );

        reset = ResetAll( w1, pNull );
        assert( reset );

        reset = ResetAll( s1, pNull );
        assert( reset );

        reset = ResetAll( s2, pNull );
        assert( reset );
        assert( ctorCount + 4 == BaseClass::GetCtorCount() );
        assert( dtorCount + 4 == BaseClass::GetDtorCount() );

        s2 = new BaseClass;
        s1 = new BaseClass;
        reset = ResetAll( w2, pNull );
        assert( reset );

        reset = ResetAll( w1, pNull );
        assert( reset );
    }
    assert( ctorCount + 6 == BaseClass::GetCtorCount() );
    assert( dtorCount + 6 == BaseClass::GetDtorCount() );
}

// ----------------------------------------------------------------------------

void DoStrongRefLinkTests( void )
{

    BaseClass * pNull = NULL; (void)pNull;
    const unsigned int ctorCount = BaseClass::GetCtorCount(); (void)ctorCount;
    const unsigned int dtorCount = BaseClass::GetDtorCount(); (void)dtorCount;
    assert( BaseClass::GetCtorCount() == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakLink_NoConvert_Assert_NoPropagate_ptr w0;
        NonConstBase_WeakLink_NoConvert_Assert_NoPropagate_ptr w1;
        NonConstBase_StrongLink_NoConvert_NoCheck_NoPropagate_ptr s0;
        NonConstBase_StrongLink_NoConvert_NoCheck_NoPropagate_ptr s1;
        assert( !s0 );
        assert( !s1 );
        assert( s0 == s1 );
        assert( s1 == pNull );
        assert( s0 == pNull );
        assert( pNull == s0 );
        assert( pNull == s1 );
        assert( s1 == s0 );
        assert( s1 == s1 );
        assert( s0 == s0 );
        assert( s0 == s1 );
        assert( s1 <= s0 );
        assert( s1 >= s0 );
        assert( s0 <= s1 );
        assert( s0 >= s1 );
        assert( !( s1 < s0 ) );
        assert( !( s1 > s0 ) );
        assert( !( s0 < s1 ) );
        assert( !( s0 > s1 ) );
        assert( !w0 );
        assert( !w1 );
        assert( w0 == pNull );
        assert( s0 == pNull );
        assert( pNull == s0 );
        assert( pNull == w0 );
        assert( w0 == s0 );
        assert( w0 == w0 );
        assert( s0 == s0 );
        assert( s0 == w0 );
        assert( w0 <= s0 );
        assert( w0 >= s0 );
        assert( s0 <= w0 );
        assert( s0 >= w0 );
        assert( !( w0 < s0 ) );
        assert( !( w0 > s0 ) );
        assert( !( s0 < w0 ) );
        assert( !( s0 > w0 ) );
        assert( w0 == w1 );
        assert( w1 == pNull );
        assert( w0 == pNull );
        assert( pNull == w0 );
        assert( pNull == w1 );
        assert( w1 == w0 );
        assert( w1 == w1 );
        assert( w0 == w0 );
        assert( w0 == w1 );
        assert( w1 <= w0 );
        assert( w1 >= w0 );
        assert( w0 <= w1 );
        assert( w0 >= w1 );
        assert( !( w1 < w0 ) );
        assert( !( w1 > w0 ) );
        assert( !( w0 < w1 ) );
        assert( !( w0 > w1 ) );
    }
    assert( ctorCount == BaseClass::GetCtorCount() );
    assert( dtorCount == BaseClass::GetDtorCount() );

    {
        NonConstBase_WeakLink_NoConvert_Assert_NoPropagate_ptr w3( new BaseClass );
        NonConstBase_WeakLink_NoConvert_Assert_NoPropagate_ptr w4( new BaseClass );
        assert( w3 != w4 );
        assert( w3 );
        assert( w4 );
        w3 = w4;
        assert( w3 == w4 );
        assert( w3 );
        assert( w4 );
        assert( dtorCount + 1 == BaseClass::GetDtorCount() );
    }
    assert( ctorCount + 2 == BaseClass::GetCtorCount() );
    assert( dtorCount + 2 == BaseClass::GetDtorCount() );

    {
        NonConstBase_StrongLink_NoConvert_NoCheck_NoPropagate_ptr s3( new BaseClass );
        NonConstBase_StrongLink_NoConvert_NoCheck_NoPropagate_ptr s4( new BaseClass );
        assert( s3 != s4 );
        assert( s3 );
        assert( s4 );
        s3 = s4;
        assert( s3 == s4 );
        assert( s3 );
        assert( s4 );
        assert( dtorCount + 3 == BaseClass::GetDtorCount() );
    }
    assert( ctorCount + 4 == BaseClass::GetCtorCount() );
    assert( dtorCount + 4 == BaseClass::GetDtorCount() );
    assert( BaseClass::GetCtorCount() == BaseClass::GetDtorCount() );

}

// ----------------------------------------------------------------------------

void DoWeakCycleTests( void )
{

    const unsigned int ctorCountMoon = Moon::GetCtorCount(); (void)ctorCountMoon;
    const unsigned int dtorCountMoon = Moon::GetDtorCount(); (void)dtorCountMoon;
    assert( Moon::AllDestroyed() );
    const unsigned int ctorCountEarth = Earth::GetCtorCount(); (void)ctorCountEarth;
    const unsigned int dtorCountEarth = Earth::GetDtorCount(); (void)dtorCountEarth;
    assert( Earth::AllDestroyed() );

    {
        Earth_WeakPtr ew0;
        Moon_WeakPtr mw0;
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon );
    assert( Moon::GetDtorCount() == dtorCountMoon );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth );
    assert( Earth::GetDtorCount() == dtorCountEarth );

    {
        Earth_WeakPtr ew1( new Earth );
    }
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+1 );
    assert( Earth::GetDtorCount() == dtorCountEarth+1 );

    {
        Moon_WeakPtr mw1( new Moon );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+1 );
    assert( Moon::GetDtorCount() == dtorCountMoon+1 );

    {
        Earth_WeakPtr ew1( new Earth );
        Moon_WeakPtr mw1( new Moon );
        ew1->SetMoon( mw1 );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+2 );
    assert( Moon::GetDtorCount() == dtorCountMoon+2 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+2 );
    assert( Earth::GetDtorCount() == dtorCountEarth+2 );

    {
        Earth_WeakPtr ew1( new Earth );
        Moon_WeakPtr mw1( new Moon );
        mw1->SetEarth( ew1 );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+3 );
    assert( Moon::GetDtorCount() == dtorCountMoon+3 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+3 );
    assert( Earth::GetDtorCount() == dtorCountEarth+3 );

    {
        Earth_WeakPtr ew1( new Earth );
        Moon_WeakPtr mw1( new Moon );
        ew1->SetMoon( mw1 );
        mw1->SetEarth( ew1 );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+4 );
    assert( Moon::GetDtorCount() == dtorCountMoon+4 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+4 );
    assert( Earth::GetDtorCount() == dtorCountEarth+4 );

    {
        Earth_StrongPtr es1( new Earth );
        Moon_StrongPtr ms1( new Moon );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+5 );
    assert( Moon::GetDtorCount() == dtorCountMoon+5 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+5 );
    assert( Earth::GetDtorCount() == dtorCountEarth+5 );

    {
        Earth_StrongPtr es1( new Earth );
        Moon_StrongPtr ms1( new Moon );
        es1->SetMoon( ms1 );
        ms1->SetEarth( es1 );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+6 );
    assert( Moon::GetDtorCount() == dtorCountMoon+6 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+6 );
    assert( Earth::GetDtorCount() == dtorCountEarth+6 );

    {
        Earth_StrongPtr es1( new Earth );
        Moon_StrongPtr ms1( new Moon );
        {
            Earth_WeakPtr ew1( es1 );
            Moon_WeakPtr mw1( ms1 );
            ew1->SetMoon( mw1 );
            mw1->SetEarth( ew1 );
        }
        // Note that dtor counts have not changed from previous test.
        assert( Moon::GetCtorCount() == ctorCountMoon+7 );
        assert( Moon::GetDtorCount() == dtorCountMoon+6 );
        assert( Earth::GetCtorCount() == ctorCountEarth+7 );
        assert( Earth::GetDtorCount() == dtorCountEarth+6 );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+7 );
    assert( Moon::GetDtorCount() == dtorCountMoon+7 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+7 );
    assert( Earth::GetDtorCount() == dtorCountEarth+7 );

    {
        Earth_StrongPtr es1;
        Moon_StrongPtr ms1;
        {
            Earth_WeakPtr ew1( new Earth );
            Moon_WeakPtr mw1( new Moon );
            ew1->SetMoon( mw1 );
            mw1->SetEarth( ew1 );
            es1 = ew1;
            ms1 = mw1;
        }
        // Note that dtor counts have not changed from previous test.
        assert( Moon::GetCtorCount() == ctorCountMoon+8 );
        assert( Moon::GetDtorCount() == dtorCountMoon+7 );
        assert( Earth::GetCtorCount() == ctorCountEarth+8 );
        assert( Earth::GetDtorCount() == dtorCountEarth+7 );
    }
    assert( Moon::AllDestroyed() );
    assert( Moon::GetCtorCount() == ctorCountMoon+8 );
    assert( Moon::GetDtorCount() == dtorCountMoon+8 );
    assert( Earth::AllDestroyed() );
    assert( Earth::GetCtorCount() == ctorCountEarth+8 );
    assert( Earth::GetDtorCount() == dtorCountEarth+8 );
}

// ----------------------------------------------------------------------------

class Ball
{
public:

    inline static unsigned int GetCtorCount( void )
    {
        return s_CtorCount;
    }

    inline static unsigned int GetDtorCount( void )
    {
        return s_DtorCount;
    }

    inline static bool AllDestroyed( void )
    {
        return ( s_CtorCount == s_DtorCount );
    }

    inline Ball( void )
    {
        s_CtorCount++;
    }

    inline ~Ball( void )
    {
        s_DtorCount++;
    }

private:
    static unsigned int s_CtorCount;
    static unsigned int s_DtorCount;
};

unsigned int Ball::s_CtorCount = 0;
unsigned int Ball::s_DtorCount = 0;

// ----------------------------------------------------------------------------

// These typedefs are for testing StrongPtr ownership policies that enforce a single owner.
typedef ::Loki::StrongPtr< Ball, false, ::Loki::SingleOwnerRefCount,
    ::Loki::DisallowConversion, ::Loki::AssertCheck, ::Loki::OnlyStrongMayReset >
    NonOwner_Counted_BallPtr;
typedef ::Loki::StrongPtr< Ball, true,  ::Loki::SingleOwnerRefCount,
    ::Loki::DisallowConversion, ::Loki::AssertCheck, ::Loki::OnlyStrongMayReset >
    Owner_Counted_BallPtr;

typedef ::Loki::StrongPtr< Ball, false, ::Loki::Lockable1OwnerRefCount,
    ::Loki::DisallowConversion, ::Loki::AssertCheck, ::Loki::OnlyStrongMayReset >
    Lockable_NonOwner_Counted_BallPtr;
typedef ::Loki::StrongPtr< Ball, true,  ::Loki::Lockable1OwnerRefCount,
    ::Loki::DisallowConversion, ::Loki::AssertCheck, ::Loki::OnlyStrongMayReset >
    Lockable_Owner_Counted_BallPtr;

typedef ::Loki::StrongPtr< Ball, false, ::Loki::SingleOwnerRefLinks,
    ::Loki::DisallowConversion, ::Loki::AssertCheck, ::Loki::OnlyStrongMayReset >
    NonOwner_Linked_BallPtr;
typedef ::Loki::StrongPtr< Ball, true,  ::Loki::SingleOwnerRefLinks,
    ::Loki::DisallowConversion, ::Loki::AssertCheck, ::Loki::OnlyStrongMayReset >
    Owner_Linked_BallPtr;

// ----------------------------------------------------------------------------

template < class OwnerPtr, class NonOwnerPtr >
class TheOwner
{
public:

    TheOwner( void ) : m_myBall(), m_otherBall() {}
    ~TheOwner( void ) {}

    void ClearBall( void )
    {
        Ball * pBall = NULL;
        ResetAll( m_myBall, pBall );
    }

    bool OwnBall( const NonOwnerPtr & ball )
    {
        if ( !ball )
            return false;
        m_myBall = ball;
        return true;
    }

    bool OwnBall( const OwnerPtr & ball )
    {
        if ( !ball )
            return false;
        m_myBall = ball;
        return true;
    }

    bool OwnBall( Ball * ball )
    {
        if ( nullptr == ball )
            return false;
        m_myBall = ball;
        return true;
    }

    bool DoesOwnBall( void ) const
    {
        const bool hasBall = m_myBall;
        return hasBall;
    }

    void UseBall( const OwnerPtr & ball )
    {
        m_otherBall = ball;
    }

    bool DoesUseBall( void ) const
    {
        const bool hasBall = m_otherBall;
        return hasBall;
    }

    const OwnerPtr & ShareMyBall( void ) const
    {
        return m_myBall;
    }

    const NonOwnerPtr & ShareOtherBall( void )
    {
        return m_otherBall;
    }

private:
    OwnerPtr m_myBall;
    NonOwnerPtr m_otherBall;
};

typedef TheOwner< Owner_Counted_BallPtr, NonOwner_Counted_BallPtr > Toddler;

typedef TheOwner< Owner_Linked_BallPtr, NonOwner_Linked_BallPtr > Kitten;

typedef TheOwner< Lockable_Owner_Counted_BallPtr, Lockable_NonOwner_Counted_BallPtr > Teenager;

// ----------------------------------------------------------------------------

template < class OwnerPtr, class NonOwnerPtr, class Owner >
void DoSingleOwnerTest( OwnerPtr & op1,
    const NonOwnerPtr & pBall1,
    const NonOwnerPtr & pBall2,
    const NonOwnerPtr & pBall3,
    Owner & owner1,
    Owner & owner2,
    Owner & owner3 )
{

    assert( !pBall1.IsStrong() );
    assert( !pBall2.IsStrong() );
    assert( !pBall3.IsStrong() );
    assert( pBall1 != nullptr );
    assert( pBall2 != nullptr );
    assert( pBall3 != nullptr );

    assert( !owner1.DoesOwnBall() );
    assert( !owner2.DoesOwnBall() );
    assert( !owner3.DoesOwnBall() );
    assert( !owner1.DoesUseBall() );
    assert( !owner2.DoesUseBall() );
    assert( !owner3.DoesUseBall() );

    owner1.OwnBall( pBall1 );
    owner2.OwnBall( pBall2 );
    owner3.OwnBall( pBall3 );
    assert( owner1.DoesOwnBall() );
    assert( owner2.DoesOwnBall() );
    assert( owner3.DoesOwnBall() );
    assert( !owner1.DoesUseBall() );
    assert( !owner2.DoesUseBall() );
    assert( !owner3.DoesUseBall() );

    assert( owner1.ShareMyBall() == pBall1 );
    assert( owner2.ShareMyBall() == pBall2 );
    assert( owner3.ShareMyBall() == pBall3 );
    assert( owner1.ShareMyBall().IsStrong() );
    assert( owner2.ShareMyBall().IsStrong() );
    assert( owner3.ShareMyBall().IsStrong() );

    owner1.UseBall( owner2.ShareMyBall() );
    owner2.UseBall( owner3.ShareMyBall() );
    owner3.UseBall( owner1.ShareMyBall() );
    assert( owner1.DoesUseBall() );
    assert( owner2.DoesUseBall() );
    assert( owner3.DoesUseBall() );

    assert( !owner1.ShareOtherBall().IsStrong() );
    assert( !owner2.ShareOtherBall().IsStrong() );
    assert( !owner3.ShareOtherBall().IsStrong() );

    assert( owner1.ShareOtherBall() == pBall2 );
    assert( owner2.ShareOtherBall() == pBall3 );
    assert( owner3.ShareOtherBall() == pBall1 );
    assert( owner1.ShareMyBall() == pBall1 );
    assert( owner2.ShareMyBall() == pBall2 );
    assert( owner3.ShareMyBall() == pBall3 );

    try
    {
        assert( owner2.ShareMyBall() == pBall2 );
        // Can owner1 own a ball that is owned by somebody else?
        owner1.OwnBall( pBall2 );
        assert( false );
    }
    catch ( const ::std::logic_error & ex )
    {
        assert( true );
        assert( ::strcmp( ex.what(), ::Loki::StrongPtr_Single_Owner_Exception_Message ) == 0 );
        (void)ex;
    }
    try
    {
        assert( owner3.ShareMyBall() == pBall3 );
        // Can owner2 own a ball that is owned by somebody else?
        owner2.OwnBall( pBall3 );
        assert( false );
    }
    catch ( const ::std::logic_error & ex )
    {
        assert( true );
        assert( ::strcmp( ex.what(), ::Loki::StrongPtr_Single_Owner_Exception_Message ) == 0 );
        (void)ex;
    }
    try
    {
        assert( owner1.ShareMyBall() == pBall1 );
        // Can owner3 own a ball that is owned by somebody else?
        owner3.OwnBall( pBall1 );
        assert( false );
    }
    catch ( const ::std::logic_error & ex )
    {
        assert( true );
        assert( ::strcmp( ex.what(), ::Loki::StrongPtr_Single_Owner_Exception_Message ) == 0 );
        (void)ex;
    }

    // These tests occur after exceptions were thrown.  Since these tests pass,
    // that means the pointers are still in a valid state.  Also, the pointers
    // are untouched by the temporary StrongPtr's that were made inside the
    // assignment operators, so therefore the pointers have strong exception
    // safety.
    owner1.ClearBall();
    owner2.ClearBall();
    owner3.ClearBall();
    assert( !owner1.DoesOwnBall() );
    assert( !owner2.DoesOwnBall() );
    assert( !owner3.DoesOwnBall() );
    assert( nullptr == pBall1 );
    assert( nullptr == pBall2 );
    assert( nullptr == pBall3 );
    assert( owner1.ShareMyBall() == pBall1 );
    assert( owner2.ShareMyBall() == pBall2 );
    assert( owner3.ShareMyBall() == pBall3 );
    assert( owner1.ShareMyBall() == nullptr );
    assert( owner2.ShareMyBall() == nullptr );
    assert( owner3.ShareMyBall() == nullptr );

    {
        Ball * baseball = new Ball;
        Ball * football = new Ball;
        Ball * softball = new Ball;

        NonOwnerPtr p1( baseball );
        NonOwnerPtr p2( football );
        NonOwnerPtr p3( softball );
        assert( nullptr != p1 );
        assert( nullptr != p2 );
        assert( nullptr != p3 );

        assert( !p1.IsStrong() );
        assert( !p2.IsStrong() );
        assert( !p3.IsStrong() );

        {
            Owner o1;
            Owner o2;
            Owner o3;

            assert( !o1.DoesOwnBall() );
            assert( !o2.DoesOwnBall() );
            assert( !o3.DoesOwnBall() );
            assert( !o1.DoesUseBall() );
            assert( !o2.DoesUseBall() );
            assert( !o3.DoesUseBall() );

            o1.OwnBall( p1 );
            o2.OwnBall( p2 );
            o3.OwnBall( p3 );

            assert( o1.DoesOwnBall() );
            assert( o2.DoesOwnBall() );
            assert( o3.DoesOwnBall() );
            assert( !o1.DoesUseBall() );
            assert( !o2.DoesUseBall() );
            assert( !o3.DoesUseBall() );

            assert( o1.ShareMyBall() == p1 );
            assert( o2.ShareMyBall() == p2 );
            assert( o3.ShareMyBall() == p3 );

            o1.ClearBall();
            o2.ClearBall();
            o3.ClearBall();
            assert( !o1.DoesOwnBall() );
            assert( !o2.DoesOwnBall() );
            assert( !o3.DoesOwnBall() );
            assert( o1.ShareMyBall() == nullptr );
            assert( o2.ShareMyBall() == nullptr );
            assert( o3.ShareMyBall() == nullptr );
            assert( nullptr == p1 );
            assert( nullptr == p2 );
            assert( nullptr == p3 );
            assert( o1.ShareMyBall() == p1 );
            assert( o2.ShareMyBall() == p2 );
            assert( o3.ShareMyBall() == p3 );
        }

        assert( Ball::AllDestroyed() );
        assert( nullptr == p1 );
        assert( nullptr == p2 );
        assert( nullptr == p3 );
    }

    {
        OwnerPtr op_1;
        OwnerPtr op_2;
        (void)op_1;
        (void)op_2;
        op_1 = op_2;
    }
    assert( Ball::AllDestroyed() );

    // Test ResetAll with owner pointer.
    {
        Ball * ball = new Ball;
        Ball * noBall = nullptr;
        OwnerPtr op_1( ball );
        NonOwnerPtr np1( op_1 );
        NonOwnerPtr np2( np1 );
        assert( ball == op_1 );
        assert( ball == np1 );
        assert( ball == np2 );

        ResetAll( op_1, noBall );
        assert( Ball::AllDestroyed() );
        assert( nullptr == op_1 );
        assert( nullptr == np1 );
        assert( nullptr == np2 );
        assert( nullptr == noBall );
    }

    // Test ReleaseAll with owner pointer.
    {
        Ball * ball = new Ball;
        Ball * noBall = nullptr;
        OwnerPtr op_1( ball );
        NonOwnerPtr np1( op_1 );
        NonOwnerPtr np2( np1 );
        assert( ball == op_1 );
        assert( ball == np1 );
        assert( ball == np2 );

        ReleaseAll( op_1, noBall );
        assert( !Ball::AllDestroyed() );
        assert( nullptr != noBall );
        assert( noBall == ball );
        assert( nullptr == op_1 );
        assert( nullptr == np1 );
        assert( nullptr == np2 );
        delete noBall;
        assert( Ball::AllDestroyed() );
    }

    // Test ResetAll with nonowner pointer.
    {
        Ball * ball = new Ball;
        Ball * noBall = nullptr;
        OwnerPtr op_1( ball );
        NonOwnerPtr np1( op_1 );
        NonOwnerPtr np2( np1 );
        assert( ball == op_1 );
        assert( ball == np1 );
        assert( ball == np2 );

        ResetAll( np1, noBall );
        // ResetAll fails for non-owner pointers.
        assert( !Ball::AllDestroyed() );
        assert( nullptr != op_1 );
        assert( nullptr != np1 );
        assert( nullptr != np2 );
        assert( nullptr == noBall );
    }
    assert( Ball::AllDestroyed() );

    // Test ReleaseAll with nonowner pointer.
    {
        Ball * ball = new Ball;
        Ball * noBall = nullptr;
        OwnerPtr op_1( ball );
        NonOwnerPtr np1( op_1 );
        NonOwnerPtr np2( np1 );
        assert( ball == op_1 );
        assert( ball == np1 );
        assert( ball == np2 );

        ReleaseAll( np1, noBall );
        // ReleaseAll fails for non-owner pointers.
        assert( !Ball::AllDestroyed() );
        assert( nullptr != op_1 );
        assert( nullptr != np1 );
        assert( nullptr != np2 );
        assert( nullptr == noBall );
    }
    assert( Ball::AllDestroyed() );

    // Test assignment operator.
    {
        OwnerPtr op2;
        Ball * ball1 = new Ball;
        Ball * ball2 = new Ball;
        op1 = ball1;
        op2 = ball2;
        assert( ball1 == op1 );
        assert( ball2 == op2 );
        op1 = nullptr;
    }
    assert( Ball::AllDestroyed() );

    // Test the Swap function for strong co-pointers.
    {
        NonOwnerPtr np1;
        NonOwnerPtr np2;
        Ball * ball1 = new Ball;
        Ball * ball2 = new Ball;
        np1 = ball1;
        np2 = ball2;
        assert( ball1 == np1 );
        assert( ball2 == np2 );
        assert( nullptr != np1 );
        assert( nullptr != np2 );

        op1 = np1;
        OwnerPtr op2( np2 );
        assert( np1 == op1 );
        assert( np2 == op2 );
        assert( np2 != op1 );
        assert( np1 != op2 );

        op1.Swap( op2 );

        assert( ball1 != op1 ); // Owners do not point to original objects.
        assert( ball2 != op2 );
        assert( ball2 == op1 ); // Owners now point to different objects.
        assert( ball1 == op2 );
        assert( ball1 != np2 ); // Non-owners still point to original objects.
        assert( ball2 != np1 );
        assert( ball2 == np2 ); // Non-owners do not point to different objects.
        assert( ball1 == np1 );
        assert( np1 != op1 );
        assert( np2 != op2 );
        assert( np2 == op1 );
        assert( np1 == op2 );
        assert( nullptr != op1 );
        assert( nullptr != op2 );
        assert( nullptr != np1 );
        assert( nullptr != np2 );
        op1 = nullptr;
    }
    assert( Ball::AllDestroyed() );

    // Test the Swap function for weak co-pointers.
    {
        NonOwnerPtr np1;
        NonOwnerPtr np2;
        Ball * ball1 = new Ball;
        Ball * ball2 = new Ball;
        np1 = ball1;
        np2 = ball2;
        assert( ball1 == np1 );
        assert( ball2 == np2 );
        assert( nullptr != np1 );
        assert( nullptr != np2 );

        op1 = np1;
        OwnerPtr op2( np2 );
        assert( np1 == op1 );
        assert( np2 == op2 );
        assert( np2 != op1 );
        assert( np1 != op2 );

        np1.Swap( np2 );

        assert( ball1 == op1 ); // Owners still point to their original objects.
        assert( ball2 == op2 );
        assert( ball2 != op1 ); // Owners do not point to different objects.
        assert( ball1 != op2 );
        assert( ball1 == np2 ); // Non-owners now point to different objects.
        assert( ball2 == np1 );
        assert( ball2 != np2 ); // Non-owners do not point to their original objects.
        assert( ball1 != np1 );
        assert( np1 != op1 );
        assert( np2 != op2 );
        assert( np2 == op1 );
        assert( np1 == op2 );
        assert( nullptr != op1 );
        assert( nullptr != op2 );
        assert( nullptr != np1 );
        assert( nullptr != np2 );
        op1 = nullptr;
    }
    assert( Ball::AllDestroyed() );

    // A Swap of a weak pointer with a strong pointer will cause a compiler error.
}

// ----------------------------------------------------------------------------

void DoSingleOwnerTests( void )
{

    // These tests exercise the lockable single-owner StrongPtr policies.
    {
        Ball * baseball = new Ball;
        Ball * football = new Ball;
        Ball * softball = new Ball;
        Lockable_Owner_Counted_BallPtr op1;
        Lockable_NonOwner_Counted_BallPtr pBaseball( baseball );
        Lockable_NonOwner_Counted_BallPtr pFootball( football );
        Lockable_NonOwner_Counted_BallPtr pSoftball( softball );
        Teenager teen1;
        Teenager teen2;
        Teenager teen3;
        DoSingleOwnerTest( op1, pBaseball, pFootball, pSoftball,
            teen1, teen2, teen3 );
    }
    assert( Ball::AllDestroyed() );

    // These tests exercise the reference-counted single-owner StrongPtr policies.
    {
        Ball * soccerBall = new Ball;
        Ball * bouncyBall = new Ball;
        Ball * basketBall = new Ball;

        NonOwner_Counted_BallPtr pSoccerBall( soccerBall );
        NonOwner_Counted_BallPtr pBouncyBall( bouncyBall );
        NonOwner_Counted_BallPtr pBasketBall( basketBall );
        Toddler child1;
        Toddler child2;
        Toddler child3;
        Owner_Counted_BallPtr op1;
        DoSingleOwnerTest( op1, pSoccerBall, pBouncyBall, pBasketBall,
            child1, child2, child3 );
    }
    assert( Ball::AllDestroyed() );

    // These tests exercise the linked-cycle single-owner StrongPtr policies.
    {
        Ball * yarnBall = new Ball;
        Ball * twineBall = new Ball;
        Ball * stringBall = new Ball;

        NonOwner_Linked_BallPtr pYarnBall( yarnBall );
        NonOwner_Linked_BallPtr pTwineBall( twineBall );
        NonOwner_Linked_BallPtr pStringBall( stringBall );
        Kitten cat1;
        Kitten cat2;
        Kitten cat3;
        Owner_Linked_BallPtr op1;
        DoSingleOwnerTest( op1, pYarnBall, pTwineBall, pStringBall,
            cat1, cat2, cat3 );
    }
    assert( Ball::AllDestroyed() );
}

// ----------------------------------------------------------------------------

void DoStrongForwardReferenceTest( void )
{
    /** @note These lines should cause the compiler to make a warning message
     about attempting to delete an undefined type.  They should also cause
     an error message about a negative subscript since
     */
    //Thingy_DeleteSingle_ptr p1;
    //Thingy_DeleteSingle_ptr p2( p1 );
    //Thingy_DeleteSingle_ptr p3;
    //p3 = p1;

    /** @note These lines should cause the compiler to make an error message
     about attempting to call the destructor for an undefined type.
     */
    //Thingy_DeleteUsingFree_ptr p4;
    //Thingy_DeleteUsingFree_ptr p5( p4 );
    //Thingy_DeleteUsingFree_ptr p6;
    //p6 = p4;

    /** @note These lines should cause the compiler to make neither a warning
     nor an error message even though the type is undefined.
     */
    Thingy_DeleteNothing_ptr p7;
    Thingy_DeleteNothing_ptr p8( p7 );
    Thingy_DeleteNothing_ptr p9;
    p9 = p7;
}

// ----------------------------------------------------------------------------

#include <algorithm>

struct Foo
{
};
typedef Loki::StrongPtr
<
    BaseClass, false, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteSingle, DontPropagateConst
>
Ptr;

bool Compare( const Ptr&, const Ptr&)
{
    return true;
}

void friend_handling2()
{
    // http://sourceforge.net/tracker/index.php?func=detail&aid=1570582&group_id=29557&atid=396644

    // friend injection
    // see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28597
    std::vector<Ptr> vec;
    std::sort( vec.begin(), vec.end(), Compare );
    std::nth_element( vec.begin(), vec.begin(), vec.end(), Compare );
    std::search( vec.begin(), vec.end(),
        vec.begin(), vec.end(), Compare );
    Ptr a, b;
    Compare( a, b );

    // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=29486
    BaseClass * pNull ;
    Ptr w1( new BaseClass );
    ReleaseAll( w1, pNull );
}

// ----------------------------------------------------------------------------

void DoStrongCompareTests( void )
{
    Earth * p1 = new Earth;
    Earth * p2 = new Earth;
    Earth_StrongPtr sp1( p1 );
    Earth_StrongPtr sp2( p2 );
    const bool isOneLess = ( p1 < p2 );
    if ( isOneLess )
    {
        assert( sp1 <   p2 );
        assert( sp1 <  sp2 );
        assert( sp1 <=  p2 );
        assert( sp1 <= sp2 );
        assert( sp1 <=  p1 );
        assert( sp1 ==  p1 );
        assert( sp1 == sp1 );
        assert( sp1 !=  p2 );
        assert( sp1 != sp2 );
        assert( sp2 >   p1 );
        assert( sp2 >  sp1 );
        assert( sp2 >=  p1 );
        assert( sp2 >= sp1 );
        assert( sp2 >=  p2 );
        assert( sp2 ==  p2 );
        assert( sp2 == sp2 );
        assert( sp2 !=  p1 );
        assert( sp2 != sp1 );
        assert(  p1 <   p2 );
        assert(  p1 <  sp2 );
        assert(  p1 <=  p2 );
        assert(  p1 <= sp2 );
        assert(  p1 <= sp1 );
        assert(  p1 ==  p1 );
        assert(  p1 == sp1 );
        assert(  p1 !=  p2 );
        assert(  p1 != sp2 );
        assert(  p2 >   p1 );
        assert(  p2 >  sp1 );
        assert(  p2 >=  p1 );
        assert(  p2 >= sp1 );
        assert(  p2 >= sp2 );
        assert(  p2 ==  p2 );
        assert(  p2 == sp2 );
        assert(  p2 !=  p1 );
        assert(  p2 != sp1 );
    }
    else
    {
        assert( sp2 <   p1 );
        assert( sp2 <  sp1 );
        assert( sp2 <=  p1 );
        assert( sp2 <=  p2 );
        assert( sp2 <= sp1 );
        assert( sp2 ==  p2 );
        assert( sp2 == sp2 );
        assert( sp2 !=  p1 );
        assert( sp2 != sp1 );
        assert( sp1 >   p2 );
        assert( sp1 >  sp2 );
        assert( sp1 >=  p2 );
        assert( sp1 >=  p1 );
        assert( sp1 >= sp2 );
        assert( sp1 ==  p1 );
        assert( sp1 == sp1 );
        assert( sp1 !=  p2 );
        assert( sp1 != sp2 );
        assert(  p2 <   p1 );
        assert(  p2 <  sp1 );
        assert(  p2 <=  p1 );
        assert(  p2 <= sp1 );
        assert(  p2 <= sp2 );
        assert(  p2 ==  p2 );
        assert(  p2 == sp2 );
        assert(  p2 !=  p1 );
        assert(  p2 != sp1 );
        assert(  p1 >   p2 );
        assert(  p1 >  sp2 );
        assert(  p1 >=  p2 );
        assert(  p1 >= sp2 );
        assert(  p1 >= sp1 );
        assert(  p1 ==  p1 );
        assert(  p1 == sp1 );
        assert(  p1 !=  p2 );
        assert(  p1 != sp2 );
    }
}

// ----------------------------------------------------------------------------

void DoStrongPtrDynamicCastTests( void )
{
    typedef ::Loki::StrongPtr< Feline, true, ::Loki::TwoRefCounts > FelineCountPtr;
    typedef ::Loki::StrongPtr< Tiger,  true, ::Loki::TwoRefCounts > TigerCountPtr;
    typedef ::Loki::StrongPtr< Lion,   true, ::Loki::TwoRefCounts > LionCountPtr;
    typedef ::Loki::StrongPtr< Dog,    true, ::Loki::TwoRefCounts > DogCountPtr;

    typedef ::Loki::StrongPtr< Feline, true, ::Loki::LockableTwoRefCounts > FelineLockPtr;
    typedef ::Loki::StrongPtr< Tiger,  true, ::Loki::LockableTwoRefCounts > TigerLockPtr;
    typedef ::Loki::StrongPtr< Lion,   true, ::Loki::LockableTwoRefCounts > LionLockPtr;
    typedef ::Loki::StrongPtr< Dog,    true, ::Loki::LockableTwoRefCounts > DogLockPtr;

    typedef ::Loki::StrongPtr< Feline, true, ::Loki::TwoRefLinks > FelineLinksPtr;
    typedef ::Loki::StrongPtr< Tiger,  true, ::Loki::TwoRefLinks > TigerLinksPtr;
    typedef ::Loki::StrongPtr< Lion,   true, ::Loki::TwoRefLinks > LionLinksPtr;
    typedef ::Loki::StrongPtr< Dog,    true, ::Loki::TwoRefLinks > DogLinksPtr;

    {
        Feline * feline = new Lion;
        Tiger * tiger = new Tiger;
        Lion * lion = new Lion;
        Dog * dog = new Dog;

        FelineCountPtr pFeline( feline );
        TigerCountPtr pTiger( tiger );
        LionCountPtr pLion( lion );
        DogCountPtr pDog( dog );

        // This is legal because C++ allows an automatic down-cast to public base class.
        pFeline = pLion;

#ifdef CHECK_TYPE_CAST
        pLion = pFeline; // Fails as the compiler cannot convert pointers in SmartPtr
#endif // CHECK_TYPE_CAST

        assert( pFeline );
        // Can up-cast from feline to lion only if the feline is a lion.
        pLion.DynamicCastFrom( pFeline );
        assert( pLion );
        assert( pLion == pFeline );

        // Can't cast from lion to tiger since although these are both types of felines,
        // they are not related to one another.
        pTiger.DynamicCastFrom( pLion );
        assert( !pTiger );

        // Can't cast from dog to lion since a dog is not a type of feline.
        pLion.DynamicCastFrom( pDog );
        assert( !pLion );

        pLion.DynamicCastFrom( pFeline );
        assert( pLion );
        assert( pLion == pFeline );

        // Can't cast from lion to dog since these animal types are not related.
        pDog.DynamicCastFrom( pLion );
        assert( !pDog );

        feline = new Lion;
        lion = new Lion;
        tiger = new Tiger;
        dog = new Dog;

        // Now do tests when converting from const pointers.
        const FelineCountPtr pcFeline( feline );
        const TigerCountPtr pcTiger( tiger );
        const LionCountPtr pcLion( lion );
        const DogCountPtr pcDog( dog );

        assert( pcFeline );
        // Can up-cast from feline to lion only if the feline is a lion.
        pLion.DynamicCastFrom( pcFeline );
        assert( pLion );
        assert( pLion == pcFeline );

        // Can't cast from lion to tiger since although these are both types of felines,
        // they are not related to one another.
        pTiger.DynamicCastFrom( pcLion );
        assert( !pTiger );

        // Can't cast from dog to lion since a dog is not a type of feline.
        pLion.DynamicCastFrom( pcDog );
        assert( !pLion );

        pLion.DynamicCastFrom( pcFeline );
        assert( pLion );
        assert( pLion == pcFeline );

        // Can't cast from lion to dog since these animal types are not related.
        pDog.DynamicCastFrom( pcLion );
        assert( !pDog );
    }

    {
        Feline * feline = new Lion;
        Tiger * tiger = new Tiger;
        Lion * lion = new Lion;
        Dog * dog = new Dog;

        FelineLockPtr pFeline( feline );
        TigerLockPtr pTiger( tiger );
        LionLockPtr pLion( lion );
        DogLockPtr pDog( dog );

        // This is legal because C++ allows an automatic down-cast to public base class.
        pFeline = pLion;

#ifdef CHECK_TYPE_CAST
        pLion = pFeline; // Fails as the compiler cannot convert pointers in SmartPtr
#endif // CHECK_TYPE_CAST

        assert( pFeline );
        // Can up-cast from feline to lion only if the feline is a lion.
        pLion.DynamicCastFrom( pFeline );
        assert( pLion );
        assert( pLion == pFeline );

        // Can't cast from lion to tiger since although these are both types of felines,
        // they are not related to one another.
        pTiger.DynamicCastFrom( pLion );
        assert( !pTiger );

        // Can't cast from dog to lion since a dog is not a type of feline.
        pLion.DynamicCastFrom( pDog );
        assert( !pLion );

        pLion.DynamicCastFrom( pFeline );
        assert( pLion );
        assert( pLion == pFeline );

        // Can't cast from lion to dog since these animal types are not related.
        pDog.DynamicCastFrom( pLion );
        assert( !pDog );

        feline = new Lion;
        tiger = new Tiger;
        lion = new Lion;
        dog = new Dog;

        // Now do tests when converting from const pointers.
        const FelineLockPtr pcFeline( feline );
        const TigerLockPtr pcTiger( tiger );
        const LionLockPtr pcLion( lion );
        const DogLockPtr pcDog( dog );

        assert( pcFeline );
        // Can up-cast from feline to lion only if the feline is a lion.
        pLion.DynamicCastFrom( pcFeline );
        assert( pLion );
        assert( pLion == pcFeline );

        // Can't cast from lion to tiger since although these are both types of felines,
        // they are not related to one another.
        pTiger.DynamicCastFrom( pcLion );
        assert( !pTiger );

        // Can't cast from dog to lion since a dog is not a type of feline.
        pLion.DynamicCastFrom( pcDog );
        assert( !pLion );

        pLion.DynamicCastFrom( pcFeline );
        assert( pLion );
        assert( pLion == pcFeline );

        // Can't cast from lion to dog since these animal types are not related.
        pDog.DynamicCastFrom( pcLion );
        assert( !pDog );
    }

    {
        Feline * feline = new Lion;
        Tiger * tiger = new Tiger;
        Lion * lion = new Lion;
        Dog * dog = new Dog;

        FelineLinksPtr pFeline( feline );
        TigerLinksPtr pTiger( tiger );
        LionLinksPtr pLion( lion );
        DogLinksPtr pDog( dog );

        // This is legal because C++ allows an automatic down-cast to public base class.
        pFeline = pLion;

#ifdef CHECK_TYPE_CAST
        pLion = pFeline; // Fails as the compiler cannot convert pointers in SmartPtr
#endif // CHECK_TYPE_CAST

        assert( pFeline );
        // Can up-cast from feline to lion only if the feline is a lion.
        pLion.DynamicCastFrom( pFeline );
        assert( pLion );
        assert( pLion == pFeline );

        // Can't cast from lion to tiger since although these are both types of felines,
        // they are not related to one another.
        pTiger.DynamicCastFrom( pLion );
        assert( !pTiger );

        // Can't cast from dog to lion since a dog is not a type of feline.
        pLion.DynamicCastFrom( pDog );
        assert( !pLion );

        pLion.DynamicCastFrom( pFeline );
        assert( pLion );
        assert( pLion == pFeline );

        // Can't cast from lion to dog since these animal types are not related.
        pDog.DynamicCastFrom( pLion );
        assert( !pDog );

        feline = new Lion;
        tiger = new Tiger;
        lion = new Lion;
        dog = new Dog;

        // Now do tests when converting from const pointers.
        const FelineLinksPtr pcFeline( feline );
        const TigerLinksPtr pcTiger( tiger );
        const LionLinksPtr pcLion( lion );
        const DogLinksPtr pcDog( dog );

        assert( pcFeline );
        // Can up-cast from feline to lion only if the feline is a lion.
        pLion.DynamicCastFrom( pcFeline );
        assert( pLion );
        assert( pLion == pcFeline );

        // Can't cast from lion to tiger since although these are both types of felines,
        // they are not related to one another.
        pTiger.DynamicCastFrom( pcLion );
        assert( !pTiger );

        // Can't cast from dog to lion since a dog is not a type of feline.
        pLion.DynamicCastFrom( pcDog );
        assert( !pLion );

        pLion.DynamicCastFrom( pcFeline );
        assert( pLion );
        assert( pLion == pcFeline );

        // Can't cast from lion to dog since these animal types are not related.
        pDog.DynamicCastFrom( pcLion );
        assert( !pDog );
    }
}

// ----------------------------------------------------------------------------


// GCC bug
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=38579

struct Policy
{
protected:
    Policy() {}
    Policy(const Policy&) {}
    int i;
};

template<int I, class P>
struct BugGcc :
    //protected P
    public P
{
    BugGcc() {}

    template<int I2, class P2>
    BugGcc(const BugGcc<I2, P2>& t) : P(t) {}
};

void foo()
{
    BugGcc<0, Policy> f1;
    BugGcc<1, Policy> f2(f1);

    // Policy members are still not public,
    // this will not compile:
    //int i = f1.i;
}

// ----------------------------------------------------------------------------

/// Use these typedefs to test DeleteArray policy.

typedef Loki::StrongPtr< Tiger, true, TwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteArray, DontPropagateConst >
    TigerArray_2RefCounts_ptr;

typedef Loki::StrongPtr< Tiger, true, TwoRefLinks, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteArray, DontPropagateConst >
    TigerArray_2RefLinks_ptr;

typedef Loki::StrongPtr< Tiger, true, LockableTwoRefCounts, DisallowConversion,
    AssertCheck, CantResetWithStrong, DeleteArray, DontPropagateConst >
    TigerArray_Lock2RefCounts_ptr;

// ----------------------------------------------------------------------------

void DoStrongArrayTests( void )
{
    cout << "Starting DoStrongArrayTests." << endl;

    {
        // test default construction.
        TigerArray_2RefCounts_ptr sp1;
        assert( !sp1 );
        assert( 0 == sp1.GetArrayCount() );

        // test assignment.
        sp1.Assign( new Tiger[ 8 ], 8 );
        assert( sp1 );
        assert( 8 == sp1.GetArrayCount() );
        sp1[ 0 ].SetStripes( 8 );
        sp1[ 1 ].SetStripes( 16 );
        sp1[ 2 ].SetStripes( 24 );
        sp1[ 3 ].SetStripes( 32 );
        sp1[ 4 ].SetStripes( 40);
        sp1[ 5 ].SetStripes( 48 );
        sp1[ 6 ].SetStripes( 56 );
        sp1[ 7 ].SetStripes( 64 );

        // test initialization construction.
        TigerArray_2RefCounts_ptr sp2( new Tiger[ 4 ], 4 );
        assert( sp2 );
        assert( 4 == sp2.GetArrayCount() );
        sp2[ 0 ].SetStripes( 5 );
        sp2[ 1 ].SetStripes( 10 );
        sp2[ 2 ].SetStripes( 15 );
        sp2[ 3 ].SetStripes( 20 );

        // test range checking.
        try
        {
            Tiger & p4 = sp2[ 4 ];
            (void)p4;
            assert( false );
        }
        catch ( const ::std::out_of_range & ex )
        {
            (void)ex;
            assert( true );
        }

        // test range checking.
        try
        {
            Tiger & p8 = sp1[ 8 ];
            (void)p8;
            assert( false );
        }
        catch ( const ::std::out_of_range & ex )
        {
            (void)ex;
            assert( true );
        }

        // test swap.
        sp2.Swap( sp1 );
        assert( sp1 );
        assert( sp2 );
        // test checking of item count.
        assert( 4 == sp1.GetArrayCount() );
        assert( 8 == sp2.GetArrayCount() );

        // test that operator[] returns reference to
        assert(  5 == sp1[ 0 ].GetStripes() );
        assert( 10 == sp1[ 1 ].GetStripes() );
        assert( 15 == sp1[ 2 ].GetStripes() );
        assert( 20 == sp1[ 3 ].GetStripes() );
        assert(  8 == sp2[ 0 ].GetStripes() );
        assert( 16 == sp2[ 1 ].GetStripes() );
        assert( 24 == sp2[ 2 ].GetStripes() );
        assert( 32 == sp2[ 3 ].GetStripes() );
        assert( 40 == sp2[ 4 ].GetStripes() );
        assert( 48 == sp2[ 5 ].GetStripes() );
        assert( 56 == sp2[ 6 ].GetStripes() );
        assert( 64 == sp2[ 7 ].GetStripes() );

        try
        {
            Tiger & p4 = sp1[ 4 ];
            (void)p4;
            assert( false );
        }
        catch ( const ::std::out_of_range & ex )
        {
            (void)ex;
            assert( true );
        }

        try
        {
            Tiger & p8 = sp2[ 8 ];
            (void)p8;
            assert( false );
        }
        catch ( const ::std::out_of_range & ex )
        {
            (void)ex;
            assert( true );
        }

        const TigerArray_2RefCounts_ptr sp3( sp1 );
        assert( sp3 == sp1 );
        assert( sp3.GetArrayCount() == sp1.GetArrayCount() );
        try
        {
            const Tiger & p4 = sp3[ 4 ];
            (void)p4;
            assert( false );
        }
        catch ( const ::std::out_of_range & ex )
        {
            (void)ex;
            assert( true );
        }

        const TigerArray_2RefCounts_ptr sp5( sp2 );
        assert( sp5 == sp2 );
        assert( sp5.GetArrayCount() == sp2.GetArrayCount() );
        try
        {
            const Tiger & p8 = sp5[ 8 ];
            (void)p8;
            assert( false );
        }
        catch ( const ::std::out_of_range & ex )
        {
            (void)ex;
            assert( true );
        }

        sp2 = sp1;
        assert( sp1 == sp2 );
        assert( sp3 == sp2 );
        assert( sp2.GetArrayCount() == sp1.GetArrayCount() );
        assert( sp2.GetArrayCount() == sp1.GetArrayCount() );
        assert( sp1 != sp5 );
        assert( sp2 != sp5 );
        assert( sp3 != sp5 );
        assert( sp1.GetArrayCount() != sp5.GetArrayCount() );
        assert( sp2.GetArrayCount() != sp5.GetArrayCount() );
        assert( sp3.GetArrayCount() != sp5.GetArrayCount() );
    }

    assert( BaseClass::AllDestroyed() );
    assert( !BaseClass::ExtraConstructions() );
    assert( !BaseClass::ExtraDestructions() );
    cout << "Finished DoStrongArrayTests." << endl;
}

// ----------------------------------------------------------------------------
