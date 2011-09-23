////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code is from the article:
//     "Generic<Programming>: volatile — Multithreaded Programmer’s Best Friend
//     Volatile-Correctness or How to Have Your Compiler Detect Race Conditions
//     for You" by Alexandrescu, Andrei.
//     Published in the February 2001 issue of the C/C++ Users Journal.
//     http://www.cuj.com/documents/s=7998/cujcexp1902alexandr/
//
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
//
// Prepared for Loki library by Richard Sposato
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_LOCKING_PTR_INC_
#define LOKI_LOCKING_PTR_INC_

// $Id$


#include <loki/ConstPolicy.h>
#include <loki/Threads.h>


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
    template < typename SharedObject, typename LockingPolicy = LOKI_DEFAULT_MUTEX,
               template<class> class ConstPolicy = LOKI_DEFAULT_CONSTNESS >
    class LockingPtr
    {
    public:

        typedef typename ConstPolicy<SharedObject>::Type ConstOrNotType;

        /** Constructor locks mutex associated with an object.
         @param object Reference to object.
         @param mutex Mutex used to control thread access to object.
         */
        LockingPtr( volatile ConstOrNotType & object, LockingPolicy & mutex )
           : pObject_( const_cast< SharedObject * >( &object ) ),
            pMutex_( &mutex )
        {
            mutex.Lock();
        }

        typedef typename std::pair<volatile ConstOrNotType *, LockingPolicy *> Pair;

        /** Constructor locks mutex associated with an object.
         @param lockpair a std::pair of pointers to the object and the mutex
         */
        explicit LockingPtr( Pair lockpair )
           : pObject_( const_cast< SharedObject * >( lockpair.first ) ),
            pMutex_( lockpair.second )
        {
            lockpair.second->Lock();
        }

        /// Destructor unlocks the mutex.
        ~LockingPtr()
        {
            pMutex_->Unlock();
        }

        /// Star-operator dereferences pointer.
        ConstOrNotType & operator * ()
        {
            return *pObject_;
        }

        /// Point-operator returns pointer to object.
        ConstOrNotType * operator -> ()
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
        ConstOrNotType * pObject_;

        /// Pointer to the mutex.
        LockingPolicy * pMutex_;

    }; // end class LockingPtr

} // namespace Loki

#endif // end file guardian

