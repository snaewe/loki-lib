#ifndef LOKI_THREADS_H_
#define LOKI_THREADS_H_

////////////////////////////////////////////////////////////////////////////////
// macro LOKI_DEFAULT_THREADING
// Selects the default threading model for certain components of Loki
// If you don't define it, it defaults to single-threaded
// All classes in Loki have configurable threading model; LOKI_DEFAULT_THREADING
// affects only default template arguments
////////////////////////////////////////////////////////////////////////////////

// $Header$


#if defined(LOKI_CLASS_LEVEL_THREADING) || defined(LOKI_OBJECT_LEVEL_THREADING)

    #define LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL ::Loki::ClassLevelLockable
    
    #if defined(LOKI_CLASS_LEVEL_THREADING) && !defined(LOKI_OBJECT_LEVEL_THREADING)
        #define LOKI_DEFAULT_THREADING ::Loki::ClassLevelLockable
    #else
        #define LOKI_DEFAULT_THREADING ::Loki::ObjectLevelLockable
    #endif
     
    #if defined(_WIN32)
        #include <windows.h> 
    #else
        #include <pthread.h>
    #endif
    
#else

    #define LOKI_DEFAULT_THREADING ::Loki::SingleThreaded
    #define LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL ::Loki::SingleThreaded
    
#endif

#include <cassert>

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template SingleThreaded
// Implementation of the ThreadingModel policy used by various classes
// Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////

    template <class Host>
    class SingleThreaded
    {
    public:
        struct Lock
        {
            Lock() {}
            explicit Lock(const SingleThreaded&) {}
            explicit Lock(const SingleThreaded*) {}
        };
        
        typedef Host VolatileType;

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
    
#if defined(_WINDOWS_) || defined(_WINDOWS_H) 

#define LOKI_THREADS_MUTEX              CRITICAL_SECTION
#define LOKI_THREADS_MUTEX_INIT         ::InitializeCriticalSection
#define LOKI_THREADS_MUTEX_DELETE       ::DeleteCriticalSection
#define LOKI_THREADS_MUTEX_LOCK         ::EnterCriticalSection
#define LOKI_THREADS_MUTEX_UNLOCK       ::LeaveCriticalSection
#define LOKI_THREADS_LONG               LONG

#define LOKI_THREADS_ATOMIC_FUNCTIONS                                   \
        static IntType AtomicIncrement(volatile IntType& lval)          \
        { return InterlockedIncrement(&const_cast<IntType&>(lval)); }   \
                                                                        \
        static IntType AtomicDecrement(volatile IntType& lval)          \
        { return InterlockedDecrement(&const_cast<IntType&>(lval)); }   \
                                                                        \
        static void AtomicAssign(volatile IntType& lval, IntType val)   \
        { InterlockedExchange(&const_cast<IntType&>(lval), val); }      \
                                                                        \
        static void AtomicAssign(IntType& lval, volatile IntType& val)  \
        { InterlockedExchange(&lval, val); }



#elif defined(_PTHREAD_H) //POSIX threads (pthread.h)


#define LOKI_THREADS_MUTEX              pthread_mutex_t
#define LOKI_THREADS_MUTEX_INIT(x)      ::pthread_mutex_init(x,0)
#define LOKI_THREADS_MUTEX_DELETE       ::pthread_mutex_destroy
#define LOKI_THREADS_MUTEX_LOCK         ::pthread_mutex_lock
#define LOKI_THREADS_MUTEX_UNLOCK       ::pthread_mutex_unlock
#define LOKI_THREADS_LONG               long

#define LOKI_THREADS_ATOMIC(x)                                           \
                pthread_mutex_lock(&atomic_mutex_);                      \
                x;                                                       \
                pthread_mutex_unlock(&atomic_mutex_)    
                
#define LOKI_THREADS_ATOMIC_FUNCTIONS                                    \
        private:                                                         \
            static pthread_mutex_t atomic_mutex_;                        \
        public:                                                          \
        static IntType AtomicIncrement(volatile IntType& lval)           \
        { LOKI_THREADS_ATOMIC( lval++ ); return lval; }                  \
                                                                         \
        static IntType AtomicDecrement(volatile IntType& lval)           \
        { LOKI_THREADS_ATOMIC(lval-- ); return lval; }                   \
                                                                         \
        static void AtomicAssign(volatile IntType& lval, IntType val)    \
        { LOKI_THREADS_ATOMIC( lval = val ); }                           \
                                                                         \
        static void AtomicAssign(IntType& lval, volatile IntType& val)   \
        { LOKI_THREADS_ATOMIC( lval = val ); }            

#endif


#if defined(_WINDOWS_) || defined(_WINDOWS_H) || defined(_PTHREAD_H) 

////////////////////////////////////////////////////////////////////////////////
// class template ObjectLevelLockable
// Implementation of the ThreadingModel policy used by various classes
// Implements a object-level locking scheme
////////////////////////////////////////////////////////////////////////////////

    template <class Host>
    class ObjectLevelLockable
    {
        mutable LOKI_THREADS_MUTEX mtx_;

    public:
        ObjectLevelLockable()
        {
           LOKI_THREADS_MUTEX_INIT(&mtx_);
        }
        
        ObjectLevelLockable(const ObjectLevelLockable&)
        {
            LOKI_THREADS_MUTEX_INIT(&mtx_);
        }

        ~ObjectLevelLockable()
        {
            LOKI_THREADS_MUTEX_DELETE(&mtx_);
        }

        class Lock;
        friend class Lock;
        
        class Lock
        { 
        public:

            explicit Lock(const ObjectLevelLockable& host) : host_(host)
            {
                LOKI_THREADS_MUTEX_LOCK(&host_.mtx_);
            }

            explicit Lock(const ObjectLevelLockable* host) : host_(*host)
            {
                LOKI_THREADS_MUTEX_LOCK(&host_.mtx_);
            }

            ~Lock()
            {
                LOKI_THREADS_MUTEX_UNLOCK(&host_.mtx_);
            }
            
        private:
            Lock();
            Lock(const Lock&);
            Lock& operator=(const Lock&);
            const ObjectLevelLockable& host_;
        };

        typedef volatile Host VolatileType;

        typedef LOKI_THREADS_LONG IntType; 
        
        LOKI_THREADS_ATOMIC_FUNCTIONS   
        
    };

#if defined(_PTHREAD_H) 
    template <class Host>
    pthread_mutex_t ObjectLevelLockable<Host>::atomic_mutex_(PTHREAD_MUTEX_INITIALIZER);
#endif
    

////////////////////////////////////////////////////////////////////////////////
// class template ClassLevelLockable
// Implementation of the ThreadingModel policy used by various classes
// Implements a class-level locking scheme
////////////////////////////////////////////////////////////////////////////////
    
    template <class Host>
    class ClassLevelLockable
    {
        struct Initializer
        {   
            LOKI_THREADS_MUTEX mtx_;
            bool init_;

            Initializer() : init_(false)
            {
                LOKI_THREADS_MUTEX_INIT(&mtx_);
                init_ = true;
            }
            ~Initializer()
            {
                assert(init_);
                LOKI_THREADS_MUTEX_DELETE(&mtx_);
            }
        };
        
        static Initializer initializer_;

    public:
    
        class Lock;
        friend class Lock;
        
        class Lock
        {    
        public:
        
            Lock()
            {
                assert(initializer_.init_);
                LOKI_THREADS_MUTEX_LOCK(&initializer_.mtx_);
            }
            
            explicit Lock(const ClassLevelLockable&)
            {
                assert(initializer_.init_);
                LOKI_THREADS_MUTEX_LOCK(&initializer_.mtx_);
            }

            explicit Lock(const ClassLevelLockable*)
            {
                assert(initializer_.init_);
                LOKI_THREADS_MUTEX_LOCK(&initializer_.mtx_);
            }
            
            ~Lock()
            {
                assert(initializer_.init_);
                LOKI_THREADS_MUTEX_UNLOCK(&initializer_.mtx_);
            }
            
        private:
            Lock(const Lock&);
            Lock& operator=(const Lock&);
        };

        typedef volatile Host VolatileType;

        typedef LOKI_THREADS_LONG IntType; 

        LOKI_THREADS_ATOMIC_FUNCTIONS
        
    };

#if defined(_PTHREAD_H) 
    template <class Host>
    pthread_mutex_t ClassLevelLockable<Host>::atomic_mutex_(PTHREAD_MUTEX_INITIALIZER);
#endif

    template <class Host>
    typename ClassLevelLockable<Host>::Initializer 
    ClassLevelLockable<Host>::initializer_;

#endif // defined(_WINDOWS_) || defined(_WINDOWS_H) || defined(_PTHREAD_H) 
  
} // namespace Loki



////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// July 26, 2005: some asserts by Peter Kümmel
////////////////////////////////////////////////////////////////////////////////


#endif

// $Log$
// Revision 1.16  2005/10/30 14:03:23  syntheticpp
// replace tabs space
//
// Revision 1.15  2005/10/24 20:35:12  syntheticpp
// small changes for Threads; add compile test for Threads.h
//
// Revision 1.14  2005/10/24 15:05:24  syntheticpp
// adding support for POSIX threads (pthreads.h), Thanks to Ilya Volvovski
//
// Revision 1.13  2005/09/26 07:33:04  syntheticpp
// move macros into LOKI_ namespace
//
// Revision 1.12  2005/07/31 14:00:48  syntheticpp
// make object level threading possible
//
// Revision 1.11  2005/07/28 14:26:09  syntheticpp
// add cvs Header/Log
//

