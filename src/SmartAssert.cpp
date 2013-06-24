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


#include <loki/SmartAssert.hpp>

#if defined( _WIN32 ) || defined( __WIN32__ ) || defined( WIN32 )
    #include <Windows.h>
#endif

#include <iostream>

#include <cassert>
#include <cstdlib>

#if defined( _MSC_VER )
    #pragma warning( push )
    #pragma warning( disable : 4365 )
    #pragma warning( disable : 4514 )
    #pragma warning( disable : 4710 )
    #pragma warning( disable : 4711 )
#endif

#if !defined( nullptr )
    #define nullptr 0
#endif


using namespace ::std;

namespace Loki
{

bool SmartAssertBase::s_alwaysIgnore = true;

const char * const SmartAssertBase::FileDesc = "file";
const char * const SmartAssertBase::LineDesc = "line";
const char * const SmartAssertBase::FunctionDesc = "function";


// ---------------------------------------------------------------------

const char * AssertInfo::GetName( DataTypeTag tag )
{
	switch ( tag )
	{
		case Unknown:         return "Unknown";
		case Boolean:         return "boolean";
		case JustChar:        return "char";
		case SignedChar:      return "signed char";
		case UnsignedChar:    return "char";
		case SignedShort:     return "signed short";
		case UnsignedShort:   return "unsigned short";
		case JustInt:         return "int";
		case SignedInt:       return "signed int";
		case UnsignedInt:     return "unsigned int";
		case Long:            return "long";
		case UnsignedLong:    return "unsigned long";
		case LongInt:         return "long int";
		case UnsignedLongInt: return "unsigned long int";
		case CharPtr:         return "char *";
		case SignedCharPtr:   return "signed char *";
		case UnsignedCharPtr: return "unsigned char *";
		case VoidPtr:         return "void *";
		case Float:           return "float";
		case Double:          return "double";
		case LongDouble:      return "long double";
		default: break;
	}
	return "undefined data type";
}

// ---------------------------------------------------------------------

void AssertInfo::DataValue::Output( DataTypeTag theType ) const
{
	switch ( theType )
	{
		case Unknown:
		{
			cout << "\t Error! Unknown data type! " << theType;
			break;
		}
		case Boolean:
		{
			const char * message = m_bool ? "true" : "false";
			cout << message;
			break;
		}
		case JustChar:
		{
			cout << m_char;
			break;
		}
		case SignedChar:
		{
			cout << m_s_char;
			break;
		}
		case UnsignedChar:
		{
			cout << m_u_char;
			break;
		}
		case SignedShort:
		{
			cout << m_s_short;
			break;
		}
		case UnsignedShort:
		{
			cout << m_u_short;
			break;
		}
		case JustInt:
		{
			cout << m_int;
			break;
		}
		case SignedInt:
		{
			cout << m_int;
			break;
		}
		case UnsignedInt:
		{
			cout << m_u_int;
			break;
		}
		case Long:
		{
			cout << m_long;
			break;
		}
		case UnsignedLong:
		{
			cout << m_u_long;
			break;
		}
		case LongInt:
		{
			cout << m_s_long_int;
			break;
		}
		case UnsignedLongInt:
		{
			cout << m_u_long_int;
			break;
		}
		case CharPtr:
		{
			cout << m_p_char;
			break;
		}
		case SignedCharPtr:
		{
			cout << m_p_s_char;
			break;
		}
		case UnsignedCharPtr:
		{
			cout << m_p_u_char;
			break;
		}
		case VoidPtr:
		{
			cout << m_p_v;
			break;
		}
		case Float:
		{
			cout << m_float;
			break;
		}
		case Double:
		{
			cout << m_double;
			break;
		}
		case LongDouble:
		{
			cout << m_l_double;
			break;
		}
		default:
		{
			cout << "\t Error! Undefined data type! " << theType;
			break;
		}
	}
}

// ---------------------------------------------------------------------

void AssertInfo::Output() const
{
	cout << "\t" << GetName( m_type ) << ": ";
	m_value.Output( m_type );
	cout << endl;
}

// ---------------------------------------------------------------------

AssertContext::AssertContext( const char * description, unsigned int line )
	: m_line( line )
	, m_value( nullptr )
	, m_description( description )
	, m_next( nullptr )
{
}

// ---------------------------------------------------------------------

AssertContext::AssertContext( const char * description, const char * value )
	: m_line( 0 )
	, m_value( value )
	, m_description( description )
	, m_next( nullptr )
{
}

// ---------------------------------------------------------------------

void AssertContext::Output() const
{
	cout << m_description << ": ";
	if ( m_value != nullptr )
	{
		cout << m_value;
	}
	else
	{
		cout << m_line;
	}
}

// ---------------------------------------------------------------------

const char * SmartAssertBase::GetName( SeverityLevel level )
{
	switch ( level )
	{
		case Info_:  return "Info";
		case Warn_:  return "Warning";
		case Error_: return "Error";
		case Fatal_: return "Fatal";
	}
	return "unknown";
}

// ---------------------------------------------------------------------

SmartAssertBase::SmartAssertBase()
	: m_context( nullptr )
	, m_info( nullptr )
	, m_level( Warn_ )
	, m_ignore( &s_alwaysIgnore )
	, m_expression( nullptr )
	, m_message( nullptr )
	, m_handled( false )
{
}

// ---------------------------------------------------------------------

SmartAssertBase::SmartAssertBase( bool * ignore, const char * expression )
	: m_context( nullptr )
	, m_info( nullptr )
	, m_level( Fatal_ ) ///< @note Default level is fatal to match behavior of assert.
	, m_ignore( ignore )
	, m_expression( expression )
	, m_message( nullptr )
	, m_handled( false )
{
}

// ---------------------------------------------------------------------

SmartAssertBase::~SmartAssertBase()
{
	if ( m_handled )
	{
		return;
	}

	try
	{
		CallOutput();
		if ( ( Fatal_ == m_level ) || ( Error_ == m_level ) )
		{
			AbortNow();
		}
	}
	catch ( ... )
	{
		if ( ( Fatal_ == m_level ) || ( Error_ == m_level ) )
		{
			// In other situations, I might allow a destructor to do nothing
			// when an exception occurs, but since this exception occurred
			// while processing a failed assertion, it is probably not safe
			// to ignore the exception. Just end the program abruptly.
			::std::abort();
		}
	}
}

// ---------------------------------------------------------------------

SmartAssertBase & SmartAssertBase::AddContext( const AssertContext & context )
{
	// Contexts are always added to the head of the linked-list since that
	// is cheaper than traversing the list every time, but the list is in
	// reverse order from how the entries are declared.
	assert( m_context != &context );
	context.m_next = m_context;
	m_context = &context;
	return *this;
}

// ---------------------------------------------------------------------

SmartAssertBase & SmartAssertBase::AddInfo( const AssertInfo & info )
{
	// AssertInfo's are always added to the head of the linked-list since
	// that is cheaper than traversing the list every time, but the list
	// is in reverse order from how the entries are declared.
	assert( m_info != &info );
	info.m_next = m_info;
	m_info = &info;
	return *this;
}

// ---------------------------------------------------------------------

void SmartAssertBase::CallOutput() const
{
	CommandLineAssertPolicy::Output( this );
}

// ---------------------------------------------------------------------

void SmartAssertBase::CallDebugger() const
{
	CommandLineAssertPolicy::Debugger( this );
}

// ---------------------------------------------------------------------

SmartAssertBase::UserResponse SmartAssertBase::AskUser() const
{
	return CommandLineAssertPolicy::AskUser( this );
}

// ---------------------------------------------------------------------

void SmartAssertBase::AbortNow() const
{
	CommandLineAssertPolicy::AbortNow( this );
}

// ---------------------------------------------------------------------

void SmartAssertBase::HandleFailure()
{
	m_handled = true;

	try
	{
		if ( Info_ == m_level )
		{
			CallOutput();
			return;
		}
		CallOutput();
		if ( m_level == Fatal_ )
		{
			AbortNow();
		}
		else
		{
			const UserResponse response = AskUser();
			switch ( response )
			{
				case IgnoreEachTime: *m_ignore = true;
				case IgnoreThisTime: break;
				case DebugNow:
				{
					CallDebugger();
					break;
				}
				case AbortProgram:
				default:
				{
					AbortNow();
				}
			}
		}
	}

	catch ( ... )
	{
		// In other situations, I might allow an object to ignore an exception, but
		// since this exception occurred while processing a failed assertion, it is
		// probably not safe to ignore the exception. Just end the program abruptly.
		if ( ( m_level == Fatal_ ) || ( m_level == Error_ ) )
		{
			::std::abort();
		}
	}
}

// ---------------------------------------------------------------------

void CommandLineAssertPolicy::Output( const SmartAssertBase * asserter )
{
	cout << SmartAssertBase::GetName( asserter->m_level )
		 << "!  Assertion failed!  " << asserter->m_expression << endl;
	if ( nullptr != asserter->m_context )
	{
		cout << "\t";
		asserter->m_context->Output();
		for ( const AssertContext * p = asserter->m_context->m_next; ( p != nullptr ); p = p->m_next )
		{
			cout << "  ";
			p->Output();
		}
		cout << endl;
	}
	if ( ( nullptr != asserter->m_message ) && ( '\0' != *asserter->m_message ) )
	{
		cout << "\t" << asserter->m_message << endl;
	}
	for ( const AssertInfo * p = asserter->m_info; ( p != nullptr ); p = p->m_next )
	{
		p->Output();
	}
}

// ---------------------------------------------------------------------

void CommandLineAssertPolicy::Debugger( const SmartAssertBase * )
{
#if defined( _WIN32 ) || defined( __WIN32__ ) || defined( WIN32 )
	::DebugBreak(); // Win32

#elif ( defined _MSC_VER ) || ( defined __BORLANDC__) || ( defined __MWERKS__ )
	__asm { int 3 };

#elif defined(__GNUC__)  // GCC
	__asm ("int $0x3");

#else
	#  error "Please supply instruction to DebugBreak (like 'int 3' on Intel processors)"
#endif
}

// ---------------------------------------------------------------------

SmartAssertBase::UserResponse CommandLineAssertPolicy::AskUser( const SmartAssertBase * asserter )
{
	const char * prompt = ( SmartAssertBase::Error_ == asserter->m_level ) ?
		"\tChoose option: (I)gnore This Time,  Ignore (E)ach Time,  (D)ebug,  (A)bort  " :
		"\tChoose option: (I)gnore This Time,  Ignore (E)ach Time,  (D)ebug  ";
	::std::cout << prompt;

	bool keep_asking = true;
	while ( keep_asking )
	{
		const char ch = static_cast< char >( ::std::cin.get() );
		switch ( ch )
		{
			case 'i': case 'I': return SmartAssertBase::IgnoreThisTime;
			case 'e': case 'E': return SmartAssertBase::IgnoreEachTime;
			case 'd': case 'D': return SmartAssertBase::DebugNow;
			case 'a': case 'A':
				if ( SmartAssertBase::Error_ == asserter->m_level )
				{
					keep_asking = false;
					break;
				}
			default: ;	// do nothing.
		}
		if ( !isspace( ch ) && keep_asking ) // ignore spaces
		{
			std::cout << prompt;
		}
	}

	return SmartAssertBase::AbortProgram;
}

// ---------------------------------------------------------------------

void CommandLineAssertPolicy::AbortNow( const SmartAssertBase * )
{
	// This might be a good time to call any cleanup services,
	// and to log any useful information to a file.
	abort();
}

// ---------------------------------------------------------------------

#if defined( _MSC_VER )
    #pragma warning( pop )
#endif

} // end namespace
