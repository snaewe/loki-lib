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

// ----------------------------------------------------------------------------

#include "ThreadPool.hpp"

#include <assert.h>

#include <process.h>


using namespace ::std;

// define nullptr even though new compilers will have this keyword just so we
// have a consistent and easy way of identifying which uses of 0 mean null.
#define nullptr 0

#if defined( DEBUG ) || defined( _DEBUG )
    #define LOKI_DEBUG_CODE( code ) code;
#else
    #define LOKI_DEBUG_CODE( code ) ;
#endif


volatile Thread * Thread::s_thread = nullptr;


// ----------------------------------------------------------------------------

Thread::Thread( volatile ThreadPool * owner ) :
    m_thread(),
    m_func( nullptr ),
    m_parm( nullptr ),
    m_status( Thread::Idle ),
    m_stop( false ),
    m_owner( owner )
{
    assert( IsValid( owner ) );
}

// ----------------------------------------------------------------------------

Thread::Thread( volatile ThreadPool * owner, CallFunction function,
    void * parm ) :
    m_thread(),
    m_func( function ),
    m_parm( parm ),
    m_status( Thread::Starting ),
    m_stop( false ),
    m_owner( owner )
{
    assert( IsValid( owner ) );
    assert( nullptr != m_func );
}

// ----------------------------------------------------------------------------

Thread::~Thread( void )
{
    assert( IsValid( m_owner ) );
    assert( Thread::Dead == m_status );
    assert( nullptr == m_func );
    assert( nullptr == m_parm );
    assert( m_stop );
}

// ----------------------------------------------------------------------------

bool Thread::WaitForThread( void ) volatile
{
    assert( IsValid( m_owner ) );
    const volatile Thread * current = Thread::GetCurrentThread();
    if ( this == current )
        return false;
    if ( m_status == Thread::Dead )
        return false;
    while ( this->m_status == Thread::Active )
    {
        // Call the wait policy.
#if defined( _MSC_VER )
        ::SleepEx( 1, true );
#else
        ::sleep( 1 );
#endif
    }
    return true;
}

// ----------------------------------------------------------------------------

bool Thread::IsValid( const volatile ThreadPool * owner ) const volatile
{
    assert( nullptr != this );
    assert( nullptr != m_owner );
    assert( nullptr != owner );
    assert( m_owner == owner );
    switch ( m_status )
    {
        case Thread::Dead:
        case Thread::Idle:
            assert( nullptr == m_func );
            assert( nullptr == m_parm );
            break;
        case Thread::Active:
        case Thread::Starting:
            assert( nullptr != m_func );
            break;
        default:
            assert( false );
            break;
    }
    return true;
}

// ----------------------------------------------------------------------------

ThreadPool::ThreadPool( unsigned int threadCount ) :
    m_threads()
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )
    if ( 0 < threadCount )
        Create( threadCount );
}

// ----------------------------------------------------------------------------

ThreadPool::~ThreadPool( void )
{
    assert( IsValid() );

    ThreadsIter end( m_threads.end() );
    for ( ThreadsIter it( m_threads.begin() ); it != end; ++it )
    {
        try
        {
            volatile Thread * thread = *it;
            assert( nullptr != thread );
            thread->m_stop = true;
            thread->WaitForThread();
            if ( thread->GetStatus() == Thread::Idle )
                thread->m_status = Thread::Dead;
            LokiThreadJoin( thread->m_thread );
            *it = nullptr;
            delete thread;
        }
        catch ( ... )
        {
        }
    }
}

// ----------------------------------------------------------------------------

unsigned int ThreadPool::Create( unsigned int threadCount ) volatile
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )

    ThreadPool * pThis = const_cast< ThreadPool * >( this );
    const unsigned int countNow = GetCount( Thread::Idle );
    if ( threadCount <= countNow )
        return threadCount;

    const unsigned int totalCount = pThis->m_threads.size();
    const unsigned int howManyToAdd = threadCount - countNow;
    if ( pThis->m_threads.capacity() <= howManyToAdd )
        pThis->m_threads.reserve( totalCount + howManyToAdd );
    for ( unsigned int ii = 0; ii < howManyToAdd; ++ii )
    {
#if defined( _MSC_VER )
        volatile Thread * thread = new Thread( this );
#else
        Thread * thread = new Thread( this );
#endif
        pThis->m_threads.push_back( thread );
        Thread * pThread = const_cast< Thread * >( thread );
        void * p = reinterpret_cast< void * >( pThread );
        // Call thread creation policy?
        LokiThreadCreate( &thread->m_thread, nullptr, TopFunction, p );
    }

    return howManyToAdd;
}

// ----------------------------------------------------------------------------

unsigned int ThreadPool::GetCount( void ) const volatile
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )
    ThreadPool * pThis = const_cast< ThreadPool * >( this );
    const unsigned int count = pThis->m_threads.size();
    return count;
}

// ----------------------------------------------------------------------------

unsigned int ThreadPool::GetCount( Thread::Status status ) const volatile
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )

    ThreadPool * pThis = const_cast< ThreadPool * >( this );
    const unsigned int totalCount = pThis->m_threads.size();
    unsigned int count = 0;
    for ( size_t ii = 0; ii < totalCount; ii++ )
    {
        const volatile Thread * thread = pThis->m_threads.at( ii );
        assert( nullptr != thread );
        if ( thread->GetStatus() == status )
            ++count;
    }

    return count;
}

// ----------------------------------------------------------------------------

volatile const Thread * ThreadPool::GetThread( unsigned int index ) const volatile
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )
    ThreadPool * pThis = const_cast< ThreadPool * >( this );

    if ( pThis->m_threads.size() <= index )
        return nullptr;
    volatile const Thread * thread = pThis->m_threads[ index ];
    return thread;
}

// ----------------------------------------------------------------------------

void ThreadPool::Join( unsigned int index ) const volatile
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )
    ThreadPool * pThis = const_cast< ThreadPool * >( this );

    if ( pThis->m_threads.size() <= index )
        return;
    volatile Thread * thread = pThis->m_threads[ index ];
    thread->WaitForThread();
}

// ----------------------------------------------------------------------------

void ThreadPool::JoinAll( void ) volatile const
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )
    ThreadPool * pThis = const_cast< ThreadPool * >( this );

    for ( size_t ii = 0; ii < pThis->m_threads.size(); ii++ )
    {
        volatile Thread * thread = pThis->m_threads.at( ii );
        assert( nullptr != thread );
        thread->WaitForThread();
    }
}

// ----------------------------------------------------------------------------

volatile Thread * ThreadPool::Start( CallFunction function, void * parm ) volatile
{
    assert( IsValid() );
    LOKI_DEBUG_CODE( Checker checker( this ); (void)checker; )
    ThreadPool * pThis = const_cast< ThreadPool * >( this );

    if ( nullptr == function )
        return nullptr;
#if defined( _MSC_VER )
    volatile Thread * thread = nullptr;
#else
    Thread * thread = nullptr;
#endif
    bool foundOne = false;

    for ( size_t ii = 0; ii < pThis->m_threads.size(); ii++ )
    {
#if defined( _MSC_VER )
        thread = pThis->m_threads.at( ii );
#else
        thread = const_cast< Thread * >( pThis->m_threads.at( ii ) );
#endif
        assert( nullptr != thread );
        if ( Thread::Idle == thread->m_status )
        {
            foundOne = true;
            break;
        }
    }

    if ( foundOne )
    {
        thread->m_func = function;
        thread->m_parm = parm;
        thread->m_status = Thread::Starting;
    }
    else
    {
        // Did not find an idle thread, so start a new one.
        thread = new Thread( this, function, parm );
        pThis->m_threads.push_back( thread );
        Thread * pThread = const_cast< Thread * >( thread );
        void * p = reinterpret_cast< void * >( pThread );
        // Call to thread creation policy?
        LokiThreadCreate( &thread->m_thread, nullptr, TopFunction, p );
    }

    return thread;
}

// ----------------------------------------------------------------------------

#if defined( _MSC_VER )
    unsigned int ThreadPool::TopFunction( void * p )
#else
    void * ThreadPool::TopFunction( void * p )
#endif
{
    assert( nullptr != p );

    volatile Thread * thread = reinterpret_cast< volatile Thread * >( p );
    Thread::SetCurrentThread( thread );
    while ( ( thread->m_status != Thread::Dead ) && ( !thread->m_stop ) )
    {
        // Call the thread's WaitPolicy here?
#if defined( _MSC_VER )
        ::SleepEx( 1, true );
#else
        ::sleep( 1 );
#endif
        if ( thread->m_status == Thread::Starting )
        {
            try
            {
                assert( nullptr != thread->m_func );
                thread->m_status = Thread::Active;
                thread->m_func( thread->m_parm );
            }
            catch ( ... )
            {
                // What to do in case of exception?
                // Call an exception policy?
            }
            thread->m_status = Thread::Idle;
            thread->m_func = nullptr;
            thread->m_parm = nullptr;
        }
    }

#if defined( _MSC_VER )
    return 0;
#else
    return nullptr;
#endif
}

// ----------------------------------------------------------------------------

bool ThreadPool::IsValid( void ) const volatile
{
    assert( nullptr != this );
    ThreadPool * pThis = const_cast< ThreadPool * >( this );
    ThreadsCIter end( pThis->m_threads.end() );
    for ( ThreadsCIter it( pThis->m_threads.begin() ); it != end; ++it )
    {
        const volatile Thread * thread = *it;
        assert( nullptr != thread );
        assert( thread->IsValid( this ) );
    }
    return true;
}

// ----------------------------------------------------------------------------
