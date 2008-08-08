////////////////////////////////////////////////////////////////////////////////
//
// Part of LevelMutex test program for The Loki Library
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


// ----------------------------------------------------------------------------

#include "Thing.hpp"

#include <assert.h>
#include <algorithm>
#include <functional>

#include <SafeFormat.h>
#include <LockingPtr.h>

using namespace ::Loki;


// ----------------------------------------------------------------------------

typedef ::Loki::LockingPtr< Thing, volatile SleepMutex > SingleThingLocker;

typedef ::Loki::LockingPtr< const Thing, volatile SleepMutex > ConstSingleThingLocker;

typedef ::Loki::LockingPtr< LevelThing, volatile SleepMutex > LevelThingLocker;

typedef ::Loki::LockingPtr< const LevelThing, volatile SleepMutex > ConstLevelThingLocker;

typedef ::Loki::LockingPtr< SomeThing, volatile SleepMutex > SomeThingLocker;

typedef ::Loki::LockingPtr< const SomeThing, volatile SleepMutex > ConstSomeThingLocker;

volatile Thing * Thing::s_thing = NULL;

Thing::ThingPool Thing::s_pool;

LevelThing::LevelThingPool LevelThing::s_pool;

TestResults * TestResults::s_instance = NULL;

MultiLevelPool::MultiThingPool MultiLevelPool::s_pool;


// ----------------------------------------------------------------------------

ExceptionTossingMutex::ExceptionTossingMutex( unsigned int level ) :
#if defined( _MSC_VER )
    SleepLevelMutex( level ),
#else
    SleepLevelMutex( level ),
#endif
    m_policy( ExceptionTossingMutex::Never )
{
    assert( NULL != this );
#if defined( _MSC_VER )
    SetSleepTime( 5 );
    SetWakable( false );
#else
    SetSleepTime( 1 );
#endif
}

// ----------------------------------------------------------------------------

ExceptionTossingMutex::~ExceptionTossingMutex( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

MutexErrors::Type ExceptionTossingMutex::Lock( void ) volatile
{
    if ( m_policy != ExceptionTossingMutex::Never )
    {
        bool toss = ( m_policy == ExceptionTossingMutex::Always );
        if ( !toss )
        {
            const unsigned int number = ( ::rand() % 16 );
            toss = ( 0 == number );
        }
        if ( toss )
        {
            throw MutexException( "Just a test exception from DoLock.",
                GetLevel(), ::Loki::MutexErrors::OtherError );
        }
    }
    return SleepLevelMutex::Lock();
}

// ----------------------------------------------------------------------------

MutexErrors::Type ExceptionTossingMutex::TryLock( void ) volatile
{
    if ( m_policy != ExceptionTossingMutex::Never )
    {
        bool toss = ( m_policy == ExceptionTossingMutex::Always );
        if ( !toss )
        {
            const unsigned int number = ( ::rand() % 16 );
            toss = ( 0 == number );
        }
        if ( toss )
        {
            throw MutexException( "Just a test exception from DoLock.",
                GetLevel(), ::Loki::MutexErrors::OtherError );
        }
    }
    return SleepLevelMutex::TryLock();
}

// ----------------------------------------------------------------------------

MutexErrors::Type ExceptionTossingMutex::Unlock( void ) volatile
{
    if ( m_policy != ExceptionTossingMutex::Never )
    {
        bool toss = ( m_policy == ExceptionTossingMutex::Always );
        if ( !toss )
        {
            const unsigned int number = ( ::rand() % 16 );
            toss = ( 0 == number );
        }
        if ( toss )
        {
            throw MutexException( "Just a test exception from DoLock.",
                GetLevel(), ::Loki::MutexErrors::OtherError );
        }
    }
    return SleepLevelMutex::Unlock();
}

// ----------------------------------------------------------------------------

bool TestResults::Create( unsigned int threadCount )
{
    if ( NULL != s_instance )
        return true;
    s_instance = new TestResults( threadCount );
    return ( NULL != s_instance );
}

// ----------------------------------------------------------------------------

void TestResults::Destroy( void )
{
    delete s_instance;
    s_instance = NULL;
}

// ----------------------------------------------------------------------------

TestResults::TestResults( unsigned int threadCount ) :
    m_results()
{
    assert( NULL != this );
    Reset( threadCount );
}

// ----------------------------------------------------------------------------

TestResults::~TestResults( void )
{
    assert( NULL != this );
    assert( this == s_instance );
}

// ----------------------------------------------------------------------------

void TestResults::Reset( unsigned int threadCount )
{
    assert( NULL != this );
    TestResult result;
    result.m_total = 0;
    result.m_fails = 0;
    m_results.clear();
    m_results.resize( threadCount, result );
}

// ----------------------------------------------------------------------------

void TestResults::SetResult( unsigned int threadIndex, unsigned int total,
    unsigned int fails )
{
    assert( NULL != this );
    assert( this == s_instance );

    if ( threadIndex <= m_results.size() )
    {
        m_results[ threadIndex ].m_total = total;
        m_results[ threadIndex ].m_fails = fails;
    }
}

// ----------------------------------------------------------------------------

void TestResults::OutputResults( void )
{
    assert( NULL != this );
    assert( this == s_instance );

    bool passed = true;
    const unsigned int count = m_results.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        Printf( "Thread: %d \t total: %d \t fails: %d\n" )( ii )
            ( m_results[ ii ].m_total )
            ( m_results[ ii ].m_fails );
        if ( m_results[ ii ].m_fails != 0 )
            passed = false;
        m_results[ ii ].m_total = 0;
        m_results[ ii ].m_fails = 0;
    }
    const char * message = ( passed ) ? "Passed!" : "FAILED!";
    Printf( "Value Test Result: %s\n" )( message );
}

// ----------------------------------------------------------------------------

TestResults::TestResult::TestResult( void ) :
    m_total( 0 ),
    m_fails( 0 )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

TestResults::TestResult::TestResult( const TestResult & that ) :
    m_total( that.m_total ),
    m_fails( that.m_fails )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

TestResults::TestResult::~TestResult( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

void GoToSleep( unsigned int milliSeconds )
{
#if defined( _MSC_VER )
    ::SleepEx( milliSeconds, true );
#elif ( __GNUC__ )
    unsigned int seconds = milliSeconds / 1000;
    if ( 0 == seconds )
        seconds = 1;
    ::_sleep( seconds );
#else
    #error "Find out if your compiler supports a sleep command and add it here."
#endif
}

// ----------------------------------------------------------------------------

void Thing::Init( unsigned int value )
{
    if ( NULL != s_thing )
        return;
    s_thing = new Thing( value );
}

// ----------------------------------------------------------------------------

void Thing::Destroy( void )
{
    if ( NULL == s_thing )
        return;
    delete s_thing;
    s_thing = NULL;
}

// ----------------------------------------------------------------------------

volatile Thing * Thing::GetFromPool( unsigned int index )
{
    if ( s_pool.size() <= index )
        return NULL;
    volatile Thing * thing = s_pool[ index ];
    return thing;
}

// ----------------------------------------------------------------------------

unsigned int Thing::GetPoolSize( void )
{
    return s_pool.size();
}

// ----------------------------------------------------------------------------

bool Thing::MakePool( unsigned int count )
{
    if ( ( 0 == count ) || ( 100 < count ) )
        return false;

    DestroyPool();
    s_pool.reserve( count );
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile Thing * thing = new Thing( ii );
        s_pool.push_back( thing );
    }

    return true;
}

// ----------------------------------------------------------------------------

void Thing::DestroyPool( void )
{
    const unsigned int count = s_pool.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile Thing * thing = s_pool[ ii ];
        delete thing;
    }
    s_pool.clear();
}

// ----------------------------------------------------------------------------

Thing::Thing( unsigned int value ) :
    m_mutex( 10 ),
    m_value( value )
{
    assert( NULL != this );
#if defined( _MSC_VER )
    m_mutex.GetMutexPolicy().SetSleepTime( 5 );
    m_mutex.GetMutexPolicy().SetWakable( false );
#else
    m_mutex.GetMutexPolicy().SetSleepTime( 1 );
#endif
}

// ----------------------------------------------------------------------------

Thing::~Thing( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

void Thing::Print( unsigned int value, unsigned int index, unsigned int startSize ) const volatile
{
    assert( NULL != this );
    volatile SleepMutex & mutex = const_cast< volatile SleepMutex & >( m_mutex );
    ConstSingleThingLocker pSafeThis( *this, mutex );
    pSafeThis->Print( value, index, startSize );
}

// ----------------------------------------------------------------------------

void Thing::Print( unsigned int value, unsigned int index, unsigned int startSize ) const
{
    assert( NULL != this );
    switch ( startSize )
    {
        default:
        case 16: ::GoToSleep( 3 ); Printf("%d: %d: ----------------\n")( value )( index );
        case 15: ::GoToSleep( 3 ); Printf("%d: %d: ---------------\n")( value )( index );
        case 14: ::GoToSleep( 3 ); Printf("%d: %d: --------------\n")( value )( index );
        case 13: ::GoToSleep( 3 ); Printf("%d: %d: -------------\n")( value )( index );
        case 12: ::GoToSleep( 3 ); Printf("%d: %d: ------------\n")( value )( index );
        case 11: ::GoToSleep( 3 ); Printf("%d: %d: -----------\n")( value )( index );
        case 10: ::GoToSleep( 3 ); Printf("%d: %d: ----------\n")( value )( index );
        case  9: ::GoToSleep( 3 ); Printf("%d: %d: ---------\n")( value )( index );
        case  8: ::GoToSleep( 3 ); Printf("%d: %d: --------\n")( value )( index );
        case  7: ::GoToSleep( 3 ); Printf("%d: %d: -------\n")( value )( index );
        case  6: ::GoToSleep( 3 ); Printf("%d: %d: ------\n")( value )( index );
        case  5: ::GoToSleep( 3 ); Printf("%d: %d: -----\n")( value )( index );
        case  4: ::GoToSleep( 3 ); Printf("%d: %d: ----\n")( value )( index );
        case  3: ::GoToSleep( 3 ); Printf("%d: %d: ---\n")( value )( index );
        case  2: ::GoToSleep( 3 ); Printf("%d: %d: --\n")( value )( index );
        case  1: ::GoToSleep( 3 ); Printf("%d: %d: -\n")( value )( index );
        case  0: ::GoToSleep( 3 ); Printf("%d: %d: \n")( value )( index );
    }
}

// ----------------------------------------------------------------------------

void Thing::SetValue( unsigned int value ) volatile
{
    assert( NULL != this );
    SingleThingLocker pSafeThis( *this, m_mutex );
    pSafeThis->m_value = value;
}

// ----------------------------------------------------------------------------

volatile LevelThing * LevelThing::GetFromPool( unsigned int index )
{
    if ( s_pool.size() <= index )
        return NULL;
    volatile LevelThing * thing = s_pool[ index ];
    return thing;
}

// ----------------------------------------------------------------------------

bool LevelThing::MakePool( unsigned int count )
{
    s_pool.clear();
    s_pool.reserve( count );
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile LevelThing * thing = new LevelThing( ii * 10, ii );
        s_pool.push_back( thing );
    }
    return true;
}

// ----------------------------------------------------------------------------

void LevelThing::DestroyPool( void )
{
    const unsigned int count = s_pool.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile LevelThing * thing = s_pool[ ii ];
        delete thing;
    }
    s_pool.clear();
}

// ----------------------------------------------------------------------------

LevelThing::LevelThing( unsigned int level, unsigned int place ) :
    m_mutex( level ),
    m_place( place ),
    m_value( 0 )
{
    assert( NULL != this );
#if defined( _MSC_VER )
    m_mutex.GetMutexPolicy().SetSleepTime( 5 );
    m_mutex.GetMutexPolicy().SetWakable( false );
#else
    m_mutex.GetMutexPolicy().SetSleepTime( 1 );
#endif
}

// ----------------------------------------------------------------------------

LevelThing::~LevelThing( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

LevelThing::Unlocker LevelThing::LockHierarchy( void ) volatile
{
    assert( NULL != this );
    for ( signed ii = m_place; 0 <= ii; --ii )
    {
        volatile LevelThing * thing = s_pool[ ii ];
        assert( NULL != thing );
        const MutexErrors::Type result = thing->m_mutex.Lock();
        if ( MutexErrors::Success != result )
            break;
    }

    Unlocker unlocker( this );
    return unlocker;
}

// ----------------------------------------------------------------------------

void LevelThing::UnlockHierarchy( void ) volatile
{
    assert( NULL != this );
    try
    {
        for ( unsigned ii = 0; ii <= m_place; ++ii )
        {
            volatile LevelThing * thing = s_pool[ ii ];
            assert( NULL != thing );
            thing->m_mutex.Unlock();
        }
    }
    catch ( ... )
    {
    }
}

// ----------------------------------------------------------------------------

void LevelThing::SetValue( unsigned int value ) volatile
{
    assert( NULL != this );
    MutexLocker locker( m_mutex, !m_mutex.IsLockedByCurrentThread() );
    (void)locker;
    m_value = value;
    if ( 0 == m_place )
        return;

    volatile LevelThing * thing = s_pool[ m_place - 1 ];
    assert( NULL != thing );
    thing->SetValue( value );
}

// ----------------------------------------------------------------------------

void LevelThing::SetValue( unsigned int value )
{
    assert( NULL != this );
    m_value = value;
    if ( 0 == m_place )
        return;

    LevelThing * thing = const_cast< LevelThing * >( s_pool[ m_place - 1 ] );
    assert( NULL != thing );
    thing->SetValue( value );
}

// ----------------------------------------------------------------------------

bool LevelThing::DoValuesMatch( unsigned int value ) const volatile
{
    assert( NULL != this );
    {
        MutexLocker locker( m_mutex, !m_mutex.IsLockedByCurrentThread() );
        (void)locker;
        if ( m_value != value )
            return false;
    }
    if ( 0 == m_place )
        return true;

    const volatile LevelThing * thing = s_pool[ m_place - 1 ];
    assert( NULL != thing );
    return thing->DoValuesMatch( value );
}

// ----------------------------------------------------------------------------

bool LevelThing::DoValuesMatch( unsigned int value ) const
{
    assert( NULL != this );
    if ( m_value != value )
        return false;
    if ( 0 == m_place )
        return true;

    const LevelThing * thing = const_cast< LevelThing * >( s_pool[ m_place - 1 ] );
    assert( NULL != thing );
    return thing->DoValuesMatch( value );
}

// ----------------------------------------------------------------------------

SomeThing::SomeThing( unsigned int level, unsigned int place ) :
    m_mutex( level ),
    m_place( place ),
    m_level( level ),
    m_value( 0 )
{
    assert( NULL != this );
#if defined( _MSC_VER )
    m_mutex.GetMutexPolicy().SetSleepTime( 5 );
    m_mutex.GetMutexPolicy().SetWakable( false );
#else
    m_mutex.GetMutexPolicy().SetSleepTime( 1 );
#endif
}

// ----------------------------------------------------------------------------

SomeThing::~SomeThing( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

void SomeThing::SetValue( unsigned int value ) volatile
{
    assert( NULL != this );
    SomeThingLocker pSafeThis( *this, m_mutex );
    pSafeThis->m_value = value;
}

// ----------------------------------------------------------------------------

void SomeThing::SetValue( unsigned int value )
{
    assert( NULL != this );
    m_value = value;
}

// ----------------------------------------------------------------------------

ManyThingsPool::ManyThingsPool( unsigned int level, unsigned int count ) :
    m_pool()
{
    assert( NULL != this );
    m_pool.reserve( count );
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile SomeThing * thing = new SomeThing( level, ii );
        m_pool.push_back( thing );
    }
}

// ----------------------------------------------------------------------------

ManyThingsPool::~ManyThingsPool( void )
{
    assert( NULL != this );
    const unsigned int count = m_pool.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile SomeThing * thing = m_pool[ ii ];
        delete thing;
    }
    m_pool.clear();
}

// ----------------------------------------------------------------------------

unsigned int ManyThingsPool::GetCount( void ) const
{
    assert( NULL != this );
    const unsigned int count = m_pool.size();
    return count;
}

// ----------------------------------------------------------------------------

volatile SomeThing * ManyThingsPool::GetFromPool( unsigned int index )
{
    assert( NULL != this );
    ManyThingsPool * pThis = const_cast< ManyThingsPool * >( this );
    if ( pThis->m_pool.size() <= index )
        return NULL;
    volatile SomeThing * thing = pThis->m_pool[ index ];
    assert( NULL != thing );
    return thing;
}

// ----------------------------------------------------------------------------

void MultiLevelPool::MakePool( unsigned int count, unsigned int thingCount )
{
    s_pool.clear();
    s_pool.reserve( count );
    unsigned int level = 100;
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        const unsigned int newCount = ( ::rand() % thingCount ) + 4;
        ManyThingsPool * pool = new ManyThingsPool( ii + level, newCount );
        s_pool.push_back( pool );
    }
}

// ----------------------------------------------------------------------------

void MultiLevelPool::DestroyPool( void )
{
    const unsigned int count = s_pool.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        ManyThingsPool * pool = s_pool[ ii ];
        delete pool;
    }
    s_pool.clear();
}

// ----------------------------------------------------------------------------

unsigned int MultiLevelPool::GetCount( void )
{
    const unsigned int count = s_pool.size();
    return count;
}

// ----------------------------------------------------------------------------

ManyThingsPool * MultiLevelPool::GetFromPool( unsigned int index )
{
    if ( s_pool.size() <= index )
        return NULL;
    ManyThingsPool * pool = s_pool[ index ];
    assert( NULL != pool );
    return pool;
}

// ----------------------------------------------------------------------------

void CheckForMatchingValues( unsigned int & failCount, unsigned int & testCount,
    unsigned int value, const SomeThingPool & pool )
{
    const unsigned int count = pool.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        SomeThing * thing = const_cast< SomeThing * >( pool[ ii ] );
        assert( NULL != thing );
        testCount++;
        if ( value != thing->GetValue() )
            failCount++;
    }
}

// ----------------------------------------------------------------------------

void CheckForMatchingValues( unsigned int & failCount, unsigned int & testCount,
    unsigned int value, const SomeThingPool & pool, bool locked )
{
    if ( !locked )
    {
        CheckForMatchingValues( failCount, testCount, value, pool );
        return;
    }

    const unsigned int count = pool.size();
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile SomeThing * thing = pool[ ii ];
        assert( NULL != thing );
        assert( thing->GetMutex().IsLockedByCurrentThread() );
        testCount++;
        if ( value != thing->GetValue() )
            failCount++;
    }
}

// ----------------------------------------------------------------------------

void MakePool( SomeThingPool & target )
{
    SomeThingPool temp;
    bool useThis = false;
    unsigned int count2 = 0;
    unsigned int r = 0;
    bool first = true;
    const unsigned int count1 = MultiLevelPool::GetCount();

    for ( unsigned int index1 = 0; index1 < count1; ++index1 )
    {
        r = ( ::rand() % 4 );
        useThis = ( 0 == index1 ) || ( r != 0 );
        if ( !useThis )
            continue;

        ManyThingsPool * pool = MultiLevelPool::GetFromPool( index1 );
        assert( NULL != pool );
        count2 = pool->GetCount();
        first = true;

        for ( unsigned int index2 = 0; index2 < count2; ++index2 )
        {
            r = ( ::rand() % 4 );
            useThis = ( 0 == index1 ) || ( r != 0 );
            if ( !useThis )
                continue;
            volatile SomeThing * thing = pool->GetFromPool( index2 );
            assert( NULL != thing );
            temp.push_back( thing );

            if ( first )
            {
                // The containers at most levels will have multiple mutexes,
                // but some levels will have just a single mutex.
                r = ( ::rand() % 4 );
                if ( r != 0 )
                    break;
            }
            first = false;
        }
    }

    temp.swap( target );
}

// ----------------------------------------------------------------------------

/// @struct SomeThingComparator Binary-functor for comparing 2 SomeThing's.
struct SomeThingComparator : std::binary_function< volatile SomeThing *, volatile SomeThing *, bool >
{
    /// Returns true if left string < right string.
	inline bool operator ()( volatile SomeThing * lhs, volatile SomeThing * rhs ) const
    {
        return ( lhs->GetLevel() < rhs->GetLevel() );
    }
};

// ----------------------------------------------------------------------------

void SortByLevel( SomeThingPool & pool )
{
    const unsigned int count = pool.size();
    assert( 0 != count );
    SomeThingComparator comparator;
    ::std::sort( pool.begin(), pool.end(), comparator );
}

// ----------------------------------------------------------------------------

void LockThese( SomeThingPool & pool )
{

    const unsigned int count = pool.size();
    assert( 0 != count );
    SortByLevel( pool );

    volatile SomeThing * thing = pool[ count - 1 ];
    assert( NULL != thing );
    unsigned int level = thing->GetLevel();
    LevelMutexInfo::MutexContainer mutexes;
    MutexErrors::Type result = MutexErrors::Success;
    signed int ii = count - 1;

    for ( ; 0 <= ii; --ii )
    {
        thing = pool[ ii ];
        assert( NULL != thing );
        if ( level != thing->GetLevel() )
        {
            assert( mutexes.size() != 0 );
            result = LevelMutexInfo::MultiLock( mutexes );
            assert( result == MutexErrors::Success );
            mutexes.clear();
            level = thing->GetLevel();
        }
        mutexes.push_back( &thing->GetMutex() );
    }

    assert( mutexes.size() != 0 );
    result = LevelMutexInfo::MultiLock( mutexes );
    assert( result == MutexErrors::Success );

    for ( ii = count - 1; 0 <= ii; --ii )
    {
        thing = pool[ ii ];
        assert( NULL != thing );
        assert( thing->GetMutex().IsLockedByCurrentThread() );
    }
}

// ----------------------------------------------------------------------------

void UnlockThese( SomeThingPool & pool )
{

    const unsigned int count = pool.size();
    assert( 0 != count );
    SortByLevel( pool );

    volatile SomeThing * thing = pool[ 0 ];
    assert( NULL != thing );
    unsigned int level = thing->GetLevel();
    LevelMutexInfo::MutexContainer mutexes;
    MutexErrors::Type result = MutexErrors::Success;
    unsigned int ii = 0;

    for ( ii = 0; ii < count; ++ii )
    {
        thing = pool[ ii ];
        assert( NULL != thing );
        if ( level != thing->GetLevel() )
        {
            assert( mutexes.size() != 0 );
            result = LevelMutexInfo::MultiUnlock( mutexes );
            assert( result == MutexErrors::Success );
            mutexes.clear();
            level = thing->GetLevel();
        }
        mutexes.push_back( &thing->GetMutex() );
    }

    assert( mutexes.size() != 0 );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( result == MutexErrors::Success );

    for ( ii = 0; ii < count; ++ii )
    {
        thing = pool[ ii ];
        assert( NULL != thing );
        assert( !thing->GetMutex().IsLockedByCurrentThread() );
    }
}

// ----------------------------------------------------------------------------

unsigned int CountLockedByThisThread( const SomeThingPool & pool )
{
    const unsigned int count = pool.size();
    unsigned int locked = 0;

    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        const volatile SomeThing * thing = pool[ ii ];
        assert( NULL != thing );
        if ( thing->GetMutex().IsLockedByCurrentThread() )
            locked++;
    }

    return locked;
}

// ----------------------------------------------------------------------------
