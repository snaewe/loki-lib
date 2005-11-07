////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Richard Sposato
// Copyright (c) 2005 Peter Kümmel
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

// define DO_EXTRA_LOKI_TESTS in src/SmallObj.cpp to get 
// a message when a SmallObject is created/deleted.

using namespace std;

// ----------------------------------------------------------------------------
//
//            LongevityLifetime Parent/Child policies
//
// ----------------------------------------------------------------------------

typedef Loki::SmallValueObject< LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
    LOKI_DEFAULT_CHUNK_SIZE, LOKI_MAX_SMALL_OBJECT_SIZE,
    LOKI_DEFAULT_OBJECT_ALIGNMENT, 
    Loki::LongevityLifetime::DieAsSmallObjectParent
>
SmallObjectParent;

class SmallObjectChild : public SmallObjectParent
{
public:

    typedef Loki::SingletonHolder< SmallObjectChild, Loki::CreateUsingNew,
        Loki::LongevityLifetime::DieAsSmallObjectChild,
        LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL >
        MySmallSingleton;

    /// Returns reference to the singleton.
    inline static SmallObjectChild & Instance( void )
    {
        return MySmallSingleton::Instance();
    }

    SmallObjectChild( void )
    {
        cout << "SmallObjectChild created" << endl;
    }
    ~SmallObjectChild( void )
    {
        cout << "~SmallObjectChild" << endl;
    }
    void DoThat( void )
    {
        cout << "SmallObjectChild::DoThat" << endl << endl;
    }
private:
    char m_stuff[ 16 ];
};


// ----------------------------------------------------------------------------
//
//            SingletonWithLongevity policy
//
// ----------------------------------------------------------------------------

typedef Loki::SmallValueObject< LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
    LOKI_DEFAULT_CHUNK_SIZE, LOKI_MAX_SMALL_OBJECT_SIZE,
    LOKI_DEFAULT_OBJECT_ALIGNMENT, 
    Loki::SingletonWithLongevity 
>
LongLivedObject;

class LongLivedSingleton : public LongLivedObject
{
public:

    typedef Loki::SingletonHolder< LongLivedSingleton, Loki::CreateUsingNew,
        Loki::SingletonWithLongevity, 
        LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL >
        MySmallSingleton;

    /// Returns reference to the singleton.
    inline static LongLivedSingleton & Instance( void )
    {
        return MySmallSingleton::Instance();
    }

    LongLivedSingleton( void )
    {
        cout << "LongLivedSingleton created" << endl;
    }
    ~LongLivedSingleton( void )
    {
        cout << "~LongLivedSingleton" << endl;
    }
    void DoThat( void )
    {
        cout << "LongLivedSingleton::DoThat" << endl << endl;
    }
private:
    char m_stuff[ 16 ];
};

inline unsigned int GetLongevity( LongLivedSingleton * )
{
    /// @note Must return a longevity level lower than the one in SmallObj.h.
    return 1;
}

#if !defined(_MSC_VER) || (_MSC_VER>=1400)

// ----------------------------------------------------------------------------
//
//            FollowIntoDeath policy
//
// ----------------------------------------------------------------------------

typedef Loki::SmallValueObject< LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
    LOKI_DEFAULT_CHUNK_SIZE, LOKI_MAX_SMALL_OBJECT_SIZE,
    LOKI_DEFAULT_OBJECT_ALIGNMENT, 
    Loki::FollowIntoDeath::With<Loki::DefaultLifetime>::AsMasterLifetime 
>
MasterObject;


class FollowerSingleton : public MasterObject
{
public:

    typedef Loki::SingletonHolder< FollowerSingleton, Loki::CreateUsingNew,
        Loki::FollowIntoDeath::AfterMaster<FollowerSingleton::ObjAllocatorSingleton>::IsDestroyed,
        LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL >
        MySmallSingleton;

    /// Returns reference to the singleton.
    inline static FollowerSingleton & Instance( void )
    {
        return MySmallSingleton::Instance();
    }

    FollowerSingleton( void )
    {
        cout << "FollowerSingleton created" << endl;
    }
    ~FollowerSingleton( void )
    {
        cout << "~FollowerSingleton" << endl;
    }
    void DoThat( void )
    {
        cout << "FollowerSingleton::DoThat" << endl << endl;
    }
private:
    char m_stuff[ 16 ];
};


#endif

// ----------------------------------------------------------------------------
//
//    detect memory leaks on MSVC Ide
//
// ----------------------------------------------------------------------------

#ifdef _MSC_VER

#include <crtdbg.h>
#include <cassert>

void heap_debug()
{
    int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

    // Turn on leak-checking bit
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

    //tmpFlag |= _CRTDBG_CHECK_MasterLWMasterYS_DF;

    // Turn off CRT block checking bit
    tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

    // Set flag to the new value
    _CrtSetDbgFlag( tmpFlag );

}
#endif


// ----------------------------------------------------------------------------
//
//    main
//
// ----------------------------------------------------------------------------

int main()
{

#ifdef _MSC_VER
    heap_debug();
#endif

    cout << endl
         << "This program tests the three recommended lifetime policies for Loki's " << endl
         << "Small-Object Allocator and singleton objects that are derived " << endl
         << "from SmallObject or SmallValueObject." << endl 
         << endl
         << "Use one of the following lifetime policies" << endl
         << "to manage the lifetime dependency:" << endl
         << endl 
         << "1. LongevityLifetime Parent/Child" << endl
         << "   SmallObject has  " << endl
         << "       LongevityLifetime::DieAsSmallObjectParent policy " << endl
         << "   and the derived Singleton has " << endl
         << "       LongevityLifetime::DieAsSmallObjectChild" << endl
         << "   This is tested by the SmallObjectChild class." << endl
         << endl
         << "2. Both SmallObject and derived Singleton use SingletonWithLongevity policy." << endl
         << "   This is tested by the LongLivedSingleton class." << endl
         << endl
#if !defined(_MSC_VER) || (_MSC_VER>=1400)         
         << "3. FollowIntoDeath:" << endl
         << "   SmallObject has  " << endl
         << "       FollowIntoDeath::With<LIFETIME>::AsMasterLiftime policy" << endl
         << "   and the derived Singleton has " << endl
         << "       FollowIntoDeath::AfterMaster<MASTERSINGLETON>::IsDestroyed policy" << endl
         << "   This is tested by the FollowerSingleton class." << endl

         << endl
#endif
         << endl << endl
         << "If this program executes without crashing or asserting" << endl
         << "at exit time, then all policies work." << endl << endl;
    
    
    SmallObjectChild::Instance().DoThat();
    LongLivedSingleton::Instance().DoThat();
    
#if !defined(_MSC_VER) || (_MSC_VER>=1400)         
    FollowerSingleton::Instance().DoThat();
#endif
    
#if defined(__BORLANDC__) || defined(__GNUC__) || defined(_MSC_VER)
    system("pause"); 
#endif

    cout << endl<< endl << "now leaving main" << endl << endl;

    return 0;
}

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.9  2005/11/07 12:06:43  syntheticpp
// change lifetime policy DieOrder to a msvc7.1 compilable version. Make this the default lifetime for SmallObject
//
// Revision 1.8  2005/11/05 17:43:55  syntheticpp
// disable FollowIntoDeath/DieOrder lifetime policies when using the msvc 7.1 compiler, bug article: 839821 'Microsoft has confirmed that this is a problem..'
//
// Revision 1.7  2005/11/02 15:00:38  syntheticpp
// use new singleton lifetime policies
//
// Revision 1.6  2005/11/02 14:15:44  syntheticpp
// use new singleton lifetime policies
//
// Revision 1.5  2005/11/02 14:11:18  syntheticpp
// use new singleton lifetime policies
//
// Revision 1.4  2005/11/02 13:58:18  syntheticpp
// use new singleton lifetime policies
//
// Revision 1.3  2005/10/30 14:03:23  syntheticpp
// replace tabs space
//
// Revision 1.2  2005/10/29 10:21:46  syntheticpp
// find loki include files without a correct sreach pathand some small fixes
//
// Revision 1.1  2005/10/14 18:48:10  rich_sposato
// Adding SmallSingleton test project to CVS.
//
