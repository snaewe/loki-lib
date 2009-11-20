////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2009 by Rich Sposato
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef LOKI_THREAD_LOCAL_H_INCLUDED
#define LOKI_THREAD_LOCAL_H_INCLUDED

// $Id$


// ----------------------------------------------------------------------------

// First assume the compiler does allow thread-local storage by #defining the
// macro which allows compiler to see the code inside this file.
// Then #undef the macro for compilers which are known for not supporting
// thread-local storage.
#define LOKI_THINKS_COMPILER_ALLOWS_THREAD_LOCAL_STORAGE 1

// The __APPLE__ macro does not refer to a compiler, but to the Apple OSX operating system.
#if defined( __APPLE__ )
    #warning "GCC for Apple does not allow thread_local storage, so you can not use some parts of Loki."
    #undef COMPILER_ALLOWS_THREAD_LOCAL_STORAGE
#endif

#if ( defined( __CYGWIN__ ) && ( __GNUC__ <= 3 ) )
    #warning "Older versions of GCC for Cygwin do not allow thread_local storage, so you can not use some parts of Loki."
    #undef COMPILER_ALLOWS_THREAD_LOCAL_STORAGE
#endif

#if defined( LOKI_THINKS_COMPILER_ALLOWS_THREAD_LOCAL_STORAGE ) && !defined( LOKI_THREAD_LOCAL )

/** @par thread_local Keyword
 Some parts of Loki require compilers to provide thread local storage - meaning
 each thread gets its own copy of the data.  The next version of C++ will have
 a new keyword, thread_local for that purpose.  Some existing compilers already
 provide thread local storage using different syntax, so these lines use
 thread_local to mimic that syntax.  If your compiler provides thread local
 storage but using different syntax besides "thread_local", you may want to
 modify these lines.  If your compiler does not support thread local storage,
 you can't use some parts of Loki.
 */
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
        #warning "Run ThreadLocal test project to see if the compiler implements thread_local correctly."
        #define LOKI_THREAD_LOCAL thread_local
    #endif

#endif // if compiler allows thread_local storage and LOKI_THREAD_LOCAL macro not defined yet

// ----------------------------------------------------------------------------

#endif // end file guardian
