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

// $Header:



#include <loki/ScopeGuard.h>
#include <vector>
#include <string>
#include <iostream>

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
	User(UserDatabase* db) : pDB_(db){}

	std::string GetName();

    void AddFriend(User& newFriend);
	void AddFriendGuarded(User& newFriend);

	size_t countFriends();

private:
	typedef std::vector<User*> UserCont;
    UserCont friends_;
    UserDatabase* pDB_;
};

std::string User::GetName()
{
	return "A name";
}

size_t User::countFriends()
{
	return friends_.size();
}

void User::AddFriend(User& newFriend)
{
    friends_.push_back(&newFriend);
    pDB_->AddFriend(GetName(), newFriend.GetName());
}

void User::AddFriendGuarded(User& newFriend)
{
    friends_.push_back(&newFriend);
	Loki::ScopeGuard guard = Loki::MakeObjGuard(friends_, &UserCont::pop_back);
    pDB_->AddFriend(GetName(), newFriend.GetName());
    guard.Dismiss();
}


int main()
{
	UserDatabase db;

	User u1(&db);
	User u2(&db);

	try{ u1.AddFriend(u2); }
    catch (...){}
	std::cout << "u1 countFriends: " << u1.countFriends() << "\n";

	try{ u2.AddFriendGuarded(u1); }
    catch (...){}
	std::cout << "u2 countFriends: " << u2.countFriends() << "\n";
}
