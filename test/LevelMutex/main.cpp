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


#include "LevelMutex.h"

#include "MultiThreadTests.hpp"
#include "Thing.hpp"

#include <assert.h>
#include <string.h>
#include <iostream>


// define nullptr even though new compilers will have this keyword just so we
// have a consistent and easy way of identifying which uses of 0 mean null.
#define nullptr 0


using namespace ::std;
using namespace ::Loki;


typedef ::Loki::LevelMutex< ::Loki::SpinLevelMutex, 1,
    ::Loki::JustReturnMutexError, ::Loki::NoMutexWait > SpinMutex;


// ----------------------------------------------------------------------------

void SingleThreadSimpleTest( void )
{

    const unsigned int priorLevel = GetCurrentThreadsLevel();
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

    SpinMutex mutex1( 1 );
    SpinMutex mutex2( 2 );
    SpinMutex mutex1a( 1 );

    bool okay = ( mutex1.GetLevel() == 1 );
    assert( okay );
    okay = ( mutex2.GetLevel() == 2 );
    assert( okay );
    okay = ( !mutex1.IsLocked() );
    assert( okay );
    okay = ( !mutex2.IsLocked() );
    assert( okay );
    okay = ( mutex1.GetLockCount() == 0 );
    assert( okay );
    okay = ( mutex2.GetLockCount() == 0 );
    assert( okay );
    MutexErrors::Type result = mutex1.Unlock();
    assert( MutexErrors::WasntLocked == result );
    result = mutex2.Unlock();
    assert( MutexErrors::WasntLocked == result );
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );

    // Check for attempt to lock mutex with level higher than current level.
    result = mutex1.Lock();
    assert( result == MutexErrors::Success );
    okay = ( GetCurrentThreadsLevel() == mutex1.GetLevel() );
    assert( okay );
    try
    {
        result = mutex2.Lock();
    }
    catch ( const MutexException & ex1 )
    {
        assert( MutexErrors::LevelTooHigh == ex1.GetReason() );
    }
    catch ( ... )
    {
        assert( false );
    }
    result = mutex2.Unlock();
    assert( result == MutexErrors::WasntLocked );
    okay = ( GetCurrentThreadsLevel() == mutex1.GetLevel() );
    assert( okay );
    try
    {
        // Now check for attempt to lock mutex at same level as current one.
        result = mutex1a.Lock();
    }
    catch ( const MutexException & ex2 )
    {
        assert( MutexErrors::LevelTooHigh == ex2.GetReason() );
    }
    catch ( ... )
    {
        assert( false );
    }
    result = mutex1.Unlock();
    assert( result == MutexErrors::Success );
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );

    // Check for unlocking mutex that is not current lock.
    result = mutex2.Lock();
    assert( result == MutexErrors::Success );
    okay = ( GetCurrentThreadsLevel() == mutex2.GetLevel() );
    assert( okay );
    result = mutex1.Lock();
    assert( result == MutexErrors::Success );
    try
    {
        result = mutex2.Unlock();
    }
    catch ( const MutexException & ex1 )
    {
        assert( MutexErrors::LevelTooHigh == ex1.GetReason() );
    }
    catch ( ... )
    {
        assert( false );
    }
    okay = ( GetCurrentThreadsLevel() == mutex1.GetLevel() );
    assert( okay );
    result = mutex1.Unlock();
    assert( result == MutexErrors::Success );
    okay = ( GetCurrentThreadsLevel() == mutex2.GetLevel() );
    assert( okay );
    result = mutex1.Unlock();
    assert( result == MutexErrors::WasntLocked );
    result = mutex2.Unlock();
    assert( result == MutexErrors::Success );

    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountLocksInCurrentThread() == priorLockCount );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
}

// ----------------------------------------------------------------------------

void SingleThreadReentrantTest( void )
{

    const unsigned int priorLevel = GetCurrentThreadsLevel();
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

    SpinMutex mutex1( 1 );
    SpinMutex mutex2( 2 );
    bool okay = ( mutex2.GetLevel() != mutex1.GetLevel() );
    assert( okay );

    okay = ( mutex2.GetLockCount() == 0 );
    assert( okay );
    okay = ( mutex1.GetLockCount() == 0 );
    assert( okay );
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountLocksInCurrentThread() == priorLockCount );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );

    MutexErrors::Type result = mutex2.Lock();
    assert( result == MutexErrors::Success );
    okay = ( GetCurrentThreadsLevel() == mutex2.GetLevel() );
    assert( okay );
    okay = ( mutex2.GetLockCount() == 1 );
    assert( okay );
    result = mutex2.Lock();
    assert( MutexErrors::Success == result );
    okay = ( mutex2.GetLockCount() == 2 );
    assert( okay );
    result = mutex2.TryLock();
    assert( result == MutexErrors::Success );
    okay = ( mutex2.GetLockCount() == 3 );
    assert( okay );
    result = mutex2.Unlock();
    okay = ( mutex2.GetLockCount() == 2 );
    assert( okay );
    result = mutex1.Lock();
    assert( MutexErrors::Success == result );
    okay = ( GetCurrentThreadsLevel() == mutex1.GetLevel() );
    assert( okay );
    okay = ( mutex1.GetLockCount() == 1 );
    assert( okay );
    result = mutex1.Lock();
    assert( MutexErrors::Success == result );
    okay = ( CountLocksInCurrentThread()   == priorLockCount + 4 );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount + 2 );
    assert( okay );
    okay = ( mutex1.GetLockCount() == 2 );
    assert( okay );
    result = mutex1.Unlock();
    assert( MutexErrors::Success == result );
    okay = ( mutex1.GetLockCount() == 1 );
    assert( okay );
    result = mutex1.Unlock();
    assert( MutexErrors::Success == result );
    okay = ( mutex1.GetLockCount() == 0 );
    assert( okay );
    okay = ( mutex2.GetLockCount() == 2 );
    assert( okay );
    result = mutex2.Unlock();
    assert( MutexErrors::Success == result );
    okay = ( mutex2.GetLockCount() == 1 );
    assert( okay );
    result = mutex2.Unlock();
    assert( MutexErrors::Success == result );

    // Now check if current thread has no locks.
    okay = ( CountLocksInCurrentThread()   == priorLockCount );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
    okay = ( mutex2.GetLockCount() == 0 );
    assert( okay );
    okay = ( mutex1.GetLockCount() == 0 );
    assert( okay );

    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountLocksInCurrentThread() == priorLockCount );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
}

// ----------------------------------------------------------------------------

void SingleThreadSimpleMultiLockTest( void )
{

    const unsigned int priorLevel = GetCurrentThreadsLevel();
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

    SpinMutex mutex1( 1 );
    SpinMutex mutex2( 1 );
    bool okay = ( mutex2.GetLevel() == mutex1.GetLevel() );
    assert( okay );

    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    LevelMutexInfo::MutexContainer mutexes;
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );

    MutexErrors::Type result = LevelMutexInfo::MultiLock( mutexes );  // locks: 1, 2
    assert( MutexErrors::Success == result );
    okay = ( GetCurrentThreadsLevel() == mutex1.GetLevel() );
    assert( okay );
    result = LevelMutexInfo::MultiUnlock( mutexes );    // locks: none
    assert( MutexErrors::Success == result );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );

    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );
    mutexes.push_back( &mutex1 );
    result = LevelMutexInfo::MultiLock( mutexes );      // locks: none
    assert( MutexErrors::DuplicateMutex == result );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );

    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( nullptr );
    mutexes.push_back( &mutex2 );
    result = LevelMutexInfo::MultiLock( mutexes );      // locks: none
    assert( MutexErrors::NullMutexPointer == result );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );

    SpinMutex mutex3( 3 );
    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );
    mutexes.push_back( &mutex3 );
    result = LevelMutexInfo::MultiLock( mutexes );      // locks: none
    assert( MutexErrors::WrongLevel == result );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );

    result = mutex1.Lock();                         // locks: 1
    assert( MutexErrors::Success == result );
    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );
    try
    {
        result = LevelMutexInfo::MultiLock( mutexes );  // locks: 1
    }
    catch ( const MutexException & ex1 )
    {
        assert( MutexErrors::LevelTooHigh == ex1.GetReason() );
    }
    catch ( ... )
    {
        assert( false );
    }
    result = mutex1.Unlock();                       // locks: none
    assert( MutexErrors::Success == result );

    result = LevelMutexInfo::MultiLock( mutexes );      // locks: 1, 2
    assert( MutexErrors::Success == result );
    const bool m1IsFirst = ( &mutex1 > &mutex2 );
    LevelMutexInfo * first  = ( m1IsFirst ) ? &mutex1 : &mutex2;
    LevelMutexInfo * second = ( m1IsFirst ) ? &mutex2 : &mutex1;
    result = first->Unlock();                       // locks: 2
    assert( MutexErrors::Success == result );
    result = LevelMutexInfo::MultiUnlock( mutexes );    // locks: 2
    assert( MutexErrors::NotRecentLock == result );
    result = second->Unlock();                      // locks: none
    assert( MutexErrors::Success == result );

    // Now combine some calls to TryLock with a call to MultiLock.
    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );
    result = LevelMutexInfo::MultiLock( mutexes );      // locks: 1, 2
    assert( MutexErrors::Success == result );
    result = mutex2.TryLock();                      // locks: 1, 2+
    assert( MutexErrors::Success == result );
    okay = ( mutex2.GetLockCount() == 2 );
    assert( okay );
    result = mutex2.Unlock();                       // locks: 1, 2
    assert( MutexErrors::Success == result );
    result = mutex1.TryLock();                      // locks: 1+, 2
    assert( result == MutexErrors::Success );
    okay = ( mutex1.GetLockCount() == 2 );
    assert( okay );
    result = mutex2.TryLock();                      // locks: 1+, 2+
    assert( result == MutexErrors::Success );
    okay = ( mutex2.GetLockCount() == 2 );
    assert( okay );
    result = mutex2.Unlock();                       // locks: 1+, 2
    assert( MutexErrors::Success == result );
    result = mutex1.Unlock();                       // locks: 1, 2
    assert( MutexErrors::Success == result );
    result = LevelMutexInfo::MultiUnlock( mutexes );    // locks: 1, 2
    assert( MutexErrors::Success == result );

    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );

    SpinMutex mutex4( 1 );
    result = mutex4.Lock();
    assert( MutexErrors::Success == result );
    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );

    try
    {
        result = LevelMutexInfo::MultiLock( mutexes );
    }
    catch ( const MutexException & ex1 )
    {
        assert( MutexErrors::LevelTooHigh == ex1.GetReason() );
    }
    catch ( ... )
    {
        assert( false );
    }
    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex4 );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( MutexErrors::NotRecentLock == result );

    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( MutexErrors::NotRecentLock == result );
    result = mutex4.Unlock();
    assert( MutexErrors::Success == result );

    // Make sure current thread has released locks acquired in this function.
    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );

    // What if this tries to unlock multiple mutexes when nothing is locked?
    mutexes.clear();
    mutexes.push_back( &mutex1 );
    mutexes.push_back( &mutex2 );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( MutexErrors::NotRecentLock == result );

    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountLocksInCurrentThread() == priorLockCount );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
}

// ----------------------------------------------------------------------------

void SingleThreadExceptionTest( void )
{

    const unsigned int priorLevel = GetCurrentThreadsLevel();
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

    ExceptionMutex mutex1a( 1 );
    mutex1a.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Always );
    ExceptionMutex mutex2a( 2 );
    mutex2a.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );
    MutexErrors::Type result = MutexErrors::Success;

    try
    {
        mutex1a.Lock();
        assert( false );
    }
    catch ( ... )
    {
        assert( true );
    }

    try
    {
        result = mutex2a.Lock();
        assert( MutexErrors::Success == result );
        mutex1a.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );
        result = mutex1a.Lock();
        assert( MutexErrors::Success == result );
        result = mutex1a.Unlock();
        assert( MutexErrors::Success == result );
        result = mutex2a.Unlock();
        assert( MutexErrors::Success == result );
    }
    catch ( ... )
    {
        assert( false );
    }

    ExceptionMutex mutex2b( 2 );
    mutex2b.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Always );
    ExceptionMutex mutex2c( 2 );
    mutex2c.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );
    ExceptionMutex mutex2d( 2 );
    mutex2d.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );
    LevelMutexInfo::MutexContainer mutexes;
    try
    {
        mutexes.push_back( &mutex2a );
        mutexes.push_back( &mutex2b );
        mutexes.push_back( &mutex2c );
        mutexes.push_back( &mutex2d );
        result = LevelMutexInfo::MultiLock( mutexes );
        assert( false );
    }
    catch ( ... )
    {
        assert( true );
    }

    bool okay = ( !mutex2a.IsLockedByCurrentThread() );
    assert( okay );
    okay = ( !mutex2b.IsLockedByCurrentThread() );
    assert( okay );
    okay = ( !mutex2c.IsLockedByCurrentThread() );
    assert( okay );
    okay = ( !mutex2d.IsLockedByCurrentThread() );
    assert( okay );

    mutex2b.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );

    ExceptionMutex mutex3a( 3 );
    mutex3a.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );
    ExceptionMutex mutex3b( 3 );
    mutex3b.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Never );
    mutexes.clear();
    try
    {
        mutexes.push_back( &mutex3a );
        mutexes.push_back( &mutex3b );
        result = LevelMutexInfo::MultiLock( mutexes ); // 3a, 3b
        assert( true );
        assert( MutexErrors::Success == result );
        okay = ( mutex3a.IsLockedByCurrentThread() );
        assert( okay );
        okay = ( mutex3b.IsLockedByCurrentThread() );
        assert( okay );
    }
    catch ( ... )
    {
        assert( false );
    }

    try
    {
        mutex2c.GetMutexPolicy().SetTossPolicy( ExceptionTossingMutex::Always );
        mutexes.clear();
        mutexes.push_back( &mutex2a );
        mutexes.push_back( &mutex2b );
        mutexes.push_back( &mutex2c );
        mutexes.push_back( &mutex2d );
        result = LevelMutexInfo::MultiLock( mutexes );
        assert( false );
    }
    catch ( ... )
    {
        assert( true );
        okay = ( !mutex2a.IsLockedByCurrentThread() );
        assert( okay );
        okay = ( !mutex2b.IsLockedByCurrentThread() );
        assert( okay );
        okay = ( !mutex2c.IsLockedByCurrentThread() );
        assert( okay );
        okay = ( !mutex2d.IsLockedByCurrentThread() );
        assert( okay );
        okay = ( mutex3a.IsLockedByCurrentThread() );
        assert( okay );
        okay = ( mutex3b.IsLockedByCurrentThread() );
        assert( okay );
    }

    mutexes.clear();
    mutexes.push_back( &mutex3a );
    mutexes.push_back( &mutex3b );
    result = LevelMutexInfo::MultiUnlock( mutexes );
    assert( MutexErrors::Success == result );

    okay = ( GetCurrentThreadsLevel() == priorLevel );
    assert( okay );
    okay = ( CountLocksInCurrentThread() == priorLockCount );
    assert( okay );
    okay = ( CountMutexesInCurrentThread() == priorMutexCount );
    assert( okay );
    okay = ( CountMutexesAtCurrentLevel()  == priorLevelMutexCount );
    assert( okay );
}

// ----------------------------------------------------------------------------

int main( unsigned int argc, const char * const argv[] )
{

    cout << "Starting LevelMutex tests." << endl;
    if ( 1 < argc )
    {
        if ( ::strcmp( argv[1], "-r" ) == 0 )
            SetToRedoSingleTests( true );
    }

    try
    {
        SingleThreadSimpleTest();
        SingleThreadReentrantTest();
        SingleThreadSimpleMultiLockTest();
        SingleThreadComplexMultiLockTest( true );
        SingleThreadExceptionTest();

        MultiThreadSimpleTest();
        MultiThreadTryLockTest();
        MultiThreadReentrantTest();
        MultiThreadMultiLockTest();
        MultiThreadRandomMultiLockTest();

        // This tests for single locking in a multi-threaded environment.
        MultiThreadHierarchySingleLockTest();
        // This tests for multi-locking and single locking in a multi-threaded environment.
        MultiThreadHierarchyMultiLockTest();
    }
    catch ( ... )
    {
        cout << "Caught unknown exception!" << endl;
    }

    ::system( "pause" );
    return 0;
}

// ----------------------------------------------------------------------------
