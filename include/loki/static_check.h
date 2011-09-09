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
#ifndef LOKI_STATIC_CHECK_INC_
#define LOKI_STATIC_CHECK_INC_

// $Id$


namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// Helper structure for the STATIC_CHECK macro
////////////////////////////////////////////////////////////////////////////////

    template<int> struct CompileTimeError;
    template<> struct CompileTimeError<true> {};
}

////////////////////////////////////////////////////////////////////////////////
// macro LOKI_STATIC_CHECK
// Invocation: LOKI_STATIC_CHECK(expr, id)
// where:
// expr is a compile-time integral or pointer expression
// id is a C++ identifier that does not need to be defined
// If expr is zero, id will appear in a compile-time error message.
////////////////////////////////////////////////////////////////////////////////

#define LOKI_CONCAT( X, Y ) LOKI_CONCAT_SUB( X, Y )
#define LOKI_CONCAT_SUB( X, Y ) X##Y

#define LOKI_STATIC_CHECK(expr, msg) enum { LOKI_CONCAT(ERROR_##msg, __LINE__) = sizeof(Loki::CompileTimeError<expr != 0 >) }


#endif // end file guardian

