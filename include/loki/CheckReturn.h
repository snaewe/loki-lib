////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2007 by Rich Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef LOKI_CHECK_RETURN_INC_
#define LOKI_CHECK_RETURN_INC_

// $Id$


#include <assert.h>

namespace Loki
{

// ----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
///  \class CheckReturn
///
///  \par Purpose
///  C++ provides no mechanism within the language itself to force code to
///  check the return value from a function call.  This simple class provides
///  a mechanism by which programmers can force calling functions to check the
///  return value.  Or at least make them consciously choose to disregard the
///  return value.  If the calling function fails to use or store the return
///  value, the destructor asserts.
///
///  \par Return Type
///  The returned value is copied into CheckReturn rather than accessed via a
///  a reference or pointer since return value could be local to a function.
///  CheckReturn works best when the return type is a built-in primitive (bool,
///  int, etc...) a pointer, or an enum (such as an error condition enum).  It
///  can work with other types that have cheap copy operations.
////////////////////////////////////////////////////////////////////////////////

template < class Value >
class CheckReturn
{
public:

    /// Conversion constructor changes Value type to CheckReturn type.
    inline CheckReturn( Value value ) :
        m_value( value ), m_checked( false ) {}

    /// Copy-constructor allows functions to call another function within the
    /// return statement.  The other CheckReturn's m_checked flag is set since
    /// its duty has been passed to the m_checked flag in this one.
    inline CheckReturn( const CheckReturn & that ) :
        m_value( that.m_value ), m_checked( false )
    { that.m_checked = true; }

    /// Destructor checks if return value was used.
    inline ~CheckReturn( void )
    {
        // If this assertion fails, then a function failed to check the
        // return value from a function call.
        assert( m_checked );
    }

    /// Conversion operator changes CheckReturn back to Value type.
    inline operator Value ( void )
    {
        m_checked = true;
        return m_value;
    }

private:
    /// Default constructor not implemented.
    CheckReturn( void );

    /// Copy-assignment operator not implemented.
    CheckReturn & operator = ( const CheckReturn & that );

    /// Copy of returned value.
    Value m_value;

    /// Flag for whether calling function checked return value yet.
    mutable bool m_checked;
};

// ----------------------------------------------------------------------------

} // namespace Loki

#endif // end file guardian

// $Log$
