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


#ifndef LOKI_SMART_ASSERT_HPP_INCLUDED
#define LOKI_SMART_ASSERT_HPP_INCLUDED


#include <loki/Concatenate.h>

#if defined ( DEBUG ) || defined( DEBUG_ ) || defined( _DEBUG )
    #define LOKI_SMART_ASSERT_DEBUG
#endif

#if defined( _MSC_VER )
    #pragma warning( push )
    #pragma warning( disable : 4514 )
    #pragma warning( disable : 4711 )
#endif

#if !defined( nullptr )
    #define LOKI_DEFINED_NULLPTR
    #define nullptr 0
#endif


namespace Loki
{

// ---------------------------------------------------------------------

/**
 @par SmartAssert Versus Dumb Assertions
  Loki's SmartAssert provides features traditional assertions don't.
  - Add a meaningful message to each assertion.
  - Output values of variables when assertions fail.
  - Different severity levels per assertion.
  - Allows program to do something besides abort when assertions fail.
  - Can ask user how to respond to assertions instead of just failing.
  - Customizable output.
  - Customizable way to ask user for response.
  - Customizable way to start debugger.
  - Allows program to do cleanup before dying.

 @par How to Use SmartAssert in 5 Minutes
 The simplest way to use Loki's SmartAssert is to follow these steps. This is
 the simplest way to use SmartAssert, and it gives you the same (or similar)
 behaviors as assert.
 - Add this line to your file: #include <loki/SmartAssert.hpp>
 - Replace "assert( condition );" with "LOKI_SMART_ASSERT( condition );"
 - Make sure you define one of these macros: DEBUG, _DEBUG, or DEBUG_.
 - Compile and run your program.

 @par Adding Messages to your Assertions.
  SmartAssert can output simple C-style strings as messages when an assertion
  fails. Each assertion may have only one message. If you add more than one,
  then only the right-most message is sent to output. To add a simple message
  to your assertions, use the .Msg() function.
 @code
  LOKI_SMART_ASSERT( cond ).Msg( "Something is rotten in the state of Denmark." );
 @endcode

 @par Adding Variables to Your Assertions.
  You can make SmartAssert output the values of variables by adding each
  variable within a pair of parantheses. It will output them in the same order
  they were added. You may chain as many variables to the assertion, but you
  may only place fundamental types in the assertion. You can put them in any
  order, but SmartAssert will store and output them in the reverse order in
  which you add them. (i.e. Leftmost variables get outputted last, and
  rightmost ones get outputted first.) Storing them in reverse order makes an
  efficiency gain of O(n) over O(n*n).
 @code
  LOKI_SMART_ASSERT( cond )( a )( b );
  LOKI_SMART_ASSERT( cond )( a )( e )( c )( b )( d );
 @endcode

  SmartAssert won't know the purpose of those variables; only their type and
  value. You can make SmartAssert output the names of the variables by using
  the "stringify" preprocessor operator. This causes SmartAssert to create a
  separate storage for the name and the variable itself.
 @code
  LOKI_SMART_ASSERT( cond )( a )( "a" )( b )( "b" );
  LOKI_SMART_ASSERT( cond )( a )( "a" )( e )( "e" )( b )( "b" );
 @endcode

 @par Setting the Severity Level
  Assertions treat all error conditions as an excuse to commit suicide. That
  one-size-fits-all solution seems overly drastic for minor errors. At least
  SmartAssert gives the developer more detailed output to help understand why
  the program died.

  SmartAssert provides multiple severity levels so developers have more options
  besides seeing their programs die. The basic options are to start a debugger,
  ignore the assertion that time, ignore it as long as the program runs, or
  stop the program. The four severity levels and their effects are:
  - Info:  Just output the assertion info and continue on.
  - Warn:  Ask user to either ignore now, ignore always, or start debugger.
  - Error: Same as above, but also give user option to abort program.
  - Fatal: Program always ends when assertion fails and never asks user.

  SmartAssert's default severity level is Fatal so you can use it as a drop-in
  replacement for assert and still get the same behavior.

  Here are some example calls to set the severity level. As you can see, the
  calls may occur in any order within the assertion.
 @code
  LOKI_SMART_ASSERT( cond )( a )( b ).Info();
  LOKI_SMART_ASSERT( cond ).Warn()( a )( b );
  LOKI_SMART_ASSERT( cond )( a ).Error().( b );
  LOKI_SMART_ASSERT( cond ).Fatal().Msg( "Goodbye cruel world!" );
  LOKI_SMART_ASSERT( cond )( d ); // No severity level implies Fatal!
 @endcode

  You could specify more than one severity level, but only the rightmost call
  matters. In this example, Info() is further to the right so it overrides the
  Fatal() call.
 @code
  LOKI_SMART_ASSERT( cond ).Fatal()( a )( b ).Info();
 @endcode

 @par User Intervention.
  Since SmartAssert's default behaviors mimic assert, it normally doesn't ask
  the user how to respond to an assertion failure. SmartAssert asks the user
  only if the developer puts a call to operator() at the end of the assertion.
  There is no need to ask the user to intervene if the severity level is Info.
  If you write a policy class to either ask the user or call the debugger, you
  should always add the call to operator().
 @code
  LOKI_SMART_ASSERT( cond ).Warn()( a )(); // Asks user to intervene.
  LOKI_SMART_ASSERT( cond ).Warn()( a );   // Does not ask user.
 @endcode

 @par Write Your Own Policy.
  Many parts of Loki were implemented using policy-based software design, and
  SmartAssert follows that tradition. SmartAssert has a default policy that
  mimics the behaviors of assert. If you want to make your own policy class,
  write that class to have the same function signatures as
  CommandLineAssertPolicy, and use the LOKI_SMART_ASSERT_POLICIED macro instead
  of LOKI_SMART_ASSERT.

 @par Potential Policies
 - To call a special debugger rather than the default one.
 - To send output to a log file instead of to cout.
 - To create a popup dialog box to ask the user.
 - To attempt last moment cleanup before the program dies.

 @code
  class MyPolicy 
  {
	static void Output( const SmartAssertBase * asserter );
	static void Debugger( const SmartAssertBase * asserter );
	static SmartAssertBase::UserResponse AskUser( const SmartAssertBase * asserter );
	static void AbortNow( const SmartAssertBase * asserter );
  };
  LOKI_SMART_ASSERT_POLICIED( cond, MyPolicy );
 @endcode

 @par Thread Safety.
  Each SmartAssert object is declared locally within a function, so it is only
  accessible by the thread executing it. However, each SmartAssert makes its
  own static boolean flag for whether to ignore its future failures. That
  boolean flag is not thread_local, so telling SmartAssert to ignore it each
  time applies to all threads. SmartAssert relies on a policy class, so it is
  only as thread-safe as the functions in its policy class.

 @par Exception Safety.
  SmartAssert wraps calls to the policy class within try-catch blocks. If an
  exception occurs for Info or Warning level conditions, SmartAssert stops
  processing the assertion and allows the calling function to continue. If an
  exception occurs when processing an Error or Fatal assertion, SmartAssert
  will abort immediately.

 @par Memory Usage.
  SmartAssert does not allocate any memory or other resources, but functions
  within policy classes might.

 @par Run-Time Efficiency.
  No function call or operation within SmartAssert takes more than O(n) steps,
  where n is the number of variables passed into SmartAssert. Most actions
  complete in constant time. Functions in policy classes may not have the same
  low run-time efficiency.
*/

// ---------------------------------------------------------------------

/** @class AssertInfo Stores extra info user wants to pass into assertion.
 This class has one constructor for each primitive data type so developers
 can pass any primitive variable into the assert.
 */
class AssertInfo
{
public:

	/** @enum DataTypeTag One tag for each primitive data type, plus a
	 few for pointers to char data types.
	 */
	enum DataTypeTag
	{
		Unknown = 0,
		Boolean,
		JustChar,
		SignedChar,
		UnsignedChar,
		SignedShort,
		UnsignedShort,
		JustInt,
		SignedInt,
		UnsignedInt,
		Long,
		UnsignedLong,
		LongInt,
		UnsignedLongInt,
		CharPtr,
		SignedCharPtr,
		UnsignedCharPtr,
		VoidPtr,
		Float,
		Double,
		LongDouble,
	};

	static const char * GetName( DataTypeTag tag );

	/// @union DataValue Can be configured as any primitive data type.
	union DataValue
	{
		bool                   m_bool;
		char                   m_char;
		signed char            m_s_char;
		unsigned char          m_u_char;
		signed short int       m_s_short;
		unsigned short int     m_u_short;
		int                    m_int;
		unsigned int           m_u_int;
		long                   m_long;
		unsigned long          m_u_long;
		signed long long int   m_s_long_int;
		unsigned long long int m_u_long_int;
		const char *           m_p_char;
		const signed char *    m_p_s_char;
		const unsigned char *  m_p_u_char;
        const void *           m_p_v;
		float                  m_float;
		double                 m_double;
		long double            m_l_double;

		DataValue()                                 : m_bool(    true ) {}
		DataValue( const bool                   v ) : m_bool(       v ) {}
		DataValue( const char                   v ) : m_char(       v ) {}
		DataValue( const signed char            v ) : m_s_char(     v ) {}
		DataValue( const unsigned char          v ) : m_u_char(     v ) {}
		DataValue( const signed short int       v ) : m_s_short(    v ) {}
		DataValue( const unsigned short int     v ) : m_u_short(    v ) {}
		DataValue( const int                    v ) : m_int(        v ) {}
		DataValue( const unsigned int           v ) : m_u_int(      v ) {}
		DataValue( const long                   v ) : m_long(       v ) {}
		DataValue( const unsigned long          v ) : m_u_long(     v ) {}
		DataValue( const long long int          v ) : m_s_long_int( v ) {}
		DataValue( const unsigned long long int v ) : m_u_long_int( v ) {}
		DataValue( const char *                 v ) : m_p_char(     v ) {}
		DataValue( const signed char *          v ) : m_p_s_char(   v ) {}
		DataValue( const unsigned char *        v ) : m_p_u_char(   v ) {}
		DataValue( const void *                 v ) : m_p_v(        v ) {}
		DataValue( const float                  v ) : m_float(      v ) {}
		DataValue( const double                 v ) : m_double(     v ) {}
		DataValue( const long double            v ) : m_l_double(   v ) {}

		void Output( DataTypeTag type ) const;
	};

	AssertInfo()                           : m_type( Unknown ),         m_value(),    m_next( nullptr ) {}
	AssertInfo( bool v )                   : m_type( Boolean ),         m_value( v ), m_next( nullptr ) {}
    AssertInfo( char v )                   : m_type( JustChar ),        m_value( v ), m_next( nullptr ) {}
	AssertInfo( signed char v )            : m_type( SignedChar ),      m_value( v ), m_next( nullptr ) {}
	AssertInfo( unsigned char v )          : m_type( UnsignedChar ),    m_value( v ), m_next( nullptr ) {}
	AssertInfo( signed short v )           : m_type( SignedShort ),     m_value( v ), m_next( nullptr ) {}
	AssertInfo( unsigned short v )         : m_type( UnsignedShort ),   m_value( v ), m_next( nullptr ) {}
	AssertInfo( signed int v )             : m_type( SignedInt ),       m_value( v ), m_next( nullptr ) {}
	AssertInfo( unsigned int v )           : m_type( UnsignedInt ),     m_value( v ), m_next( nullptr ) {}
	AssertInfo( long v )                   : m_type( Long ),            m_value( v ), m_next( nullptr ) {}
	AssertInfo( unsigned long v )          : m_type( UnsignedLong ),    m_value( v ), m_next( nullptr ) {}
	AssertInfo( long long int v )          : m_type( LongInt ),         m_value( v ), m_next( nullptr ) {}
	AssertInfo( unsigned long long int v ) : m_type( UnsignedLongInt ), m_value( v ), m_next( nullptr ) {}
	AssertInfo( const char * v )           : m_type( CharPtr ),         m_value( v ), m_next( nullptr ) {}
	AssertInfo( const signed char * v )    : m_type( SignedCharPtr ),   m_value( v ), m_next( nullptr ) {}
	AssertInfo( const unsigned char * v )  : m_type( UnsignedCharPtr ), m_value( v ), m_next( nullptr ) {}
	AssertInfo( const void * v )           : m_type( VoidPtr ),         m_value( v ), m_next( nullptr ) {}
	AssertInfo( float v )                  : m_type( Float ),           m_value( v ), m_next( nullptr ) {}
	AssertInfo( double v )                 : m_type( Double ),          m_value( v ), m_next( nullptr ) {}
	AssertInfo( long double v )            : m_type( LongDouble ),      m_value( v ), m_next( nullptr ) {}

	/// Function provides default output action.
	void Output() const;

	DataTypeTag  m_type;  ///< What type of data this stores.
	DataValue    m_value; ///< Value of that data.
	mutable const AssertInfo * m_next; ///< Pointer to next piece of info, if any.
};

// ---------------------------------------------------------------------

/** @class AssertContext Stores info about file, line, and function
 where assertion occurred. Only two or three of these per SmartAssert.

 @note This class is optimized for storing info about the assertion's
 location, not for storing general values. Use AssertInfo for storing
 values of arbitary primitive types.
 */
class AssertContext
{
public:

	/// Constructs context for source code line within file.
	AssertContext( const char * description, unsigned int line );

	/// Constructs context for source code filename or function name.
	AssertContext( const char * description, const char * value );

	/// Function provides default output action.
	void Output() const;

	unsigned int m_line;         ///< Line number within file.
	const char * m_value;        ///< Pointer to either filename or function name.
	const char * m_description;  ///< C-style string for describing piece of context.
	mutable const AssertContext * m_next;  ///< Pointer to next piece of context, if any.
};

// ---------------------------------------------------------------------

class SmartAssertBase
{
public:

	enum SeverityLevel
	{
		Info_,   ///< Just warn user and do nothing else. (same as Warning and Ignore-Each-Time)
		Warn_,   ///< Give user options: (Ignore Once, Ignore Always, Debug)
		Error_,  ///< Give user options: (Ignore Once, Ignore Always, Debug, Abort)
		Fatal_   ///< Always abort on failure. User gets no option to choose otherwise.
	};

	/// @enum Possible replies by the user when asked what to do about assertion.
    enum UserResponse
    {
		/// Program continues as if assertion never happened, and asks
		/// user what to do when assertion occurs again.
		IgnoreThisTime = 'I',
		/// Program continues executing, and never checks assertion for
		/// the rest of the program's execution. If the program ends and
		/// restarts, SmartAssert will check it again.
		IgnoreEachTime = 'E',
		/// Start the debugger at the place where the assertion failed.
		DebugNow       = 'D',
		/// End the program now.
		AbortProgram   = 'A'
	};

	/// Gets one word description of severity level.
	static const char * GetName( SeverityLevel level );

	/// These three C-style strings contain descriptions used for assertion contexts.
	static const char * const FileDesc;
	static const char * const LineDesc;
	static const char * const FunctionDesc;

	mutable const AssertContext * m_context; /// Linked-list of contexts of where assertion occurred.
	mutable const AssertInfo * m_info; ///< Linked-list of values provided for output purposes.
	SeverityLevel m_level;  ///< How bad is this assertion?
	bool *        m_ignore; ///< Pointer to ignore-always flag.
	const char *  m_expression; ///< Pointer to C-style string of failed assertion expression.
	const char *  m_message;  ///< Simple message made by developer.
	bool          m_handled;  ///< True if this assertion was handled before destructor.

protected:

	/// Default constructor is used for release builds. It ignores assertions.
	SmartAssertBase();

	/** This constructor gets used in debug builds.
	 @param ignore Pointer to boolean flag to ignore this assertion each time.
	 @param expression C-style string showing failed assertion.
	 */
	SmartAssertBase( bool * ignore, const char * expression );

	/// Destructor handles assertion only when if not handled earlier.
	virtual ~SmartAssertBase();

	/// Called by derived class to add context information.
	SmartAssertBase & AddContext( const AssertContext & info );

	/// Called by derived class to add values used to display info about assertion.
	SmartAssertBase & AddInfo( const AssertInfo & info );

	/// Called to handle assertion failure.
	void HandleFailure();

	/// Default implementation of code to output information about assertion.
	virtual void CallOutput() const;

	/// Default implementation of code to call debugger.
	virtual void CallDebugger() const;

	/// Default implementation of code to ask user what to do.
	virtual UserResponse AskUser() const;

	/// Default implementation of code to abort program.
	virtual void AbortNow() const;

	/// Ignore-always flag used by assertions created in release builds.
	static bool s_alwaysIgnore;
};

// ---------------------------------------------------------------------

/** @class CommandLineAssertPolicy Default policy for command line programs.
 Developers can implement their own policies to handle assertions by making a
 class with the same function signatures as this class. If you write your own
 policy class, you should also use the LOKI_SMART_ASSERT_POLICIED macro, and
 not the LOKI_SMART_ASSERT macro.
*/
class CommandLineAssertPolicy
{
public:

	/// Displays information about assertion to the user.
	static void Output( const SmartAssertBase * asserter );

	/// Asks user how to handle assertion.
	static SmartAssertBase::UserResponse AskUser( const SmartAssertBase * asserter );

	/// Calls debugger.
	static void Debugger( const SmartAssertBase * asserter );

	/// This call should end the program.
	static void AbortNow( const SmartAssertBase * asserter );

};

// ---------------------------------------------------------------------

/** @class class SmartAssert
 */
template< class AssertPolicy >
class SmartAssert : public SmartAssertBase
{
public:

	/// Default constructor used for when assertion passes. Should get optimized away.
	SmartAssert() : SmartAssertBase() {}

	/// Constructor used when assertion fails.
	SmartAssert( bool * ignore, const char * expression )
		: SmartAssertBase( ignore, expression ) {}

	/// Destructor is trivial.
	virtual ~SmartAssert() {}

	/// Provides simple hard-coded C-style string message for output.
	SmartAssert & Msg( const char * message ) { m_message = message; return *this; }

	SmartAssert & Info()  { m_level = SmartAssertBase::Info_;  return *this; }
	SmartAssert & Warn()  { m_level = SmartAssertBase::Warn_;  return *this; }
	SmartAssert & Error() { m_level = SmartAssertBase::Error_; return *this; }
	SmartAssert & Fatal() { m_level = SmartAssertBase::Fatal_; return *this; }

	/// Called to do non-default actions when assertion fails.
	void operator ()() { HandleFailure(); }

	/// Adds one piece of information to assertion, generally a variable or result of function call.
	SmartAssert & operator ()( const AssertInfo & info )
	{
		return static_cast< SmartAssert & >( AddInfo( info ) );
	}

	/// Called to add one piece of context to assertion, such as filename, line, or function name.
	SmartAssert & Add( const AssertContext & info )
	{
		return static_cast< SmartAssert & >( AddContext( info ) );
	}

private:

	virtual void CallOutput() const
	{
		AssertPolicy::Output( dynamic_cast< const SmartAssertBase * >( this ) );
	}

	virtual void CallDebugger() const
	{
		AssertPolicy::Debugger( dynamic_cast< const SmartAssertBase * >( this ) );
	}

	virtual SmartAssertBase::UserResponse AskUser() const
	{
		return AssertPolicy::AskUser( dynamic_cast< const SmartAssertBase * >( this ) );
	}

	virtual void AbortNow() const
	{
		AssertPolicy::AbortNow( dynamic_cast< const SmartAssertBase * >( this ) );
	}

};

// ---------------------------------------------------------------------

} // namespace Loki


/// These lines let Loki put filename and line into AssertContext.
// note: using 'const char LOKI_SMART_ASSERT_FILE[] = __FILE__'
// does not work, since __FILE__ = "SmartAssert.hpp"
#define LOKI_SMART_ASSERT_FILE __FILE__
#define LOKI_USE_NUMBER_FOR_UNIQUE_NAME __LINE__


/** These lines determine if Loki can get the function name into AssertContext.
 They also declare the macro used for creating unique names for SmartAssert
 classes and variables.
 */
#define LOKI_SMART_ASSERT_FUNCTION_EXISTS

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000))
	# define LOKI_SMART_ASSERT_FUNCTION __PRETTY_FUNCTION__
	# undef LOKI_USE_NUMBER_FOR_UNIQUE_NAME
	# define LOKI_USE_NUMBER_FOR_UNIQUE_NAME __COUNTER__

#elif defined(__FUNCSIG__)
	# define LOKI_SMART_ASSERT_FUNCTION __FUNCSIG__

#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	# define LOKI_SMART_ASSERT_FUNCTION __FUNC__

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	# define LOKI_SMART_ASSERT_FUNCTION __func__

#elif (defined __MSC_VER) && (__MSC_VER >= 1300)
	# define LOKI_SMART_ASSERT_FUNCTION __FUNCDNAME__
	# undef LOKI_USE_NUMBER_FOR_UNIQUE_NAME
	# define LOKI_USE_NUMBER_FOR_UNIQUE_NAME __COUNTER__

#elif  defined(__IBMCPP__) && (__IBMCPP__ <= 500)
	# define LOKI_SMART_ASSERT_FUNCTION __FUNCTION__

#elif (defined __HP_aCC) && (__HP_aCC <= 33300)
	# define LOKI_SMART_ASSERT_FUNCTION __FUNCTION__

#else
	# undef LOKI_SMART_ASSERT_FUNCTION_EXISTS
#endif

#define LOKI_MAKE_UNIQUE_NAME( str )    LOKI_CONCATENATE( str, LOKI_USE_NUMBER_FOR_UNIQUE_NAME )
#define LOKI_SMART_ASSERT_IGNORE_NAME   LOKI_MAKE_UNIQUE_NAME( smartAssert_ignore_ )
#define LOKI_SMART_ASSERT_CLASS_NAME    LOKI_MAKE_UNIQUE_NAME( SmartAssert_ )
#define LOKI_SMART_ASSERT_HIDDEN_CLASS  LOKI_SMART_ASSERT_CLASS( LOKI_SMART_ASSERT_CLASS_NAME, LOKI_SMART_ASSERT_IGNORE_NAME )


// ---------------------------------------------------------------------

// @note This checks if LOKI_SMART_ASSERT_CONTEXT was defined already in
// case developers made their own context-creating macro. If you make
// your own context chain, follow the example here and expect SmartAssert
// to display the context information in the reverse order of how they
// are added here. (e.g. - The contexts are added in function-line-file
// order, but shown in file-line-function order.)
#ifndef LOKI_SMART_ASSERT_CONTEXT

    #ifdef LOKI_SMART_ASSERT_FUNCTION_EXISTS
        #define LOKI_SMART_ASSERT_CONTEXT \
			Add( ::Loki::AssertContext( ::Loki::SmartAssertBase::FunctionDesc, LOKI_SMART_ASSERT_FUNCTION ) ). \
			Add( ::Loki::AssertContext( ::Loki::SmartAssertBase::LineDesc, __LINE__ ) ). \
			Add( ::Loki::AssertContext( ::Loki::SmartAssertBase::FileDesc, LOKI_SMART_ASSERT_FILE ) )
    #else
        #define LOKI_SMART_ASSERT_CONTEXT \
			Add( ::Loki::AssertContext( ::Loki::SmartAssertBase::LineDesc, __LINE__ ) ). \
			Add( ::Loki::AssertContext( ::Loki::SmartAssertBase::FileDesc, LOKI_SMART_ASSERT_FILE ) )
    #endif

#endif // LOKI_SMART_ASSERT_CONTEXT


// ---------------------------------------------------------------------

#define LOKI_MAKE_SMART_ASSERT( class_name, ignore_var_name, expr ) \
	static bool ignore_var_name = false; \
	class class_name : public ::Loki::SmartAssert< ::Loki::CommandLineAssertPolicy > \
	{ public: \
		typedef ::Loki::SmartAssert< ::Loki::CommandLineAssertPolicy > BaseClass; \
		class_name( bool * ignore, const char * expression ) \
			: BaseClass( ignore, expression ) {} \
		virtual ~class_name() {} \
	}; \
	if ( ignore_var_name || ( expr ) ) ; else \
	class_name( &ignore_var_name, #expr ).LOKI_SMART_ASSERT_CONTEXT


#ifdef LOKI_SMART_ASSERT_DEBUG
	#define LOKI_SMART_ASSERT( expr ) \
		LOKI_MAKE_SMART_ASSERT( LOKI_SMART_ASSERT_CLASS_NAME, LOKI_SMART_ASSERT_IGNORE_NAME, (expr) )

#else
	#define LOKI_SMART_ASSERT( expr ) \
		if ( true ) ; else \
			::Loki::SmartAssert< ::Loki::CommandLineAssertPolicy >()
			// Do nothing. Compiler should optimize away the else branch.
#endif


// ---------------------------------------------------------------------

#define LOKI_MAKE_SMART_ASSERT_POLICIED( class_name, ignore_var_name, expr, policy ) \
	static bool ignore_var_name = false; \
	class class_name : public ::Loki::SmartAssert< policy > \
	{ public: \
		typedef ::Loki::SmartAssert< policy > BaseClass; \
		class_name( bool * ignore, const char * expression ) \
			: BaseClass( ignore, expression ) {} \
		virtual ~class_name() {} \
	}; \
	if ( ignore_var_name || ( expr ) ) ; else \
	class_name( &ignore_var_name, #expr ).LOKI_SMART_ASSERT_CONTEXT


#ifdef LOKI_SMART_ASSERT_DEBUG
	#define LOKI_SMART_ASSERT_POLICIED( expr, policy ) \
		LOKI_MAKE_SMART_ASSERT_POLICIED( LOKI_SMART_ASSERT_CLASS_NAME, LOKI_SMART_ASSERT_IGNORE_NAME, (expr), policy )

#else
	#define LOKI_SMART_ASSERT_POLICIED( expr, policy ) \
		if ( true ) ; else \
			::Loki::SmartAssert< policy >()
			// Do nothing. Compiler should optimize away the else branch.
#endif

// ---------------------------------------------------------------------

#if defined( LOKI_DEFINED_NULLPTR )
    #undef LOKI_DEFINED_NULLPTR
    #undef nullptr
#endif

#if defined( _MSC_VER )
    #pragma warning( pop )
#endif

#endif // LOKI_SMART_ASSERT_HPP_INCLUDED
