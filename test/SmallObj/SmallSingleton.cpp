////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Richard Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The authors make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$


#include "../../include/loki/SmallObj.h"
#include "../../include/loki/Singleton.h"
#include <iostream>


using namespace std;


// ----------------------------------------------------------------------------

typedef Loki::SmallValueObject< LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
    LOKI_DEFAULT_CHUNK_SIZE, LOKI_MAX_SMALL_OBJECT_SIZE,
    LOKI_DEFAULT_OBJECT_ALIGNMENT, Loki::SingletonWithLongevity >
LongLivedObject;

typedef Loki::SmallValueObject< LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
    LOKI_DEFAULT_CHUNK_SIZE, LOKI_MAX_SMALL_OBJECT_SIZE,
    LOKI_DEFAULT_OBJECT_ALIGNMENT, Loki::NoDestroy >
ImmortalObject;

// ----------------------------------------------------------------------------

class LongLivedSingleton : public LongLivedObject
{
public:

    typedef Loki::SingletonHolder< LongLivedSingleton, Loki::CreateUsingNew,
        Loki::SingletonWithLongevity, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL >
        MySmallSingleton;

    /// Returns reference to the singleton.
    inline static LongLivedSingleton & Instance( void )
    {
        return MySmallSingleton::Instance();
    }

    LongLivedSingleton( void )
    {
        cout << "LongLivedSingleton" << endl;
    }
    ~LongLivedSingleton( void )
    {
        cout << "~LongLivedSingleton" << endl;
    }
    void DoThat( void )
    {
        cout << "LongLivedSingleton::DoThat" << endl;
    }
private:
    char m_stuff[ 16 ];
};

// ----------------------------------------------------------------------------

inline unsigned int GetLongevity( LongLivedSingleton * )
{
    /// @note Must return a longevity level lower than the one in SmallObj.h.
    return 1;
}

// ----------------------------------------------------------------------------

class ImmortalSingleton : public ImmortalObject
{
public:

    typedef Loki::SingletonHolder< ImmortalSingleton, Loki::CreateUsingNew,
        Loki::NoDestroy, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL >
        MySmallSingleton;

    /// Returns reference to the singleton.
    inline static ImmortalSingleton & Instance( void )
    {
        return MySmallSingleton::Instance();
    }

    ImmortalSingleton( void )
    {
        cout << "ImmortalSingleton" << endl;
    }
    ~ImmortalSingleton( void )
    {
        cout << "~ImmortalSingleton destructor should never get called!" << endl;
    }
    void DoThat( void )
    {
        cout << "ImmortalSingleton::DoThat" << endl;
    }
private:
    char m_stuff[ 16 ];
};

// ----------------------------------------------------------------------------

class MortalSingleton : public ImmortalObject
{
public:

    typedef Loki::SingletonHolder< MortalSingleton, Loki::CreateUsingNew,
        Loki::SingletonWithLongevity, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL >
        MySmallSingleton;

    /// Returns reference to the singleton.
    inline static MortalSingleton & Instance( void )
    {
        return MySmallSingleton::Instance();
    }

    MortalSingleton( void )
    {
        cout << "MortalSingleton" << endl;
    }
    ~MortalSingleton( void )
    {
        cout << "~MortalSingleton" << endl;
    }
    void DoThat( void )
    {
        cout << "MortalSingleton::DoThat" << endl;
    }
private:
    char m_stuff[ 16 ];
};

// ----------------------------------------------------------------------------

inline unsigned int GetLongevity( MortalSingleton * )
{
    /// @note Must return a longevity level lower than the one in SmallObj.h.
    return 1;
}

// ----------------------------------------------------------------------------

int main()
{
    cout << "This program tests the three recommended combinations of singleton" << endl
         << "lifetime policies for Loki's Small-Object Allocator and singleton" << endl
         << "objects that are derived from SmallObject or SmallValueObject." << endl << endl
         << "Those 3 recommended combinations are:" << endl
         << "\t1. Both SmallObject and derived Singleton use NoDestroy policy." << endl
         << "\t   This is tested by the ImmortalSingleton class." << endl
         << "\t2. Both use SingletonWithLongevity policy." << endl
         << "\t   This is tested by the LongLivedSingleton class." << endl
         << "\t3. SmallObject has NoDestroy policy but the derived Singleton has" << endl
         << "\t   SingletonWithLongevity policy." << endl
         << "\t   This is tested by the MortalSingleton class." << endl << endl
         << "If this program executes without crashing or asserting at exit time," << endl
         << "then all 3 combinations work." << endl << endl;
    MortalSingleton::Instance().DoThat();
    LongLivedSingleton::Instance().DoThat();
    ImmortalSingleton::Instance().DoThat();
	system("PAUSE");
	return 0;
}

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.2  2005/10/29 10:21:46  syntheticpp
// find loki include files without a correct sreach pathand some small fixes
//
// Revision 1.1  2005/10/14 18:48:10  rich_sposato
// Adding SmallSingleton test project to CVS.
//
