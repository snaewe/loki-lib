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

//#define LOKI_CLASS_LEVEL_THREADING

#ifndef LOKI_CLASS_LEVEL_THREADING
    #define LOKI_OBJECT_LEVEL_THREADING
#endif

#if defined(_WIN32)
    #include <windows.h>
    #include <process.h>
#endif

#include <loki/Threads.h>
#include <loki/StrongPtr.h>
#include <loki/SafeFormat.h>

#include <vector>
#include <iostream>
#include <cassert>


// ----------------------------------------------------------------------------

#if defined(_WIN32)

    typedef unsigned int ( WINAPI * ThreadFunction_ )( void * );

    #define LOKI_pthread_t HANDLE

    #define LOKI_pthread_create(handle,attr,func,arg) \
        (int)((*handle=(HANDLE) _beginthreadex (NULL,0,(ThreadFunction_)func,arg,0,NULL))==NULL)

    #define LOKI_pthread_join(thread, result) \
        ((::WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))

#else

    #define LOKI_pthread_t \
                 pthread_t
    #define LOKI_pthread_create(handle,attr,func,arg) \
                 pthread_create(handle,attr,func,arg)
    #define LOKI_pthread_join(thread, result) \
                 pthread_join(thread, result)

#endif

using namespace std;
using namespace Loki;

// ----------------------------------------------------------------------------

class Thread
{
public:

    typedef void * ( * CallFunction )( void * );

    Thread( CallFunction func, void * parm )
        : pthread_()
        , func_( func )
        , parm_( parm )
    {
    }

    void AssignTask( CallFunction func, void * parm )
    {
        func_ = func;
        parm_ = parm;
    }

    int Start( void )
    {
        return LOKI_pthread_create( &pthread_, NULL, func_, parm_ );
    }

    int WaitForThread( void ) const
    {
        int status = 0;
        (void) status;
        return LOKI_pthread_join( pthread_,
            reinterpret_cast< void * * >( &status ) );
    }

private:
    LOKI_pthread_t pthread_;
    CallFunction func_;
    void * parm_;
};

// ----------------------------------------------------------------------------

class ThreadPool
{
public:
    ThreadPool( void ) : m_threads()
    {
    }

    void Create( size_t threadCount, Thread::CallFunction function )
    {
        for( size_t ii = 0; ii < threadCount; ii++ )
        {
            Printf( "Creating thread %d\n" )( ii );
            Thread * thread = new Thread( function,
                reinterpret_cast< void * >( ii ) );
            m_threads.push_back( thread );
        }
    }

    void Start( void )
    {
        for ( size_t ii = 0; ii < m_threads.size(); ii++ )
            m_threads.at( ii )->Start();
    }

    void Join( void ) const
    {
        for ( size_t ii = 0; ii < m_threads.size(); ii++ )
            m_threads.at( ii )->WaitForThread();
    }

    ~ThreadPool( void )
    {
        for ( size_t ii = 0; ii < m_threads.size(); ii++ )
        {
            delete m_threads.at(ii);
        }
    }

private:
    typedef std::vector< Thread * > Threads;

    Threads m_threads;
};

// ----------------------------------------------------------------------------

static const unsigned int loop = 5;

class A
{
public:

    A( void ) {}

#define  BIG_FOR_LOOP for( unsigned int i = 0; i < 10000000; i++ ) g++;

    void Print( void * id ) const
    {
        BIG_FOR_LOOP;Printf("%p: ----------------\n")(id);
        BIG_FOR_LOOP;Printf("%p: ---------------\n")(id);
        BIG_FOR_LOOP;Printf("%p: --------------\n")(id);
        BIG_FOR_LOOP;Printf("%p: -------------\n")(id);
        BIG_FOR_LOOP;Printf("%p: ------------\n")(id);
        BIG_FOR_LOOP;Printf("%p: -----------\n")(id);
        BIG_FOR_LOOP;Printf("%p: ----------\n")(id);
        BIG_FOR_LOOP;Printf("%p: ---------\n")(id);
        BIG_FOR_LOOP;Printf("%p: --------\n")(id);
        BIG_FOR_LOOP;Printf("%p: -------\n")(id);
        BIG_FOR_LOOP;Printf("%p: ------\n")(id);
        BIG_FOR_LOOP;Printf("%p: -----\n")(id);
        BIG_FOR_LOOP;Printf("%p: ----\n")(id);
        BIG_FOR_LOOP;Printf("%p: ---\n")(id);
        BIG_FOR_LOOP;Printf("%p: --\n")(id);
        BIG_FOR_LOOP;Printf("%p: -\n")(id);
        BIG_FOR_LOOP;Printf("%p: \n")(id);
    }

private:

    static unsigned int g;
};

unsigned int A::g = 0;

typedef Loki::StrongPtr< A, true, TwoRefCounts, DisallowConversion,
    NoCheck, NeverReset, DeleteSingle, DontPropagateConst >
    A_ptr;

typedef Loki::StrongPtr< A, true, LockableTwoRefCounts, DisallowConversion,
    NoCheck, NeverReset, DeleteSingle, DontPropagateConst >
    A_Lockable_ptr;

// ----------------------------------------------------------------------------

class SafeA
{
public:
    static SafeA & GetIt( void )
    {
        if ( NULL == s_instance )
            s_instance = new SafeA;
        return *s_instance;
    }

    static void Destroy( void )
    {
        if ( NULL == s_instance )
        {
             delete s_instance;
             s_instance = NULL;
        }
    }

    A_Lockable_ptr GetA (void) { return m_ptr; }

private:
    static SafeA * s_instance;

    SafeA( void ) : m_ptr( new A ) {}

    ~SafeA( void ) {}

    A_Lockable_ptr m_ptr;
};

SafeA * SafeA::s_instance = NULL;

// ----------------------------------------------------------------------------

class UnsafeA
{
public:
    static UnsafeA & GetIt( void )
    {
        if ( NULL == s_instance )
            s_instance = new UnsafeA;
        return *s_instance;
    }

    static void Destroy( void )
    {
        if ( NULL == s_instance )
        {
             delete s_instance;
             s_instance = NULL;
        }
    }

    A_ptr GetA (void) { return m_ptr; }

private:
    static UnsafeA * s_instance;

    UnsafeA( void ) : m_ptr( new A ) {}

    ~UnsafeA( void ) {}

    A_ptr m_ptr;
};

UnsafeA * UnsafeA::s_instance = NULL;

// ----------------------------------------------------------------------------

void * RunLocked( void * id )
{
    A_Lockable_ptr ap( SafeA::GetIt().GetA() );
    for( unsigned int i = 0; i < loop; i++ )
    {
        ap.Lock();
        ap->Print( id );
        ap.Unlock();
    }
    return 0;
}

// ----------------------------------------------------------------------------

void * Run( void * id )
{
    A_ptr ap( UnsafeA::GetIt().GetA() );
    for( unsigned int i = 0; i < loop; i++ )
    {
        ap->Print( id );
    }
    return 0;
}

// ----------------------------------------------------------------------------

void DoLockedPtrTest( void )
{
#if defined(_MSC_VER)
     /** @note For some reason, mutexes implemented using Microsoft's
       CriticalSection don't work properly unless this mutex is here.
       */
     Loki::Mutex mutex;

     SafeA::GetIt();
     UnsafeA::GetIt();
     ::system( "pause" );
     {
         ThreadPool pool;
         pool.Create( 5, RunLocked );
         pool.Start();
         pool.Join();
     }
     ::system( "pause" );
     {
         ThreadPool pool;
         pool.Create( 5, Run );
         pool.Start();
         pool.Join();
     }
     SafeA::Destroy();
     UnsafeA::Destroy();
#endif
}

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.1  2006/04/28 00:34:21  rich_sposato
// Added test for thread-safe StrongPtr policy.
//
