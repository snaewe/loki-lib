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

#ifndef SINGLETON_INC_
#define SINGLETON_INC_

#include "Threads.h"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <new>

namespace Loki
{
    namespace Private
    {
        template< typename T >
        struct MSVCNeverTrue
        {
            enum { value = false };
        };

        template <class MetaFunctionWrapper, class T>
        struct Apply
        {
        #ifdef _MSC_VER
        // based on the (non-conforming) MSVC trick from MPL

        template<bool>
        struct MetaFunctionWrapper_VC : MetaFunctionWrapper {};

        //illegal C++ which causes VC to admit that MetaFunctionWrapper_VC
        //can have a nested template:

        template<>
        struct MetaFunctionWrapper_VC<true>
        {template<class> struct VolatileType; };

        typedef typename MetaFunctionWrapper_VC<
        MSVCNeverTrue<MetaFunctionWrapper>::value
        >::template VolatileType<T>::Result Result;
        #else
        typedef typename MetaFunctionWrapper::template VolatileType<T>::Result Result;
        #endif
        };

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

        void AtExitFn(); // declaration needed below
    
    } // namespace Private

////////////////////////////////////////////////////////////////////////////////
// function template SetLongevity
// Assigns an object a longevity; ensures ordered destructions of objects 
//     registered thusly during the exit sequence of the application
////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename Destroyer>
    void SetLongevity(T* pDynObject, unsigned int longevity,
        Destroyer d = Private::Deleter<T>::Delete)
    {
        using namespace Private;
        
        TrackerArray pNewArray = static_cast<TrackerArray>(
                realloc(pTrackerArray, 
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
        atexit(Private::AtExitFn);
    }

////////////////////////////////////////////////////////////////////////////////
// class template CreateUsingNew
// Implementation of the CreationPolicy used by SingletonHolder
// Creates objects using a straight call to the new operator 
////////////////////////////////////////////////////////////////////////////////

    struct CreateUsingNew
    {
        template <class T>
        static T* Create(T*)
        { return new T; }

        template <class T>
        static void Destroy(T* p)
        { delete p; }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template CreateUsingNew
// Implementation of the CreationPolicy used by SingletonHolder
// Creates objects using a call to std::malloc, followed by a call to the 
//     placement new operator
////////////////////////////////////////////////////////////////////////////////

    struct CreateUsingMalloc
    {

        template <class T>
        static T* Create(T*)
        {
            void* p = malloc(sizeof(T));
            if (!p) return 0;
            return new(p) T;
        }

        template <class T>
        static void Destroy(T* p)
        {
            p->~T();
            free(p);
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

    struct CreateStatic
    {
        template <class T>
        static T* Create(T*)
        {
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

            static MaxAlign staticMemory_;
            return new(&staticMemory_) T;
        }

        template <class T>
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

    struct DefaultLifetime
    {
        template <class T>
        static void ScheduleDestruction(T*, void (*pFun)())
        { atexit(pFun); }

        template <class T>
        static void OnDeadReference(T*)
        { throw std::exception("Dead Reference Detected"); }
    };

////////////////////////////////////////////////////////////////////////////////
// class template PhoenixSingleton
// Implementation of the LifetimePolicy used by SingletonHolder
// Schedules an object's destruction as per C++ rules, and it allows object 
//    recreation by not throwing an exception from OnDeadReference
////////////////////////////////////////////////////////////////////////////////

    class PhoenixSingleton
    {
    public:
        template <class T>
        static void ScheduleDestruction(T*, void (*pFun)())
        {
#ifndef ATEXIT_FIXED
            if (!destroyedOnce_)
#endif
                atexit(pFun);
        }

        template <class T>
        static void OnDeadReference(T*)
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
    bool PhoenixSingleton::destroyedOnce_ = false;
#endif
        
////////////////////////////////////////////////////////////////////////////////
// class template Adapter
// Helper for SingletonWithLongevity below
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class T>
        struct Adapter
        {
            void operator()(T*) { pFun_(); }
            void (*pFun_)();
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template SingletonWithLongevity
// Implementation of the LifetimePolicy used by SingletonHolder
// Schedules an object's destruction in order of their longevities
// Assumes a visible function GetLongevity(T*) that returns the longevity of the
//     object
////////////////////////////////////////////////////////////////////////////////

    class SingletonWithLongevity
    {
    public:
        template <class T>
        static void ScheduleDestruction(T* pObj, void (*pFun)())
        {
            Private::Adapter<T> adapter = { pFun };
            SetLongevity(pObj, GetLongevity(pObj), adapter);
        }

        template <class T>
        static void OnDeadReference(T*)
        { throw std::exception("Dead Reference Detected"); }
    };

////////////////////////////////////////////////////////////////////////////////
// class template NoDestroy
// Implementation of the LifetimePolicy used by SingletonHolder
// Never destroys the object
////////////////////////////////////////////////////////////////////////////////

    struct NoDestroy
    {
        template <class T>
        static void ScheduleDestruction(T*, void (*)())
        {}

        template <class T>
        static void OnDeadReference(T*)
        {}
    };

////////////////////////////////////////////////////////////////////////////////
// class template SingletonHolder
// Provides Singleton amenities for a type T
// To protect that type from spurious instantiations, you have to protect it
//     yourself.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class CreationPolicy = CreateUsingNew,
        class LifetimePolicy = DefaultLifetime,
        class ThreadingModel = SingleThreaded
    >
    class SingletonHolder
    {
    public:
        static T& Instance();
        
    private:
        // Helpers
        static void MakeInstance();
        static void DestroySingleton();
        
        // Protection
        SingletonHolder();
        
        // Data
        typedef typename Private::Apply<ThreadingModel,T*>::Result PtrInstanceType;

//        typedef typename ThreadingModel::VolatileType<T*>::Result PtrInstanceType;
        static PtrInstanceType pInstance_;
        static bool destroyed_;
    };
    
////////////////////////////////////////////////////////////////////////////////
// SingletonHolder's data
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        class C,
        class L,
        class M
    >
    typename SingletonHolder<T, C, L, M>::PtrInstanceType
        SingletonHolder<T, C, L, M>::pInstance_;

    template
    <
        class T,
        class C,
        class L,
        class M
    >
    bool SingletonHolder<T, C, L, M>::destroyed_;

////////////////////////////////////////////////////////////////////////////////
// SingletonHolder::Instance
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        class CreationPolicy,
        class LifetimePolicy,
        class ThreadingModel
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
        class CreationPolicy,
        class LifetimePolicy,
        class ThreadingModel
    >
    void SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel>::MakeInstance()
    {
        typename ThreadingModel::Lock<T> guard;
        (void)guard;
        
        if (!pInstance_)
        {
            if (destroyed_)
            {
                T* dummy;

                LifetimePolicy::OnDeadReference(dummy);
                destroyed_ = false;
            }

            T* dummy;

            pInstance_ = CreationPolicy::Create(dummy);
            LifetimePolicy::ScheduleDestruction(pInstance_, 
                &DestroySingleton);
        }
    }

    template
    <
        class T,
        class CreationPolicy,
        class L,
        class M
    >
    void SingletonHolder<T, CreationPolicy, L, M>::DestroySingleton()
    {
        assert(!destroyed_);
        CreationPolicy::Destroy(pInstance_);
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
// October 12, 2002: Ported to MSVC 6 by Terje Slettebø.
//      Interface for SingletonHolder changed from using template template
//      parameters, to using classes with member templates.
////////////////////////////////////////////////////////////////////////////////

#endif // SINGLETON_INC_
