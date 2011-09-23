////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Richard Sposato
// Copyright (c) 2006 Peter Kümmel
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
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_CONST_POLICY_INC_
#define LOKI_CONST_POLICY_INC_

// $Id$


namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
/// @note These policy classes are used in LockingPtr and SmartPtr to define
///  how const is propagated from the pointee.
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
///  \class DontPropagateConst
///
///  \ingroup ConstGroup
///  Don't propagate constness of pointed or referred object.
////////////////////////////////////////////////////////////////////////////////

    template< class T >
    struct DontPropagateConst
    {
        typedef T Type;
    };

////////////////////////////////////////////////////////////////////////////////
///  \class PropagateConst
///
///  \ingroup ConstGroup
///  Propagate constness of pointed or referred object.
////////////////////////////////////////////////////////////////////////////////

    template< class T >
    struct PropagateConst
    {
        typedef const T Type;
    };

// default will not break existing code
#ifndef LOKI_DEFAULT_CONSTNESS
#define LOKI_DEFAULT_CONSTNESS ::Loki::DontPropagateConst
#endif

} // end namespace Loki

#endif // end file guardian
