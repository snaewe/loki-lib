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

#include <sstream>

// ----------------------------------------------------------------------------

Thread::Thread( CallFunction func, void * parm )
	: pthread_()
	, func_( func )
	, parm_( parm )
{
}

// ----------------------------------------------------------------------------

void Thread::AssignTask( CallFunction func, void * parm )
{
	func_ = func;
	parm_ = parm;
}

// ----------------------------------------------------------------------------

int Thread::Start( void )
{
	return LOKI_pthread_create( &pthread_, NULL, func_, parm_ );
}

// ----------------------------------------------------------------------------

int Thread::WaitForThread( void ) const
{
	return LOKI_pthread_join( pthread_ );
}

// ----------------------------------------------------------------------------

ThreadPool::ThreadPool( void ) : m_threads()
{
}

// ----------------------------------------------------------------------------

ThreadPool::~ThreadPool( void )
{
	for ( size_t ii = 0; ii < m_threads.size(); ++ii )
	{
		delete m_threads.at(ii);
	}
}

// ----------------------------------------------------------------------------

void ThreadPool::Create( size_t threadCount, Thread::CallFunction function )
{
	for( size_t ii = 0; ii < threadCount; ii++ )
	{
		::std::stringstream buffer;
		Thread * thread = new Thread( function,
			reinterpret_cast< void * >( ii + 1 ) );
		m_threads.push_back( thread );
	}
}

// ----------------------------------------------------------------------------

void ThreadPool::Start( void )
{
	for ( size_t ii = 0; ii < m_threads.size(); ii++ )
	{
		::std::stringstream buffer;
		m_threads.at( ii )->Start();
	}
}

// ----------------------------------------------------------------------------

void ThreadPool::Join( void ) const
{
	for ( size_t ii = 0; ii < m_threads.size(); ii++ )
		m_threads.at( ii )->WaitForThread();
}

// ----------------------------------------------------------------------------
