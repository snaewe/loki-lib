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

#include <stdexcept>
#include <iostream>
#include <vector>


#if !defined( nullptr )
    #define nullptr NULL
#endif

#if !defined( NULL )
    #define NULL 0
#endif

using namespace std;


// ----------------------------------------------------------------------------

/* This class has 2 invariants.  The this pointer may never equal NULL, and the
 value may not equal zero.
 */
class Thingy
{
public:

    static void ChangeThat( void );

    static unsigned int GetThat( void );

    explicit Thingy( unsigned int value );

    Thingy( const Thingy & that );

    Thingy & operator = ( const Thingy & that );

    ~Thingy( void );

    void Swap( Thingy & that );

    bool operator == ( const Thingy & that ) const;

    void DoSomethingEvil( void );

    unsigned int GetValue( void ) const;

    unsigned int DoSomething( bool doThrow ) const;

    void DoSomethingElse( void ) const;

    void AddCount( unsigned int count );

    unsigned int GetCount( unsigned int index ) const;

    void ClearCounts( void );

private:

    /// This is a static validator.
    static bool StaticIsValid( void );

    /// This is a per-instance validator.
    bool IsValid( void ) const;

    /// This can be used to validate pre-conditions and post-conditions.
    bool IsValidEmpty( void ) const;

    /// This can be used to validate pre-conditions and post-conditions.
    bool IsValidFull( void ) const;

    // These lines show how to declare checkers for non-static functions in a host class.
    typedef ::Loki::ContractChecker< Thingy, ::Loki::CheckForNoThrow  > CheckForNoThrow;
    typedef ::Loki::ContractChecker< Thingy, ::Loki::CheckForNoChangeOrThrow > CheckForNoChangeOrThrow;
    typedef ::Loki::ContractChecker< Thingy, ::Loki::CheckForNoChange > CheckForNoChange;
    typedef ::Loki::ContractChecker< Thingy, ::Loki::CheckForEquality > CheckForEquality;
    typedef ::Loki::ContractChecker< Thingy, ::Loki::CheckForNothing  > CheckInvariants;

    // These lines show how to declare checkers for static functions of a host class.
    typedef ::Loki::StaticChecker< ::Loki::CheckStaticForNoThrow  > CheckStaticForNoThrow;
    typedef ::Loki::StaticChecker< ::Loki::CheckStaticForNothing  > CheckStaticInvariants;

    typedef ::std::vector< unsigned int > IntBlock;

    static unsigned int s_value;

    unsigned int m_value;

    IntBlock m_counts;

};

unsigned int Thingy::s_value = 10;

// ----------------------------------------------------------------------------

// This example shows how static functions can use a no-throw checkers.
void Thingy::ChangeThat( void )
{
    CheckStaticForNoThrow checker( &Thingy::StaticIsValid );
    (void)checker;
    s_value--;
}

// ----------------------------------------------------------------------------

// This example shows how static functions can use an invariant checker.
unsigned int Thingy::GetThat( void )
{
    CheckStaticInvariants checker( &Thingy::StaticIsValid );
    (void)checker;
    return s_value;
}

// ----------------------------------------------------------------------------

// This example shows how ctors can use an invariant checker.
Thingy::Thingy( unsigned int value ) :
    m_value( value ),
    m_counts()
{
    CheckInvariants checker( this, &Thingy::IsValid );
    (void)checker;
}

// ----------------------------------------------------------------------------

Thingy::Thingy( const Thingy & that ) :
    m_value( that.m_value ),
    m_counts( that.m_counts )
{
    CheckInvariants checker( this, &Thingy::IsValid );
    (void)checker;
}

// ----------------------------------------------------------------------------

Thingy & Thingy::operator = ( const Thingy & that )
{
    CheckInvariants checker( this, &Thingy::IsValid );
    (void)checker;
    if ( &that != this )
    {
        Thingy temp( that );
        temp.Swap( *this );
    }
    return *this;
}

// ----------------------------------------------------------------------------

// A destructor really doesn't need a checker, but does need to confirm
// the object is valid at the start of the destructor.
Thingy::~Thingy( void )
{
    assert( IsValid() );
}

// ----------------------------------------------------------------------------

// A swap function gets 2 checkers - one for this, and another for that.
void Thingy::Swap( Thingy & that )
{
    CheckInvariants checker1( this, &Thingy::IsValid );
    (void)checker1;
    CheckInvariants checker2( &that, &Thingy::IsValid );
    (void)checker2;

    const IntBlock counts( m_counts );
    m_counts = that.m_counts;
    that.m_counts = counts;
    const unsigned int value = m_value;
    m_value = that.m_value;
    that.m_value = value;
}

// ----------------------------------------------------------------------------

bool Thingy::operator == ( const Thingy & that ) const
{
    return ( m_value == that.m_value );
}

// ----------------------------------------------------------------------------

void Thingy::DoSomethingEvil( void )
{
    m_value = 0;
}

// ----------------------------------------------------------------------------

// This example shows how to use the no-throw checker.
unsigned int Thingy::GetValue( void ) const
{
    CheckForNoThrow checker( this, &Thingy::IsValid );
    (void)checker;
    return m_value;
}

// ----------------------------------------------------------------------------

// This example shows how to use the equality checker.
unsigned int Thingy::DoSomething( bool doThrow ) const
{
    CheckForEquality checker( this, &Thingy::IsValid );
    (void)checker;
    if ( doThrow )
        throw ::std::logic_error( "Test Exception." );
    return m_value;
}

// ----------------------------------------------------------------------------

// This example shows how to use the no-change checker.
void Thingy::DoSomethingElse( void ) const
{
    CheckForNoChange checker( this, &Thingy::IsValid );
    (void)checker;
}

// ----------------------------------------------------------------------------

void Thingy::AddCount( unsigned int count )
{
    // This function's checker cares about class invariants and post-conditions,
    // but does not need to check pre-conditions, so it passes in a nullptr for
    // the pre-condition validator.  Ths post-condition validator just makes sure
    // the container has at least 1 element.
    CheckInvariants checker( this, &Thingy::IsValid, nullptr, &Thingy::IsValidFull );
    m_counts.push_back( count );
}

// ----------------------------------------------------------------------------

unsigned int Thingy::GetCount( unsigned int index ) const
{
    // This function's checker cares about class invariants and both the pre- and
    // post-conditions, so it passes in pointers for all 3 validators.  The pre-
    // and post-conditions are both about making sure the container is not empty.
    CheckForNoChangeOrThrow checker( this, &Thingy::IsValid, &Thingy::IsValidFull, &Thingy::IsValidFull );
    if ( m_counts.size() <= index )
        return 0;
    const unsigned int count = m_counts[ index ];
    return count;
}

// ----------------------------------------------------------------------------

void Thingy::ClearCounts( void )
{
    // This function's checker cares about class invariants and post-conditions,
    // but does not need to check pre-conditions, so it passes in a nullptr for
    // the pre-condition validator.  Ths post-condition validator just makes sure
    // the container has no elements.
    CheckForNoThrow checker( this, &Thingy::IsValid, nullptr, &Thingy::IsValidEmpty );
    m_counts.clear();
}

// ----------------------------------------------------------------------------

// This is a static validator.
bool Thingy::StaticIsValid( void )
{
    assert( s_value != 0 );
    return true;
}

// ----------------------------------------------------------------------------

// This is a per-instance validator.
bool Thingy::IsValid( void ) const
{
    assert( nullptr != this );
    assert( m_value != 0 );
    return true;
}

// ----------------------------------------------------------------------------

bool Thingy::IsValidEmpty( void ) const
{
    assert( nullptr != this );
    assert( m_counts.size() == 0 );
    return true;
}

// ----------------------------------------------------------------------------

bool Thingy::IsValidFull( void ) const
{
    assert( nullptr != this );
    assert( m_counts.size() != 0 );
    return true;
}

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

void ThrowTest( void )
{
    Thingy thingy( 10 );
    throw ::std::logic_error( "Will Thingy assert during an exception?" );
}

// ----------------------------------------------------------------------------

int main( unsigned int argc, const char * const argv[] )
{

    try
    {
        cout << "Just before call to ThrowTest." << endl;
        ThrowTest();
        cout << "Just after call to ThrowTest." << endl;
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( ... )
    {
        cout << "Caught an exception!" << endl;
    }

    unsigned int count = 0;
    try
    {
        cout << "Running basic tests with Thingy." << endl;
        // First do some tests on class member functions.
        Thingy t1( 1 );
        t1.DoSomething( false );
        Thingy t2( 2 );
        t2.DoSomething( true );
        cout << "Done with basic tests with Thingy." << endl;
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( ... )
    {
        cout << "Caught an exception!" << endl;
    }

    try
    {
        Thingy t1( 1 );
        cout << "Now running tests with Thingy counts." << endl;
        // These lines will exercise the functions with pre- and post-conditions.
        t1.AddCount( 11 );
        t1.AddCount( 13 );
        t1.AddCount( 17 );
        t1.AddCount( 19 );
        count = t1.GetCount( 3 );
        assert( count == 19 );
        count = t1.GetCount( 0 );
        assert( count == 11 );
        t1.ClearCounts();
        cout << "Done with tests with Thingy counts." << endl;
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( ... )
    {
        cout << "Caught an exception!" << endl;
    }

    try
    {
        cout << "Now run tests on static member functions" << endl;
        // Next do some tests with static member functions.
        Thingy::ChangeThat();
        const unsigned int value = Thingy::GetThat();
        assert( value != 0 );
        cout << "Done with tests on static member functions" << endl;
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( ... )
    {
        cout << "Caught an exception!" << endl;
    }

    try
    {
        cout << "Now run test on a standalone function." << endl;
        // Then do a test with a standalone function.
        DoSomething();
        cout << "Done with test on a standalone function." << endl;
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
    }
    catch ( ... )
    {
        cout << "Caught an exception!" << endl;
    }

    cout << "All done!" << endl;
    return 0;
}

// ----------------------------------------------------------------------------
