////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter K�mmel
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$


#include <loki/ScopeGuard.h>

#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

using namespace Loki;


// ----------------------------------------------------------------------------

void HasNone( void )
{
}

void HasOne( int p1 )
{
    (void)p1;
}

void HasTwo( int p1, int p2 )
{
    (void)p1;
    (void)p2;
}

void HasThree( int p1, int p2, int p3 )
{
    (void)p1;
    (void)p2;
    (void)p3;
}

void HasFour( int p1, int p2, int p3, int p4 )
{
    (void)p1;
    (void)p2;
    (void)p3;
    (void)p4;
}

void HasFive( int p1, int p2, int p3, int p4, int p5 )
{
    (void)p1;
    (void)p2;
    (void)p3;
    (void)p4;
    (void)p5;
}

void Decrement( unsigned int & x )
{
    --x;
}

struct UserDatabase
{
    void AddFriend(const std::string&, const std::string&)
    {
        throw 55;
    }
};

class User
{
public:
    User(UserDatabase* db) : fCount(0), pDB_(db)
    {}

    std::string GetName() const;

    void AddFriend(User& newFriend);
    void AddFriendGuarded(User& newFriend);
    void AddFriendGuardedMacros(User& newFriend);

    size_t countFriends() const;

	void DoSomething() const;

    unsigned int fCount;

    void HasNone( void ) const;
    void HasOne( int p1 ) const;
    void HasTwo( int p1, int p2 ) const;
    void HasThree( int p1, int p2, int p3 ) const;
    void HasFour( int p1, int p2, int p3, int p4 ) const;
    void HasFive( int p1, int p2, int p3, int p4, int p5 ) const;

private:
	void CheckIfValid( const char * function, unsigned int line ) const;

    typedef std::vector<User*> UserCont;
    UserCont friends_;
    UserDatabase* pDB_;
};

void User::HasNone( void ) const
{
}

void User::HasOne( int p1 ) const
{
    (void)p1;
}

void User::HasTwo( int p1, int p2 ) const
{
    (void)p1;
    (void)p2;
}

void User::HasThree( int p1, int p2, int p3 ) const
{
    (void)p1;
    (void)p2;
    (void)p3;
}

void User::HasFour( int p1, int p2, int p3, int p4 ) const
{
    (void)p1;
    (void)p2;
    (void)p3;
    (void)p4;
}

void User::HasFive( int p1, int p2, int p3, int p4, int p5 ) const
{
    (void)p1;
    (void)p2;
    (void)p3;
    (void)p4;
    (void)p5;
}

void User::DoSomething() const
{
}

void User::CheckIfValid( const char * function, unsigned int line ) const
{
	assert( friends_.size() == fCount );
	(void)function;
	(void)line;
}

std::string User::GetName() const
{
    return "A name";
}

size_t User::countFriends() const
{
    return friends_.size();
}

void User::AddFriend(User& newFriend)
{
	ScopeGuard invariantGuard = MakeObjGuard( *this,
		&User::CheckIfValid, __FUNCTION__, __LINE__ );
	(void)invariantGuard;
    friends_.push_back(&newFriend);
    fCount++;
    pDB_->AddFriend(GetName(), newFriend.GetName());
}

void User::AddFriendGuarded(User& newFriend)
{
	ScopeGuard invariantGuard = MakeObjGuard( *this,
		&User::CheckIfValid, __FUNCTION__, __LINE__ );
	(void)invariantGuard;
	ScopeGuard guard1 = MakeObjGuard( *this, &User::DoSomething );
    (void)guard1;

    friends_.push_back(&newFriend);
    Loki::ScopeGuard guard = Loki::MakeObjGuard(friends_, &UserCont::pop_back);

    fCount++;
    Loki::ScopeGuard guardRef = Loki::MakeGuard(Decrement, Loki::ByRef(fCount));

    pDB_->AddFriend(GetName(), newFriend.GetName());
    guard.Dismiss();
    guardRef.Dismiss();
}

void User::AddFriendGuardedMacros(User&)
{
	ScopeGuard invariantGuard = MakeObjGuard( *this,
		&User::CheckIfValid, __FUNCTION__, __LINE__ );
	(void)invariantGuard;
    LOKI_ON_BLOCK_EXIT_OBJ(friends_, &UserCont::pop_back); (void) LOKI_ANONYMOUS_VARIABLE(scopeGuard);
    // GCC 4.2 Bug
#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 10 + __GNUC_MINOR__ * 1)
#endif
#if !defined(__GNUC__) || (GCC_VERSION < 42) || (GCC_VERSION > 42)
    LOKI_ON_BLOCK_EXIT(Decrement, Loki::ByRef(fCount)); (void) LOKI_ANONYMOUS_VARIABLE(scopeGuard);
#endif
}

void DoStandaloneFunctionTests()
{
    ScopeGuard guard0 = MakeGuard( &HasNone );
    ScopeGuard guard1 = MakeGuard( &HasOne, 1 );
    ScopeGuard guard2 = MakeGuard( &HasTwo, 1, 2 );
    ScopeGuard guard3 = MakeGuard( &HasThree, 1, 2, 3 );
    ScopeGuard guard4 = MakeGuard( &HasFour, 1, 2, 3, 4 );
    ScopeGuard guard5 = MakeGuard( &HasFive, 1, 2, 3, 4, 5 );
    (void)guard0;
    (void)guard1;
    (void)guard2;
    (void)guard3;
    (void)guard4;
    (void)guard5;
}

void DoMemberFunctionTests( User & user )
{
    ScopeGuard guard0 = MakeObjGuard( user, &User::HasNone );
    ScopeGuard guard1 = MakeObjGuard( user, &User::HasOne,   1 );
    ScopeGuard guard2 = MakeObjGuard( user, &User::HasTwo,   1, 2 );
    ScopeGuard guard3 = MakeObjGuard( user, &User::HasThree, 1, 2, 3 );
    (void)guard0;
    (void)guard1;
    (void)guard2;
    (void)guard3;
}

// ----------------------------------------------------------------------------

void CalledAlways( void )
{
	const bool anyThrown = ::std::uncaught_exception();
	const char * message = ( anyThrown ) ? "thrown" : "not thrown";
	::std::cout << __FUNCTION__ << "   Exception was " << message << ::std::endl;
}

// ----------------------------------------------------------------------------

void CalledIfNoException( void )
{
	const bool anyThrown = ::std::uncaught_exception();
	const char * message = ( anyThrown ) ? "thrown" : "not thrown";
	::std::cout << __FUNCTION__ << "   Exception was " << message << ::std::endl;
}

// ----------------------------------------------------------------------------

void CalledIfException( void )
{
	const bool anyThrown = ::std::uncaught_exception();
	const char * message = ( anyThrown ) ? "thrown" : "not thrown";
	::std::cout << __FUNCTION__ << "   Exception was " << message << ::std::endl;
}

// ----------------------------------------------------------------------------

void FunctionMightThrow( bool willThrow )
{
	if ( willThrow )
		throw ::std::exception();
}

// ----------------------------------------------------------------------------

void DoExceptionTests( void )
{
	::std::cout << ::std::endl;

	{
		try
		{
			::std::cout << "The function should be called." << ::std::endl;
			ScopeGuard guard = MakeGuard( &CalledAlways );
			(void)guard;
			FunctionMightThrow( false );
		}
		catch ( ... )
		{
			::std::cout << "Caught exception." << ::std::endl;
			assert( false );
		}
	}

	::std::cout << ::std::endl;

	{
		try
		{
			::std::cout << "The function should be called." << ::std::endl;
			ScopeGuard guard = MakeGuard( &CalledAlways );
			(void)guard;
			FunctionMightThrow( true );
			assert( false );
		}
		catch ( ... )
		{
			::std::cout << "Caught exception." << ::std::endl;
		}
	}

	::std::cout << ::std::endl;

	{
		try
		{
			::std::cout << "The function should be called." << ::std::endl;
			ScopeGuard guard = MakeGuard( &CalledIfNoException );
			guard.SetExceptionPolicy( ScopeGuardImplBase::CallIfNoException );
			FunctionMightThrow( false );
			::std::cout << "No exception thrown." << ::std::endl;
		}
		catch ( ... )
		{
			::std::cout << "Caught exception." << ::std::endl;
			assert( false );
		}
	}

	::std::cout << ::std::endl;

	{
		try
		{
			::std::cout << "The function should not be called." << ::std::endl;
			ScopeGuard guard = MakeGuard( &CalledIfNoException );
			guard.SetExceptionPolicy( ScopeGuardImplBase::CallIfNoException );
			FunctionMightThrow( true );
			assert( false );
		}
		catch ( ... )
		{
			::std::cout << "Caught exception." << ::std::endl;
		}
	}

	::std::cout << ::std::endl;

	{
		try
		{
			::std::cout << "The function should not be called." << ::std::endl;
			ScopeGuard guard = MakeGuard( &CalledIfException );
			guard.SetExceptionPolicy( ScopeGuardImplBase::CallIfException );
			FunctionMightThrow( false );
			::std::cout << "No exception thrown." << ::std::endl;
		}
		catch ( ... )
		{
			::std::cout << "Caught exception." << ::std::endl;
			assert( false );
		}
	}

	::std::cout << ::std::endl;

	{
		try
		{
			::std::cout << "The function should be called." << ::std::endl;
			ScopeGuard guard = MakeGuard( &CalledIfException );
			guard.SetExceptionPolicy( ScopeGuardImplBase::CallIfException );
			FunctionMightThrow( true );
			assert( false );
		}
		catch ( ... )
		{
			::std::cout << "Caught exception." << ::std::endl;
		}
	}

	::std::cout << ::std::endl;
}

// ----------------------------------------------------------------------------

class Junk
{
public:

	Junk( void );
	~Junk( void );

	bool IsUgly( void ) const;

	bool IsUseful( void ) const;

};

// ----------------------------------------------------------------------------

Junk::Junk( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

Junk::~Junk( void )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

bool Junk::IsUgly( void ) const
{
    assert( NULL != this );
    return true;
}

// ----------------------------------------------------------------------------

bool Junk::IsUseful( void ) const
{
    assert( NULL != this );
    return true;
}

// ----------------------------------------------------------------------------

class Attic
{
public:

	Attic( void );
	~Attic( void );

	Junk * RemoveUselessJunk( void );

private:

	typedef ::std::vector< Junk * > JunkPile;
	typedef JunkPile::iterator JunkPileIter;

	void CheckInvariants( const char * function, unsigned int line ) const;

	void CallObservers( void ) const;

	void EliminateHoles( void );

    JunkPile m_storage;

};

// ----------------------------------------------------------------------------

Attic::Attic( void ) :
	m_storage()
{
}

// ----------------------------------------------------------------------------

Attic::~Attic( void )
{
}

// ----------------------------------------------------------------------------

Junk * Attic::RemoveUselessJunk( void )
{
    ScopeGuard callGuard = MakeObjGuard( *this, &Attic::CallObservers );
    ScopeGuard testGuard = MakeObjGuard( *this, &Attic::CheckInvariants, __FUNCTION__, __LINE__ );
    ScopeGuard fixGuard = MakeObjGuard( *this, &Attic::EliminateHoles );
	callGuard.SetExceptionPolicy( ScopeGuardImplBase::CallIfNoException );
	(void)testGuard;
	(void)fixGuard;

    for ( JunkPileIter it( m_storage.begin() );
          it != m_storage.end();
          ++it )
    {
        Junk * junk = *it;
        if ( junk->IsUgly() )
            return junk;
        if ( junk->IsUseful() )
			continue;
        delete junk;
        *it = NULL;
    }

    return NULL;
}

// ----------------------------------------------------------------------------

void Attic::CheckInvariants( const char * function, unsigned int line ) const
{
    assert( NULL != this );
	(void)function;
	(void)line;
}

// ----------------------------------------------------------------------------

void Attic::CallObservers( void ) const
{
}

// ----------------------------------------------------------------------------

void Attic::EliminateHoles( void )
{
}

// ----------------------------------------------------------------------------

int main()
{
    UserDatabase db;

    User u1(&db);
    User u2(&db);

    try{ u1.AddFriend(u2); }
    catch (...){}
    std::cout << "u1 countFriends: " << u1.countFriends() << "\n";
    std::cout << "u1 fCount      : " << u1.fCount << "\n";

    try{ u2.AddFriendGuarded(u1); }
    catch (...){}
    std::cout << "u2 countFriends: " << u2.countFriends() << "\n";
    std::cout << "u2 fCount      : " << u2.fCount << "\n";

    DoStandaloneFunctionTests();
    DoMemberFunctionTests( u1 );
	DoExceptionTests();

#if defined(__BORLANDC__) || defined(_MSC_VER)
    system("PAUSE");
#endif

}
