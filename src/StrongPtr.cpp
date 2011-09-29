////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Rich Sposato
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

// $Id$


#include <loki/StrongPtr.h>

#include <memory.h>
#ifdef DO_EXTRA_LOKI_TESTS
    #include <cassert>
#endif

#include <loki/SmallObj.h>

#if defined( _MSC_VER )
    #pragma warning( push )
    #pragma warning( disable: 4355 )
#endif


// ----------------------------------------------------------------------------

namespace Loki
{

namespace Private
{

// ----------------------------------------------------------------------------

OneOwnerRefCountInfo::OneOwnerRefCountInfo( SingleOwnerRefCount * ptr )
    : m_pointer( NULL )
    , m_strongPtr( ptr )
    , m_weakCount( ( NULL == ptr ) ? 1 : 0 )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

OneOwnerRefCountInfo::OneOwnerRefCountInfo( const void * p,
    SingleOwnerRefCount * ptr )
    : m_pointer( p )
    , m_strongPtr( ptr )
    , m_weakCount( ( NULL == ptr ) ? 1 : 0 )
{
    assert( NULL != this );
}

// ----------------------------------------------------------------------------

void OneOwnerRefCountInfo::SetStrongCoPtr( SingleOwnerRefCount * ptr )
{
    assert( NULL != this );
    m_strongPtr = ptr;
}

// ----------------------------------------------------------------------------

} // end namespace Private

// ----------------------------------------------------------------------------

TwoRefCounts::TwoRefCounts( bool strong )
    : m_counts( NULL )
{
    void * temp = SmallObject<>::operator new(
        sizeof(Loki::Private::TwoRefCountInfo) );
#ifdef DO_EXTRA_LOKI_TESTS
    assert( temp != 0 );
#endif
    m_counts = new ( temp ) Loki::Private::TwoRefCountInfo( strong );
}

// ----------------------------------------------------------------------------

TwoRefCounts::TwoRefCounts( const void * p, bool strong )
    : m_counts( NULL )
{
    void * temp = SmallObject<>::operator new(
        sizeof(Loki::Private::TwoRefCountInfo) );
#ifdef DO_EXTRA_LOKI_TESTS
    assert( temp != 0 );
#endif
    void * p2 = const_cast< void * >( p );
    m_counts = new ( temp ) Loki::Private::TwoRefCountInfo( p2, strong );
}

// ----------------------------------------------------------------------------

TwoRefCounts::TwoRefCounts( const TwoRefCounts & rhs, bool isNull, bool strong )
    : m_counts( ( isNull ) ? NULL : rhs.m_counts )
{
    if ( isNull )
    {
        void * temp = SmallObject<>::operator new(
            sizeof(Loki::Private::TwoRefCountInfo) );
        assert( temp != 0 );
        m_counts = new ( temp ) Loki::Private::TwoRefCountInfo( strong );
    }
    else
    {
        Increment( strong );
    }
}

// ----------------------------------------------------------------------------

void TwoRefCounts::Increment( bool strong )
{
    if ( strong )
    {
        m_counts->IncStrongCount();
    }
    else
    {
        m_counts->IncWeakCount();
    }
}

// ----------------------------------------------------------------------------

bool TwoRefCounts::Decrement( bool strong )
{
    if ( strong )
    {
        m_counts->DecStrongCount();
    }
    else
    {
        m_counts->DecWeakCount();
    }
    return !m_counts->HasStrongPointer();
}

// ----------------------------------------------------------------------------

void TwoRefCounts::Swap( TwoRefCounts & rhs )
{
    ::std::swap( m_counts, rhs.m_counts );
}

// ----------------------------------------------------------------------------

void TwoRefCounts::ZapPointer( void )
{
#ifdef DO_EXTRA_LOKI_TESTS
    assert( !m_counts->HasStrongPointer() );
#endif
    if ( m_counts->HasWeakPointer() )
    {
        m_counts->ZapPointer();
    }
    else
    {
        SmallObject<>::operator delete ( m_counts,
            sizeof(Loki::Private::TwoRefCountInfo) );
        m_counts = NULL;
    }
}

// ----------------------------------------------------------------------------

SingleOwnerRefCount::SingleOwnerRefCount( bool strong )
    : m_info( NULL )
{
    assert( NULL != this );

    void * temp = SmallObject<>::operator new(
        sizeof(Loki::Private::OneOwnerRefCountInfo) );
#ifdef DO_EXTRA_LOKI_TESTS
    assert( temp != 0 );
#endif

    SingleOwnerRefCount * ptr = ( strong ) ? this : NULL;
    m_info = new ( temp ) ::Loki::Private::OneOwnerRefCountInfo( ptr );
}

// ----------------------------------------------------------------------------

SingleOwnerRefCount::SingleOwnerRefCount( const void * p, bool strong )
    : m_info( NULL )
{
    assert( NULL != this );

    void * temp = SmallObject<>::operator new(
        sizeof(Loki::Private::OneOwnerRefCountInfo) );
#ifdef DO_EXTRA_LOKI_TESTS
    assert( temp != 0 );
#endif

    SingleOwnerRefCount * ptr = ( strong ) ? this : NULL;
    m_info = new ( temp ) ::Loki::Private::OneOwnerRefCountInfo( p, ptr );
}

// ----------------------------------------------------------------------------

SingleOwnerRefCount::SingleOwnerRefCount( const SingleOwnerRefCount & rhs,
    bool strong )
    : m_info( NULL )
{
    assert( NULL != this );

    if ( strong && rhs.HasStrongPointer() )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }

    m_info = rhs.m_info;
    if ( strong )
    {
        m_info->SetStrongCoPtr( this );
    }
    else
    {
        m_info->IncWeakCount();
    }
}

// ----------------------------------------------------------------------------

SingleOwnerRefCount::SingleOwnerRefCount( const SingleOwnerRefCount & rhs,
    bool isNull, bool strong )
    : m_info( NULL )
{
    assert( NULL != this );

    if ( isNull )
    {
        void * temp = SmallObject<>::operator new(
            sizeof(Loki::Private::OneOwnerRefCountInfo) );
#ifdef DO_EXTRA_LOKI_TESTS
        assert( temp != 0 );
#endif

        SingleOwnerRefCount * ptr = ( strong ) ? this : NULL;
        m_info = new ( temp ) ::Loki::Private::OneOwnerRefCountInfo( ptr );
        return;
    }

    if ( strong && rhs.HasStrongPointer() )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }

    m_info = rhs.m_info;
    if ( strong )
    {
        m_info->SetStrongCoPtr( this );
    }
    else
    {
        m_info->IncWeakCount();
    }
}

// ----------------------------------------------------------------------------

void SingleOwnerRefCount::Swap( SingleOwnerRefCount & rhs )
{
    assert( NULL != this );

    if ( IsStrong() && rhs.IsStrong() )
    {
        // These two strong pointers are trading resources.
        SingleOwnerRefCount * temp = rhs.m_info->GetStrongCoPointer();
        rhs.m_info->SetStrongCoPtr( this );
        m_info->SetStrongCoPtr( temp );
    }
    ::std::swap( m_info, rhs.m_info );
}

// ----------------------------------------------------------------------------

bool SingleOwnerRefCount::Release( bool strong )
{
    assert( NULL != this );

    assert( strong == IsStrong() );
    if ( strong )
    {
        m_info->SetStrongCoPtr( NULL );
        return true;
    }

    assert( 0 < m_info->GetWeakCount() );
    m_info->DecWeakCount();
    const bool noOwner = ( !m_info->HasStrongPointer() );
    return ( ( 0 == m_info->GetWeakCount() ) && noOwner );
}

// ----------------------------------------------------------------------------

void SingleOwnerRefCount::ZapPointer( void )
{
    assert( !m_info->HasStrongPointer() );
    if ( m_info->HasWeakPointer() )
    {
        m_info->ZapPointer();
    }
    else
    {
        SmallObject<>::operator delete ( m_info,
            sizeof(Loki::Private::OneOwnerRefCountInfo) );
        m_info = NULL;
    }
}

// ----------------------------------------------------------------------------

void SingleOwnerRefCount::SetPointer( void * p )
{
    assert( NULL != this );
    if ( IsStrong() || ( 1 == m_info->GetWeakCount() ) )
    {
        // Only a strong pointer or the last weak pointer may change a resource.
        m_info->SetPointer( p );
    }
}

// ----------------------------------------------------------------------------

TwoRefLinks::TwoRefLinks( const void * p, bool strong )
    : m_pointer( const_cast< void * >( p ) )
    , m_prev( this )
    , m_next( this )
    , m_strong( strong )
{
    assert( IsValid() );
}

// ----------------------------------------------------------------------------

TwoRefLinks::TwoRefLinks( const TwoRefLinks & rhs, bool strong )
    : m_pointer( rhs.m_pointer )
    , m_prev( const_cast< TwoRefLinks * >( &rhs ) )
    , m_next( rhs.m_next )
    , m_strong( strong )
{
    m_prev->m_next = this;
    m_next->m_prev = this;

#ifdef DO_EXTRA_LOKI_TESTS
    assert( rhs.m_next->HasNextNode( this ) );
    assert( rhs.m_prev->HasPrevNode( this ) );
    assert( CountPrevCycle( this ) == CountNextCycle( &rhs ) );
#endif
    assert( IsValid() );
}

// ----------------------------------------------------------------------------

TwoRefLinks::TwoRefLinks( const TwoRefLinks & rhs, bool isNull, bool strong )
    : m_pointer( ( isNull ) ? 0 : rhs.m_pointer )
    , m_prev( ( isNull ) ? 0 : const_cast< TwoRefLinks * >( &rhs ) )
    , m_next( ( isNull ) ? 0 : rhs.m_next )
    , m_strong( strong )
{
    assert( rhs.IsValid() );

    if ( isNull )
    {
        m_prev = m_next = this;
    }
    else
    {
        m_prev->m_next = this;
        m_next->m_prev = this;
        assert( rhs.m_next->HasNextNode( this ) );
        assert( rhs.m_prev->HasPrevNode( this ) );
        assert( CountPrevCycle( this ) == CountNextCycle( &rhs ) );
    }
    assert( IsValid() );
}

// ----------------------------------------------------------------------------

TwoRefLinks::~TwoRefLinks( void )
{
    assert( IsValid() );

    if ( ( NULL != m_prev ) && ( this != m_prev ) )
    {
        TwoRefLinks * next = m_next;
        m_prev->m_next = next;
        m_next->m_prev = m_prev;
    }
}

// ----------------------------------------------------------------------------

void TwoRefLinks::SetPointer( void * p )
{
    assert( IsValid() );

    TwoRefLinks * node = m_next;
    if ( ( this == node ) || ( NULL == node ) )
    {
        m_pointer = p;
        return;
    }

    while ( node != this )
    {
        node->m_pointer = p;
        node = node->m_next;
    }
    m_pointer = p;

    assert( IsValid() );
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::Release( bool strong )
{
    assert( IsValid() );
    assert( strong == m_strong );
    (void)strong;

    if ( NULL == m_next )
    {
        // Return false so it does not try to destroy shared object
        // more than once.
        return false;
    }
    else if ( m_next == this )
    {
        // Set these to NULL to prevent re-entrancy.
        m_prev = NULL;
        m_next = NULL;
        // Last one in the cycle has to release the pointer.
        return true;
    }

    assert( this != m_prev );
    assert( NULL != m_prev );
    // If a single node is strong, then return false so it won't release.
    if ( HasStrongPointer() )
    {
        // A cyclic chain of pointers is only as strong as the strongest link.
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
        m_next = this;
        m_prev = this;
        assert( IsValid() );
        return false;
    }

    assert( IsValid() );
    return true;
}

// ----------------------------------------------------------------------------

void TwoRefLinks::ZapAllNodes( void )
{
    assert( IsValid() );

    TwoRefLinks * p = m_prev;
    if ( ( this == p ) || ( NULL == p ) )
        return;
#ifdef DO_EXTRA_LOKI_TESTS
    assert( AllNodesHaveSamePointer() );
#endif

    while ( p != this )
    {
        TwoRefLinks * p1 = p->m_prev;
        p->m_pointer = NULL;
        p->m_next = p;
        p->m_prev = p;
        p = p1;
    }
    m_pointer = NULL;
    m_next = this;
    m_prev = this;

    assert( IsValid() );
}

// ----------------------------------------------------------------------------

void TwoRefLinks::Swap( TwoRefLinks & rhs )
{
    assert( IsValid() );
    assert( rhs.IsValid() );
    if ( GetPointer() == rhs.GetPointer() )
        return;

    ::std::swap( rhs.m_pointer, m_pointer );
    if (m_next == this)
    {
#ifdef DO_EXTRA_LOKI_TESTS
        // This is in a cycle by itself.
        assert(m_prev == this);
#endif
        if (rhs.m_next == &rhs)
        {
#ifdef DO_EXTRA_LOKI_TESTS
            assert(rhs.m_prev == &rhs);
#endif
            // both are in 1-node cycles - thus there is nothing to do.
            return;
        }
        m_prev = rhs.m_prev;
        m_next = rhs.m_next;
        m_prev->m_next = m_next->m_prev = this;
        rhs.m_next = rhs.m_prev = &rhs;
        return;
    }
    if (rhs.m_next == &rhs)
    {
#ifdef DO_EXTRA_LOKI_TESTS
        // rhs is in a cycle by itself.
        assert( rhs.m_prev == &rhs );
#endif
        rhs.m_prev = m_prev;
        rhs.m_next = m_next;
        m_prev->m_next = m_next->m_prev = &rhs;
        m_next = m_prev = this;
        return;
    }
    if (m_next == &rhs ) // rhs is next neighbour
    {
        if ( m_prev == &rhs )
            return;  // cycle of 2 pointers - no need to swap.
        std::swap(m_prev, m_next);
        std::swap(rhs.m_prev, rhs.m_next);
        std::swap(rhs.m_prev, m_next);
        std::swap(rhs.m_prev->m_next,m_next->m_prev);
    }
    else if ( m_prev == &rhs ) // rhs is prev neighbor
    {
        if ( m_next == &rhs )
            return;  // cycle of 2 pointers - no need to swap.
        std::swap( m_prev, m_next );
        std::swap( rhs.m_next, rhs.m_prev );
        std::swap( rhs.m_next, m_prev );
        std::swap( rhs.m_next->m_prev, m_prev->m_next );
    }
    else // not neighhbors
    {
        std::swap(m_prev, rhs.m_prev);
        std::swap(m_next, rhs.m_next);
        std::swap(m_prev->m_next, rhs.m_prev->m_next);
        std::swap(m_next->m_prev, rhs.m_next->m_prev);
    }

    assert( IsValid() );
    assert( rhs.IsValid() );
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::AllNodesHaveSamePointer( void ) const
{
    const TwoRefLinks * next = m_next;
    if ( NULL == next )
        return true;
    do
    {
        if ( next->m_pointer != m_pointer )
            return false;
        next = next->m_next;
    } while ( next != this );
    return true;
}

// ----------------------------------------------------------------------------

unsigned int TwoRefLinks::CountPrevCycle( const TwoRefLinks * pThis )
{
    if ( NULL == pThis )
        return 0;
    const TwoRefLinks * p = pThis->m_prev;
    if ( NULL == p )
        return 0;
    if ( pThis == p )
        return 1;

    unsigned int count = 1;
    do
    {
        p = p->m_prev;
        ++count;
    } while ( p != pThis );

    return count;
}

// ----------------------------------------------------------------------------

unsigned int TwoRefLinks::CountNextCycle( const TwoRefLinks * pThis )
{
    if ( NULL == pThis )
        return 0;
    const TwoRefLinks * p = pThis->m_next;
    if ( NULL == p )
        return 0;
    if ( pThis == p )
        return 1;

    unsigned int count = 1;
    while ( p != pThis )
    {
        p = p->m_next;
        ++count;
    }

    return count;
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::HasPrevNode( const TwoRefLinks * p ) const
{
    if ( this == p )
        return true;

    const TwoRefLinks * prev = m_prev;
    if ( NULL == prev )
        return false;
    while ( prev != this )
    {
        if ( p == prev )
            return true;
        prev = prev->m_prev;
    }
    return false;
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::HasNextNode( const TwoRefLinks * p ) const
{
    if ( this == p )
        return true;

    const TwoRefLinks * next = m_next;
    if ( NULL == next )
        return false;
    while ( next != this )
    {
        if ( p == next )
            return true;
        next = next->m_next;
    }
    return false;
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::HasStrongPointer( void ) const
{
    assert( IsValid() );

    const TwoRefLinks * next = m_next;
    if ( NULL == next )
        return false;
    while ( next != this )
    {
        if ( next->m_strong )
            return true;
        next = next->m_next;
    }
    return false;
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::Merge( TwoRefLinks & rhs )
{
    assert( IsValid() );
    assert( rhs.IsValid() );

    if ( NULL == m_next )
    {
        assert( NULL == m_prev );
        return false;
    }
    TwoRefLinks * prhs = &rhs;
    if ( prhs == this )
        return true;
    if ( NULL == prhs->m_next )
    {
        assert( NULL == prhs->m_prev );
        return true;
    }

#ifdef DO_EXTRA_LOKI_TESTS
    assert( CountPrevCycle( prhs ) == CountNextCycle( prhs ) );
#endif
    // If rhs node is already in this cycle, then no need to merge.
    if ( HasPrevNode( &rhs ) )
    {
#ifdef DO_EXTRA_LOKI_TESTS
        assert( HasNextNode( &rhs ) );
#endif
        return true;
    }

    if ( prhs == prhs->m_next )
    {
        /// rhs is in a cycle with 1 node.
        assert( prhs->m_prev == prhs );
        prhs->m_prev = m_prev;
        prhs->m_next = this;
        m_prev->m_next = prhs;
        m_prev = prhs;
    }
    else if ( this == m_next )
    {
        /// this is in a cycle with 1 node.
        assert( m_prev == this );
        m_prev = prhs->m_prev;
        m_next = prhs;
        prhs->m_prev->m_next = this;
        prhs->m_prev = this;
    }
    else
    {
        m_next->m_prev = prhs->m_prev;
        prhs->m_prev->m_next = m_prev;
        m_next = prhs;
        prhs->m_prev = this;
    }


#ifdef DO_EXTRA_LOKI_TESTS
    assert( IsValid() );
#endif

    return true;
}

// ----------------------------------------------------------------------------

unsigned int TwoRefLinks::GetStrongPointerCount( void ) const
{
    assert( IsValid() );

    unsigned int strongCount = ( m_strong ) ? 1 : 0;
    const TwoRefLinks * next = m_next;
    if ( ( this == next ) || ( NULL == next ) )
        return strongCount;

    while ( next != this )
    {
        if ( next->m_strong )
        {
            ++strongCount;
        }
        next = next->m_next;
    }

    return strongCount;
}

// ----------------------------------------------------------------------------

const TwoRefLinks * TwoRefLinks::GetNextStrongPointer( void ) const
{
    assert( IsValid() );

    for ( const TwoRefLinks * next = m_next;
        ( next != this ); next = next->m_next )
    {
        if ( next->m_strong )
        {
            return next;
        }
    }

    return this;
}

// ----------------------------------------------------------------------------

bool TwoRefLinks::IsValid( void ) const
{
    assert( NULL != this );

    const bool isThisNext = ( m_next == this );
    const bool isThisPrev = ( m_prev == this );
    assert( isThisNext == isThisPrev );
    (void)isThisNext;
    (void)isThisPrev;

    const bool isNextNull = ( m_next == NULL );
    const bool isPrevNull = ( m_prev == NULL );
    assert( isNextNull == isPrevNull );
    (void)isNextNull;
    (void)isPrevNull;

    if ( NULL != m_prev )
    {
        assert( NULL != m_next );
        assert( this == m_next->m_prev );
        assert( this == m_prev->m_next );
        assert( m_prev->HasPrevNode( this ) );
    }
    if ( NULL != m_next )
    {
        assert( NULL != m_prev );
        assert( this == m_next->m_prev );
        assert( this == m_prev->m_next );
        assert( m_next->HasNextNode( this ) );
    }
    const unsigned int prevCycleCount = CountPrevCycle( this );
    const unsigned int nextCycleCount = CountNextCycle( this );
    assert( prevCycleCount == nextCycleCount );
    (void)prevCycleCount;
    (void)nextCycleCount;

    assert( AllNodesHaveSamePointer() );

    return true;
}

// ----------------------------------------------------------------------------

SingleOwnerRefLinks::SingleOwnerRefLinks( bool strong )
    : TwoRefLinks( strong )
{
    assert( NULL != this );
    const unsigned int strongCount = GetStrongPointerCount();
    if ( 1 < strongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
}

// ----------------------------------------------------------------------------

SingleOwnerRefLinks::SingleOwnerRefLinks( const void * p, bool strong )
    : TwoRefLinks( p, strong )
{
    assert( NULL != this );
    const unsigned int strongCount = GetStrongPointerCount();
    if ( 1 < strongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
}

// ----------------------------------------------------------------------------

SingleOwnerRefLinks::SingleOwnerRefLinks( const SingleOwnerRefLinks & rhs, bool strong )
    : TwoRefLinks( rhs, strong )
{
    assert( NULL != this );
    const unsigned int strongCount = rhs.GetStrongPointerCount();
    if ( 1 < strongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
}

// ----------------------------------------------------------------------------

SingleOwnerRefLinks::SingleOwnerRefLinks( const SingleOwnerRefLinks & rhs,
    bool isNull, bool strong )
    : TwoRefLinks( rhs, isNull, strong )
{
    assert( NULL != this );
    const unsigned int strongCount = GetStrongPointerCount();
    if ( 1 < strongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
}

// ----------------------------------------------------------------------------

SingleOwnerRefLinks::~SingleOwnerRefLinks( void )
{
    assert( NULL != this );
    const unsigned int strongCount = GetStrongPointerCount();
    // Use assert instead of throw inside a destructor.
    assert( strongCount < 2 );
    (void)strongCount;
}

// ----------------------------------------------------------------------------

void SingleOwnerRefLinks::Swap( SingleOwnerRefLinks & rhs )
{
    assert( NULL != this );
    if ( GetPointer() == rhs.GetPointer() )
        return;

    const unsigned int lhsStrongCount = GetStrongPointerCount();
    if ( 1 < lhsStrongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
    const unsigned int rhsStrongCount = rhs.GetStrongPointerCount();
    if ( 1 < rhsStrongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }

    TwoRefLinks::Swap( rhs );
}

// ----------------------------------------------------------------------------

bool SingleOwnerRefLinks::Merge( SingleOwnerRefLinks & rhs )
{
    assert( NULL != this );

    const unsigned int lhsStrongCount = GetStrongPointerCount();
    if ( 1 < lhsStrongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
    const unsigned int rhsStrongCount = rhs.GetStrongPointerCount();
    if ( 1 < rhsStrongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }
    if ( 1 < lhsStrongCount + rhsStrongCount )
    {
        throw ::std::logic_error( StrongPtr_Single_Owner_Exception_Message );
    }

    const bool success = TwoRefLinks::Merge( rhs );
    return success;
}

// ----------------------------------------------------------------------------

bool SingleOwnerRefLinks::Release( bool strong )
{
    assert( NULL != this );

    if ( strong )
    {
        const TwoRefLinks * pStrong = GetNextStrongPointer();
        if ( this != pStrong )
        {
            // There is another strong pointer in this linked pointer cycle,
            // so just return false to prevent this strong pointer from
            // deleting the same resource multiple times.
            TwoRefLinks::Release( strong );
            return false;
        }
    }

    const unsigned int strongCount = GetStrongPointerCount();
    // Use assert instead of throw because Release function only gets called
    // from StrongPtr destructor, and destructors must not throw exceptions.
    assert( strongCount < 2 );
    (void)strongCount;

    const bool doZap = TwoRefLinks::Release( strong );
    return doZap;
}

// ----------------------------------------------------------------------------

#if defined( _MSC_VER )
    #pragma warning( pop )
#endif

} // end namespace Loki
