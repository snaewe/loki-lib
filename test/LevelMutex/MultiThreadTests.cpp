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

#include "MultiThreadTests.hpp"

#include <assert.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>

#include <SafeFormat.h>

#include "ThreadPool.hpp"
#include "Thing.hpp"


// define nullptr even though new compilers will have this keyword just so we
// have a consistent and easy way of identifying which uses of 0 mean null.
#define nullptr 0


using namespace ::Loki;

static const unsigned int thingCount = 8;

static bool s_redo = false;


// ----------------------------------------------------------------------------

void SetToRedoSingleTests( bool redo )
{
    s_redo = redo;
}

// ----------------------------------------------------------------------------

bool WillRedoSingleTests( void )
{
    return s_redo;
}

// ----------------------------------------------------------------------------

#if defined(_WIN32)

    typedef unsigned int ( WINAPI * ThreadFunction_ )( void * );

    #define LOKI_pthread_t HANDLE

    #define LOKI_pthread_create(handle,attr,func,arg) \
        (int)((*handle=(HANDLE) _beginthreadex (nullptr,0,(ThreadFunction_)func,arg,0,nullptr))==nullptr)

    #define LOKI_pthread_join(thread) \
        ((::WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))

#else

    #define LOKI_pthread_t \
                 pthread_t
    #define LOKI_pthread_create(handle,attr,func,arg) \
                 pthread_create(handle,attr,func,arg)
    #define LOKI_pthread_join(thread) \
                 pthread_join(thread, NULL)

#endif

using namespace std;
using namespace Loki;

// ----------------------------------------------------------------------------

void * PrintSafeThread( void * p )
{
    unsigned int value = reinterpret_cast< unsigned int >( p );
    volatile Thing & thing = Thing::GetIt();
    try
    {
        for ( unsigned int ii = 0; ii < 5; ++ii )
        {
            thing.Print( value, ii, 16 );
            ::GoToSleep( 2 );
            if ( WillRedoSingleTests() )
            {
                SingleThreadSimpleTest();
                SingleThreadReentrantTest();
                SingleThreadSimpleMultiLockTest();
                SingleThreadComplexMultiLockTest( false );
                SingleThreadExceptionTest();
            }
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    return nullptr;
}

// ----------------------------------------------------------------------------

void * PrintUnsafeThread( void * p )
{
    unsigned int value = reinterpret_cast< unsigned int >( p );
    Thing & thing = const_cast< Thing & >( Thing::GetIt() );
    try
    {
        for ( unsigned int ii = 0; ii < 5; ++ii )
        {
            thing.Print( value, ii, 16 );
            ::GoToSleep( 2 );
            if ( WillRedoSingleTests() )
            {
                SingleThreadSimpleTest();
                SingleThreadReentrantTest();
                SingleThreadSimpleMultiLockTest();
                SingleThreadComplexMultiLockTest( false );
                SingleThreadExceptionTest();
            }
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    return nullptr;
}

// ----------------------------------------------------------------------------

void OutputResults( unsigned int loop, unsigned int value, unsigned int result )
{
    static volatile SleepMutex mutex( 2 );
    static bool initialized = false;
    if ( !initialized )
    {
        initialized = true;
        #if defined( _MSC_VER )
            mutex.GetMutexPolicy().SetSleepTime( 5 );
            mutex.GetMutexPolicy().SetWakable( true );
        #else
            mutex.GetMutexPolicy().SetSleepTime( 1 );
        #endif
    }

    MutexLocker locker( mutex );
    (void)locker;

    const char * message = ( result == value ) ? "yes" : "no!";
    Printf( "Loop = %d \t Value = %d \t Result = %d \t Pass? %s \n" )
        ( loop )( value )( result )( message );
}

// ----------------------------------------------------------------------------

void * ValueSafeThread( void * p )
{

    const unsigned int testCount = 8;
    unsigned int fails = 0;
    const unsigned int value = reinterpret_cast< unsigned int >( p );
    volatile Thing & thing = Thing::GetIt();
    try
    {
        for ( unsigned int ii = 0; ii < testCount; ++ii )
        {
            MutexLocker locker( thing.GetMutex() );
            (void)locker;
            thing.SetValue( value );
            ::GoToSleep( 3 );
            const unsigned int result = thing.GetValue();
            OutputResults( ii, value, result );
            if ( result != value )
                fails++;
            assert( result == value );
        }
    }
    catch ( ... )
    {
        assert( false );
    }
    TestResults::GetIt()->SetResult( value, testCount, fails );

    return nullptr;
}

// ----------------------------------------------------------------------------

void * ValueUnsafeThread( void * p )
{

    const unsigned int testCount = 8;
    unsigned int fails = 0;
    const unsigned int value = reinterpret_cast< unsigned int >( p );
    // cast away volatility so the mutex doesn't get used by volatile functions.
    Thing & thing = const_cast< Thing & >( Thing::GetIt() );
    try
    {
        for ( unsigned int ii = 0; ii < testCount; ++ii )
        {
            thing.SetValue( value );
            ::GoToSleep( 3 );
            const unsigned int result = thing.GetValue();
            OutputResults( ii, value, result );
            if ( result != value )
                fails++;
        }
    }
    catch ( ... )
    {
        assert( false );
    }
    TestResults::GetIt()->SetResult( value, testCount, fails );

    return nullptr;
}

// ----------------------------------------------------------------------------

void MultiThreadSimpleTest( void )
{
    Thing::Init( 0 );
    const unsigned int threadCount = 5;
    ThreadPool pool( threadCount );

    cout << endl << "Doing thread-locked print test.  This test should pass. and not deadlock" << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( PrintSafeThread, p );
    }
    pool.JoinAll();

    cout << endl << "Doing thread-unsafe print test.  This test may fail." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( PrintUnsafeThread, p );
    }
    pool.JoinAll();

    Thing::Destroy();
}

// ----------------------------------------------------------------------------

void * TryLockThread( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    volatile Thing & thing = Thing::GetIt();
    volatile SleepMutex & mutex = thing.GetMutex();
    assert( mutex.IsLockedByAnotherThread() );

    for ( unsigned int ii = 0; ii < 5; ++ii )
    {
        MutexErrors::Type result = mutex.TryLock();
        const bool locked = ( result == MutexErrors::Success );
        assert( !locked );
        if ( locked )
        {
            // let's pretend we actually locked it,
            // and try to change the value.
            thing.SetValue( value );
            result = mutex.Unlock();
            assert( result == MutexErrors::Success );
        }
        const unsigned int gotValue = thing.GetValue();
        assert( gotValue != value );
    }
    assert( mutex.IsLockedByAnotherThread() );

    return nullptr;
}

// ----------------------------------------------------------------------------

void MultiThreadTryLockTest( void )
{
    static const unsigned int threadCount = 3;
    Thing::Init( 0 );
    volatile Thing & thing = Thing::GetIt();
    volatile SleepMutex & mutex = thing.GetMutex();

    cout << endl << "Doing multi-threaded TryLock test.  This test should not deadlock." << endl;
    ::system( "pause" );
    // First step is to lock the mutex in the main thread so no child thread
    // can ever lock it, change the value, or anything like that.
    MutexErrors::Type result = mutex.Lock();
    assert( MutexErrors::Success == result );
    bool okay = mutex.IsLockedByCurrentThread();
    assert( okay );
    thing.SetValue( threadCount );
    ThreadPool pool( threadCount );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( TryLockThread, p );
    }
    pool.JoinAll();
    const unsigned int value = thing.GetValue();
    assert( value == threadCount );
    result = mutex.Unlock();
    assert( MutexErrors::Success == result );
    okay = !mutex.IsLockedByCurrentThread();
    assert( okay );
    okay = !mutex.IsLocked();
    assert( okay );

    Thing::Destroy();
}

// ----------------------------------------------------------------------------

void MultiThreadReentrantTest( void )
{
    Thing::Init( 0 );
    const unsigned int threadCount = 8;
    TestResults::Create( threadCount );
    ThreadPool pool( threadCount );

    cout << endl << "Doing thread-safe value test.  This test should pass and not deadlock." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( ValueSafeThread, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    cout << endl << "Doing thread-unsafe value test.  This test may fail." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( ValueUnsafeThread, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    TestResults::Destroy();
    Thing::Destroy();
}

// ----------------------------------------------------------------------------

bool RandomizeMutexOrder( LevelMutexInfo::MutexContainer & mutexes )
{
    unsigned int count = mutexes.size();
    if ( count < 2 )
        return false;

    LevelMutexInfo::MutexContainer randomMutexes;
    for ( unsigned int ii = 0; ii < count; ++ii )
    {
        volatile LevelMutexInfo * mutex = nullptr;
        const unsigned int sizeNow = mutexes.size();
        if ( 1 < sizeNow )
        {
            unsigned int index = ( ::rand() % sizeNow );
            mutex = mutexes[ index ];
            if ( index < sizeNow - 1 )
                mutexes[ index ] = mutexes[ sizeNow - 1 ];
        }
        else
        {
            mutex = mutexes[ 0 ];
        }
        mutexes.pop_back();
        randomMutexes.push_back( mutex );
    }

    mutexes = randomMutexes;
    return true;
}

// ----------------------------------------------------------------------------

void SingleThreadComplexMultiLockTest( bool doSetup )
{

    ::srand( static_cast< unsigned int >( time( nullptr ) ) );
    if ( doSetup )
    {
        Thing::MakePool( thingCount );
    }

    const unsigned int priorLevel      = GetCurrentThreadsLevel();
    const unsigned int priorLockCount  = CountLocksInCurrentThread();
    const unsigned int priorMutexCount = CountMutexesInCurrentThread();
    const unsigned int priorLevelMutexCount = CountMutexesAtCurrentLevel();
    if ( LevelMutexInfo::UnlockedLevel == priorLevel )
    {
        assert( priorLockCount  == 0 );
        assert( priorMutexCount == 0 );
        assert( priorLevelMutexCount == 0 );
    }
    else
    {
        assert( priorLockCount  != 0 );
        assert( priorMutexCount != 0 );
        assert( priorLevelMutexCount != 0 );
        assert( priorLevelMutexCount <= priorMutexCount );
        assert( priorMutexCount <= priorLockCount );
    }

    volatile Thing * thing0 = Thing::GetFromPool( 0 );
    volatile Thing * thing1 = Thing::GetFromPool( 1 );
    volatile Thing * thing2 = Thing::GetFromPool( 2 );
    volatile Thing * thing3 = Thing::GetFromPool( 3 );
    volatile Thing * thing4 = Thing::GetFromPool( 4 );
    volatile Thing * thing5 = Thing::GetFromPool( 5 );
    volatile Thing * thing6 = Thing::GetFromPool( 6 );
    volatile Thing * thing7 = Thing::GetFromPool( 7 );
    assert( nullptr != thing0 );
    assert( nullptr != thing1 );
    assert( nullptr != thing2 );
    assert( nullptr != thing3 );
    assert( nullptr != thing4 );
    assert( nullptr != thing5 );
    assert( nullptr != thing6 );
    assert( nullptr != thing7 );

    LevelMutexInfo::MutexContainer mutexes;
    mutexes.reserve( thingCount );
    MutexErrors::Type result = LevelMutexInfo::MultiLock( mutexes );
    assert( result == MutexErrors::EmptyContainer );

    mutexes.push_back( &thing0->GetMutex() );
    mutexes.push_back( &thing1->GetMutex() );
    mutexes.push_back( &thing2->GetMutex() );
    mutexes.push_back( &thing3->GetMutex() );
    mutexes.push_back( &thing4->GetMutex() );
    mutexes.push_back( &thing5->GetMutex() );
    mutexes.push_back( &thing6->GetMutex() );
    mutexes.push_back( &thing7->GetMutex() );

    result = LevelMutexInfo::MultiLock( mutexes );
    assert( result == MutexErrors::Success );
    assert( GetCurrentThreadsLevel()      == thing0->GetMutex().GetLevel() );
    assert( CountLocksInCurrentThread()   == priorLockCount  + thingCount );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( result == MutexErrors::Success );
    assert( GetCurrentThreadsLevel()      == priorLevel );
    assert( CountLocksInCurrentThread()   == priorLockCount );
    assert( CountMutexesInCurrentThread() == priorMutexCount );
    assert( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );

    mutexes.clear();
    mutexes.push_back( &thing7->GetMutex() );
    mutexes.push_back( &thing6->GetMutex() );
    mutexes.push_back( &thing5->GetMutex() );
    mutexes.push_back( &thing4->GetMutex() );
    mutexes.push_back( &thing3->GetMutex() );
    mutexes.push_back( &thing2->GetMutex() );
    mutexes.push_back( &thing1->GetMutex() );
    mutexes.push_back( &thing0->GetMutex() );
    result = LevelMutexInfo::MultiLock( mutexes );
    assert( result == MutexErrors::Success );
    assert( GetCurrentThreadsLevel()      == thing0->GetMutex().GetLevel() );
    assert( CountLocksInCurrentThread()   == priorLockCount  + thingCount );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( result == MutexErrors::Success );
    assert( GetCurrentThreadsLevel()      == priorLevel );
    assert( CountLocksInCurrentThread()   == priorLockCount );
    assert( CountMutexesInCurrentThread() == priorMutexCount );
    assert( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );

    RandomizeMutexOrder( mutexes );
    result = LevelMutexInfo::MultiLock( mutexes );
    assert( result == MutexErrors::Success );
    assert( GetCurrentThreadsLevel()      == thing0->GetMutex().GetLevel() );
    assert( CountLocksInCurrentThread()   == priorLockCount  + thingCount );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( result == MutexErrors::Success );
    assert( GetCurrentThreadsLevel()      == priorLevel );
    assert( CountLocksInCurrentThread()   == priorLockCount );
    assert( CountMutexesInCurrentThread() == priorMutexCount );
    assert( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );

    if ( doSetup )
        Thing::DestroyPool();
}

// ----------------------------------------------------------------------------

void * MultiLockSafeThread( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    LevelMutexInfo::MutexContainer mutexes( thingCount );
    volatile Thing * thing = nullptr;
    unsigned int jj = 0;
    for ( ; jj < thingCount; ++jj )
    {
        thing = Thing::GetFromPool( jj );
        assert( nullptr != thing );
        mutexes[ jj ] = &thing->GetMutex();
    }

    unsigned int fails = 0;
    unsigned int tests = 0;
    unsigned int randomIndex = 0;
    try
    {
        for ( unsigned int ii = 0; ii < thingCount; ++ii )
        {
            MultiMutexLocker locker( mutexes );
            (void)locker;

            for ( jj = 0; jj < thingCount; ++jj )
            {
                thing = Thing::GetFromPool( jj );
                assert( nullptr != thing );
                thing->SetValue( value );
            }
            ::GoToSleep( 2 );

            if ( WillRedoSingleTests() )
            {
                SingleThreadSimpleTest();
                SingleThreadReentrantTest();
                SingleThreadSimpleMultiLockTest();
                SingleThreadComplexMultiLockTest( false );
                SingleThreadExceptionTest();
            }

            thing = Thing::GetFromPool( ii );
            assert( nullptr != thing );
            thing->Print( value, ii, 7 );
            randomIndex = ( ::rand() % thingCount );
            thing = Thing::GetFromPool( randomIndex );
            assert( nullptr != thing );
            thing->Print( value, ii + 10, 7 );

            for ( jj = 0; jj < thingCount; ++jj )
            {
                thing = Thing::GetFromPool( jj );
                assert( nullptr != thing );
                if ( thing->GetValue() != value )
                    fails++;
                tests++;
            }

            bool okay = locker.Unlock();
            assert( okay );
            ::GoToSleep( 2 );
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, tests, fails );

    return nullptr;
}

// ----------------------------------------------------------------------------

void * MultiLockUnsafeThread( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    Thing * thing = nullptr;
    unsigned int jj = 0;
    unsigned int tests = 0;
    unsigned int fails = 0;
    unsigned int randomIndex = 0;
    try
    {
        for ( unsigned int ii = 0; ii < thingCount; ++ii )
        {
            for ( jj = 0; jj < thingCount; ++jj )
            {
                thing = const_cast< Thing * >( Thing::GetFromPool( jj ) );
                assert( nullptr != thing );
                thing->SetValue( value );
            }
            ::GoToSleep( 2 );

            if ( WillRedoSingleTests() )
            {
                SingleThreadSimpleTest();
                SingleThreadReentrantTest();
                SingleThreadSimpleMultiLockTest();
                SingleThreadComplexMultiLockTest( false );
                SingleThreadExceptionTest();
            }

            thing = const_cast< Thing * >( Thing::GetFromPool( ii ) );
            assert( nullptr != thing );
            thing->Print( value, ii, 7 );
            randomIndex = ( ::rand() % thingCount );
            thing = const_cast< Thing * >( Thing::GetFromPool( randomIndex ) );
            assert( nullptr != thing );
            thing->Print( value, ii, 7 );

            for ( jj = 0; jj < thingCount; ++jj )
            {
                thing = const_cast< Thing * >( Thing::GetFromPool( jj ) );
                assert( nullptr != thing );
                if ( thing->GetValue() != value )
                    fails++;
                tests++;
            }

            ::GoToSleep( 2 );
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, tests, fails );

    return nullptr;
}

// ----------------------------------------------------------------------------

void MultiThreadMultiLockTest( void )
{
    Thing::MakePool( thingCount );
    const unsigned int threadCount = 8;
    TestResults::Create( threadCount );
    ThreadPool pool( threadCount );

    cout << endl << "Doing thread-safe multi-lock test.  This test should pass and not deadlock." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( MultiLockSafeThread, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    cout << endl << "Doing thread-unsafe multi-lock test.  This test may fail." << endl;
    TestResults::GetIt()->Reset( threadCount );
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( MultiLockUnsafeThread, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    TestResults::Destroy();
    Thing::DestroyPool();
}

// ----------------------------------------------------------------------------

void * MultiLockRandomSafeThread( void * p )
{
    const unsigned int value = reinterpret_cast< unsigned int >( p );
    unsigned int testCount = 0;
    unsigned int failCount = 0;
    volatile Thing * thing = nullptr;
    Thing::ThingPool pool;
    LevelMutexInfo::MutexContainer mutexes;
    mutexes.reserve( thingCount );

    unsigned int jj = 0;
    unsigned int place = 0;
    try
    {
        for ( unsigned int ii = 0; ii < thingCount; ++ii )
        {

            pool.clear();
            pool.reserve( thingCount );
            for ( place = 0; place < thingCount; ++place )
            {
                place += ::rand() % 3;
                if ( thingCount <= place )
                    break;
                thing = Thing::GetFromPool( place );
                assert( nullptr != thing );
                pool.push_back( thing );
            }
            const unsigned int poolCount = pool.size();

            mutexes.clear();
            for ( jj = 0; jj < poolCount; ++jj )
            {
                thing = pool[ jj ];
                assert( nullptr != thing );
                mutexes.push_back( &thing->GetMutex() );
            }

            MultiMutexLocker locker( mutexes );
            (void)locker;
            for ( jj = 0; jj < poolCount; ++jj )
            {
                thing = pool[ jj ];
                assert( nullptr != thing );
                thing->SetValue( value );
            }
            ::GoToSleep( 3 );

            for ( jj = 0; jj < poolCount; ++jj )
            {
                thing = pool[ jj ];
                assert( nullptr != thing );
                if ( thing->GetValue() != value )
                    failCount++;
                testCount++;
            }
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, testCount, failCount );

    return nullptr;
}

// ----------------------------------------------------------------------------

void * MultiLockRandomUnsafeThread( void * p )
{
    const unsigned int value = reinterpret_cast< unsigned int >( p );
    unsigned int testCount = 0;
    unsigned int failCount = 0;
    Thing * thing = nullptr;
    UnsafeThingPool pool;

    unsigned int jj = 0;
    unsigned int place = 0;
    try
    {
        for ( unsigned int ii = 0; ii < thingCount; ++ii )
        {

            pool.clear();
            pool.reserve( thingCount );
            for ( place = 0; place < thingCount; ++place )
            {
                place += ::rand() % 3;
                if ( thingCount <= place )
                    break;
                thing = thing = const_cast< Thing * >( Thing::GetFromPool( place ) );
                assert( nullptr != thing );
                pool.push_back( thing );
            }
            const unsigned int poolCount = pool.size();

            for ( jj = 0; jj < poolCount; ++jj )
            {
                thing = pool[ jj ];
                assert( nullptr != thing );
                thing->SetValue( value );
            }
            ::GoToSleep( 3 );

            for ( jj = 0; jj < poolCount; ++jj )
            {
                thing = pool[ jj ];
                assert( nullptr != thing );
                if ( thing->GetValue() != value )
                    failCount++;
                testCount++;
            }
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, testCount, failCount );

    return nullptr;
}

// ----------------------------------------------------------------------------

void MultiThreadRandomMultiLockTest( void )
{
    Thing::MakePool( thingCount );
    const unsigned int threadCount = 8;
    TestResults::Create( threadCount );
    ThreadPool pool( threadCount );

    cout << endl << "Doing thread-safe random multi-lock test.  This test should pass and not deadlock." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( MultiLockRandomSafeThread, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    cout << endl << "Doing thread-unsafe random multi-lock test.  This test may fail." << endl;
    TestResults::GetIt()->Reset( threadCount );
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( MultiLockRandomUnsafeThread, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    TestResults::Destroy();
    Thing::DestroyPool();
}

// ----------------------------------------------------------------------------

void * SafeHierarchyTest( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    volatile LevelThing * thing = nullptr;
    unsigned int testCount = 0;
    unsigned int failCount = 0;

    try
    {
        for ( unsigned int ii = 0; ii < thingCount; ++ii )
        {
            {
                thing = LevelThing::GetFromPool( ii );
                assert( nullptr != thing );
                LevelThing::Unlocker unlocker( thing->LockHierarchy() );
                (void)unlocker;
                thing->SetValue( value );
                ::GoToSleep( 3 );
                if ( !thing->DoValuesMatch( value ) )
                    failCount++;
                testCount++;
            }
            {
                const unsigned int randomIndex = ( ::rand() % thingCount );
                thing = LevelThing::GetFromPool( randomIndex );
                assert( nullptr != thing );
                LevelThing::Unlocker unlocker( thing->LockHierarchy() );
                (void)unlocker;
                thing->SetValue( value );
                ::GoToSleep( 3 );
                if ( !thing->DoValuesMatch( value ) )
                    failCount++;
                testCount++;
            }
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, testCount, failCount );

    return nullptr;
}

// ----------------------------------------------------------------------------

void * UnsafeHierarchyTest( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    LevelThing * thing = nullptr;
    unsigned int testCount = 0;
    unsigned int failCount = 0;

    try
    {
        for ( unsigned int ii = 0; ii < thingCount; ++ii )
        {
            {
                thing = const_cast< LevelThing * >( LevelThing::GetFromPool( ii ) );
                assert( nullptr != thing );
                thing->SetValue( value );
                ::GoToSleep( 3 );
                if ( !thing->DoValuesMatch( value ) )
                    failCount++;
                testCount++;
            }
            {
                const unsigned int randomIndex = ( ::rand() % thingCount );
                thing = const_cast< LevelThing * >( LevelThing::GetFromPool( randomIndex ) );
                assert( nullptr != thing );
                thing->SetValue( value );
                ::GoToSleep( 3 );
                if ( !thing->DoValuesMatch( value ) )
                    failCount++;
                testCount++;
            }
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, testCount, failCount );

    return nullptr;
}

// ----------------------------------------------------------------------------

void MultiThreadHierarchySingleLockTest( void )
{
    LevelThing::MakePool( thingCount );
    const unsigned int threadCount = 8;
    TestResults::Create( threadCount );
    ThreadPool pool( threadCount );

    cout << endl << "Doing thread-safe hierarchy test.  This test should pass and not deadlock." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( SafeHierarchyTest, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    cout << endl << "Doing thread-unsafe hierarchy test.  This test may fail." << endl;
    ::system( "pause" );
    TestResults::GetIt()->Reset( threadCount );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( UnsafeHierarchyTest, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    TestResults::Destroy();
    LevelThing::DestroyPool();
}

// ----------------------------------------------------------------------------

void * SafeHierarchyMultiLockTest( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    unsigned int testCount = 0;
    unsigned int failCount = 0;
    unsigned int totalTestCount = 0;
    unsigned int totalFailCount = 0;

    try
    {
        for ( unsigned int ii = 0; ii < 20; ++ii )
        {
            SomeThingPool pool;
            MakePool( pool );
            assert( CountLockedByThisThread( pool ) == 0 );
            LockThese( pool );
            assert( CountLockedByThisThread( pool ) == pool.size() );

            for ( SomeThingPoolIter it( pool.begin() );
                  it != pool.end();
                  ++it )
            {
                volatile SomeThing * thing = *it;
                assert( nullptr != thing );
                thing->SetValue( value );
            }

            ::GoToSleep( 2 );
            testCount = 0;
            failCount = 0;
            CheckForMatchingValues( failCount, testCount, value, pool, true );
            totalTestCount += testCount;
            totalFailCount += failCount;

            UnlockThese( pool );
            assert( CountLockedByThisThread( pool ) == 0 );
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, totalTestCount, totalFailCount );

    return nullptr;
}

// ----------------------------------------------------------------------------

void * UnsafeHierarchyMultiLockTest( void * p )
{

    const unsigned int value = reinterpret_cast< unsigned int >( p );
    unsigned int testCount = 0;
    unsigned int failCount = 0;
    unsigned int totalTestCount = 0;
    unsigned int totalFailCount = 0;

    try
    {
        for ( unsigned int ii = 0; ii < 20; ++ii )
        {
            SomeThingPool pool;
            MakePool( pool );
            for ( SomeThingPoolIter it( pool.begin() );
                  it != pool.end();
                  ++it )
            {
                SomeThing * thing = const_cast< SomeThing * >( *it );
                assert( nullptr != thing );
                thing->SetValue( value );
            }
            ::GoToSleep( 2 );
            testCount = 0;
            failCount = 0;
            CheckForMatchingValues( failCount, testCount, value, pool );
            totalTestCount += testCount;
            totalFailCount += failCount;
        }
    }
    catch ( ... )
    {
        assert( false );
    }

    TestResults::GetIt()->SetResult( value, totalTestCount, totalFailCount );

    return nullptr;
}

// ----------------------------------------------------------------------------

void MultiThreadHierarchyMultiLockTest( void )
{

    MultiLevelPool::MakePool( 10, thingCount );
    const unsigned int threadCount = 8;
    TestResults::Create( threadCount );
    ThreadPool pool( threadCount );

    cout << endl << "Doing thread-safe multilock hierarchy test.  This test should pass and not deadlock." << endl;
    ::system( "pause" );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( SafeHierarchyMultiLockTest, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    cout << endl << "Doing thread-unsafe multilock hierarchy test.  This test may fail." << endl;
    ::system( "pause" );
    TestResults::GetIt()->Reset( threadCount );
    for ( unsigned int ii = 0; ii < threadCount; ++ii )
    {
        void * p = reinterpret_cast< void * >( ii );
        pool.Start( UnsafeHierarchyMultiLockTest, p );
    }
    pool.JoinAll();
    TestResults::GetIt()->OutputResults();

    TestResults::Destroy();
    MultiLevelPool::DestroyPool();
}

// ----------------------------------------------------------------------------
