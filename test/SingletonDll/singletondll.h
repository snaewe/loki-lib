////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_SINGLETONDLL_H
#define LOKI_SINGLETONDLL_H

// $Header:

#include "singletondll_export.h"

class Foo;

// Use the predefined Loki::Singleton
// and export Loki::Singleton<Foo>
#include <loki/Singleton.h>

template class SINGLETONDLL_EXPORT Loki::Singleton<Foo>;


// declare the Singleton template by yourself
// and export Singleton<Foo>
// so the singleton is not in the Loki namespace
template<class T>
class Singleton
{
public:
	static T& Instance();
};

template class SINGLETONDLL_EXPORT Singleton<Foo>;



#endif
