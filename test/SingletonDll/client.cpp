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

// $Header$

/*

Test for singletons in a shared libraries:

- there is a Foo class in Foo.dll/so
- there is a Foo singleton object which is exported by SingletonDll.dll/so
- the Foo singleton object is  managed by a Loki::SingletonHolder typedef
- the declaration of the Loki::SingletonHolder type is visiable only
  in singletondll.cpp
- a client (this file) imports the singleton object from SingletonDll.dll/so

*/

#include "singletondll.h"
#include "foo.h"



int main()
{
	Foo& foo = Singleton<Foo>::Instance();
	Foo& lokifoo = Loki::Singleton<Foo>::Instance();
	foo.foo();
	lokifoo.foo();


#if defined(__BORLANDC__) || defined(_MSC_VER)
    system("PAUSE");
#endif

	return 0;
}