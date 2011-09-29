////////////////////////////////////////////////////////////////////////////////
//
// ThreadLocal test program for The Loki Library
// Copyright (c) 2009 by Richard Sposato
// The copyright on this file is protected under the terms of the MIT license.
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
//
////////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------

#include <vector>

#if !defined( NULL )
    #define NULL 0
#endif

// define nullptr even though new compilers will have this keyword just so we
// have a consistent and easy way of identifying which uses of 0 mean null.
#if !defined( nullptr )
    #define nullptr NULL
#endif

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

// ----------------------------------------------------------------------------

class Thread
{
public:

    typedef void * ( * CallFunction )( void * );

    Thread( CallFunction func, void * parm );

    void AssignTask( CallFunction func, void * parm );

    int Start( void );

    int WaitForThread( void ) const;

private:

    LOKI_pthread_t pthread_;

    CallFunction func_;

    void * parm_;

};

// ----------------------------------------------------------------------------

class ThreadPool
{
public:

    ThreadPool( void );

    ~ThreadPool( void );

    void Create( size_t threadCount, Thread::CallFunction function );

    void Start( void );

    void Join( void ) const;

private:

    typedef ::std::vector< Thread * > Threads;

    Threads m_threads;
};

// ----------------------------------------------------------------------------
