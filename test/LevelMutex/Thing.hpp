////////////////////////////////////////////////////////////////////////////////
//
// Part of LevelMutex test program for The Loki Library
// Copyright (c) 2008 Richard Sposato
// The copyright on this file is protected under the terms of the MIT license.
//
// Permission to use, copy, modify, distribute and sell this software for any 
// purpose is hereby granted without fee, provided that the above copyright 
// notice appear in all copies and that both that copyright notice and this 
// permission notice appear in supporting documentation.
//
// The author makes no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
//
////////////////////////////////////////////////////////////////////////////////

// $Id$


// ----------------------------------------------------------------------------

#ifndef LOKI_TEST_LEVEL_MUTEX_THING_H
#define LOKI_TEST_LEVEL_MUTEX_THING_H

#include <LevelMutex.h>
#include <Allocator.h>

#include <vector>


// ----------------------------------------------------------------------------

void GoToSleep( unsigned int milliSeconds );

// ----------------------------------------------------------------------------

class ExceptionTossingMutex : public ::Loki::SleepLevelMutex
{
public:

    enum ThrowingPolicy
    {
        Never,
        Always,
        Randomly
    };

    ExceptionTossingMutex( unsigned int level );

    virtual ~ExceptionTossingMutex( void );

    virtual ::Loki::MutexErrors::Type Lock( void ) volatile;

    virtual ::Loki::MutexErrors::Type TryLock( void ) volatile;

    virtual ::Loki::MutexErrors::Type Unlock( void ) volatile;

    inline ThrowingPolicy GetTossPolicy( void ) const { return m_policy; }

    inline void SetTossPolicy( ThrowingPolicy policy ) { m_policy = policy; }

private:

    ExceptionTossingMutex( void );
    ExceptionTossingMutex( const ExceptionTossingMutex & );
    ExceptionTossingMutex & operator = ( const ExceptionTossingMutex & );

    ThrowingPolicy m_policy;
};

typedef ::Loki::LevelMutex< ExceptionTossingMutex, 1,
    ::Loki::JustReturnMutexError, ::Loki::NoMutexWait > ExceptionMutex;

typedef ::Loki::LevelMutex< ::Loki::SleepLevelMutex, 1,
    ::Loki::JustReturnMutexError, ::Loki::MutexSleepWaits > SleepMutex;


// ----------------------------------------------------------------------------

class TestResults
{
public:

    static bool Create( unsigned int threadCount );

    static void Destroy( void );

    inline static TestResults * GetIt( void ) { return s_instance; }

    void Reset( unsigned int threadCount );

    void SetResult( unsigned int threadIndex, unsigned int total,
        unsigned int fails );

    void OutputResults( void );

private:

    struct TestResult
    {
        TestResult( void );
        TestResult( const TestResult & that );
        ~TestResult( void );
        unsigned int m_total;
        unsigned int m_fails;
    };

    typedef ::std::vector< TestResult, ::Loki::LokiAllocator< TestResult > > Results;

    explicit TestResults( unsigned int threadCount );

    ~TestResults( void );

    TestResults( void );
    TestResults( const TestResults & );
    TestResults & operator = ( const TestResults & );

    static TestResults * s_instance;

    Results m_results;
};

// ----------------------------------------------------------------------------

class Thing
{
public:

    typedef ::std::vector< volatile Thing * > ThingPool;

    static volatile Thing & GetIt( void ) { return *s_thing; }

    static void Init( unsigned int value );

    static void Destroy( void );

    static volatile Thing * GetFromPool( unsigned int index );

    static bool MakePool( unsigned int count );

    static unsigned int GetPoolSize( void );

    static void DestroyPool( void );

    void Print( unsigned int value, unsigned int index, unsigned int startSize ) const volatile;

    void Print( unsigned int value, unsigned int index, unsigned int startSize ) const;

    unsigned int GetValue( void ) const volatile { return m_value; }

    unsigned int GetValue( void ) const { return m_value; }

    void SetValue( unsigned int value ) volatile;

    void SetValue( unsigned int value ) { m_value = value; }

    inline volatile SleepMutex & GetMutex( void ) volatile { return m_mutex; }

    inline const volatile SleepMutex & GetMutex( void ) const volatile { return m_mutex; }

private:

    explicit Thing( unsigned int value );

    ~Thing( void );

    Thing( void );
    Thing( const Thing & );
    Thing & operator = ( const Thing & );

    static volatile Thing * s_thing;

    static ThingPool s_pool;

    static TestResults s_results;

    mutable volatile SleepMutex m_mutex;
    unsigned int m_value;
};

typedef ::std::vector< Thing * > UnsafeThingPool;

// ----------------------------------------------------------------------------

template < class Thingy >
class Unlocker
{
public:
    explicit inline Unlocker( volatile Thingy * thing ) : m_thing( thing ) {}
    inline Unlocker( const Unlocker & that ) : m_thing( that.m_thing )
    {
        const_cast< Unlocker & >( that ).m_thing = NULL;
    }
    inline ~Unlocker( void ) { Unlock(); }
    inline void Unlock( void )
    {
        if ( NULL != m_thing )
            m_thing->UnlockHierarchy();
    }
private:
    volatile Thingy * m_thing;
};

// ----------------------------------------------------------------------------

class LevelThing
{
public:

    typedef Unlocker< LevelThing > Unlocker;

    static volatile LevelThing * GetFromPool( unsigned int index );

    static bool MakePool( unsigned int count );

    static void DestroyPool( void );

    Unlocker LockHierarchy( void ) volatile;

    void UnlockHierarchy( void ) volatile;

    void SetValue( unsigned int value ) volatile;

    void SetValue( unsigned int value );

    inline unsigned int GetValue( void ) const volatile { return m_value; }

    inline unsigned int GetValue( void ) const { return m_value; }

    bool DoValuesMatch( unsigned int value ) const volatile;

    bool DoValuesMatch( unsigned int value ) const;

    inline volatile ::Loki::LevelMutexInfo & GetMutex( void ) volatile { return m_mutex; }

    inline const volatile ::Loki::LevelMutexInfo & GetMutex( void ) const volatile { return m_mutex; }

private:

    typedef ::std::vector< volatile LevelThing * > LevelThingPool;

    LevelThing( unsigned int level, unsigned int place );

    ~LevelThing( void );

    LevelThing( void );
    LevelThing( const LevelThing & );
    LevelThing & operator = ( const LevelThing & );

    static LevelThingPool s_pool;

    mutable volatile SleepMutex m_mutex;
    const unsigned int m_place;
    unsigned int m_value;
};

// ----------------------------------------------------------------------------

class SomeThing
{
public:

    SomeThing( unsigned int level, unsigned int place );

    ~SomeThing( void );

    inline unsigned int GetLevel( void ) const volatile { return m_level; }

    void SetValue( unsigned int value ) volatile;

    void SetValue( unsigned int value );

    inline unsigned int GetValue( void ) const volatile { return m_value; }

    inline unsigned int GetValue( void ) const { return m_value; }

    inline volatile ::Loki::LevelMutexInfo & GetMutex( void ) volatile { return m_mutex; }

    inline const volatile ::Loki::LevelMutexInfo & GetMutex( void ) const volatile { return m_mutex; }

private:

    SomeThing( void );
    SomeThing( const SomeThing & );
    SomeThing & operator = ( const SomeThing & );

    mutable volatile SleepMutex m_mutex;
    const unsigned int m_place;
    const unsigned int m_level;
    unsigned int m_value;
};

typedef ::std::vector< volatile SomeThing * > SomeThingPool;

typedef SomeThingPool::iterator SomeThingPoolIter;

// ----------------------------------------------------------------------------

class ManyThingsPool
{
public:

    explicit ManyThingsPool( unsigned int level, unsigned int count );

    ~ManyThingsPool( void );

    volatile SomeThing * GetFromPool( unsigned int index );

    unsigned int GetCount( void ) const;

private:

    ManyThingsPool( void );
    ManyThingsPool( const ManyThingsPool & );
    ManyThingsPool & operator = ( const ManyThingsPool & );

    SomeThingPool m_pool;
};

// ----------------------------------------------------------------------------

class MultiLevelPool
{
public:

    static void MakePool( unsigned int count, unsigned int thingCount );

    static void DestroyPool( void );

    static ManyThingsPool * GetFromPool( unsigned int index );

    static unsigned int GetCount( void );

private:

    typedef ::std::vector< ManyThingsPool * > MultiThingPool;

    ~MultiLevelPool( void );
    MultiLevelPool( void );
    MultiLevelPool( const MultiLevelPool & );
    MultiLevelPool & operator = ( const MultiLevelPool & );

    static MultiThingPool s_pool;
};

// ----------------------------------------------------------------------------

void CheckForMatchingValues( unsigned int & failCount, unsigned int & testCount,
    unsigned int value, const SomeThingPool & pool );

void CheckForMatchingValues( unsigned int & failCount, unsigned int & testCount,
    unsigned int value, const SomeThingPool & pool, bool locked );

void MakePool( SomeThingPool & pool );

void LockThese( SomeThingPool & pool );

void UnlockThese( SomeThingPool & pool );

unsigned int CountLockedByThisThread( const SomeThingPool & pool );

// ----------------------------------------------------------------------------

#endif
