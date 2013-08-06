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

#include <cassert>


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

/** @class ListAssertInfoOnSameLine
 This policy class demonstrates how to write and use your own policies.
 It lists assert info descriptions on the same line as their values. It
 assumes that if an AssertInfo type is a C-style string, then it must be
 a description of the next AssertInfo value. It also puts a blank at the
 start of the output to vertically separate the assertion info from any
 previous output by the host program.
 */
class ListAssertInfoOnSameLine : public Loki::CerrAssertPolicy
{
public:
	/// Displays information about assertion to the user.
	static void Output( const Loki::SmartAssertBase * asserter );
};

// ---------------------------------------------------------------------

void ListAssertInfoOnSameLine::Output( const ::Loki::SmartAssertBase * asserter )
{
    assert( nullptr != asserter );

	::std::cerr << ::std::endl << ::Loki::SmartAssertBase::GetName( asserter->m_level )
		 << "!  Assertion failed!  " << asserter->m_expression << ::std::endl;
	if ( nullptr != asserter->m_context )
	{
		::std::cerr << "\t";
		asserter->m_context->Output( true );
		for ( const ::Loki::AssertContext * p = asserter->m_context->m_next; ( p != nullptr ); p = p->m_next )
		{
			::std::cerr << "  ";
			p->Output( true );
		}
		::std::cerr << ::std::endl;
	}
	if ( ( nullptr != asserter->m_message ) && ( '\0' != *asserter->m_message ) )
	{
		::std::cerr << "\t" << asserter->m_message << ::std::endl;
	}

    const char * description = nullptr;
	for ( const ::Loki::AssertInfo * p = asserter->m_info; ( p != nullptr ); p = p->m_next )
	{
        if ( ( ::Loki::AssertInfo::CharPtr == p->m_type )
          && ( p->m_next != nullptr ) )
        {
            description = p->m_value.m_p_char;
            p = p->m_next;
        }
        else
        {
            description = "";
        }
	    ::std::cerr << "\t" << Loki::AssertInfo::GetName( p->m_type ) << ": " << description << " (";
	    p->m_value.Output( p->m_type, true );
	    ::std::cerr << ')' << ::std::endl;
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

    const unsigned int counted = 57;
    const char * name = "Darth Cuddles";
    const bool result = true;
    // Show how to put variables names into assertion info.
	LOKI_SMART_ASSERT( 0 == argc ).Warn()( counted )( "counted" )( name )( "name" )( result )( "result" )();

    // Same as before, but this shows how to use a policy to put variable names on the same line as their values.
	LOKI_SMART_ASSERT_POLICIED( 0 == argc, ListAssertInfoOnSameLine ).Warn()
        ( counted )( "counted" )( name )( "name" )( result )( "result" )();

    // Same as before, but this shows how to use a policy to put variable descriptions on the same line as their values.
	LOKI_SMART_ASSERT_POLICIED( 0 == argc, ListAssertInfoOnSameLine ).Warn()
        ( counted )( "death toll" )( name )( "villain" )( result )( "Does evil lose?" )();

    // Same as before, but this shows how to mix names/descriptions and unnamed variables in the same assertion output.
	LOKI_SMART_ASSERT_POLICIED( 0 == argc, ListAssertInfoOnSameLine ).Warn()( a1 )
        ( counted )( "death toll" )( name )( "villain" )( a3 )( result )( "Does evil lose?" )( a2 )();

	test_data_types( argc, argv );
	test_ignore( argc );

	return 0;
}

// ---------------------------------------------------------------------

#if defined( _MSC_VER )
    #pragma warning( pop )
#endif
