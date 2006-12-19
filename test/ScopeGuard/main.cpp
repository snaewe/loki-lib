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

// $Id$


#include <loki/ScopeGuard.h>

#include <vector>
#include <string>
#include <iostream>
#include <assert.h>


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
   
    unsigned int fCount;

private:
	void CheckIfValid( const char * function, unsigned int line ) const;

    typedef std::vector<User*> UserCont;
    UserCont friends_;
    UserDatabase* pDB_;
};

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
	::Loki::ScopeGuard invariantGuard = ::Loki::MakeObjGuard( *this, &User::CheckIfValid, __FUNCTION__, __LINE__);
	(void)invariantGuard;
    friends_.push_back(&newFriend);
    fCount++;
    pDB_->AddFriend(GetName(), newFriend.GetName());
}

void User::AddFriendGuarded(User& newFriend)
{
	::Loki::ScopeGuard invariantGuard = ::Loki::MakeObjGuard( *this, &User::CheckIfValid, __FUNCTION__, __LINE__);
	(void)invariantGuard;

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
	::Loki::ScopeGuard invariantGuard = ::Loki::MakeObjGuard( *this, &User::CheckIfValid, __FUNCTION__, __LINE__);
	(void)invariantGuard;
    LOKI_ON_BLOCK_EXIT_OBJ(friends_, &UserCont::pop_back); (void) LOKI_ANONYMOUS_VARIABLE(scopeGuard);
    LOKI_ON_BLOCK_EXIT(Decrement, Loki::ByRef(fCount)); (void) LOKI_ANONYMOUS_VARIABLE(scopeGuard);
}


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

#if defined(__BORLANDC__) || defined(_MSC_VER)
    system("PAUSE");
#endif

}
