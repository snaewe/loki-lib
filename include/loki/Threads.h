////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_THREADS_INC_
#define LOKI_THREADS_INC_

// $Id$


///  @defgroup  ThreadingGroup Threading
///  Policies to for the threading model:
///
///  - SingleThreaded
///  - ObjectLevelLockable
///  - ClassLevelLockable
///
///  All classes in Loki have configurable threading model.
///
///  The macro LOKI_DEFAULT_THREADING selects the default
///  threading model for certain components of Loki
///  (it affects only default template arguments)
///
///  \par Usage:
///
///  To use a specific threading model define
///
///  - nothing, single-theading is default
///  - LOKI_OBJECT_LEVEL_THREADING for object-level-threading
///  - LOKI_CLASS_LEVEL_THREADING for class-level-threading
///
///  \par Supported platfroms:
///
///  - Windows (windows.h)
///  - POSIX (pthread.h):
///    No recursive mutex support with pthread.
///    This means: calling Lock() on a Loki::Mutex twice from the
///    same thread before unlocking the mutex deadlocks the system.
///    To avoid this redesign your synchronization. See also:
///    http://sourceforge.net/tracker/index.php?func=detail&aid=1516182&group_id=29557&atid=396647


#include <cassert>

#if defined( LOKI_CLASS_LEVEL_THREADING ) || defined( LOKI_OBJECT_LEVEL_THREADING )

    #define LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL ::Loki::ClassLevelLockable

    #if defined(LOKI_CLASS_LEVEL_THREADING) && !defined(LOKI_OBJECT_LEVEL_THREADING)
        #define LOKI_DEFAULT_THREADING ::Loki::ClassLevelLockable
    #else
        #define LOKI_DEFAULT_THREADING ::Loki::ObjectLevelLockable
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        #include <windows.h>
        #define LOKI_WINDOWS_H
    #else
        #include <pthread.h>
        #define LOKI_PTHREAD_H
    #endif

#else

    #define LOKI_DEFAULT_THREADING ::Loki::SingleThreaded
    #define LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL ::Loki::SingleThreaded

#endif

#if !defined( LOKI_DEFAULT_MUTEX )
    #define LOKI_DEFAULT_MUTEX ::Loki::Mutex
#endif

#if defined( LOKI_WINDOWS_H )

#define LOKI_THREADS_MUTEX(x)           CRITICAL_SECTION (x);
#define LOKI_THREADS_MUTEX_INIT(x)      ::InitializeCriticalSection (x)
#define LOKI_THREADS_MUTEX_DELETE(x)    ::DeleteCriticalSection (x)
#define LOKI_THREADS_MUTEX_LOCK(x)      ::EnterCriticalSection (x)
#define LOKI_THREADS_MUTEX_UNLOCK(x)    ::LeaveCriticalSection (x)
#define LOKI_THREADS_LONG               LONG
#define LOKI_THREADS_MUTEX_CTOR(x)

#define LOKI_THREADS_ATOMIC_FUNCTIONS                                   \
    private:                                                            \
        static CRITICAL_SECTION atomic_mutex_;                          \
    public:                                                             \
        static IntType AtomicMultiply(volatile IntType& lval, const IntType val) \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            lval *= val;                                                \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicDivide(volatile IntType& lval, const IntType val)  \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            lval /= val;                                                \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicIncrement(volatile IntType& lval)          \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            ++lval;                                                     \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicDecrement(volatile IntType& lval)          \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            --lval;                                                     \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicAssign(volatile IntType& lval, const IntType val)   \
        {                                                               \
            InterlockedExchange(&const_cast<IntType&>(lval), val);      \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicAssign(IntType& lval, volatile const IntType& val)  \
        {                                                               \
            InterlockedExchange(&lval, val);                            \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicIncrement(volatile IntType& lval, const IntType compare, bool & matches )  \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            ++lval;                                                     \
            matches = ( lval == compare );                              \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicDecrement(volatile IntType& lval, const IntType compare, bool & matches )  \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            --lval;                                                     \
            matches = ( lval == compare );                              \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicAdd(volatile IntType& lval, const IntType val, const IntType compare, bool & matches )  \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            lval += val;                                                \
            matches = ( lval == compare );                              \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicSubtract(volatile IntType& lval, const IntType val, const IntType compare, bool & matches ) \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            lval -= val;                                                \
            matches = ( lval == compare );                              \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicMultiply(volatile IntType& lval, const IntType val, const IntType compare, bool & matches ) \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                   \
            lval *= val;                                                \
            matches = ( lval == compare );                              \
            ::LeaveCriticalSection( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicDivide(volatile IntType& lval, const IntType val, const IntType compare, bool & matches )   \
        {                                                               \
            ::EnterCriticalSection( &atomic_mutex_ );                    \
            lval /= val;                                                \
            matches = ( lval == compare );                              \
            ::LeaveCriticalSection( &atomic_mutex_ );                    \
            return lval;                                                \
        }

#elif defined(LOKI_PTHREAD_H)


#define LOKI_THREADS_MUTEX(x)           pthread_mutex_t (x);

#define LOKI_THREADS_MUTEX_INIT(x)      ::pthread_mutex_init(x, 0)

// define to 1 to enable recursive mutex support
#if 0
// experimental recursive mutex support
#define LOKI_THREADS_MUTEX_CTOR(x)      : x(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
#else
// no recursive mutex support
#define LOKI_THREADS_MUTEX_CTOR(x)
#endif

#define LOKI_THREADS_MUTEX_DELETE(x)    ::pthread_mutex_destroy (x)
#define LOKI_THREADS_MUTEX_LOCK(x)      ::pthread_mutex_lock (x)
#define LOKI_THREADS_MUTEX_UNLOCK(x)    ::pthread_mutex_unlock (x)
#define LOKI_THREADS_LONG               long

#define LOKI_THREADS_ATOMIC(x)                                           \
                pthread_mutex_lock(&atomic_mutex_);                      \
                x;                                                       \
                pthread_mutex_unlock(&atomic_mutex_)

#define LOKI_THREADS_ATOMIC_FUNCTIONS                                \
    private:                                                         \
        static pthread_mutex_t atomic_mutex_;                        \
    public:                                                          \
        static IntType AtomicMultiply(volatile IntType& lval, const IntType val) \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            lval *= val;                                                 \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicDivide(volatile IntType& lval, const IntType val) \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            lval /= val;                                                 \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicIncrement(volatile IntType& lval)           \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            ++lval;                                                      \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicDecrement(volatile IntType& lval)           \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            --lval;                                                      \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicAssign(volatile IntType& lval, const IntType val) \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            lval = val;                                                  \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicAssign(IntType& lval, volatile const IntType& val) \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            lval = val;                                                  \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicIncrement(volatile IntType& lval, const IntType compare, bool & matches ) \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            ++lval;                                                      \
            matches = ( compare == lval );                               \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
                                                                         \
        static IntType AtomicDecrement(volatile IntType& lval, const IntType compare, bool & matches ) \
        {                                                                \
            ::pthread_mutex_lock( &atomic_mutex_ );                      \
            --lval;                                                      \
            matches = ( compare == lval );                               \
            ::pthread_mutex_unlock( &atomic_mutex_ );                    \
            return lval;                                                 \
        }                                                                \
        static IntType AtomicMultiply(volatile IntType& lval, const IntType val, const IntType compare, bool & matches ) \
        {                                                               \
            ::pthread_mutex_lock( &atomic_mutex_ );                     \
            lval *= val;                                                \
            matches = ( lval == compare );                              \
            ::pthread_mutex_unlock( &atomic_mutex_ );                   \
            return lval;                                                \
        }                                                               \
                                                                        \
        static IntType AtomicDivide(volatile IntType& lval, const IntType val, const IntType compare, bool & matches ) \
        {                                                               \
            ::pthread_mutex_lock( &atomic_mutex_ );                     \
            lval /= val;                                                \
            matches = ( lval == compare );                              \
            ::pthread_mutex_unlock( &atomic_mutex_ );                   \
            return lval;                                                \
        }

#else // single threaded

#define LOKI_THREADS_MUTEX(x)
#define LOKI_THREADS_MUTEX_INIT(x)
#define LOKI_THREADS_MUTEX_DELETE(x)
#define LOKI_THREADS_MUTEX_LOCK(x)
#define LOKI_THREADS_MUTEX_UNLOCK(x)
#define LOKI_THREADS_LONG
#define LOKI_THREADS_MUTEX_CTOR(x)

#endif



namespace Loki
{

    ////////////////////////////////////////////////////////////////////////////////
    ///  \class Mutex
    //
    ///  \ingroup ThreadingGroup
    ///  A simple and portable Mutex.  A default policy class for locking objects.
    ////////////////////////////////////////////////////////////////////////////////

    class Mutex
    {
    public:
        Mutex() LOKI_THREADS_MUTEX_CTOR(mtx_)
        {
            LOKI_THREADS_MUTEX_INIT(&mtx_);
        }
        ~Mutex()
        {
            LOKI_THREADS_MUTEX_DELETE(&mtx_);
        }
        void Lock()
        {
            LOKI_THREADS_MUTEX_LOCK(&mtx_);
        }
        void Unlock()
        {
            LOKI_THREADS_MUTEX_UNLOCK(&mtx_);
        }
    private:
        /// Copy-constructor not implemented.
        Mutex(const Mutex &);
        /// Copy-assignement operator not implemented.
        Mutex & operator = (const Mutex &);
        LOKI_THREADS_MUTEX(mtx_)
    };


     ////////////////////////////////////////////////////////////////////////////////
    ///  \class SingleThreaded
    ///
    ///  \ingroup ThreadingGroup
    ///  Implementation of the ThreadingModel policy used by various classes
    ///  Implements a single-threaded model; no synchronization
    ////////////////////////////////////////////////////////////////////////////////
    template <class Host, class MutexPolicy = LOKI_DEFAULT_MUTEX>
    class SingleThreaded
    {
    public:
        /// \struct Lock
        /// Dummy Lock class
        struct Lock
        {
            Lock() {}
            explicit Lock(const SingleThreaded&) {}
            explicit Lock(const SingleThreaded*) {}
        };

        typedef Host VolatileType;

        typedef int IntType;

        static IntType AtomicAdd(volatile IntType& lval, const IntType val)
        { return lval += val; }

        static IntType AtomicSubtract(volatile IntType& lval, const IntType val)
        { return lval -= val; }

        static IntType AtomicMultiply(volatile IntType& lval, const IntType val)
        { return lval *= val; }

        static IntType AtomicDivide(volatile IntType& lval, const IntType val)
        { return lval /= val; }

        static IntType AtomicIncrement(volatile IntType& lval)
        { return ++lval; }

        static IntType AtomicDecrement(volatile IntType& lval)
        { return --lval; }

        static IntType AtomicAssign(volatile IntType & lval, const IntType val)
        {
            lval = val;
            return lval;
        }

        static IntType AtomicAssign(IntType & lval, volatile IntType & val)
        {
            lval = val;
            return lval;
        }

        static IntType AtomicAdd(volatile IntType& lval, const IntType val, const IntType compare, bool & matches )
        {
            lval += val;
            matches = ( lval == compare );
            return lval;
        }

        static IntType AtomicSubtract(volatile IntType& lval, const IntType val, const IntType compare, bool & matches )
        {
            lval -= val;
            matches = ( lval == compare );
            return lval;
        }

        static IntType AtomicMultiply(volatile IntType& lval, const IntType val, const IntType compare, bool & matches )
        {
            lval *= val;
            matches = ( lval == compare );
            return lval;
        }

        static IntType AtomicDivide(volatile IntType& lval, const IntType val, const IntType compare, bool & matches )
        {
            lval /= val;
            matches = ( lval == compare );
            return lval;
        }

        static IntType AtomicIncrement(volatile IntType& lval, const IntType compare, bool & matches )
        {
            ++lval;
            matches = ( lval == compare );
            return lval;
        }

        static IntType AtomicDecrement(volatile IntType& lval, const IntType compare, bool & matches )
        {
            --lval;
            matches = ( lval == compare );
            return lval;
        }

    };


#if defined(LOKI_WINDOWS_H) || defined(LOKI_PTHREAD_H)

    ////////////////////////////////////////////////////////////////////////////////
    ///  \class ObjectLevelLockable
    ///
    ///  \ingroup ThreadingGroup
    ///  Implementation of the ThreadingModel policy used by various classes
    ///  Implements a object-level locking scheme
    ////////////////////////////////////////////////////////////////////////////////
    template < class Host, class MutexPolicy = LOKI_DEFAULT_MUTEX >
    class ObjectLevelLockable
    {
        mutable MutexPolicy mtx_;

    public:
        ObjectLevelLockable() : mtx_() {}

        ObjectLevelLockable(const ObjectLevelLockable&) : mtx_() {}

        ~ObjectLevelLockable() {}

        class Lock;
        friend class Lock;

        ///  \struct Lock
        ///  Lock class to lock on object level
        class Lock
        {
        public:

            /// Lock object
            explicit Lock(const ObjectLevelLockable& host) : host_(host)
            {
                host_.mtx_.Lock();
            }

            /// Lock object
            explicit Lock(const ObjectLevelLockable* host) : host_(*host)
            {
                host_.mtx_.Lock();
            }

            /// Unlock object
            ~Lock()
            {
                host_.mtx_.Unlock();
            }

        private:
            /// private by design of the object level threading
            Lock();
            Lock(const Lock&);
            Lock& operator=(const Lock&);
            const ObjectLevelLockable& host_;
        };

        typedef volatile Host VolatileType;

        typedef LOKI_THREADS_LONG IntType;

        LOKI_THREADS_ATOMIC_FUNCTIONS

    };

#ifdef LOKI_PTHREAD_H
    template <class Host, class MutexPolicy>
    pthread_mutex_t ObjectLevelLockable<Host, MutexPolicy>::atomic_mutex_ = PTHREAD_MUTEX_INITIALIZER;
#endif

    ////////////////////////////////////////////////////////////////////////////////
    ///  \class ClassLevelLockable
    ///
    ///  \ingroup ThreadingGroup
    ///  Implementation of the ThreadingModel policy used by various classes
    ///  Implements a class-level locking scheme
    ////////////////////////////////////////////////////////////////////////////////
    template <class Host, class MutexPolicy = LOKI_DEFAULT_MUTEX >
    class ClassLevelLockable
    {
        struct Initializer
        {

            /// This function provides a Scott-Meyers type of Singleton as the initializer
            /// for the shared mutex.
            static Initializer & GetIt( void )
            {
                static Initializer initializer_;
                return initializer_;
            }

            inline bool IsInit( void ) { return init_; }
            inline MutexPolicy & GetMutex( void ) { return mtx_; }

        private:
            bool init_;
            MutexPolicy mtx_;

            Initializer() : init_(false), mtx_()
            {
                init_ = true;
            }

            ~Initializer()
            {
                assert(init_);
            }

            Initializer( const Initializer & );
            Initializer & operator = ( const Initializer & );
        };

    public:

        class Lock;
        friend class Lock;

        ///  \struct Lock
        ///  Lock class to lock on class level
        class Lock
        {
        public:

            /// Lock class
            Lock()
            {
                Initializer & initializer = Initializer::GetIt();
                assert( initializer.IsInit() );
                initializer.GetMutex().Lock();
            }

            /// Lock class
            explicit Lock(const ClassLevelLockable&)
            {
                Initializer & initializer = Initializer::GetIt();
                assert( initializer.IsInit() );
                initializer.GetMutex().Lock();
            }

            /// Lock class
            explicit Lock(const ClassLevelLockable*)
            {
                Initializer & initializer = Initializer::GetIt();
                assert( initializer.IsInit() );
                initializer.GetMutex().Lock();
            }

            /// Unlock class
            ~Lock()
            {
                Initializer & initializer = Initializer::GetIt();
                assert( initializer.IsInit() );
                initializer.GetMutex().Unlock();
            }

        private:
            Lock(const Lock&);
            Lock& operator=(const Lock&);
        };

        typedef volatile Host VolatileType;

        typedef LOKI_THREADS_LONG IntType;

        LOKI_THREADS_ATOMIC_FUNCTIONS

    };

#ifdef LOKI_PTHREAD_H
    template <class Host, class MutexPolicy>
    pthread_mutex_t ClassLevelLockable<Host, MutexPolicy>::atomic_mutex_ = PTHREAD_MUTEX_INITIALIZER;
#endif

#endif // #if defined(LOKI_WINDOWS_H) || defined(LOKI_PTHREAD_H)

} // namespace Loki


#endif // end file guardian

