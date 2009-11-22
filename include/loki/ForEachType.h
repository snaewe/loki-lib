
////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (C) 2009 Andy Balaam
// Copyright (c) 2009 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
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

