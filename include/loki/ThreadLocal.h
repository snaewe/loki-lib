////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2009 by Rich Sposato
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

#ifndef LOKI_THREAD_LOCAL_H_INCLUDED
#define LOKI_THREAD_LOCAL_H_INCLUDED

// $Id$


// ----------------------------------------------------------------------------

// First assume the compiler does allow thread-local storage by #defining the
// macro which allows compiler to see the code inside this file.
// Then #undef the macro for compilers which do not support thread-local
// storage or do not implement it correctly.
#define LOKI_THINKS_COMPILER_ALLOWS_THREAD_LOCAL_STORAGE 1

#if defined( __GNUC__ )
    // The __APPLE__ macro does not refer to a compiler, but to the Apple OSX operating system.
    #if defined( __APPLE__ )
        #warning "GCC for Apple does not allow thread_local storage, so you can not use some parts of Loki."
        #undef LOKI_THINKS_COMPILER_ALLOWS_THREAD_LOCAL_STORAGE

    #elif defined( __CYGWIN__ )
        #if ( __GNUC__ <= 3 )
            #warning "Older versions of GCC for Cygwin do not allow thread_local storage, so you can not use some parts of Loki."
            #undef LOKI_THINKS_COMPILER_ALLOWS_THREAD_LOCAL_STORAGE
        #endif

    #elif ( __GNUC__ == 4 ) // GNU versions other than Cygwin.
        #if ( __GNUC_MINOR__ < 4 )
            #warning "GCC versions before 4.4 implement thread_local storage incorrectly, so you can not use some parts of Loki."
            #undef LOKI_THINKS_COMPILER_ALLOWS_THREAD_LOCAL_STORAGE
        #elif ( __GNUC_MINOR__ == 4 )
            #warning "GCC version 4.4 implements thread_local storage incorrectly for some platforms but not others."
        #endif

    #endif
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
