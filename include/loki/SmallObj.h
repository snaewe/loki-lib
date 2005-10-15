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


#ifndef LOKI_SMALLOBJ_INC_
#define LOKI_SMALLOBJ_INC_

#include "Threads.h"
#include "Singleton.h"
#include <cstddef>
#include <new> // needed for std::nothrow_t parameter.

#ifndef LOKI_DEFAULT_CHUNK_SIZE
#define LOKI_DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef LOKI_MAX_SMALL_OBJECT_SIZE
#define LOKI_MAX_SMALL_OBJECT_SIZE 256
#endif

#ifndef LOKI_DEFAULT_OBJECT_ALIGNMENT
#define LOKI_DEFAULT_OBJECT_ALIGNMENT 4
#endif


namespace Loki
{
    class FixedAllocator;

    /** @class SmallObjAllocator Manages pool of fixed-size allocators.
     Designed to be a non-templated base class of AllocatorSingleton so that
     implementation details can be safely hidden in the source code file.
     */
    class SmallObjAllocator
    {
    protected:
        /** The only available constructor needs certain parameters in order to
         initialize all the FixedAllocator's.  This throws only if
         @param pageSize # of bytes in a page of memory.
         @param maxObjectSize Max # of bytes which this may allocate.
         @param objectAlignSize # of bytes between alignment boundaries.
         */
        SmallObjAllocator( std::size_t pageSize, std::size_t maxObjectSize,
            std::size_t objectAlignSize );

        /** Destructor releases all blocks, all Chunks, and FixedAllocator's.
         Any outstanding blocks are unavailable, and should not be used after
         this destructor is called.  The destructor is deliberately non-virtual
         because it is protected, not public.
         */
        ~SmallObjAllocator( void );

    public:
        /** Allocates a block of memory of requested size.  Complexity is often
         constant-time, but might be O(C) where C is the number of Chunks in a
         FixedAllocator. 

         @par Exception Safety Level
         Provides either strong-exception safety, or no-throw exception-safety
         level depending upon doThrow parameter.  The reason it provides two
         levels of exception safety is because it is used by both the nothrow
         and throwing new operators.  The underlying implementation will never
         throw of its own accord, but this can decide to throw if it does not
         allocate.  The only exception it should emit is std::bad_alloc.

         @par Allocation Failure
         If it does not allocate, it will call TrimExcessMemory and attempt to
         allocate again, before it decides to throw or return NULL.  Many
         allocators loop through several new_handler functions, and terminate
         if they can not allocate, but not this one.  It only makes one attempt
         using its own implementation of the new_handler, and then returns NULL
         or throws so that the program can decide what to do at a higher level.
         (Side note: Even though the C++ Standard allows allocators and
         new_handlers to terminate if they fail, the Loki allocator does not do
         that since that policy is not polite to a host program.)

         @param size # of bytes needed for allocation.
         @param doThrow True if this should throw if unable to allocate, false
          if it should provide no-throw exception safety level.
         @return NULL if nothing allocated and doThrow is false.  Else the
          pointer to an available block of memory.
         */
        void * Allocate( std::size_t size, bool doThrow );

        /** Deallocates a block of memory at a given place and of a specific
        size.  Complexity is almost always constant-time, and is O(C) only if
        it has to search for which Chunk deallocates.  This never throws.
         */
        void Deallocate( void * p, std::size_t size );

        /** Deallocates a block of memory at a given place but of unknown size
        size.  Complexity is O(F + C) where F is the count of FixedAllocator's
        in the pool, and C is the number of Chunks in all FixedAllocator's.  This
        does not throw exceptions.  This overloaded version of Deallocate is
        called by the nothow delete operator - which is called when the nothrow
        new operator is used, but a constructor throws an exception.
         */
        void Deallocate( void * p );

        /// Returns max # of bytes which this can allocate.
        inline std::size_t GetMaxObjectSize() const { return maxSmallObjectSize_; }

        /// Returns # of bytes between allocation boundaries.
        inline std::size_t GetAlignment() const { return objectAlignSize_; }

        /** Releases empty Chunks from memory.  Complexity is O(F + C) where F
        is the count of FixedAllocator's in the pool, and C is the number of
        Chunks in all FixedAllocator's.  This will never throw.  This is called
        by AllocatorSingleto::ClearExtraMemory, the new_handler function for
        Loki's allocator, and is called internally when an allocation fails.
        @return True if any memory released, or false if none released.
         */
        bool TrimExcessMemory( void );

    private:
        /// Default-constructor is not implemented.
        SmallObjAllocator( void );
        /// Copy-constructor is not implemented.
        SmallObjAllocator( const SmallObjAllocator & );
        /// Copy-assignment operator is not implemented.
        SmallObjAllocator & operator = ( const SmallObjAllocator & );

        /// Pointer to array of fixed-size allocators.
        Loki::FixedAllocator * pool_;

        /// Largest object size supported by allocators.
        std::size_t maxSmallObjectSize_;

        /// Size of alignment boundaries.
        std::size_t objectAlignSize_;
    };


    /** @class AllocatorSingleton This template class is derived from
     SmallObjAllocator in order to pass template arguments into it, and still
     have a default constructor for the singleton.  Each instance is a unique
     combination of all the template parameters, and hence is singleton only 
     with respect to those parameters.  The template parameters have default
     values and the class has typedefs identical to both SmallObject and
     SmallValueObject so that this class can be used directly instead of going
     through SmallObject or SmallValueObject.  That design feature allows
     clients to use the new_handler without having the name of the new_handler
     function show up in classes derived from SmallObject or SmallValueObject.
     Thus, the only functions in the allocator which show up in SmallObject or
     SmallValueObject inheritance heierarchies are the new and delete
     operators.
    */
    template
    <
        template <class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
        std::size_t chunkSize = LOKI_DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = LOKI_MAX_SMALL_OBJECT_SIZE,
        std::size_t objectAlignSize = LOKI_DEFAULT_OBJECT_ALIGNMENT,
        template <class> class LifetimePolicy = Loki::NoDestroy
    >
    class AllocatorSingleton : public SmallObjAllocator
    {
    public:

        /// Defines type of allocator.
        typedef AllocatorSingleton< ThreadingModel, chunkSize,
            maxSmallObjectSize, objectAlignSize, LifetimePolicy > MyAllocator;

        /// Defines type for thread-safety locking mechanism.
        typedef ThreadingModel< MyAllocator > MyThreadingModel;

        /// Defines singleton made from allocator.
        typedef Loki::SingletonHolder< MyAllocator, Loki::CreateStatic,
            LifetimePolicy, ThreadingModel > MyAllocatorSingleton;

        /// Returns reference to the singleton.
        inline static AllocatorSingleton & Instance( void )
        {
            return MyAllocatorSingleton::Instance();
        }

        /// The default constructor is not meant to be called directly.
        inline AllocatorSingleton() :
            SmallObjAllocator( chunkSize, maxSmallObjectSize, objectAlignSize )
            {}

        /// The destructor is not meant to be called directly.
        inline ~AllocatorSingleton( void ) {}

        /** Clears any excess memory used by the allocator.  Complexity is
         O(F + C) where F is the count of FixedAllocator's in the pool, and C
         is the number of Chunks in all FixedAllocator's.  This never throws.
         @note This function can be used as a new_handler when Loki and other
         memory allocators can no longer allocate.  Although the C++ Standard
         allows new_handler functions to terminate the program when they can
         not release any memory, this will not do so.
         */
        static void ClearExtraMemory( void );

    private:
        /// Copy-constructor is not implemented.
        AllocatorSingleton( const AllocatorSingleton & );
        /// Copy-assignment operator is not implemented.
        AllocatorSingleton & operator = ( const AllocatorSingleton & );
    };

    template
    <
        template <class> class TM,
        std::size_t CS,
        std::size_t MSOS,
        std::size_t OAS,
        template <class> class LP
    >
    void AllocatorSingleton< TM, CS, MSOS, OAS, LP >::ClearExtraMemory( void )
    {
        typename MyThreadingModel::Lock lock;
        (void)lock; // get rid of warning
        Instance().TrimExcessMemory();
    }


    /** This standalone function provides the longevity level for Small-Object
     Allocators which use the Loki::SingletonWithLongevity policy.  The
     SingletonWithLongevity class can find this function through argument-
     dependent lookup.

     @par Longevity Levels
     No Small-Object Allocator depends on any other Small-Object allocator, so
     this does not need to calculate dependency levels among allocators, and
     it returns just a constant.  All allocators must live longer than the
     objects which use the allocators, it must return a longevity level higher
     than any such object.
     */
    template
    <
        template <class> class TM,
        std::size_t CS,
        std::size_t MSOS,
        std::size_t OAS,
        template <class> class LP
    >
    inline unsigned int GetLongevity(
        AllocatorSingleton< TM, CS, MSOS, OAS, LP > * )
    {
        // Returns highest possible value.
        return 0xFFFFFFFF;
    }


    /** @class SmallObjectBase Base class for small object allocation classes.
     The shared implementation of the new and delete operators are here instead
     of being duplicated in both SmallObject or SmallValueObject.  This class
     is not meant to be used directly by clients, or derived from by clients.
     Class has no data members so compilers can use Empty-Base-Optimization.

     @par Singleton Lifetime Policies and Small-Object Allocator
     At this time, the only Singleton Lifetime policies recommended for
     use with the Small-Object Allocator are Loki::SingletonWithLongevity and
     Loki::NoDestroy.  The Loki::DefaultLifetime and Loki::PhoenixSingleton
     policies are *not* recommended since they can cause the allocator to be
     destroyed and release memory for singletons which inherit from either
     SmallObject or SmallValueObject.  The default is Loki::NoDestroy to
     insure that memory is never released before the object using that
     memory is destroyed.  Loki::SingletonWithLongevity can also insure that
     no memory is released before the owning object is destroyed, and also
     insure that any memory controlled by a Small-Object Allocator is
     released.

     @par Small Singletons and Lifetime Policies
     If you a singleton is derived from SmallValueObject or SmallObject, you
     have to use compatible lifetime policies for both the singleton and the
     allocator.  The 3 possible options are:
     - They may both be Loki::NoDestroy.  Since neither is ever destroyed, the
       destruction order does not matter.
     - They may both be Loki::SingletonWithLongevity as long as the longevity
       level for the singleton is lower than that for the allocator.  This is
       why the allocator's GetLongevity function returns the highest value.
     - The singleton may use Loki::SingletonWithLongevity, and the allocator
       may use Loki::NoDestroy.  This is why the allocator's default policy is
       Loki::NoDestroy.
     You should *not* use Loki::NoDestroy for the singleton, and then use
     Loki::SingletonWithLongevity for the allocator.  This causes the allocator
     to be destroyed and release the memory for the singleton while the
     singleton is still alive.
     */
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

#if (LOKI_MAX_SMALL_OBJECT_SIZE != 0) && (LOKI_DEFAULT_CHUNK_SIZE != 0) && (LOKI_DEFAULT_OBJECT_ALIGNMENT != 0)

        /// Defines type of allocator.
        typedef AllocatorSingleton< ThreadingModel, chunkSize,
            maxSmallObjectSize, objectAlignSize, LifetimePolicy > MyAllocator;

        /// Defines type for thread-safety locking mechanism.
        typedef ThreadingModel< MyAllocator > MyThreadingModel;

        /// Defines singleton made from allocator.
        typedef Loki::SingletonHolder< MyAllocator, Loki::CreateStatic,
            LifetimePolicy, ThreadingModel > MyAllocatorSingleton;
		
    public:

        /// Throwing single-object new throws bad_alloc when allocation fails.
#ifdef _MSC_VER
        /// @note MSVC complains about non-empty exception specification lists.
        static void * operator new ( std::size_t size )
#else
        static void * operator new ( std::size_t size ) throw ( std::bad_alloc )
#endif
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return MyAllocatorSingleton::Instance().Allocate( size, true );
        }

        /// Non-throwing single-object new returns NULL if allocation fails.
        static void * operator new ( std::size_t size, const std::nothrow_t & ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return MyAllocatorSingleton::Instance().Allocate( size, false );
        }

        /// Placement single-object new merely calls global placement new.
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

        /** Non-throwing single-object delete is only called when nothrow
         new operator is used, and the constructor throws an exception.
         */
        static void operator delete ( void * p, const std::nothrow_t & ) throw()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            MyAllocatorSingleton::Instance().Deallocate( p );
        }

        /// Placement single-object delete merely calls global placement delete.
        inline static void operator delete ( void * p, void * place )
        {
            ::operator delete ( p, place );
        }

#ifdef LOKI_SMALL_OBJECT_USE_NEW_ARRAY

        /// Throwing array-object new throws bad_alloc when allocation fails.
#ifdef _MSC_VER
        /// @note MSVC complains about non-empty exception specification lists.
        static void * operator new [] ( std::size_t size )
#else
        static void * operator new [] ( std::size_t size )
            throw ( std::bad_alloc )
#endif
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return MyAllocatorSingleton::Instance().Allocate( size, true );
        }

        /// Non-throwing array-object new returns NULL if allocation fails.
        static void * operator new [] ( std::size_t size,
            const std::nothrow_t & ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            return MyAllocatorSingleton::Instance().Allocate( size, false );
        }

        /// Placement array-object new merely calls global placement new.
        inline static void * operator new [] ( std::size_t size, void * place )
        {
            return ::operator new( size, place );
        }

        /// Array-object delete.
        static void operator delete [] ( void * p, std::size_t size ) throw ()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            MyAllocatorSingleton::Instance().Deallocate( p, size );
        }

        /** Non-throwing array-object delete is only called when nothrow
         new operator is used, and the constructor throws an exception.
         */
        static void operator delete [] ( void * p,
            const std::nothrow_t & ) throw()
        {
            typename MyThreadingModel::Lock lock;
            (void)lock; // get rid of warning
            MyAllocatorSingleton::Instance().Deallocate( p );
        }

        /// Placement array-object delete merely calls global placement delete.
        inline static void operator delete [] ( void * p, void * place )
        {
            ::operator delete ( p, place );
        }
#endif  // #if use new array functions.

#endif  // #if default template parameters are not zero

    protected:
        inline SmallObjectBase( void ) {}
        inline SmallObjectBase( const SmallObjectBase & ) {}
        inline SmallObjectBase & operator = ( const SmallObjectBase & ) {}
        inline ~SmallObjectBase() {}
    }; // end class SmallObjectBase


    /** @class SmallObject Base class for polymorphic small objects, offers fast
     allocations & deallocations.  Destructor is virtual and public.  Default
     constructor is trivial.   Copy-constructor and copy-assignment operator are
     not implemented since polymorphic classes almost always disable those
     operations.  Class has no data members so compilers can use
     Empty-Base-Optimization.
     */
    template
    <
        template <class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
        std::size_t chunkSize = LOKI_DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = LOKI_MAX_SMALL_OBJECT_SIZE,
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


    /** @class SmallValueObject Base class for small objects with value-type
     semantics - offers fast allocations & deallocations.  Destructor is
     non-virtual, inline, and protected to prevent unintentional destruction
     through base class.  Default constructor is trivial.   Copy-constructor
     and copy-assignment operator are trivial since value-types almost always
     need those operations.  Class has no data members so compilers can use
     Empty-Base-Optimization.
     */
    template
    <
        template <class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
        std::size_t chunkSize = LOKI_DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = LOKI_MAX_SMALL_OBJECT_SIZE,
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
//
// $Log$
// Revision 1.15  2005/10/15 19:41:23  syntheticpp
// fix bug 1327060. Add missing template parameter to make different static variables possible
//
// Revision 1.14  2005/10/13 22:43:03  rich_sposato
// Added documentation comments about lifetime policies.
//
// Revision 1.13  2005/10/07 01:22:09  rich_sposato
// Added GetLongevity function so allocator can work with a certain lifetime
// policy class used with Loki::SingletonHolder.
//
// Revision 1.12  2005/10/06 00:19:56  rich_sposato
// Added clarifying comment about destructor.
//
// Revision 1.11  2005/09/27 00:41:13  rich_sposato
// Added array forms of new and delete.
//
// Revision 1.10  2005/09/26 21:38:54  rich_sposato
// Changed include path to be direct instead of relying upon project settings.
//
// Revision 1.9  2005/09/26 07:33:04  syntheticpp
// move macros into LOKI_ namespace
//
// Revision 1.8  2005/09/09 00:24:59  rich_sposato
// Added functions to trim extra memory within allocator.  Made a new_handler
// function for allocator.  Added deallocator function for nothrow delete
// operator to insure nothing is leaked when constructor throws.
//
// Revision 1.7  2005/09/01 22:01:33  rich_sposato
// Added #ifdef to deal with MSVC warning about exception specification lists.
//
// Revision 1.6  2005/08/27 13:22:56  syntheticpp
// samll fix
//
// Revision 1.5  2005/08/25 15:49:51  syntheticpp
// small corrections
//
// Revision 1.4  2005/08/25 15:23:14  syntheticpp
// small corrections
//
// Revision 1.3  2005/07/31 14:00:48  syntheticpp
// make object level threading possible
//
// Revision 1.2  2005/07/31 13:51:31  syntheticpp
// replace old implementation with the ingeious from Rich Sposato
//
// Revision 1.2  2005/07/22 00:22:38  rich_sposato
// Added SmallValueObject, SmallObjectBase, and AllocatorSingleton classes.
//
////////////////////////////////////////////////////////////////////////////////

#endif // SMALLOBJ_INC_

