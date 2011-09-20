////////////////////////////////////////////////////////////////////////////////
//
// ThreadLocal test program for The Loki Library
// Copyright (c) 2009 by Richard Sposato
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

#include <loki/ThreadLocal.h>
#include <loki/Threads.h>

#include <vector>
#include <sstream>
#include <iostream>

#include <stdlib.h>

#if defined(_WIN32)

    #include <process.h>
    #include <windows.h>

    typedef unsigned int ( WINAPI * ThreadFunction_ )( void * );

    #define LOKI_pthread_t HANDLE

    #define LOKI_pthread_create( handle, attr, func, arg ) \
        ( int )( ( *handle = ( HANDLE ) _beginthreadex ( NULL, 0, ( ThreadFunction_ )func, arg, 0, NULL ) ) == NULL )

    #define LOKI_pthread_join( thread ) \
        ( ( WaitForSingleObject( ( thread ), INFINITE ) != WAIT_OBJECT_0 ) || !CloseHandle( thread ) )

#else

	#include <pthread.h>

    #define LOKI_pthread_t \
                 pthread_t
    #define LOKI_pthread_create(handle,attr,func,arg) \
                 pthread_create(handle,attr,func,arg)
    #define LOKI_pthread_join(thread) \
                 pthread_join(thread, NULL)

#endif


using namespace ::std;

#if !defined( NULL )
    #define NULL 0
#endif

// define nullptr even though new compilers will have this keyword just so we
// have a consistent and easy way of identifying which uses of 0 mean null.
#if !defined( nullptr )
    #define nullptr NULL
#endif

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
        return LOKI_pthread_join( pthread_ );
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
            stringstream buffer;
            buffer << "Creating thread " << ii << endl;
            cout << buffer.rdbuf();
            Thread * thread = new Thread( function,
                reinterpret_cast< void * >( ii + 1 ) );
            m_threads.push_back( thread );
        }
    }

    void Start( void )
    {
        for ( size_t ii = 0; ii < m_threads.size(); ii++ )
        {
            stringstream buffer;
            buffer << "Starting thread " << ii << endl;
            cout << buffer.rdbuf();
            m_threads.at( ii )->Start();
        }
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

typedef ::std::vector< unsigned int > IntVector;

static LOKI_THREAD_LOCAL unsigned int StandaloneStaticValue = 0;

static const unsigned int ThreadCount = 4;

// ----------------------------------------------------------------------------

IntVector & GetIntVector( void )
{
    unsigned int v = 0;
    static IntVector addresses( ThreadCount, v );
    return addresses;
}

// ----------------------------------------------------------------------------

void * AddToIntVector( void * p )
{
    assert( 0 == StandaloneStaticValue );
    const unsigned int ii = reinterpret_cast< unsigned int >( p );
    assert( 0 < ii );
    assert( ii < ThreadCount + 1 );
    StandaloneStaticValue = ii;
    IntVector & v = GetIntVector();
    v[ ii - 1 ] = StandaloneStaticValue;
    assert( ii == StandaloneStaticValue );
    assert( v[ ii - 1 ] == StandaloneStaticValue );
    return nullptr;
}

// ----------------------------------------------------------------------------

bool TestThreadLocalStaticValue( void )
{
    assert( StandaloneStaticValue == 0 );
    {
        ThreadPool pool;
        pool.Create( ThreadCount, &AddToIntVector );
        pool.Start();
        pool.Join();
    }

    bool allDifferent = true;
    IntVector & v = GetIntVector();
    for ( unsigned int i1 = 0; i1 < ThreadCount - 1; ++i1 )
    {
        const unsigned int v1 = v[ i1 ];
        for ( unsigned int i2 = i1 + 1; i2 < ThreadCount; ++i2 )
        {
            const unsigned int v2 = v[ i2 ];
            if ( v1 == v2 )
            {
                allDifferent = false;
                break;
            }
        }
        if ( !allDifferent )
            break;
    }
    assert( StandaloneStaticValue == 0 );

    return allDifferent;
}

// ----------------------------------------------------------------------------

unsigned int & GetFunctionThreadLocalValue( void )
{
    static LOKI_THREAD_LOCAL unsigned int FunctionStaticValue = 0;
    return FunctionStaticValue;
}

// ----------------------------------------------------------------------------

void * ChangeFunctionStaticValue( void * p )
{
    unsigned int & thatValue = GetFunctionThreadLocalValue();
    assert( 0 == thatValue );
    const unsigned int ii = reinterpret_cast< unsigned int >( p );
    assert( 0 < ii );
    assert( ii < ThreadCount + 1 );
    thatValue = ii + ThreadCount;
    IntVector & v = GetIntVector();
    v[ ii - 1 ] = thatValue + ThreadCount;
    assert( ii + ThreadCount == thatValue );
    assert( v[ ii - 1 ] == thatValue + ThreadCount );
    return nullptr;
}

// ----------------------------------------------------------------------------

bool TestThreadLocalFunctionStaticValue( void )
{
    assert( GetFunctionThreadLocalValue() == 0 );

    IntVector & v = GetIntVector();
    for ( unsigned int i0 = 0; i0 < v.size(); ++i0 )
    {
        v[ i0 ] = 0;
    }

    {
        ThreadPool pool;
        pool.Create( ThreadCount, &ChangeFunctionStaticValue );
        pool.Start();
        pool.Join();
    }

    bool allDifferent = true;
    for ( unsigned int i1 = 0; i1 < ThreadCount - 1; ++i1 )
    {
        const unsigned int v1 = v[ i1 ];
        for ( unsigned int i2 = i1 + 1; i2 < ThreadCount; ++i2 )
        {
            const unsigned int v2 = v[ i2 ];
            if ( v1 == v2 )
            {
                allDifferent = false;
                break;
            }
        }
        if ( !allDifferent )
            break;
    }
    assert( GetFunctionThreadLocalValue() == 0 );

    return allDifferent;
}

// ----------------------------------------------------------------------------

class ThreadAware
{
public:

    static inline void SetValue( unsigned int value ) { ClassThreadLocal = value; }

    static inline unsigned int GetValue( void ) { return ClassThreadLocal; }

private:

    static LOKI_THREAD_LOCAL unsigned int ClassThreadLocal;

};

LOKI_THREAD_LOCAL unsigned int ThreadAware::ClassThreadLocal = 0;

// ----------------------------------------------------------------------------

void * ChangeClassStaticValue( void * p )
{
    assert( ThreadAware::GetValue() == 0 );
    const unsigned int ii = reinterpret_cast< unsigned int >( p );
    assert( 0 < ii );
    assert( ii < ThreadCount + 1 );
    ThreadAware::SetValue( ii + 2 * ThreadCount );
    IntVector & v = GetIntVector();
    v[ ii - 1 ] = ThreadAware::GetValue();
    assert( v[ ii - 1 ] == ThreadAware::GetValue() );
    assert( ThreadAware::GetValue() == ii + 2 * ThreadCount );
    return nullptr;
}

// ----------------------------------------------------------------------------

bool TestThreadLocalClassStaticValue( void )
{
    assert( ThreadAware::GetValue() == 0 );

    IntVector & v = GetIntVector();
    for ( unsigned int i0 = 0; i0 < v.size(); ++i0 )
    {
        v[ i0 ] = 0;
    }

    {
        ThreadPool pool;
        pool.Create( ThreadCount, &ChangeClassStaticValue );
        pool.Start();
        pool.Join();
    }

    bool allDifferent = true;
    for ( unsigned int i1 = 0; i1 < ThreadCount - 1; ++i1 )
    {
        const unsigned int v1 = v[ i1 ];
        for ( unsigned int i2 = i1 + 1; i2 < ThreadCount; ++i2 )
        {
            const unsigned int v2 = v[ i2 ];
            if ( v1 == v2 )
            {
                allDifferent = false;
                break;
            }
        }
        if ( !allDifferent )
            break;
    }
    assert( ThreadAware::GetValue() == 0 );

    return allDifferent;
}

// ----------------------------------------------------------------------------

int main( int argc, const char * const argv[] )
{
	(void)argc;
	(void)argv;

    bool okay = true;
    cout << "Starting ThreadLocal tests." << endl;
    cout << "If any tests fail, or any assertions fail," << endl
         << "then your compiler does not implement thread_local storage correctly." << endl;

    cout << endl << "Testing static thread_local storage inside classes." << endl;
    okay = TestThreadLocalClassStaticValue();
    if ( okay )
        cout << "Your compiler correctly implements thread_local storage for class static values." << endl;
    else
        cout << "Your compiler does not properly implement thread_local storage for class static values." << endl;

    cout << endl << "Testing static thread_local storage inside functions." << endl;
    okay = TestThreadLocalFunctionStaticValue();
    if ( okay )
        cout << "Your compiler correctly implements thread_local storage for function static values." << endl;
    else
        cout << "Your compiler does not properly implement thread_local storage for function static values." << endl;

    cout << endl << "Testing standalone static thread_local storage." << endl;
    okay = TestThreadLocalStaticValue();
    if ( okay )
        cout << "Your compiler correctly implements thread_local storage for standalone static values." << endl;
    else
        cout << "Your compiler does not properly implement thread_local storage for standalone static values." << endl;

    ::system( "pause" );
    return 0;
}

// ----------------------------------------------------------------------------
