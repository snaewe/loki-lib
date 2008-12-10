////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2007 Rich Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$

#include <loki/CheckReturn.h>

#include <iostream>
#include <string>

using namespace std;


// ----------------------------------------------------------------------------

typedef ::Loki::CheckReturn< bool > BoolReturn;

typedef ::Loki::CheckReturn< string > StringReturn;

typedef ::Loki::CheckReturn< bool , ::Loki::FprintfStderr > BoolReturnStderr;


// ----------------------------------------------------------------------------

bool NoCheckRequired( void )
{
    return true;
}

// ----------------------------------------------------------------------------

BoolReturn CheckRequired( void )
{
    return BoolReturn( true );
}

// ----------------------------------------------------------------------------

BoolReturnStderr CheckRequiredStderr( void )
{
    return BoolReturnStderr( true );
}


// ----------------------------------------------------------------------------

BoolReturn CheckRequired( bool value )
{
    // By passing false into CheckRequired, CheckRequired calls an overloaded
    // version of CheckRequired, thus demonstrating that one can use a function
    // call within the return statement and not have to create a named
    // temporary variable.
    if ( !value )
        return CheckRequired();

    return BoolReturn( value );
}

// ----------------------------------------------------------------------------

string GetString( void )
{
    return string( "a" );
}

// ----------------------------------------------------------------------------

StringReturn MustUseString( const char * s )
{
    return StringReturn( s );
}

// ----------------------------------------------------------------------------

void StringEater( const string & s )
{
    (void)s;
}

// ----------------------------------------------------------------------------

int main( unsigned int argc, const char * argv[] )
{

    if ( 2 == argc )
    {
        const char * const a = argv[ argc - 1 ];

        // okay to call without storing or using return value.
        GetString();
        cout << "Called GetString without using return value." << endl;

        // Should not assert since caller stores return value.
        const string s = MustUseString( a );
        cout << "Called MustUseString and stored return value." << endl;

        {
            // Should not assert since caller passes return value into another
            // function, thus converting CheckReturn back into actual type.
            StringEater( MustUseString( a ) );
            cout <<
                "Called MustUseString and passed return value into another function."
                << endl;
        }

        // This should assert since caller does not check return value.
        MustUseString( s.c_str() );
        cout << "Should assert before this line!  How did we get here?" << endl;
    }


    // okay to call without storing or using return value.
    NoCheckRequired();
    cout << "Called NoCheckRequired without using return value." << endl;

    // Should not assert since caller stores return value.
    bool okay = CheckRequired();
    cout << "Called CheckRequired and stored return value." << endl;

    if ( CheckRequired( okay ) )
    {
        cout << "Called CheckRequired and used return value." << endl;
    }

    {
        // Should not assert since caller stores return value ...
        BoolReturn checkBool = CheckRequired();
        // and then deliberately ignores it before destructor runs.
        (bool)checkBool;
        cout << "Called CheckRequired, stored return value, and ignored it."
             << endl;
    }

    {
        // This should not assert since caller deliberately chooses to not
        // check return value by casting to return value to correct type.
        (bool)CheckRequired();
    }

    {
        // This should not assert since caller deliberately chooses to not
        // check return value by casting to return value to correct type.
        (bool)CheckRequired( false );
        cout << "Made a nested call to CheckRequired." << endl;
    }

	{
		BoolReturnStderr check = CheckRequiredStderr();
	}
	cout << "There should be a error message: \nCheckReturn: return value was not checked" << endl;

    // This should assert since caller does not check return value.
    CheckRequired();
    cout << "Should assert before this line!  How did we get here?" << endl;

	return 0;
}

// ----------------------------------------------------------------------------

// $Log$
