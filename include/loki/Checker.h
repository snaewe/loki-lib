////////////////////////////////////////////////////////////////////////////////
//
// The Loki Library
// Copyright (c) 2008, 2009 Rich Sposato
// The copyright on this file is protected under the terms of the MIT license.
//
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
//
////////////////////////////////////////////////////////////////////////////////

// $Id$

/// @file Checker.h This file provides Loki's Checker facility.


// ----------------------------------------------------------------------------

#ifndef LOKI_CHECKER_H_INCLUDED
#define LOKI_CHECKER_H_INCLUDED

#include <exception>  // needed for calls to uncaught_exception.
#include <assert.h>


namespace Loki
{

/** @par ContractChecker and StaticChecker Overview
 ContractChecker and StaticChecker classes provide a mechanism to enforce
 "Design by Contract" programming practices.  According to Design by Contract,
 each function and class provides a contract with client code.  Each contract
 says:
 - what operations the class or function performs,
 - what input it needs,
 - what output it provides,
 - what exception safety level it guarantees, and
 - what constraints it enforces on its data.

 ContractChecker and StaticChecker encourage Design by Contract by:
 - providing a mechanism by which programmers can determine if functions
   violate class/data invariants,
 - and determining which exception safety level a function provides.

 @par Class & Data Invariants
 ContractChecker and StaticChecker define invariants as "conditions that are
 true for particular objects".  They call a function which returns true if all
 conditions are valid, and returns false if any condition is invalid.  This is
 called the validator function, and the host class or function provides a
 pointer to it. The validator may assert for any invariant which fails rather
 than return false.  If the validator is a static member function, you can use
 it with checkers in any function, but especially standalone functions and class
 static functions.  If the validator is a non-static member function, you can
 use it only within non-static member functions.

 @par Exception Safety Levels
 Years ago, David Abrahams formalized a framework for assessing the exception
 safety level a function provides.  His framework describes three levels of
 guarantees.  Any function which does not provide any of these levels is
 considered unsafe.  ContractChecker and StaticChecker determine a function's
 safety level through the use of policy classes.  Checker's policy classes can
 show if a function provides any of these three guarantees.  Since there is no
 universal way to detect leaks, this facility provides no mechanism for finding
 leaks, but users may create their own validators which do.  StaticChecker's
 policy classes only provide direct checking for the no-throw and invariant
 guarantees.  With some finesse, a programmer can write a validator for
 StaticChecker that checks for the Strong guarantee.

 - No-throw guarantee: A function will not throw any exceptions.
 - Strong guarantee: A function will not change data if an exception occurs.
   (Which I call the no-change guarantee.)
 - Basic guarantee: A function will not leak resources and data will remain
   in a valid state if an exception occurs.  (Which I call either the no-leak
   or no-break guarantee depending on context.)

 @par Provided Exception Policies
 Loki provides several exception policies for use with ContractChecker.
 - CheckForNoThrowOrChange
 - CheckForNoThrow
 - CheckForNoChangeOnThrow
 - CheckForNoChange
 - CheckForNothing
 Loki also provides these two policies for StaticChecker.
 - CheckStaticForNoThrow
 - CheckStaticForNothing

 @par Reasons to Write Your Own Policies for ContractChecker
 Loki provides several exception policies for ContractChecker that assert if an
 object changed or a function threw an exception.  These are good reasons to
 write your own policies:
 - you want to log failures to a file,
 - you want to create a bug report,
 - you want to stop your unit-test framework,
 - your classes don't have copy-constructors or equality operators,
 - or whatever else.

 @par Guidelines for Writing Your Own Policies for ContractChecker
 Please follow these guidelines when writing your own policies:
 - Each policy class must provide three public functions, a constructor, a destructor, and a Check function.
 - The destructor could be implied.  (Not actually written, and provided by the compiler.)
 - The constructor and Check functions accept a pointer to const instance of the host class.
 - Your policy class will become a base class of ContractChecker.
 - Check should return true if all is okay, and false for any failures.
 - Check should never throw any exceptions since it is called by a destructor.
 - You may add other functions or features to your policy class.

 @par Using a Memento
 Sometimes copying or comparing the host object is very expensive.  If you do
 not want ContractChecker to copy the host object, you can provide an optional
 template parameter called Memento.  The memento stores a little information on
 the host object's state so that when the Host's function ends, ContractChecker
 will use the memento to determine if the host object changed.  A Host class can
 declare a Memento class as an internal class.  These guidelines will help you
 design a memento class:
 - Your exception policy's constructor should not copy the host object but use a memento to store info about the host.
 - The memento should provide a constructor, an equality operator, and a destructor.
 - The memento's destructor could be implied.  (Not actually written, and provided by the compiler.)
 - The memento's constructor and equality operator must accept a reference to a const Host object.
 - The memento's equality operator should return false if the memento differs from the host.

 This code snippet shows the template parameters for an exception policy that
 uses a Memento and the signatures for the three required functions for a
 ContractChecker policy.
 @code
 template < class Host, class Memento >
 class YourPolicy
 {
 public:
    explicit YourPolicy( const Host * );
    bool Check( const Host * ) const;
    ~YourPolicy();
  private:
    Memento m_compare;
 }
 @endcode

 The following code snippet shows the template parameters and function
 declarations for an exception policy that does not use a Memento.
 @code
 template < class Host >
 class YourPolicy< Host, void >
 {
 public:
    explicit YourPolicy( const Host * );
    bool Check( const Host * ) const;
    ~YourPolicy();
 }
 @endcode

 @par Writing Your Own Policies for StaticChecker
 Loki provides two exception policies for StaticChecker - one that asserts if
 an exception occurred, and one that does not care about exceptions.  You can
 make your own policy to log failures, send an email, file a bug report, or do
 whatever.  Just write a policy with a default constructor and a function call
 Check which look like those shown below.  Make sure your Check function never
 throws any exceptions.  Any additional functions or features of the policy
 are up to you.
 @code
 class YourPolicy
 { public:
    YourPolicy();
    bool Check() const;
 }
 @endcode

 @par Requirements for Host Object
 CheckForNoThrow and CheckForNothing impose no restrictions on the host class.
 The policies for StaticChecker impose no restrictions either.  All other
 policies require the Host class to either provide a Memento class or have a
 public copy-constructor, destructor, and equality operator.  If the Host class
 provides a Memento class, then follow the guidelines listed above in the
 section called "Using a Memento".

 @par Writing Your Own Policies for StaticChecker
 Loki provides two exception policies for StaticChecker - one that asserts if
 an exception occurred, and one that does not care about exceptions.  Please
 follow these guidelines when writing policies:
 - Each policy needs a default constructor, a destructor, and a function named Check.
 - The constructor and destructor may be implied.
 - Make sure your Check function never throws any exceptions.
 - Any additional functions or features of the policy are up to you.

 This code snippet shows the signatures for the three required functions for a
 StaticChecker policy.
 @code
 class YourPolicy
 { public:
    YourPolicy();
    bool Check() const;
    ~YourPolicy();
 }
 @endcode
 */


// ----------------------------------------------------------------------------

/** @class CheckForNoThrow

 @par Exception Safety Level:
 This exception-checking policy class for ContractChecker asserts if an
 exception exists.  Host classes can use this to show that a member function
 provides the no-throw exception safety guarantees.  Since this policy does not
 care if the host object changed, use this policy for operations which change
 the host object but never throw.

 @par Requirements For Host Class:
 This policy imposes no requirements on a host class.  This ignores the Memento
 template parameter.
 */

template < class Host, class Memento >
class CheckForNoThrow
{
public:

    inline explicit CheckForNoThrow( const Host * ) {}

    inline bool Check( const Host * ) const
    {
        const bool okay = ( !::std::uncaught_exception() );
        assert( okay );
        return okay;
    }
};

// ----------------------------------------------------------------------------

template < class Host, class Memento > class CheckForNoChangeOnThrow;

/** @class CheckForNoChangeOnThrow

 @par Exception Safety Level:
 This exception-checking policy class for ContractChecker asserts only if a
 copy of the host differs from the host object when an exception occurs.  Host
 classes can use this policy to show which member functions provide the strong
 exception guarantee. Such functions provide transaction semantics - either the
 action succeeds or the object rollsback to its previous state.

 @par Requirements:
 This policy requires hosts to provide both the copy-constructor and the
 equality operator, and is intended for classes with value semantics.
 */

template < class Host >
class CheckForNoChangeOnThrow< Host, void >
{
public:

    inline explicit CheckForNoChangeOnThrow( const Host * host ) :
        m_compare( *host ) {}

    inline bool Check( const Host * host ) const
    {
        const bool okay = ( !::std::uncaught_exception() )
            || ( m_compare == *host );
        assert( okay );
        return okay;
    }

private:
    Host m_compare;
};

template < class Host, class Memento >
class CheckForNoChangeOnThrow
{
public:

    inline explicit CheckForNoChangeOnThrow( const Host * host ) :
        m_compare( *host ) {}

    inline bool Check( const Host * host ) const
    {
        const bool okay = ( !::std::uncaught_exception() )
            || ( m_compare == *host );
        assert( okay );
        return okay;
    }

private:
    Memento m_compare;
};

// ----------------------------------------------------------------------------

template < class Host, class Memento > class CheckForNoThrowOrChange;

/** @class CheckForNoThrowOrChange
 This policy comes in two forms - one uses a memento, and one does not.  The
 memento form does not copy the host object, but stores info about the host in
 a memento for later comparison with the host.  The other form copies the host
 object to a temporary and then compares that to the original.

 @par Exception Safety Level:
 This exception-checking policy class for ContractChecker asserts either if a
 copy of the host differs from the original host object, or if an exception
 occurs. Host classes can use this policy to show which member functions provide
 the no-throw exception guarantee and never change data anyway.

 @par Requirements For Host Class:
 This policy requires hosts to provide both the copy-constructor and the
 equality operator, and is intended for classes with value semantics.
 */

template < class Host, class Memento >
class CheckForNoThrowOrChange
{
public:

    inline explicit CheckForNoThrowOrChange( const Host * host ) :
        m_compare( *host ) {}

    inline bool Check( const Host * host ) const
    {
        bool okay = ( !::std::uncaught_exception() );
        assert( okay );
        okay = ( m_compare == *host );
        assert( okay );
        return okay;
    }

private:
    Memento m_compare;
};

template < class Host >
class CheckForNoThrowOrChange< Host, void >
{
public:

    inline explicit CheckForNoThrowOrChange( const Host * host ) :
        m_compare( *host ) {}

    inline bool Check( const Host * host ) const
    {
        bool okay = ( !::std::uncaught_exception() );
        assert( okay );
        okay = ( m_compare == *host );
        assert( okay );
        return okay;
    }

private:
    Host m_compare;
};

// ----------------------------------------------------------------------------

template < class Host, class Memento > class CheckForNoChange;

/** @class CheckForNoChange

 @par Exception Safety Level:
 This exception-checking policy class for ContractChecker asserts if a copy of
 the host differs from the host object regardless of whether an exception occurs.
 Host classes can use this policy to show which member functions never change
 data members, and thereby provide the strong exception safety level by default.

 @par Requirements For Host Class:
 This policy requires hosts to provide both the copy-constructor and the
 equality operator, and is intended for classes with value semantics.

 @par Requirements For Memento Class:
 This policy requires Memento to provide a constructor and an equality operator
 that accept a reference to a const host.
 */

template < class Host, class Memento >
class CheckForNoChange
{
public:

    inline explicit CheckForNoChange( const Host * host ) :
        m_compare( *host ) {}

    inline bool Check( const Host * host ) const
    {
        const bool okay = ( m_compare == *host );
        assert( okay );
        return okay;
    }

private:
    Memento m_compare;
};

template < class Host >
class CheckForNoChange< Host, void >
{
public:

    inline explicit CheckForNoChange( const Host * host ) :
        m_compare( *host ) {}

    inline bool Check( const Host * host ) const
    {
        const bool okay = ( m_compare == *host );
        assert( okay );
        return okay;
    }

private:
    Host m_compare;
};

// ----------------------------------------------------------------------------

/** @class CheckForNothing

 @par Exception Safety Level:
 This exception-checking policy class for ContractChecker does nothing when
 called.  Host classes can use this to show which member functions provide
 neither the strong nor no-throw exception guarantees.  The best guarantee such
 functions can provide is that nothing gets leaked.  Use this policy for any
 function that may throw exceptions and will change the host object.

 @par Requirements For Host Class:
 This policy imposes no requirements on a host class.  This ignores the Memento
 template parameter.
 */

template < class Host, class Memento >
class CheckForNothing
{
public:
    inline explicit CheckForNothing( const Host * ) {}
    inline bool Check( const Host * ) const { return true; }
};

// ----------------------------------------------------------------------------

/** @class ContractChecker
 This class determines if a function violated any class invariant, but it also
 determines if a function fulfills its contract with client code.  In the
 "Design by Contract" paradigm, each function has certain pre-conditions and
 post-conditions which may differ from the class invariants.  This asserts if a
 check for an invariant fails as well as if any pre- or post-condition fails.
 It also demonstrate which exception safety level a function provides.

 @par Usage
 -# Implement a function that checks each class invariant.  The function must
    have the signature similar to the Validator type.  Something like:
    "bool Host::IsValid( void ) const;"
    - The function should return true if everything is okay, but false if
      something is wrong.
    - Or it could assert if anything is wrong.
    - Ideally, it should be private.
    - It should never throw an exception.
 -# Optionally implement similar functions to check for pre-conditions and post-
    conditions.  Functions which verify pre-conditions and post-conditions do
    not need to check all class invariants, just conditions specific to certain
    public functions in the host class.  The post-condition function should never
    throw exceptions.
 -# Add this line in the class declaration:
    - typedef ::Loki::CheckFor< Host > CheckFor;
 -# Add one of these lines at the top of various class member functions to
   construct a checker near the top of each public function.  You may also pass
   in pointers to functions which check pre- and post-conditions.
   - CheckFor::NoThrowOrChange checker( this, &Host::IsValid );
   - CheckFor::NoChangeOnThrow checker( this, &Host::IsValid );
   - CheckFor::NoThrow checker( this, &Host::IsValid );
   - CheckFor::NoChange checker( this, &Host::IsValid );
   - CheckFor::Invariants checker( this, &Host::IsValid );
 -# Use these guidelines to decide which policy to use inside which function:
    - If the function never throws, then use the CheckForNoThrow policy.
    - If the function never changes any data members, then use CheckForNoChange
      policy.
    - If the function's normal execution flow changes data, but must make sure
      data remains unchanged when any exceptions occur, then use the
      CheckForNoChangeOnThrow policy.
    - Otherwise use the CheckInvariants policy.
 -# Recompile a debug version of your program, run the program and all the unit
    tests, and look for which assertions failed.
 */

template
<
    class Host,
    template < class, class > class ExceptionPolicy,
    class Memento = void
>
class ContractChecker : public ExceptionPolicy< Host, Memento >
{
    /// Shorthand for the ExceptionPolicy class.
    typedef ExceptionPolicy< Host, Memento > Ep;

public:

    /// Signature for the validation function.
    typedef bool ( Host:: * Validator )( void ) const;

    /** The constructor makes sure the host is valid at the time the checker
     was created, thus insuring the host object was not corrupt from the start.
     @par host Pointer to host object.
     @par validator Pointer to function that checks class invariants.
     @par pre Optional pointer to function that checks pre-conditions.
     @par post Optional pointer to function that checks post-conditions.
     */
    inline ContractChecker( const Host * host, Validator validator,
        Validator pre = 0, Validator post = 0 ) :
        Ep( host ),
        m_host( host ),
        m_validator( validator ),
        m_pre( pre ),
        m_post( post )
    {
        assert( Check() );
        if ( 0 != m_pre )
        {
            assert( ( m_host->*( m_pre ) )() );
        }
    }

    /** The destructor checks if any Host invariants failed, and then calls the
     ExceptionPolicy's Check function to determine what to do in case of an
     exception.
     */
    inline ~ContractChecker( void )
    {
        assert( Check() );
        if ( 0 != m_post )
        {
            assert( ( m_host->*( m_post ) )() );
        }
        assert( Ep::Check( m_host ) );
    }

    /** This first checks the invariants for ContractChecker, and then calls the
     validator function for the host to make sure no class invariants were
     broken by the host within the Host's member function body.  The host
     member function can call Check directly to verify the object remains valid
     at any time.  This does not care if the pre- and post-condition validator
     pointers are null since a host class may pass in NULL pointers for either
     to indicate the pre-conditions or post-conditions are the same as the
     overall class invariants.
     */
    inline bool Check( void ) const
    {
        assert( 0 != this );
        assert( 0 != m_host );
        assert( 0 != m_validator );
        // Now that this confirms the pointers to the host and validation
        // functions are not null, go ahead and validate the host object.
        const bool okay = ( m_host->*( m_validator ) )();
        assert( okay );
        return okay;
    }

private:

    /// Default constructor is not implemented.
    ContractChecker( void );
    /// Copy constructor is not implemented.
    ContractChecker( const ContractChecker & );
    /// Copy-assignment operator is not implemented.
    ContractChecker & operator = ( const ContractChecker & );

    /// Pointer to the host object.
    const Host * m_host;

    /// Pointer to member function that checks Host object's invariants.
    Validator m_validator;

    /// Pointer to member function that checks Host function's pre-conditions.
    Validator m_pre;

    /// Pointer to member function that checks Host function's post-conditions.
    Validator m_post;

};

// ----------------------------------------------------------------------------

/** @struct CheckFor
 This struct declares types of checkers used to validate a host object.  All of
 Loki's exception-checking policies are named here as typedef's so host classes
 have a one-stop convenience place for declaring them.  If you write your own
 exception policies for ContractChecker, you might want to also write a struct
 similiar to CheckFor to conveniently declare all your policies.
 */
template < class Host, class Memento = void >
struct CheckFor
{
    // These lines declare checkers for non-static functions in a host class.
    typedef ContractChecker< Host, CheckForNoThrowOrChange, Memento > NoThrowOrChange;
    typedef ContractChecker< Host, CheckForNoChangeOnThrow, Memento > NoChangeOnThrow;
    typedef ContractChecker< Host, CheckForNoThrow,         Memento > NoThrow;
    typedef ContractChecker< Host, CheckForNoChange,        Memento > NoChange;
    typedef ContractChecker< Host, CheckForNothing,         Memento > Invariants;

    typedef bool ( Host:: * Validator )( void ) const;
};

// ----------------------------------------------------------------------------

/** @class CheckStaticForNoThrow

 @par Exception Safety Level:
 This exception-checking policy class for StaticChecker asserts if an exception
 exists.  Functions can use this to show they provide the no-throw exception
 safety guarantee.
 */
class CheckStaticForNoThrow
{
public:
    inline bool Check( void )
    {
        const bool okay = !::std::uncaught_exception();
        assert( okay );
        return okay;
    }
};

// ----------------------------------------------------------------------------

/** @class CheckStaticForNothing

 @par Exception Safety Level:
 This exception-checking policy class for StaticChecker does nothing when called.
 Functions can use this to show they might provide the weak exception guarantee.
 The best guarantee such functions can provide is that nothing gets leaked.
 */
class CheckStaticForNothing
{
public:
    inline bool Check( void ) { return true; }
};

// ----------------------------------------------------------------------------

/** @class StaticChecker
 This class checks if a function provides the no-throw exception safety level
 and if the function violated any invariants.  Invariants for stand-alone and
 static functions act as pre-conditions and post-conditions.

 @par Usage
 -# Implement a function that checks the invariants associated with a function,
    or with the static data for a class.  The function must
    have the signature similar to the Validator type.  Something like:
    "static bool Host::StaticIsValid( void );" or "bool IsOkay( void );"
    - The function should return true if everything is okay, but false if
      something is wrong.
    - Or it could assert if anything is wrong.
    - But it should never throw an exception.
 -# If the checker validates static functions within a class, add this line to
    the class declaration.
    - typedef ::Loki::CheckStaticFor CheckStaticFor;
 -# Construct a checker near the top of each member function using one of
    these lines:
    - CheckStaticFor::NoThrow checker( &Host::StaticIsValid );
    - CheckStaticFor::Invariants checker( &Host::StaticIsValid );
 -# These guidelines can help you decide which exception policy to use within
    each function.
    - If the function never throws, then use the CheckForNoThrow policy.
    - Otherwise use the CheckInvariants policy.
 -# If the checker validates standalone functions, then just add one of these
    lines at the top of the function.  You may also want to write validation
    functions which check pre-conditions and post-conditions of standalone
    functions which you can pass into the checker as optional 2nd and 3rd
    parameters.
    - ::Loki::CheckStaticFor::NoThrow checker( &AllIsValid );
    - ::Loki::CheckStaticFor::Invariants checker( &AllIsValid );
 -# Recompile a debug version of your program, run it, and see if an assertion
    fails.
 */

template
<
    class ExceptionPolicy
>
class StaticChecker : public ExceptionPolicy
{
    /// Shorthand for the ExceptionPolicy class.
    typedef ExceptionPolicy Ep;

public:

    /// Signature for the validation function.
    typedef bool ( * Validator )( void );

    /** The constructor makes sure the host is valid at the time the checker
     was created, thus insuring the host object was not corrupt from the start.
     @par validator Pointer to function that checks class invariants.
     @par pre Optional pointer to function that checks pre-conditions.
     @par post Optional pointer to function that checks post-conditions.
     */
    inline explicit StaticChecker( Validator validator,
        Validator pre = 0, Validator post = 0 ) :
        Ep(),
        m_validator( validator ),
        m_pre( pre ),
        m_post( post )
    {
        assert( Check() );
        if ( 0 != m_pre )
            assert( m_pre() );
    }

    /** The destructor checks if any Host invariants failed, and then calls the
     ExceptionPolicy's Check function to determine what to do in case of an
     exception.
     */
    inline ~StaticChecker( void )
    {
        assert( Check() );
        if ( 0 != m_post )
            assert( m_post() );
        assert( Ep::Check() );
    }

    /** This first checks its own invariants, and then calls the validator
     function to make sure no invariants were broken by the function which
     created this checker.  That function can call Check directly to verify the
     data remains valid at any time.  This does not care if the pre- and post-
     condition validator pointers are null since a host class may pass in NULL
     pointers for either to indicate the pre-conditions or post-conditions are
     the same as the overall class invariants.
     */
    inline bool Check( void ) const
    {
        assert( 0 != this );
        assert( 0 != m_validator );
        // Now that this confirms the pointers to the host and validation
        // functions are not null, go ahead and validate the host object.
        const bool okay = m_validator();
        assert( okay );
        return okay;
    }

private:

    /// Default constructor is not implemented.
    StaticChecker( void );
    /// Copy constructor is not implemented.
    StaticChecker( const StaticChecker & );
    /// Copy-assignment operator is not implemented.
    StaticChecker & operator = ( const StaticChecker & );

    /// Pointer to member function that checks Host object's invariants.
    Validator m_validator;

    /// Pointer to member function that checks Host function's pre-conditions.
    Validator m_pre;

    /// Pointer to member function that checks Host function's post-conditions.
    Validator m_post;

};

// ----------------------------------------------------------------------------

/** @struct CheckStaticFor
 This struct declares types of checkers used to validate standalone functions
 and class static functions.  All of Loki's exception-checking policies for
 StaticChecker are named here as typedef's.  If you write your own
 exception policies for StaticChecker, you might want to also write a struct
 similiar to CheckStaticFor to conveniently declare all your policies.
 */
struct CheckStaticFor
{
    // These lines declare checkers for static functions of a host class
    // or for standalone functions outside any class or struct.
    typedef StaticChecker< CheckStaticForNoThrow > NoThrow;
    typedef StaticChecker< CheckStaticForNothing > Invariants;

    typedef bool ( * Validator )( void );
};

// ----------------------------------------------------------------------------

} // end namespace Loki

#endif
