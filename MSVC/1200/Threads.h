#ifndef THREADS_H_
#define THREADS_H_

////////////////////////////////////////////////////////////////////////////////
// macro DEFAULT_THREADING
// Selects the default threading model for certain components of Loki
// If you don't define it, it defaults to single-threaded
// All classes in Loki have configurable threading model; DEFAULT_THREADING
// affects only default template arguments
////////////////////////////////////////////////////////////////////////////////

// Last update: June 20, 2001

#ifndef DEFAULT_THREADING
#define DEFAULT_THREADING /**/ ::Loki::SingleThreaded
#endif

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template SingleThreaded
// Implementation of the ThreadingModel policy used by various classes
// Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////

    class SingleThreaded
    {
    public:
        template <class Host>
        struct Lock
        {
            Lock() {}
            Lock(const Host&) {}
        };

        template <class Host>
        struct VolatileType
        {
            typedef Host Result;
        };

        typedef int IntType; 

        static IntType AtomicAdd(volatile IntType& lval, IntType val)
        { return lval += val; }
        
        static IntType AtomicSubtract(volatile IntType& lval, IntType val)
        { return lval -= val; }

        static IntType AtomicMultiply(volatile IntType& lval, IntType val)
        { return lval *= val; }
        
        static IntType AtomicDivide(volatile IntType& lval, IntType val)
        { return lval /= val; }
        
        static IntType AtomicIncrement(volatile IntType& lval)
        { return ++lval; }
        
        static IntType AtomicDecrement(volatile IntType& lval)
        { return --lval; }
        
        static void AtomicAssign(volatile IntType & lval, IntType val)
        { lval = val; }
        
        static void AtomicAssign(IntType & lval, volatile IntType & val)
        { lval = val; }
    };
    
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__MWERKS__)

#include <windows.h>

////////////////////////////////////////////////////////////////////////////////
// class template ObjectLevelLockable
// Implementation of the ThreadingModel policy used by various classes
// Implements a object-level locking scheme
////////////////////////////////////////////////////////////////////////////////

    class ObjectLevelLockable
    {
        CRITICAL_SECTION mtx_;

    public:
        ObjectLevelLockable()
        {
            InitializeCriticalSection(&mtx_);
        }

        ~ObjectLevelLockable()
        {
            DeleteCriticalSection(&mtx_);
        }

        template <class Host>
        class Lock
        {
            ObjectLevelLockable& host_;
            
            Lock(const Lock&);
            Lock& operator=(const Lock&);
        public:
            Lock(Host& host) : host_(host)
            {
                ::EnterCriticalSection(&host_.mtx_);
            }
            ~Lock()
            {
                ::LeaveCriticalSection(&host_.mtx_);
            }
        };

        template <class Host>
        struct VolatileType
        {
            typedef volatile Host Result;
        };

        typedef LONG IntType; 

        static IntType AtomicIncrement(volatile IntType& lval)
        { return InterlockedIncrement(&const_cast<IntType&>(lval)); }
        
        static IntType AtomicDecrement(volatile IntType& lval)
        { return InterlockedDecrement(&const_cast<IntType&>(lval)); }
        
        static void AtomicAssign(volatile IntType& lval, IntType val)
        { InterlockedExchange(&const_cast<IntType&>(lval), val); }
        
        static void AtomicAssign(IntType& lval, volatile IntType& val)
        { InterlockedExchange(&lval, val); }
    };
    
    class ClassLevelLockable
    {
        struct Initializer;
        friend struct Initializer;
        struct Initializer
        {
            Initializer()
            {
                InitializeCriticalSection(&mtx_);
            }
            ~Initializer()
            {
                DeleteCriticalSection(&mtx_);
            }
        };
        
        static Initializer initializer_;

    public:
        static CRITICAL_SECTION mtx_;

        template <class Host>
        class Lock
        {
            Lock(const Lock&);
            Lock& operator=(const Lock&);
        public:
            Lock()
            {
                EnterCriticalSection(&mtx_);
            }
            Lock(Host&)
            {
                EnterCriticalSection(&mtx_);
            }
            ~Lock()
            {
                LeaveCriticalSection(&mtx_);
            }
        };

        template <class Host>
        struct VolatileType
        {
            typedef Host Result;
        };

        typedef LONG IntType; 

        static IntType AtomicIncrement(volatile IntType& lval)
        { return InterlockedIncrement(&const_cast<IntType&>(lval)); }
        
        static IntType AtomicDecrement(volatile IntType& lval)
        { return InterlockedDecrement(&const_cast<IntType&>(lval)); }
        
        static void AtomicAssign(volatile IntType& lval, IntType val)
        { InterlockedExchange(&const_cast<IntType&>(lval), val); }
        
        static void AtomicAssign(IntType& lval, volatile IntType& val)
        { InterlockedExchange(&lval, val); }
    };
    
    CRITICAL_SECTION ClassLevelLockable::mtx_;
    
    ClassLevelLockable::Initializer ClassLevelLockable::initializer_;
    
#endif    
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// January 10, 2002: Fixed bug in AtomicDivide - credit due to Jordi Guerrero
// August 14, 2002: Changed some AtomicDivide's to AtomicDecrement's MKH
// October 12, 2002: Ported to MSVC 6 by Terje Slettebø.
//     SingleThreaded, ObjectLevelLockable and ClassLevelLockable changed from
//     templates to classes with member templates, to be usable as policies in
//     the other ported components.
////////////////////////////////////////////////////////////////////////////////

#endif
