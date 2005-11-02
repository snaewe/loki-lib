////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef LOKI_SINGLETON_INC_
#define LOKI_SINGLETON_INC_

#include "Threads.h"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <new>
#include <vector>

#ifdef _MSC_VER
#define LOKI_C_CALLING_CONVENTION_QUALIFIER __cdecl 
#else
#define LOKI_C_CALLING_CONVENTION_QUALIFIER 
#endif


namespace Loki
{
    typedef void (LOKI_C_CALLING_CONVENTION_QUALIFIER *atexit_pfn_t)();

    namespace Private
    {
        ////////////////////////////////////////////////////////////////////////////////
        // class LifetimeTracker
        // Helper class for SetLongevity
        ////////////////////////////////////////////////////////////////////////////////

        class LifetimeTracker
        {
        public:
            LifetimeTracker(unsigned int x) : longevity_(x) 
            {}
            
            virtual ~LifetimeTracker() = 0;
            
            static bool Compare(const LifetimeTracker* lhs,
                const LifetimeTracker* rhs)
            {
                return lhs->longevity_ > rhs->longevity_;
            }
            
        private:
            unsigned int longevity_;
        };
        
        // Definition required
        inline LifetimeTracker::~LifetimeTracker() {} 
        
        // Helper data
        typedef LifetimeTracker** TrackerArray;
        extern TrackerArray pTrackerArray;
        extern unsigned int elements;

        // Helper destroyer function
        template <typename T>
        struct Deleter
        {
            typedef void (*Type)(T*);
            static void Delete(T* pObj)
            { delete pObj; }
        };

        // Concrete lifetime tracker for objects of type T
        template <typename T, typename Destroyer>
        class ConcreteLifetimeTracker : public LifetimeTracker
        {
        public:
            ConcreteLifetimeTracker(T* p,unsigned int longevity, Destroyer d)
                : LifetimeTracker(longevity)
                , pTracked_(p)
                , destroyer_(d)
            {}
            
            ~ConcreteLifetimeTracker()
            { destroyer_(pTracked_); }
            
        private:
            T* pTracked_;
            Destroyer destroyer_;
        };

        void LOKI_C_CALLING_CONVENTION_QUALIFIER AtExitFn(); // declaration needed below   
         
    } // namespace Private

    ////////////////////////////////////////////////////////////////////////////////
    // function template SetLongevity
    // Assigns an object a longevity; ensures ordered destructions of objects 
    //     registered thusly during the exit sequence of the application
    ////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename Destroyer>
    void SetLongevity(T* pDynObject, unsigned int longevity,
        Destroyer d)
    {
        using namespace Private;
        
        TrackerArray pNewArray = static_cast<TrackerArray>(
                std::realloc(pTrackerArray, 
                    sizeof(*pTrackerArray) * (elements + 1)));
        if (!pNewArray) throw std::bad_alloc();
        
        // Delayed assignment for exception safety
        pTrackerArray = pNewArray;
        
        LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(
            pDynObject, longevity, d);
        
        // Insert a pointer to the object into the queue
        TrackerArray pos = std::upper_bound(
            pTrackerArray, 
            pTrackerArray + elements, 
            p, 
            LifetimeTracker::Compare);
        std::copy_backward(
            pos, 
            pTrackerArray + elements,
            pTrackerArray + elements + 1);
        *pos = p;
        ++elements;
        
        // Register a call to AtExitFn
        std::atexit(Private::AtExitFn);
    }

    template <typename T>
    void SetLongevity(T* pDynObject, unsigned int longevity,
        typename Private::Deleter<T>::Type d = Private::Deleter<T>::Delete)
    {
        SetLongevity<T, typename Private::Deleter<T>::Type>(pDynObject, longevity, d);
    }
        
    ////////////////////////////////////////////////////////////////////////////////
    // class template CreateUsingNew
    // Implementation of the CreationPolicy used by SingletonHolder
    // Creates objects using a straight call to the new operator 
    ////////////////////////////////////////////////////////////////////////////////

    template <class T> struct CreateUsingNew
    {
        static T* Create()
        { return new T; }
        
        static void Destroy(T* p)
        { delete p; }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // class template CreateUsingNew
    // Implementation of the CreationPolicy used by SingletonHolder
    // Creates objects using a call to std::malloc, followed by a call to the 
    //     placement new operator
    ////////////////////////////////////////////////////////////////////////////////

    template <class T> struct CreateUsingMalloc
    {
        static T* Create()
        {
            void* p = std::malloc(sizeof(T));
            if (!p) return 0;
            return new(p) T;
        }
        
        static void Destroy(T* p)
        {
            p->~T();
            std::free(p);
        }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // class template CreateStatic
    // Implementation of the CreationPolicy used by SingletonHolder
    // Creates an object in static memory
    // Implementation is slightly nonportable because it uses the MaxAlign trick 
    //     (an union of all types to ensure proper memory alignment). This trick is 
    //     nonportable in theory but highly portable in practice.
    ////////////////////////////////////////////////////////////////////////////////

    template <class T> struct CreateStatic
    {
        
#ifdef _MSC_VER
#pragma warning( push ) 
#pragma warning( disable : 4121 )
// alignment of a member was sensitive to packing
#endif // _MSC_VER

        union MaxAlign
        {
            char t_[sizeof(T)];
            short int shortInt_;
            int int_;
            long int longInt_;
            float float_;
            double double_;
            long double longDouble_;
            struct Test;
            int Test::* pMember_;
            int (Test::*pMemberFn_)(int);
        };
        
#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER
        
        static T* Create()
        {
            static MaxAlign staticMemory_;
            return new(&staticMemory_) T;
        }
        
        static void Destroy(T* p)
        {
            p->~T();
        }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // class template DefaultLifetime
    // Implementation of the LifetimePolicy used by SingletonHolder
    // Schedules an object's destruction as per C++ rules
    // Forwards to std::atexit
    ////////////////////////////////////////////////////////////////////////////////

    template <class T>
    struct DefaultLifetime
    {
        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        { std::atexit(pFun); }
        
        static void OnDeadReference()
        { throw std::logic_error("Dead Reference Detected"); }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // class template PhoenixSingleton
    // Implementation of the LifetimePolicy used by SingletonHolder
    // Schedules an object's destruction as per C++ rules, and it allows object 
    //    recreation by not throwing an exception from OnDeadReference
    ////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class PhoenixSingleton
    {
    public:
        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        {
#ifndef ATEXIT_FIXED
            if (!destroyedOnce_)
#endif
                std::atexit(pFun);
        }
        
        static void OnDeadReference()
        {
#ifndef ATEXIT_FIXED
            destroyedOnce_ = true;
#endif
        }
        
    private:
#ifndef ATEXIT_FIXED
        static bool destroyedOnce_;
#endif
    };
    
#ifndef ATEXIT_FIXED
    template <class T> bool PhoenixSingleton<T>::destroyedOnce_ = false;
#endif

    ////////////////////////////////////////////////////////////////////////////////
    // class template DeletableSingleton
    // Copyright (c) 2004 by Curtis Krauskopf - curtis@decompile.com
    //
    // A DeletableSingleton allows the instantiated singleton to be 
    // destroyed at any time. The singleton can be reinstantiated at 
    // any time, even during program termination.
    // If the singleton exists when the program terminates, it will 
    // be automatically deleted.
    //
    // The singleton can be deleted manually:
    // DeletableSingleton<MyClass>::GracefulDelete();
    ////////////////////////////////////////////////////////////////////////////////
        
    template <class T>
    class DeletableSingleton
    {
    public:

        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        {
            static bool firstPass = true;
            isDead = false;
            deleter = pFun;
            if (firstPass || needCallback)
            {
                std::atexit(atexitCallback);
                firstPass = false;
                needCallback = false;
            }
        }
    
        static void OnDeadReference()
        { 
        }
    
        static void GracefulDelete()
        {
            if (isDead)
                return;
            isDead = true;
            deleter();
        }
    
    protected:
        static atexit_pfn_t deleter;
        static bool isDead;
        static bool needCallback;
        
        static void atexitCallback()
        {
#ifdef ATEXIT_FIXED
            needCallback = true;
#else
            needCallback = false;
#endif
            GracefulDelete();
        }
    };
    
    template <class T>
    atexit_pfn_t DeletableSingleton<T>::deleter = 0;
    
    template <class T>
    bool DeletableSingleton<T>::isDead = true;
    
    template <class T>
    bool DeletableSingleton<T>::needCallback = true;

    ////////////////////////////////////////////////////////////////////////////////
    // class template Adapter
    // Helper for SingletonWithLongevity below
    ////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class T>
        struct Adapter
        {
            void operator()(T*) { return pFun_(); }
            atexit_pfn_t pFun_;
        };
    }

    ////////////////////////////////////////////////////////////////////////////////
    // class template SingletonWithLongevity
    // Implementation of the LifetimePolicy used by SingletonHolder
    // Schedules an object's destruction in order of their longevities
    // Assumes a visible function GetLongevity(T*) that returns the longevity of the
    //     object
    ////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class SingletonWithLongevity
    {
    public:
        static void ScheduleDestruction(T* pObj, atexit_pfn_t pFun)
        {
            Private::Adapter<T> adapter = { pFun };
            SetLongevity(pObj, GetLongevity(pObj), adapter);
        }
        
        static void OnDeadReference()
        { throw std::logic_error("Dead Reference Detected"); }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // class template NoDestroy
    // Implementation of the LifetimePolicy used by SingletonHolder
    // Never destroys the object
    ////////////////////////////////////////////////////////////////////////////////

    template <class T>
    struct NoDestroy
    {
        static void ScheduleDestruction(T*, atexit_pfn_t)
        {}
        
        static void OnDeadReference()
        {}
    };

    
    ////////////////////////////////////////////////////////////////////////////////
    // DieOrder lifetime policy: 
    // Policy to handle lifetime dependencies
    ////////////////////////////////////////////////////////////////////////////////

    template <unsigned int Longevity, class T>
    class SingletonFixedLongevity
    {
    public:
        static void ScheduleDestruction(T* pObj, atexit_pfn_t pFun)
        {
            Private::Adapter<T> adapter = { pFun };
            SetLongevity(pObj, Longevity , adapter);
        }
        
        static void OnDeadReference()
        { throw std::logic_error("Dead Reference Detected"); }
    };

    struct DieOrder
    {
        template <class T>
        struct Last  : public SingletonFixedLongevity<0xFFFFFFFF ,T>
        {};
        
        template <class T>
        struct First : public SingletonFixedLongevity<0xFFFFFFFF-1,T>
        {};
    };

    ////////////////////////////////////////////////////////////////////////////////
    // FollowIntoDeath lifetime policy: followers will die after the master dies 
    // Followers will not die, if
    //      - master never dies (NoDestroy policy)
    //      - master never created
    //        - master dies not in the function registered with atexit
    //      - master dies not by a call of a the atexit registerd function
    //        (DeletableSingleton::GreathFullDeath)         
    //
    // Usage:
    // Lifetime of the master singleton e.g. with a Master class:
    //     SingletonHolder<Master,
    //            FollowIntoDeath::With<DefaultLifetime>::AsMasterLifetime>
    //       MasterSingleton;
    //
    // Lifetime of the follower singleton  e.g. with a Follower class:
    //      SingletonHolder<Follower,CreateUsingNew, 
    //            FollowIntoDeath::AfterMaster<MasterSingleton>::IsDestroyed>
    //        FollowerSingleton
    ////////////////////////////////////////////////////////////////////////////////

    struct FollowIntoDeath
    {
        template<class T>
        class Followers
        {
            typedef std::vector<atexit_pfn_t> Container;
            typedef typename Container::iterator iterator;
            static Container* followers_;
        
        public:
            static void Init()
            {
                static bool done = false;
                if(!done)
                {
                    followers_ = new Container;
                    done = true;
                }
            }

            static void AddFollower(atexit_pfn_t ae)
            {
                Init();
                followers_->push_back(ae);
            }

            static void DestroyFollowers()
            {
                Init();
                for(iterator it = followers_->begin();it != followers_->end();++it)
                    (*it)();    
                delete followers_;
            }
        };
            

        template<template <class> class Lifetime>
        struct With
        {
            template<class Master>
            struct AsMasterLifetime
            {
                static void ScheduleDestruction(Master* pObj, atexit_pfn_t pFun)
                {
                    Followers<Master>::Init();
                    Lifetime<Master>::ScheduleDestruction(pObj, pFun);

                    // use same policy for the followers and force a new 
                    // template instantiation,  this adds a additional atexit entry
                    // does not work with SetLonlevity, but there you can control
                    // the lifetime with the GetLongevity function.
                    Lifetime<Followers<Master> >::ScheduleDestruction(0,Followers<Master>::DestroyFollowers);
                }
            
                static void OnDeadReference()
                { 
                    throw std::logic_error("Dead Reference Detected"); 
                }
            };
        };

        template<class Master>
        struct AfterMaster
        {
            template<class F>
            struct IsDestroyed
            {
                static void ScheduleDestruction(F*, atexit_pfn_t pFun)
                {
                    Followers<Master>::AddFollower(pFun);
                }
        
                static void OnDeadReference()
                { 
                    throw std::logic_error("Dead Reference Detected"); 
                }
            };
        };
    };

    template<class T>
    typename FollowIntoDeath::Followers<T>::Container* 
    FollowIntoDeath::Followers<T>::followers_ = 0;


    ////////////////////////////////////////////////////////////////////////////////
    // class template SingletonHolder
    // Provides Singleton amenities for a type T
    // To protect that type from spurious instantiations, you have to protect it
    //     yourself.
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        template <class> class CreationPolicy = CreateUsingNew,
        template <class> class LifetimePolicy = DefaultLifetime,
        template <class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL
    >
    class SingletonHolder
    {
    public:
        static T& Instance();
        
    private:
        // Helpers
        static void MakeInstance();
        static void LOKI_C_CALLING_CONVENTION_QUALIFIER DestroySingleton();
        
        // Protection
        SingletonHolder();
        
        // Data
        typedef typename ThreadingModel<T*>::VolatileType PtrInstanceType;
        static PtrInstanceType pInstance_;
        static bool destroyed_;
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder's data
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class C,
        template <class> class L,
        template <class> class M
    >
    typename SingletonHolder<T, C, L, M>::PtrInstanceType
        SingletonHolder<T, C, L, M>::pInstance_;

    template
    <
        class T,
        template <class> class C,
        template <class> class L,
        template <class> class M
    >
    bool SingletonHolder<T, C, L, M>::destroyed_;

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::Instance
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy,
        template <class> class ThreadingModel
    >
    inline T& SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel>::Instance()
    {
        if (!pInstance_)
        {
            MakeInstance();
        }
        return *pInstance_;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::MakeInstance (helper for Instance)
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy,
        template <class> class ThreadingModel
    >
    void SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel>::MakeInstance()
    {
        typename ThreadingModel<SingletonHolder>::Lock guard;
        (void)guard;
        
        if (!pInstance_)
        {
            if (destroyed_)
            {
                destroyed_ = false;
                LifetimePolicy<T>::OnDeadReference();
            }
            pInstance_ = CreationPolicy<T>::Create();
            LifetimePolicy<T>::ScheduleDestruction(pInstance_, 
                &DestroySingleton);
        }
    }

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class L,
        template <class> class M
    >
    void LOKI_C_CALLING_CONVENTION_QUALIFIER 
    SingletonHolder<T, CreationPolicy, L, M>::DestroySingleton()
    {
        assert(!destroyed_);
        CreationPolicy<T>::Destroy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// May 21, 2001: Correct the volatile qualifier - credit due to Darin Adler
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// January 08, 2002: Fixed bug in call to realloc - credit due to Nigel Gent and
//      Eike Petersen
// March 08, 2002: moved the assignment to pTrackerArray in SetLongevity to fix
//      exception safety issue. Credit due to Kari Hoijarvi
// May 09, 2002: Fixed bug in Compare that caused longevities to act backwards.
//      Credit due to Scott McDonald.
////////////////////////////////////////////////////////////////////////////////

#endif // SINGLETON_INC_
