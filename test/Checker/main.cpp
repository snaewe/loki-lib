////////////////////////////////////////////////////////////////////////////////
//
// Test program for The Loki Library
// Copyright (c) 2008, 2009 Richard Sposato
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

    void DoSomethingElse( unsigned int count, bool doThrow );

    void AddCount( unsigned int count );

    unsigned int GetCount( unsigned int index ) const;

    void ClearCounts( void );

private:

    class Memento
    {
    public:
        explicit Memento( const Thingy & t ) : m_count( t.m_counts.size() ) {}
        bool operator == ( const Thingy & t ) const
        {
            return ( m_count == t.m_counts.size() );
        }
        unsigned int m_count;
    };

    /// This is a static validator.
    static bool StaticIsValid( void );

    /// This is a per-instance validator.
    bool IsValid( void ) const;

    /// This can be used to validate pre-conditions and post-conditions.
    bool IsValidEmpty( void ) const;

    /// This can be used to validate pre-conditions and post-conditions.
    bool IsValidFull( void ) const;

    // This shows how to declare checkers for non-static functions in a host class.
    typedef ::Loki::CheckFor< Thingy > CheckFor;

    // This shows how to declare checkers for non-static functions in a host class.
    typedef ::Loki::CheckFor< Thingy, Memento > CheckMementoFor;

    // This shows how to declare checkers for static functions of a host class.
    typedef ::Loki::CheckStaticFor CheckStaticFor;

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
    CheckStaticFor::NoThrow checker( &Thingy::StaticIsValid );
    (void)checker;
    s_value--;
}

// ----------------------------------------------------------------------------

// This example shows how static functions can use an invariant checker.
unsigned int Thingy::GetThat( void )
{
    CheckStaticFor::Invariants checker( &Thingy::StaticIsValid );
    (void)checker;
    return s_value;
}

// ----------------------------------------------------------------------------

// This example shows how ctors can use an invariant checker.
Thingy::Thingy( unsigned int value ) :
    m_value( value ),
    m_counts()
{
    CheckFor::Invariants checker( this, &Thingy::IsValid );
    (void)checker;
}

// ----------------------------------------------------------------------------

Thingy::Thingy( const Thingy & that ) :
    m_value( that.m_value ),
    m_counts( that.m_counts )
{
    CheckFor::Invariants checker( this, &Thingy::IsValid );
    (void)checker;
}

// ----------------------------------------------------------------------------

Thingy & Thingy::operator = ( const Thingy & that )
{
    CheckFor::Invariants checker( this, &Thingy::IsValid );
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
    CheckFor::Invariants checker1( this, &Thingy::IsValid );
    (void)checker1;
    CheckFor::Invariants checker2( &that, &Thingy::IsValid );
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
    CheckFor::NoThrow checker( this, &Thingy::IsValid );
    (void)checker;
    return m_value;
}

// ----------------------------------------------------------------------------

// This example shows how to use the equality checker.
unsigned int Thingy::DoSomething( bool doThrow ) const
{
    CheckMementoFor::NoChange checker( this, &Thingy::IsValid );
    (void)checker;
    if ( doThrow )
        throw ::std::logic_error( "Test Exception." );
    return m_value;
}

// ----------------------------------------------------------------------------

// This example shows how to use the no-change checker.
void Thingy::DoSomethingElse( unsigned int count, bool doThrow )
{
    CheckMementoFor::NoChangeOnThrow checker( this, &Thingy::IsValid );
    (void)checker;

	IntBlock counts( m_counts );
	counts.push_back( count );
    if ( doThrow )
        throw ::std::logic_error( "Test Exception." );
	counts.swap( m_counts );
}

// ----------------------------------------------------------------------------

void Thingy::AddCount( unsigned int count )
{
    // This function's checker cares about class invariants and post-conditions,
    // but does not need to check pre-conditions, so it passes in a nullptr for
    // the pre-condition validator.  Ths post-condition validator just makes sure
    // the container has at least 1 element.
    CheckFor::Invariants checker( this, &Thingy::IsValid, nullptr, &Thingy::IsValidFull );
    m_counts.push_back( count );
}

// ----------------------------------------------------------------------------

unsigned int Thingy::GetCount( unsigned int index ) const
{
    // This function's checker cares about class invariants and both the pre- and
    // post-conditions, so it passes in pointers for all 3 validators.  The pre-
    // and post-conditions are both about making sure the container is not empty.
    CheckMementoFor::NoThrowOrChange checker( this, &Thingy::IsValid, &Thingy::IsValidFull, &Thingy::IsValidFull );
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
    CheckFor::NoThrow checker( this, &Thingy::IsValid, nullptr, &Thingy::IsValidEmpty );
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

void DoSomething( void )
{
    // This example shows how to use a checker in a stand-alone function.
    ::Loki::CheckStaticFor::NoThrow checker( &AllIsValid );
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

int main( int argc, const char * const argv[] )
{

    (void)argc;
    (void)argv;
    cout << "These tests confirm the Checker exception policies work properly." << endl;
    cout << "The tests will throw exceptions. If you see any assertions, the test failed." << endl;

    try
    {
        cout << "Just before call to ThrowTest." << endl;
        ThrowTest();
        cout << "Just after call to ThrowTest." << endl;
		assert( false );
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught a logic_error exception!  " << ex.what() << endl;
		assert( true );
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught a standard exception!  " << ex.what() << endl;
		assert( false );
    }
    catch ( ... )
    {
        cout << "Caught an unknown exception!" << endl;
		assert( false );
    }

    unsigned int count = 0;
    try
    {
        cout << "Starting test of CheckFor::NoChange policy." << endl;
        // First do some tests on class member functions.
        Thingy t1( 1 );
        t1.DoSomething( false );
		assert( true );
        Thingy t2( 2 );
        t2.DoSomething( true );
		assert( false );
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
		assert( true );
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught a standard exception!  " << ex.what() << endl;
    }
    catch ( ... )
    {
        cout << "Caught an unknown exception!" << endl;
    }
	cout << "Done testing CheckFor::NoChange policy." << endl;

    try
    {
        cout << "Starting test of CheckFor::NoChangeOnThrow policy." << endl;
        Thingy t1( 1 );
        t1.DoSomethingElse( 3, false );
        Thingy t2( 2 );
        t2.DoSomethingElse( 5, true );
		assert( false );
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught an exception!  " << ex.what() << endl;
		assert( true );
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught a standard exception!  " << ex.what() << endl;
		assert( false );
    }
    catch ( ... )
    {
        cout << "Caught an unknown exception!" << endl;
		assert( false );
    }
	cout << "Finished test of CheckFor::NoChangeOnThrow policy." << endl;

    try
    {
        cout << "Starting tests of CheckFor::Invariants and CheckFor::NoThrowOrChange policy." << endl;
        Thingy t1( 1 );
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
        cout << "Caught a logic_error exception!  " << ex.what() << endl;
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught a standard exception!  " << ex.what() << endl;
		assert( false );
    }
    catch ( ... )
    {
        cout << "Caught an unknown exception!" << endl;
		assert( false );
    }
	cout << "Finished tests of CheckFor::Invariants and CheckFor::NoThrowOrChange policy." << endl;

    try
    {
        cout << "Starting test of CheckStaticFor::Invariants policy." << endl;
        // Next do some tests with static member functions.
        Thingy::ChangeThat();
        const unsigned int value = Thingy::GetThat();
        assert( value != 0 );
		assert( true );
    }
    catch ( const ::std::logic_error & ex )
    {
        cout << "Caught a logic_error exception!  " << ex.what() << endl;
		assert( false );
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught a standard exception!  " << ex.what() << endl;
		assert( false );
    }
    catch ( ... )
    {
        cout << "Caught an unknown exception!" << endl;
		assert( false );
    }
	cout << "Finished test of CheckStaticFor::Invariants policy." << endl;

    try
    {
        cout << "Starting test of CheckStaticFor::NoThrow policy." << endl;
        // Then do a test with a standalone function.
        DoSomething();
		assert( true );
    }
    catch ( const ::std::exception & ex )
    {
        cout << "Caught a standard exception!  " << ex.what() << endl;
		assert( false );
    }
    catch ( ... )
    {
        cout << "Caught an unknown exception!" << endl;
		assert( false );
    }
	cout << "Finished test of CheckStaticFor::NoThrow policy." << endl;

    cout << "All done!  If you see this line, and no assertions failed, then the test passed!" << endl;
    return 0;
}

// ----------------------------------------------------------------------------
