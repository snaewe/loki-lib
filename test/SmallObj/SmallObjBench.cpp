////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter Kümmel
// Copyright (c) 2005 Richard Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The authors make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$

//#define LOKI_CLASS_LEVEL_THREADING
//#define LOKI_OBJECT_LEVEL_THREADING

// Uncomment this to test new [] and delete [].
//#define LOKI_SMALL_OBJECT_USE_NEW_ARRAY

#include "SmallObj.h"
#include "timer.h"

#include <iostream>
#include <string>


using namespace std;
	
	
		
class ThisIsASmallObject
{
	int i;
	double d;
	//std::string s;
};	

template<class T>		
struct Base : public ThisIsASmallObject, public T {};
template<>		
struct Base<void> : public ThisIsASmallObject {};

typedef Base<void> 
A;
typedef Base<Loki::SmallObject<> > 
B;
typedef Base<Loki::SmallValueObject<> > 
C;


/*
class A 
{ int i; int* p;};
class B : public Loki::SmallObject<>
{ int i; int* p;};
class C : public Loki::SmallValueObject<> 
{ int i; int* p;};
*/


template<class T>
int run_new_delete(int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/	 
	for (int i=0; i<loop; ++i)
	{		
		T* p = new T;
		delete p;
	}
	/****************************************************************/	 
	t.stop();
	t.print(t.t(),s);
	return t.t();
}

template<class T>
int run_new_delete(T** array, int N, int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/	 
	for (int i=0; i<loop; ++i)
		for (int n=0; n<N; n++)
		{	
			array[n] = new T;
			delete array[n];
		}
	/****************************************************************/	 
	t.stop();	
	t.print(t.t(),s);
	return t.t();
}

template<class T>
int run_new(T** array, int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/	 
	for (int i=0; i<loop; ++i)	
		array[i] = new T;
	/****************************************************************/	 
	t.stop();	
	t.print(t.t(),s);
	return t.t();
}
template<class T>
int run_delete(T** array, int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/	 
	for (int i=0; i<loop; ++i)	
		delete array[i];
	/****************************************************************/	 
	t.stop();	
	t.print(t.t(),s);
	return t.t();
}


template<class T>
int run_new_delete_array(int N, int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/	 
	for (int i=0; i<loop; ++i)
	{		
		T* p = new T[N];
		delete [] p;
	}
	/****************************************************************/	 
	t.stop();
	t.print(t.t(),s);
	return t.t();
}

template<class T>
int run_new_array( int N, T** array, int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/
	for (int i=0; i<loop; ++i)	
		array[i] = new T[N];
	/****************************************************************/
	t.stop();
	t.print(t.t(),s);
	return t.t();
}
template<class T>
int run_delete_array( T** array, int loop, Timer& t, char* s)
{
	t.start();
	/****************************************************************/
	for (int i=0; i<loop; ++i)	
		delete [] array[i];
	/****************************************************************/
	t.stop();
	t.print(t.t(),s);
	return t.t();
}



int main()
{	
	int loop = 1000000
	;
	
	std::cout << "\n";
	
	Timer t;

	t.t100 = 0;
	t.t100 = run_new_delete<A>(loop,t,"new & delete A : ");
	run_new_delete<B>(loop,t,"new & delete B : ");
	run_new_delete<C>(loop,t,"new & delete C : ");
	
	cout << endl << endl;
//    Loki::AllocatorSingleton<>::ClearExtraMemory();
////////////////////////////////////////////////////////////////////////////////
	
	int N = 100000;
	int loop2 = loop/N*10;
	
	A** a = new A*[N];
	B** b = new B*[N];
	C** c = new C*[N];
	
	for(int i=0; i<N; i++)
	{
		a[i]=0;
		b[i]=0;
		c[i]=0;
	}
	
	t.t100 = 0;
	t.t100 = run_new_delete(a,N,loop2,t,"new & del. A on array: ");
	run_new_delete(b,N,loop2,t,"new & del. B on array: ");
	run_new_delete(c,N,loop2,t,"new & del. C on array: ");

	cout << endl << endl;
//    Loki::AllocatorSingleton<>::ClearExtraMemory();
////////////////////////////////////////////////////////////////////////////////


	t.t100 = 0;
	t.t100 = run_new(a,N,t,"new A on array : ");
	run_new(b,N,t,"new B on array : ");
	run_new(c,N,t,"new C on array : ");

	cout << endl;
////////////////////////////////////////////////////////////////////////////////

	t.t100 = 0;
	t.t100 = run_delete(a,N,t,"delete A on array : ");
	run_delete(b,N,t,"delete B on array : ");
	run_delete(c,N,t,"delete C on array : ");

	cout << endl << endl;
//    Loki::AllocatorSingleton<>::ClearExtraMemory();
////////////////////////////////////////////////////////////////////////////////


	N = 5;
	
	t.t100 = 0;
	t.t100 = run_new_delete_array<A>(N,loop,t,"new & delete [] A : ");
	run_new_delete_array<B>(N,loop,t,"new & delete [] B : ");
	run_new_delete_array<C>(N,loop,t,"new & delete [] C : ");

	cout << endl << endl;
////////////////////////////////////////////////////////////////////////////////


    int count = 1000;
	t.t100 = 0;
	t.t100 = run_new_array(N,a,count,t,"new [] A on array : ");
	run_new_array(N,b,count,t,"new [] B on array : ");
	run_new_array(N,c,count,t,"new [] C on array : ");

	cout << endl;
////////////////////////////////////////////////////////////////////////////////

	t.t100 = 0;
	t.t100 = run_delete_array(a,count,t,"delete [] A on array : ");
	run_delete_array(b,count,t,"delete [] B on array : ");
	run_delete_array(c,count,t,"delete [] C on array : ");


	delete [] a;
	delete [] b;
	delete [] c;
	
	cout << endl << endl;
    Loki::AllocatorSingleton<>::ClearExtraMemory();
////////////////////////////////////////////////////////////////////////////////


	cout << endl;
	system("PAUSE");
	return 0;
}

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.7  2005/10/14 18:35:06  rich_sposato
// Added cvs keywords.
//
