////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (C) 2009 Andy Balaam
// Copyright (c) 2009 Peter Kümmel
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

#ifndef LOKI_FOR_EACH_TYPE
#define LOKI_FOR_EACH_TYPE

#include <loki/NullType.h>
#include <loki/Typelist.h>

namespace Loki
{

    ////////////////////////////////////////////////////////////////////////////////
    // class template ForEachType
    // Calls a templated callable for every element of a Typelist
    // Supplies an int template parameter for the position in the TypeList.
    // Invocation (TList is a typelist):
    // ForEachType<TList> dummy();
    // Calls the supplied method during construction of the object dummy.
    ////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        // type of recursive function
        template <class TList, class Callable>
        struct ForEachTypeImpl;

        // Recursion rule
        template <class Head, class Tail, class Callable>
        struct ForEachTypeImpl<Typelist<Head, Tail>, Callable>
            :  public ForEachTypeImpl<Tail, Callable>
        {
            enum { value = 1 + ForEachTypeImpl<Tail, Callable>::value };

            ForEachTypeImpl( Callable& callable ) : ForEachTypeImpl<Tail, Callable>(callable)
            {
#ifdef _MSC_VER
                callable.operator()<value, Head>();
#else
                callable.template operator()<value, Head>();
#endif
            }

        };

        // Recursion end
        template <class Head, class Callable>
        struct ForEachTypeImpl<Typelist<Head, NullType>, Callable>
        {
        public:

            enum { value = 0 };

            ForEachTypeImpl( Callable& callable )
            {
#ifdef _MSC_VER
                callable.operator()<value, Head>();
#else
                callable.template operator()<value, Head>();
#endif
            }
        };


    }


    struct OrderPolicyForward;
    struct OrderPolicyBackward;

    template <class TList, class Callable, class OrderPolicy = OrderPolicyForward>
    struct ForEachType;

    template <class TList, class Callable >
    struct ForEachType<TList, Callable, OrderPolicyForward>
      : public Private::ForEachTypeImpl<typename TL::Reverse<TList>::Result, Callable >
    {
        ForEachType( Callable& callable )
        : Private::ForEachTypeImpl<typename TL::Reverse<TList>::Result, Callable >( callable )
        {
        }
    };

    template <class TList, class Callable >
    struct ForEachType<TList, Callable, OrderPolicyBackward>
    : public Private::ForEachTypeImpl< TList, Callable >
    {
        ForEachType( Callable& callable )
        : Private::ForEachTypeImpl< TList, Callable >( callable )
        {
        }
    };


}

#endif

