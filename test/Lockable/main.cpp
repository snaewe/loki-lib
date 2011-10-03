////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2011 by Rich Sposato
//
// This code does not accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#define LOKI_OBJECT_LEVEL_THREADING
#include <loki/Threads.h>

#include <vector>
#include <iostream>

#include <loki/SafeFormat.h>
#include "ThreadPool.hpp"


using namespace std;

static unsigned int g = 0;

#define  DO for(int i=0; i<10000000; i++) g++;

static const unsigned int ThreadCount = 10;

static const unsigned int ObjectCount = 50;

static const unsigned int ClassCount = 5;

static unsigned int FailCounts[ ThreadCount ];

// ----------------------------------------------------------------------------

class LockableObject : public ::Loki::ObjectLevelLockable< LockableObject >
{
public:

    typedef ::Loki::ObjectLevelLockable< LockableObject > BaseClass;

    explicit LockableObject( unsigned int index ) :
        BaseClass(), m_index( index ), m_value( ObjectCount ) {}

    ~LockableObject( void ) {}

    unsigned int GetIndex( void ) const { return m_index; }

    unsigned int GetValue( void ) const { return m_value; }

    void SetValue( unsigned int value ) { m_value = value; }

    void DoSomething( void );

    void Print( unsigned int threadIndex );

private:

    const unsigned int m_index;
    unsigned int m_value;

};

// ----------------------------------------------------------------------------

void LockableObject::DoSomething( void)
{
    assert( NULL != this );
    DO;
}

// ----------------------------------------------------------------------------

void LockableObject::Print( unsigned int threadIndex )
{
    assert( NULL != this );
    const char * result = ( threadIndex != m_value ) ? "Mismatch!" : "";
    ::std::string message;
    ::Loki::SPrintf( message, "Object: [%u]  Thread: [%u]  Value: [%u]  %s\n" )
        ( m_index )( threadIndex )( m_value )( result );
    cout << message;
}

// ----------------------------------------------------------------------------

typedef ::std::vector< LockableObject * > LockableObjects;

LockableObjects & GetLockableObjects( void )
{
    static LockableObjects objects;
    return objects;
}

// ----------------------------------------------------------------------------

LockableObject * GetLockableObject( unsigned int index )
{
    LockableObjects & objects = GetLockableObjects();
    if ( objects.size() <= index )
        return NULL;

    LockableObject * object = objects[ index ];
    return object;
}

// ----------------------------------------------------------------------------

void * RunObjectTest( void * p )
{
    const unsigned int threadIndex = reinterpret_cast< unsigned int >( p );
    assert( threadIndex < ThreadCount );

    unsigned int failCount = 0;
    for ( unsigned int ii = 0; ii < ObjectCount; ++ii )
    {
        LockableObject * object = GetLockableObject( ii );
        assert( NULL != object );
        LockableObject::Lock lock( *object );
        (void)lock;
        object->SetValue( threadIndex );
        object->DoSomething();
        object->Print( threadIndex );
        object->DoSomething();
        const unsigned int value = object->GetValue();
        if ( value != threadIndex )
            ++failCount;
    }

    FailCounts[ threadIndex ] = failCount;

    return NULL;
}

// ----------------------------------------------------------------------------

void DoObjectLockTest( void )
{
    cout << "Starting DoObjectLockTest" << endl;

    LockableObjects & objects = GetLockableObjects();
    objects.reserve( ObjectCount );
    for ( unsigned int ii = 0; ii < ObjectCount; ++ii )
    {
        LockableObject * object = new LockableObject( ii );
        objects.push_back( object );
    }

    {
        ThreadPool pool;
        pool.Create( ThreadCount, &RunObjectTest );
        pool.Start();
        pool.Join();
    }

    unsigned int totalFails = 0;
    for ( unsigned int ii = 0; ii < ThreadCount; ++ii )
    {
        const unsigned int failCount = FailCounts[ ii ];
        ::Loki::Printf( "Thread: [%u]  Failures: [%u]\n" )( ii )( failCount );
        totalFails += failCount;
    }
    const char * result = ( 0 == totalFails ) ? "Passed" : "FAILED";

    cout << "Finished DoObjectLockTest.  Total Fails: " << totalFails << "  Result: "
        << result << endl;
}

// ----------------------------------------------------------------------------

class LockableClass : public ::Loki::ClassLevelLockable< LockableClass >
{
public:

    typedef ::Loki::ClassLevelLockable< LockableClass > BaseClass;

    explicit LockableClass( unsigned int index ) : BaseClass(), m_index( index ) {}

    ~LockableClass( void ) {}

    unsigned int GetIndex( void ) const { return m_index; }

    void Print( unsigned int threadIndex );

private:
    /// Assignment operator is not implemented.
    LockableClass & operator = ( const LockableClass & );
    const unsigned int m_index;
};

// ----------------------------------------------------------------------------

void LockableClass::Print( unsigned int threadIndex )
{
    assert( NULL != this );
    DO; ::Loki::Printf( "%u: %u: -----\n" )( m_index )( threadIndex );
    DO; ::Loki::Printf( "%u: %u: ----\n" )( m_index )( threadIndex );
    DO; ::Loki::Printf( "%u: %u: ---\n" )( m_index )( threadIndex );
    DO; ::Loki::Printf( "%u: %u: --\n" )( m_index )( threadIndex );
    DO; ::Loki::Printf( "%u: %u: -\n" )( m_index )( threadIndex );
    DO; ::Loki::Printf( "%u: %u:\n" )( m_index )( threadIndex );
}

// ----------------------------------------------------------------------------

typedef ::std::vector< LockableClass * > LockableClasses;

LockableClasses & GetLockableClasses( void )
{
    static LockableClasses objects;
    return objects;
}

// ----------------------------------------------------------------------------

LockableClass * GetLockableClass( unsigned int index )
{
    LockableClasses & objects = GetLockableClasses();
    if ( objects.size() <= index )
        return NULL;

    LockableClass * object = objects[ index ];
    return object;
}

// ----------------------------------------------------------------------------

void * RunClassTest( void * p )
{
    const unsigned int threadIndex = reinterpret_cast< unsigned int >( p );
    assert( threadIndex < ThreadCount );

    for ( unsigned int ii = 0; ii < ClassCount; ++ii )
    {
        LockableClass * object = GetLockableClass( ii );
        assert( NULL != object );
        LockableClass::Lock lock( *object );
        (void)lock;
        object->Print( threadIndex );
    }

    return NULL;
}

// ----------------------------------------------------------------------------

void DoClassLockTest( void )
{
    cout << "Starting DoClassLockTest" << endl;

    LockableClasses & objects = GetLockableClasses();
    objects.reserve( ClassCount );
    for ( unsigned int ii = 0; ii < ClassCount; ++ii )
    {
        LockableClass * object = new LockableClass( ii );
        objects.push_back( object );
    }

    {
        ThreadPool pool;
        pool.Create( ThreadCount, &RunClassTest );
        pool.Start();
        pool.Join();
    }

    cout << "Finished DoClassLockTest" << endl;
}

// ----------------------------------------------------------------------------

int main( int argc, const char * const argv[] )
{
    (void)argc;
    (void)argv;
    char ender;

    DoObjectLockTest();
    cout << "Press <Enter> key to continue. ";
    cin.get( ender );

    DoClassLockTest();
    cout << "Press <Enter> key to finish. ";
    cin.get( ender );

    return 0;
}

// ----------------------------------------------------------------------------
