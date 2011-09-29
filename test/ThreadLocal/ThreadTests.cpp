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

#include "ThreadPool.hpp"

#include <loki/ThreadLocal.h>

#include <vector>
#include <sstream>
#include <iostream>

#include <cassert>


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
