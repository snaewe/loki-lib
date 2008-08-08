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


#ifndef LOKI_MULTI_THREAD_TESTS_H_INCLUDED
#define LOKI_MULTI_THREAD_TESTS_H_INCLUDED

// ----------------------------------------------------------------------------

void SetToRedoSingleTests( bool redo );

bool WillRedoSingleTests( void );

void SingleThreadSimpleTest( void );

void SingleThreadReentrantTest( void );

void SingleThreadSimpleMultiLockTest( void );

void SingleThreadComplexMultiLockTest( bool doSetup );

void SingleThreadExceptionTest( void );

void MultiThreadSimpleTest( void );

void MultiThreadTryLockTest( void );

void MultiThreadReentrantTest( void );

void MultiThreadMultiLockTest( void );

void MultiThreadRandomMultiLockTest( void );

void MultiThreadHierarchySingleLockTest( void );

void MultiThreadHierarchyMultiLockTest( void );

// ----------------------------------------------------------------------------

#endif
