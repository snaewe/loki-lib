////////////////////////////////////////////////////////////////////////////////
//
// Test program for The Loki Library
// Copyright (c) 2008 Richard Sposato
// The copyright on this file is protected under the terms of the MIT license.
//
// Permission to use, copy, modify, distribute and sell this software for any 
// purpose is hereby granted without fee, provided that the above copyright 
// notice appear in all copies and that both that copyright notice and this 
// permission notice appear in supporting documentation.
//
// The author makes no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
//
////////////////////////////////////////////////////////////////////////////////

// $Id$

/// @file main.cpp This provides examples on how to use Loki's Checker facility.


// ----------------------------------------------------------------------------

#include "../../include/loki/Checker.h"


#if !defined( nullptr )
    #define nullptr NULL
#endif

#if !defined( NULL )
    #define NULL 0
#endif


// ----------------------------------------------------------------------------

/* This class has 2 invariants.  The this pointer may never equal NULL, and the
 value may not equal zero.
 */
class Thingy
{
public:

    // This example shows how static functions can use a no-throw checkers.
    static void ChangeThat( void )
    {
        CheckStaticForNoThrow checker( &Thingy::StaticIsValid );
        (void)checker;
        s_value--;
    }

    // This example shows how static functions can use an invariant checkers.
    static unsigned int GetThat( void )
    {
        CheckStaticInvariants checker( &Thingy::StaticIsValid );
        (void)checker;
        return s_value;
    }

    // This example shows how ctors can use an invariant checker.
    explicit Thingy( unsigned int value ) : m_value( value )
    {
        CheckInvariants checker( this, &Thingy::IsValid );
        (void)checker;
        checker.Check();
    }

    // A destructor really doesn't need a checker, but does need to confirm
    // the object is valid at the start of the destructor.
    ~Thingy( void )
    {
        assert( IsValid() );
    }

    bool operator == ( const Thingy & that ) const
    {
        return ( m_value == that.m_value );
    }

    void DoSomethingEvil( void )
    {
        m_value = 0;
    }

    // This example shows how to use the no-throw checker.
    unsigned int GetValue( void ) const
    {
        CheckForNoThrow checker( this, &Thingy::IsValid );
        (void)checker;
        return m_value;
    }

    // This example shows how to use the equality checker.
    unsigned int DoSomething( bool doThrow ) const
    {
        CheckForEquality checker( this, &Thingy::IsValid );
        (void)checker;
        if ( doThrow )
            throw ::std::exception( "Test Exception." );
        return m_value;
    }

    // This example shows how to use the no-change checker.
    void DoSomethingElse( void ) const
    {
        CheckForNoChange checker( this, &Thingy::IsValid );
        (void)checker;
    }

private:

    // This is a static validator.
    static bool StaticIsValid( void )
    {
        assert( s_value != 0 );
        return true;
    }

    // This is a per-instance validator.
    bool IsValid( void ) const
    {
        assert( nullptr != this );
        assert( m_value != 0 );
        return true;
    }

    // These lines show how to declare checkers for non-static functions in a host class.
    typedef ::Loki::Checker< Thingy, ::Loki::CheckForNoThrow  > CheckForNoThrow;
    typedef ::Loki::Checker< Thingy, ::Loki::CheckForNoChangeOrThrow > CheckForNoChangeOrThrow;
    typedef ::Loki::Checker< Thingy, ::Loki::CheckForNoChange > CheckForNoChange;
    typedef ::Loki::Checker< Thingy, ::Loki::CheckForEquality > CheckForEquality;
    typedef ::Loki::Checker< Thingy, ::Loki::CheckForNothing  > CheckInvariants;

    // These lines show how to declare checkers for static functions of a host class.
    typedef ::Loki::StaticChecker< ::Loki::CheckStaticForNoThrow  > CheckStaticForNoThrow;
    typedef ::Loki::StaticChecker< ::Loki::CheckStaticForNothing  > CheckStaticInvariants;

    static unsigned int s_value;

    unsigned int m_value;

};

unsigned int Thingy::s_value = 10;

// ----------------------------------------------------------------------------

// This is a validator function called by checkers inside standalone functions.
bool AllIsValid( void )
{
    assert( Thingy::GetThat() != 0 );
    return true;
}

// ----------------------------------------------------------------------------

// These lines show how to declare checkers for standalone functions.
typedef ::Loki::StaticChecker< ::Loki::CheckStaticForNoThrow  > CheckStaticForNoThrow;
typedef ::Loki::StaticChecker< ::Loki::CheckStaticForNothing  > CheckStaticInvariants;

// ----------------------------------------------------------------------------

void DoSomething( void )
{
    // This example shows how to use a checker in a stand-alone function.
    CheckStaticForNoThrow checker( &AllIsValid );
    (void)checker;
    Thingy::ChangeThat();
}

// ----------------------------------------------------------------------------

int main( unsigned int argc, const char * const argv[] )
{

    try
    {
        // First do some tests on class member functions.
        Thingy t1( 1 );
        t1.DoSomething( false );
        Thingy t2( 2 );
        t2.DoSomething( true );

        // Next do some tests with static member functions.
        Thingy::ChangeThat();
        const unsigned int value = Thingy::GetThat();
        assert( value != 0 );

        // Then do a test with a standalone function.
        DoSomething();
    }
    catch ( ... )
    {
    }

    return 0;
}

// ----------------------------------------------------------------------------
