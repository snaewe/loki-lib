////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code is from the article:
//     "Generic<Programming>: volatile — Multithreaded Programmer’s Best Friend
//     Volatile-Correctness or How to Have Your Compiler Detect Race Conditions
//     for You" by Alexandrescu, Andrei.
//     Published in the February 2001 issue of the C/C++ Users Journal.
//     http://www.cuj.com/documents/s=7998/cujcexp1902alexandr/
// Prepared for Loki library by Richard Sposato
////////////////////////////////////////////////////////////////////////////////

// $Header$


#ifndef LOKI_LOCKING_PTR_INC_
#define LOKI_LOCKING_PTR_INC_

namespace Loki
{

    /** @class LockingPtr
     Locks a volatile object and casts away volatility so that the object
     can be safely used in a single-threaded region of code.
     Original version of LockingPtr had only one template - for the shared
     object, but not the mutex type.  This version allows users to specify a
     the mutex type as a LockingPolicy class.  The only requirements for a
     LockingPolicy class are to provide Lock and Unlock methods.
     */
    template < typename SharedObject, typename LockingPolicy >
    class LockingPtr
    {
    public:

        /** Constructor locks mutex associated with an object.
         @param obj Reference to object.
         @param mtx Mutex used to control thread access to object.
         */
        LockingPtr( volatile SharedObject & object, LockingPolicy & mutex )
           : pObject_( const_cast< SharedObject * >( &object ) ),
            pMutex_( &mutex )
        {
            mutex.Lock();
        }

        /// Destructor unlocks the mutex.
        ~LockingPtr()
        {
            pMutex_->Unlock();
        }

        /// Star-operator dereferences pointer.
        SharedObject & operator * ()
        {
            return *pObject_;
        }

        /// Point-operator returns pointer to object.
        SharedObject * operator -> ()
        {
            return pObject_;
        }

    private:

        /// Default constructor is not implemented.
        LockingPtr();

        /// Copy-constructor is not implemented.
        LockingPtr( const LockingPtr & );

        /// Copy-assignment-operator is not implemented.
        LockingPtr & operator = ( const LockingPtr & );

        /// Pointer to the shared object.
        SharedObject * pObject_;

        /// Pointer to the mutex.
        LockingPolicy * pMutex_;

    }; // end class LockingPtr

} // namespace Loki

#endif  // end file guardian

// $Log$
// Revision 1.3  2006/01/16 18:34:37  rich_sposato
// Changed return type from LockingPtr to SharedObject.
//
// Revision 1.2  2006/01/14 00:20:10  syntheticpp
// remove c&p error
//
// Revision 1.1  2005/11/19 22:00:23  rich_sposato
// Adding LockingPtr class to Loki project.
//
