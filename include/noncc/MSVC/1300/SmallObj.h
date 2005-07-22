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

// $Header$


#ifndef SMALLOBJ_INC_
#define SMALLOBJ_INC_

#include "Threads.h"
#include "Singleton.h"
#include <cstddef>
#include <new> // needed for std::nothrow_t parameter.

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 256
#endif

#ifndef LOKI_DEFAULT_OBJECT_ALIGNMENT
#define LOKI_DEFAULT_OBJECT_ALIGNMENT 4
#endif


namespace Loki
{
    class FixedAllocator;

////////////////////////////////////////////////////////////////////////////////
// class SmallObjAllocator
// Manages pool of fixed-size allocators.
////////////////////////////////////////////////////////////////////////////////

    class SmallObjAllocator
    {
    protected:
        SmallObjAllocator( std::size_t pageSize, std::size_t maxObjectSize,
            std::size_t objectAlignSize );

        ~SmallObjAllocator( void );

    public:
        void * Allocate( std::size_t size, bool doThrow );

        void Deallocate( void * p, std::size_t size );

        inline std::size_t GetMaxObjectSize() const { return maxSmallObjectSize_; }

        inline std::size_t GetAlignment() const { return objectAlignSize_; }

    private:
        /// Copy-constructor is not implemented.
        SmallObjAllocator( const SmallObjAllocator & );
        /// Copy-assignment operator is not implemented.
        SmallObjAllocator & operator = ( const SmallObjAllocator & );

        Loki::FixedAllocator * pool_;

        std::size_t maxSmallObjectSize_;

        std::size_t objectAlignSize_;
    };

////////////////////////////////////////////////////////////////////////////////
// class AllocatorSingleton
// This template class is derived from SmallObjAllocator in order to pass template
// arguments into SmallObjAllocator, and still have a default constructor for the
// singleton.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        template <class> class ThreadingModel,
        std::size_t chunkSize,
        std::size_t maxSmallObjectSize,
        std::size_t objectAlignSize
    >
    class AllocatorSingleton : public SmallObjAllocator
    {
    public:
        inline AllocatorSingleton() :
            SmallObjAllocator( chunkSize, maxSmallObjectSize, objectAlignSize )
            {}
        inline ~AllocatorSingleton( void ) {}

    private:
        /// Copy-constructor is not implemented.
        AllocatorSingleton( const AllocatorSingleton & );
        /// Copy-assignment operator is not implemented.
        AllocatorSingleton & operator = ( const AllocatorSingleton & );
    };



////////////////////////////////////////////////////////////////////////////////
// class SmallObjectBase
// Base class for small object allocation classes.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        template <class> class ThreadingModel,
        std::size_t chunkSize,
        std::size_t maxSmallObjectSize,
        std::size_t objectAlignSize,
        template <class> class LifetimePolicy
    >
    class SmallObjectBase
    {

#if (MAX_SMALL_OBJECT_SIZE != 0) && (DEFAULT_CHUNK_SIZE != 0) && (LOKI_DEFAULT_OBJECT_ALIGNMENT != 0)

        /// Defines type of allocator.
        typedef AllocatorSingleton< ThreadingModel, chunkSize,
            maxSmallObjectSize, objectAlignSize > MyAllocator;

        /// Defines type for thread-safety locking mechanism.
        typedef ThreadingModel< MyAllocator > MyThreadingModel;

        /// Defines singleton made from allocator.
        typedef Loki::SingletonHolder< MyAllocator, Loki::CreateStatic,
            LifetimePolicy, ThreadingModel > MyAllocatorSingleton;

    public:

        /** Throwing single-object new.
           @note The exception specification is commented to prevent warning
           from MS compiler.
         */
        static void * operator new ( std::size_t size ) // throw ( std::bad_alloc )
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return MyAllocatorSingleton::Instance().Allocate( size, true );
        }

        /// Non-throwing single-object new.
        static void * operator new ( std::size_t size, const std::nothrow_t & ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return MyAllocatorSingleton::Instance().Allocate( size, false );
        }

        /// Placement single-object new.
        inline static void * operator new ( std::size_t size, void * place )
        {
            return ::operator new( size, place );
        }

        /// Single-object delete.
        static void operator delete ( void * p, std::size_t size ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            MyAllocatorSingleton::Instance().Deallocate( p, size );
        }

        /// Non-throwing single-object delete.
        static void operator delete ( void * p, std::size_t size,
            const std::nothrow_t & ) throw()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            MyAllocatorSingleton::Instance().Deallocate( p, size );
        }

        /// Placement single-object delete.
        inline static void operator delete ( void * p, void * place )
        {
            ::operator delete ( p, place );
        }

#endif  // #if default template parameters are not zero

    protected:
        inline SmallObjectBase( void ) {}
        inline SmallObjectBase( const SmallObjectBase & ) {}
        inline SmallObjectBase & operator = ( const SmallObjectBase & ) {}
        inline ~SmallObjectBase() {}
    }; // end class SmallObjectBase


////////////////////////////////////////////////////////////////////////////////
// class SmallObject
// Base class for polymorphic small objects, offers fast allocations &
//     deallocations.  Destructor is virtual and public.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        template <class> class ThreadingModel = DEFAULT_THREADING,
        std::size_t chunkSize = DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE,
        std::size_t objectAlignSize = LOKI_DEFAULT_OBJECT_ALIGNMENT,
        template <class> class LifetimePolicy = Loki::NoDestroy
    >
    class SmallObject : public SmallObjectBase< ThreadingModel, chunkSize,
            maxSmallObjectSize, objectAlignSize, LifetimePolicy >
    {

    public:
        virtual ~SmallObject() {}
    protected:
        inline SmallObject( void ) {}

    private:
        /// Copy-constructor is not implemented.
        SmallObject( const SmallObject & );
        /// Copy-assignment operator is not implemented.
        SmallObject & operator = ( const SmallObject & );
    }; // end class SmallObject


////////////////////////////////////////////////////////////////////////////////
// class SmallValueObject
// Base class for small objects with value semantics - offers fast allocations &
//     deallocations.  Destructor is non-virtual, inline, and protected.
////////////////////////////////////////////////////////////////////////////////

    template
    <
        template <class> class ThreadingModel = DEFAULT_THREADING,
        std::size_t chunkSize = DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE,
        std::size_t objectAlignSize = LOKI_DEFAULT_OBJECT_ALIGNMENT,
        template <class> class LifetimePolicy = Loki::NoDestroy
    >
    class SmallValueObject : public SmallObjectBase< ThreadingModel, chunkSize,
            maxSmallObjectSize, objectAlignSize, LifetimePolicy >
    {
    protected:
        inline SmallValueObject( void ) {}
        inline SmallValueObject( const SmallValueObject & ) {}
        inline SmallValueObject & operator = ( const SmallValueObject & ) {}
        inline ~SmallValueObject() {}
    }; // end class SmallValueObject

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Nov. 26, 2004: re-implemented by Rich Sposato.
////////////////////////////////////////////////////////////////////////////////

#endif // SMALLOBJ_INC_

// $Log$
// Revision 1.2  2005/07/22 00:22:38  rich_sposato
// Added SmallValueObject, SmallObjectBase, and AllocatorSingleton classes.
//
