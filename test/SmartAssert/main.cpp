////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 by Rich Sposato
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


#if defined( _MSC_VER )
    #pragma warning( push )
    #pragma warning( disable : 4127 )
    #pragma warning( disable : 4514 )
    #pragma warning( disable : 4738 )
#endif

#if !defined( nullptr )
    #define nullptr 0
#endif

#include <loki/SmartAssert.hpp>

#include <iostream>

// ---------------------------------------------------------------------

class Bogus { };

// ---------------------------------------------------------------------

void test_data_types( int argc, const char * argv[] )
{
	// To quiet the compiler warning when compiling in release mode.
	(void)argc;
	(void)argv;

	const bool b = ( argv[0] != nullptr );
	LOKI_SMART_ASSERT( 2 != argc ).Info()( b )();

	const char ch1 = 'A';
	LOKI_SMART_ASSERT( 5 != argc ).Info()( ch1 )();

	const signed char ch2 = 'b';
	LOKI_SMART_ASSERT( 6 != argc ).Warn()( ch2 )();

	const unsigned char ch3 = 'c';
	LOKI_SMART_ASSERT( 7 != argc ).Warn()( ch3 )();

	const short sh1 = -41;
	LOKI_SMART_ASSERT( 8 != argc ).Warn()( sh1 )();

	const unsigned short sh2 = 42;
	LOKI_SMART_ASSERT( 9 != argc ).Warn()( sh2 )();

	const int int1 = 16;
	LOKI_SMART_ASSERT( 10 != argc ).Warn()( int1 )();

	const unsigned int int2 = 24;
	LOKI_SMART_ASSERT( 11 != argc ).Warn()( int2 ).Msg( "Not allowed to enter exactly 10 options!" )();

	const long L1 = -12345;
	LOKI_SMART_ASSERT( 12 != argc ).Warn()( L1 )();

	const unsigned long L2 = 12345;
	LOKI_SMART_ASSERT( 13 != argc ).Warn()( L2 )();

	const long long int LL1 = -12345;
	LOKI_SMART_ASSERT( 14 != argc ).Warn()( LL1 )();

	const unsigned long long int LL2 = 12345;
	LOKI_SMART_ASSERT( 15 != argc ).Warn()( LL2 )();

	const char * pc1 = "hello, world!";
	LOKI_SMART_ASSERT( 16 != argc ).Warn()( pc1 )();

	const signed char * pc2 = reinterpret_cast< const signed char * >( pc1 );
	LOKI_SMART_ASSERT( 17 != argc ).Warn()( pc2 )();

	const unsigned char * pc3 = reinterpret_cast< const unsigned char * >( pc1 );
	LOKI_SMART_ASSERT( 18 != argc ).Warn()( pc3 )();

	void * pv = nullptr;
	LOKI_SMART_ASSERT( 19 != argc ).Info()( pv )();

	const void * pcv = nullptr;
	LOKI_SMART_ASSERT( 20 != argc ).Info()( pcv )();

	Bogus * pb = nullptr; // Demonstrate that pointer-to-anything convert to pointer-to-void for SmartAssert.
	LOKI_SMART_ASSERT( 21 != argc ).Info()( pb )();

	const Bogus * pcb = nullptr;
	LOKI_SMART_ASSERT( 22 != argc ).Info()( pcb )();

	const float f1 = 12345.67890F;
	LOKI_SMART_ASSERT( 23 != argc ).Warn()( f1 )();

	const double d1 = 12345.67890;
	LOKI_SMART_ASSERT( 24 != argc ).Warn()( d1 )();

	const long double d2 = 12345.67890;
	LOKI_SMART_ASSERT( 25 != argc ).Warn()( d2 )();
}

// ---------------------------------------------------------------------

void test_ignore( int argc )
{
	// To quiet the compiler warning when compiling in release mode.
	(void)argc;

	for ( unsigned int ii = 0; ii < 4; ++ii )
	{
		::std::cout << ::std::endl << "loop: " << ii << ::std::endl;
		LOKI_SMART_ASSERT( 2 == argc ).Warn()();
		::std::cout << ::std::endl;
		LOKI_SMART_ASSERT( 3 == argc ).Warn()();
		::std::cout << ::std::endl;
		LOKI_SMART_ASSERT( 4 == argc ).Error()();
		::std::cout << ::std::endl;
		LOKI_SMART_ASSERT( 5 == argc ).Error()();
	}
}

// ---------------------------------------------------------------------

int main( int argc, const char * argv[] )
{

	// Test severity level of info.
	LOKI_SMART_ASSERT( 2 == argc ).Error().Msg( "Negative number of command line args!" )();

	// Test severity level of error rather than warning.
	LOKI_SMART_ASSERT( 3 != argc ).Error()( argc )();

	// Show what message output looks like.
	LOKI_SMART_ASSERT( 4 == argc ).Warn()( argc )( argv[0] ).Msg( "Need exactly 3 args!" )();

	// Show that variables are outputted in reverse order from how they are added.
    int a1 = 1;
    int a2 = 2;
    int a3 = 3;
	LOKI_SMART_ASSERT( 0 == argc ).Warn()( a1 )( a2 )( a3 )();

    // Show how to put names of variables into assertion info.
	LOKI_SMART_ASSERT( 0 == argc ).Warn()( a1 )( "a1" )( a2 )( "a2" )( a3 )( "a3" )();

	test_data_types( argc, argv );
	test_ignore( argc );

	return 0;
}

// ---------------------------------------------------------------------

#if defined( _MSC_VER )
    #pragma warning( pop )
#endif
