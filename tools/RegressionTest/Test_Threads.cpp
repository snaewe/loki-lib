//
//Test the Thread components
// MKH

#include <Loki\Threads.h>
using namespace ::Loki;

#include <iostream>
using std::cout;
using std::endl;

namespace
{
}

extern void Test_Threads()
{
	cout << "Testing Loki\\Threads" << endl;
	SingleThreaded<void>();
	cout << endl << endl;
}