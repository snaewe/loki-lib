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

#include <vector>

// ----------------------------------------------------------------------------

typedef void * ( * CallFunction )( void * );

#if defined( _MSC_VER )
    #include <Windows.h>

    #define LokiThread HANDLE

    #define LokiThreadCreate( handle, attr, func, arg ) \
        (int)(( * handle = ( HANDLE ) _beginthreadex ( nullptr, 0, func, arg, 0, nullptr ) ) == nullptr )

    #define LokiThreadJoin( thread ) \
        ((::WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))

#else
    #include <pthread.h>

    typedef void * ( * CallFunction )( void * );

    #define LokiThread pthread_t
    #define LokiThreadCreate( handle, attr, func, arg ) \
                 pthread_create( handle, attr, func, arg )
    #define LokiThreadJoin( thread ) pthread_join( thread, NULL )

#endif


/** @par thread_local Keyword
 The mutexes require compilers to provide thread local storage - meaning each
 thread gets its own copy of the data.  The next version of C++ will have a
 new keyword, thread_local for that purpose.  Some existing compilers already
 provide thread local storage using different syntax, so these lines use
 thread_local to mimic that syntax.  If your compiler provides thread local
 storage but using different syntax besides "thread_local", you may want to
 modify these lines.  If your compiler does not support thread local storage,
 you can't use LevelMutex.
 */
#ifndef LOKI_THREAD_LOCAL
    #if defined( _MSC_VER )
        #if ( _MSC_VER >= 1300 )
            #define LOKI_THREAD_LOCAL __declspec( thread )
        #else
            #error "Only Visual Studio versions 7.0 and after supported."
        #endif

    #elif ( __GNUC__ )
        #define LOKI_THREAD_LOCAL __thread

    #else
        #warning "Check if your compiler provides thread local storage."
        #define LOKI_THREAD_LOCAL thread_local
    #endif
#endif


class ThreadPool;

class Thread
{
public:

    enum Status
    {
        Dead = 0,
        Idle,
        Starting,
        Active
    };

    inline static volatile Thread * GetCurrentThread( void )
    {
        return s_thread;
    }

    bool WaitForThread( void ) volatile;

    inline CallFunction GetFunction( void ) const volatile { return m_func; }

    inline void * GetParameter( void ) const volatile { return m_parm; }

    inline Status GetStatus( void ) const volatile { return m_status; }

    inline const volatile ThreadPool * GetThreadPool( void ) const volatile
    {
        return m_owner;
    }

    inline volatile ThreadPool * GetThreadPool( void ) volatile
    {
        return m_owner;
    }

private:

    friend class ThreadPool;

    inline static void SetCurrentThread( volatile Thread * thread )
    {
        s_thread = thread;
    }

    explicit Thread( volatile ThreadPool * owner );

    Thread( volatile ThreadPool * owner, CallFunction function, void * parm );

    ~Thread( void );

    /// Default constructor not implemented.
    Thread( void );

    /// not implemented.
    Thread( const Thread & );

    /// not implemented.
    Thread & operator = ( const Thread & );

    bool IsValid( const volatile ThreadPool * owner ) const volatile;

    static LOKI_THREAD_LOCAL volatile Thread * s_thread;

    LokiThread m_thread;
    CallFunction m_func;
    void * m_parm;
    Status m_status;
    bool m_stop;
    volatile ThreadPool * const m_owner;
};

class ThreadPool
{
public:

    explicit ThreadPool( unsigned int threadCount = 0 );

    ~ThreadPool( void );

    unsigned int Create( unsigned int threadCount ) volatile;

    volatile Thread * Start( CallFunction function, void * parm ) volatile;

    void Join( unsigned int index ) const volatile;

    void JoinAll( void ) const volatile;

    unsigned int GetCount( void ) const volatile;

    unsigned int GetCount( Thread::Status status ) const volatile;

    volatile const Thread * GetThread( unsigned int index ) const volatile;

    inline volatile Thread * GetThread( unsigned int index ) volatile
    {
        return const_cast< volatile Thread * >(
            const_cast< const volatile ThreadPool * >( this )->
                GetThread( index ) );
    }

private:

    /** @class Checker
     Performs validity check on ThreadPool to insure no class invariants were
     violated inside any member function.  This class only gets used in debug
     builds, and any instance of it gets optimized away in a release build.  A
     checker is created inside many of member functions so that it's destructor
     gets called when the function exits.
     */
    class Checker
    {
    public:
        inline explicit Checker( const volatile ThreadPool * mutex ) :
            m_pool( mutex ) {}
        inline ~Checker( void ) { m_pool->IsValid(); }
    private:
        Checker( void );
        Checker( const Checker & );
        Checker & operator = ( const Checker & );
        const volatile ThreadPool * m_pool;
    };

    /// Copy-constructor is not implemented.
    ThreadPool( const ThreadPool & );

    /// Copy-assignment operator is not implemented.
    ThreadPool & operator = ( const ThreadPool & );

#if defined( _MSC_VER )
    static unsigned int __stdcall TopFunction( void * p );
#else
    static void * TopFunction( void * p );
#endif

    /** Returns true if no class invariant broken, otherwise asserts.  This function
    only gets called in debug builds.
     */
    bool IsValid( void ) const volatile;

    typedef ::std::vector< volatile Thread * > Threads;
    typedef Threads::iterator ThreadsIter;
    typedef Threads::const_iterator ThreadsCIter;

    Threads m_threads;
};

// ----------------------------------------------------------------------------
