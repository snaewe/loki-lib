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
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Oct 28, 2002
// Because MSVC 6.0 does not allow a functions with a return 
// type of cv void to return an expression of type "cv void" (6.6.3), this
// port currently *does not* support void as return type.
//
// Because the VC 6.0 does not support explicit template argument specification 
// for member functions (14.8.1), I added dummy parameters to functions 
// requiring this feature.
// For example:
// The original declaration of BasicDispatcher::Add looks like this:
//
//	template <class SomeLhs, class SomeRhs>
//	void Add(CallbackType fun);

// and you call it like this:
//	obj.Add<Type1, Type2>(yourFun);
//
// This port uses:
//
//	template <class SomeLhs, class SomeRhs>
//	void Add(CallbackType fun, SomeLhs* pDummy1, SomeRhs* pDummy2);
//
// and you call it like this:
//	obj.Add(yourFun, (Type1*)0, (Type2*)0);


#ifndef MULTIMETHODS_INC_
#define MULTIMETHODS_INC_

#include "Typelist.h"
#include "TypeInfo.h"
#include "Functor.h"
#include "AssocVector.h"

////////////////////////////////////////////////////////////////////////////////
// IMPORTANT NOTE:
// The double dispatchers implemented below differ from the excerpts shown in
// the book - they are simpler while respecting the same interface.
////////////////////////////////////////////////////////////////////////////////

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template InvocationTraits (helper)
// Helps implementing optional symmetry
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template <class SomeLhs, class SomeRhs, 
			class Executor, typename ResultType>
        struct InvocationTraits
        {
            static ResultType 
			DoDispatch(SomeLhs& lhs, SomeRhs& rhs, Executor& exec, Int2Type<false>)
            {
                return exec.Fire(lhs, rhs);
            }
            static ResultType DoDispatch(	SomeLhs& lhs, SomeRhs& rhs, 
											Executor& exec, Int2Type<true>)
            {
                return exec.Fire(rhs, lhs);
            }
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template StaticDispatcher
// Implements an automatic static double dispatcher based on two typelists
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class Executor,
        class BaseLhs, 
        class TypesLhs,
        bool symmetric = true,
        class BaseRhs = BaseLhs,
        class TypesRhs = TypesLhs,
        typename ResultType = int/*void*/
    >
    class StaticDispatcher
    {
        template <class SomeLhs>
        static ResultType DispatchRhs(	SomeLhs& lhs, BaseRhs& rhs,
										Executor exec, NullType)
        { return exec.OnError(lhs, rhs); }
        
        template <class Head, class Tail, class SomeLhs>
        static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
										Executor exec, Typelist<Head, Tail>)
        {            
            if (Head* p2 = dynamic_cast<Head*>(&rhs))
            {
                Int2Type<(symmetric &&
                          int(TL::IndexOf<TypesRhs, Head>::value) <
                          int(TL::IndexOf<TypesLhs, SomeLhs>::value))> i2t;

                typedef Private::InvocationTraits< 
                        SomeLhs, Head, Executor, ResultType> CallTraits;
                    
                return CallTraits::DoDispatch(lhs, *p2, exec, i2t);
            }
            return DispatchRhs(lhs, rhs, exec, Tail());
        }

        static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
										Executor exec, NullType)
        { return exec.OnError(lhs, rhs); }
        
        template <class Head, class Tail>
        static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
										Executor exec, Typelist<Head, Tail>)
        {            
            if (Head* p1 = dynamic_cast<Head*>(&lhs))
            {
                return DispatchRhs(*p1, rhs, exec, TypesRhs());
            }
            return DispatchLhs(lhs, rhs, exec, Tail());
        }

    public:
        static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
								Executor exec)
        { return DispatchLhs(lhs, rhs, exec, TypesLhs()); }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template BasicDispatcher
// Implements a logarithmic double dispatcher for functors (or functions)
// Doesn't offer automated casts or symmetry
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class BaseLhs,
        class BaseRhs = BaseLhs,
        typename ResultType = int/*void*/,
        typename CallbackType = ResultType (*)(BaseLhs&, BaseRhs&)
    >
    class BasicDispatcher
    {
        typedef std::pair<TypeInfo,TypeInfo> KeyType;
        typedef CallbackType MappedType;
        typedef AssocVector<KeyType, MappedType> MapType;
        MapType callbackMap_;
        
        void DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun);
        bool DoRemove(TypeInfo lhs, TypeInfo rhs);
        
    public:
        template <class SomeLhs, class SomeRhs>
        void Add(CallbackType fun, SomeLhs* pDummy1, SomeRhs* pDummy2)
        {
            DoAdd(typeid(SomeLhs), typeid(SomeRhs), fun);
        }
        
        template <class SomeLhs, class SomeRhs>
        bool Remove(SomeLhs pDummy1, SomeRhs pDummy2)
        {
            return DoRemove(typeid(SomeLhs), typeid(SomeRhs));
        }
        
        ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
    };

    // Non-inline to reduce compile time overhead...
    template <class BaseLhs, class BaseRhs, 
		typename ResultType, typename CallbackType>
    void BasicDispatcher<BaseLhs,BaseRhs,ResultType,CallbackType>
         ::DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun)
    {
        callbackMap_[KeyType(lhs, rhs)] = fun;
    }
        
    template <class BaseLhs, class BaseRhs, 
        typename ResultType, typename CallbackType>
    bool BasicDispatcher<BaseLhs,BaseRhs,ResultType,CallbackType>
         ::DoRemove(TypeInfo lhs, TypeInfo rhs)
    {
        return callbackMap_.erase(KeyType(lhs, rhs)) == 1;
    }

    template <class BaseLhs, class BaseRhs, 
        typename ResultType, typename CallbackType>
    ResultType BasicDispatcher<BaseLhs,BaseRhs,ResultType,CallbackType>
               ::Go(BaseLhs& lhs, BaseRhs& rhs)
    {
        typename MapType::key_type k(typeid(lhs),typeid(rhs));
        typename MapType::iterator i = callbackMap_.find(k);
        if (i == callbackMap_.end())
        {
                throw std::runtime_error("Function not found");
        }
        return (i->second)(lhs, rhs);
    }

	struct BasicDispatcherWrapper
	{
		template <class T, class U, class V, class W>
		struct In
		{
			typedef BasicDispatcher<T, U, V, W> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template StaticCaster
// Implementation of the CastingPolicy used by FunctorDispatcher
////////////////////////////////////////////////////////////////////////////////

    template <class To, class From>
    struct StaticCaster
    {
        static To& Cast(From& obj)
        {
            return static_cast<To&>(obj);
        }
    };
	struct StaticCasterWrapper
	{
		template <class T, class U>
		struct In
		{
			typedef StaticCaster<T, U> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template DynamicCaster
// Implementation of the CastingPolicy used by FunctorDispatcher
////////////////////////////////////////////////////////////////////////////////

    template <class To, class From>
    struct DynamicCaster
    {
        static To& Cast(From& obj)
        {
            return dynamic_cast<To&>(obj);
        }
    };
	struct DynamicCasterWrapper
	{
		template <class T, class U>
		struct In
		{
			typedef DynamicCaster<T, U> type;
		};
	};

////////////////////////////////////////////////////////////////////////////////
// class template Private::FnDispatcherHelper
// Implements trampolines and argument swapping used by FnDispatcher
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        template 
		<
			class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			typename ResultType, class CastLhs, class CastRhs,
			ResultType (*Callback)(SomeLhs&, SomeRhs&)
		>
        struct FnDispatcherHelper
        {
            static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs)
            {
                return Callback(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
            static ResultType TrampolineR(BaseRhs& rhs, BaseLhs& lhs)
            {
                return Trampoline(lhs, rhs);
            }
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template FnDispatcher
// Implements an automatic logarithmic double dispatcher for functions
// Features automated conversions
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
              typename ResultType = int/*void*/,
              class CastingPolicy = DynamicCasterWrapper,
              class DispatcherBackend = BasicDispatcherWrapper>
    class FnDispatcher
    {
        ApplyInnerType4<DispatcherBackend, BaseLhs, BaseRhs, ResultType, 
            ResultType (*)(BaseLhs&, BaseRhs&)>::type backEnd_;
        
    public:
        template <class SomeLhs, class SomeRhs>
        void Add(ResultType (*pFun)(BaseLhs&, BaseRhs&), SomeLhs* pDummy1, SomeRhs* pDummy2)
        {
            return backEnd_.Add(pFun, pDummy1, pDummy2);
        }        
        
        template <class SomeLhs, class SomeRhs,
            ResultType (*callback)(SomeLhs&, SomeRhs&)>
        void Add(SomeLhs* pDummy1, SomeRhs* pDummy2)
        {
        typedef Private::FnDispatcherHelper<
                    BaseLhs, BaseRhs, 
                    SomeLhs, SomeRhs,
                    ResultType,
                    typename ApplyInnerType2<CastingPolicy, SomeLhs,BaseLhs>::type, 
                    typename ApplyInnerType2<CastingPolicy,SomeRhs,BaseRhs>::type, 
                    callback> Local;

            Add(&Local::Trampoline, (SomeLhs*)0, (SomeRhs*)0);
        }
        
        template <class SomeLhs, class SomeRhs,
            ResultType (*callback)(SomeLhs&, SomeRhs&)>
        void Add(SomeLhs* pDummy1, SomeRhs* pDummy2, bool Symmetric)
        {
        typedef Private::FnDispatcherHelper<
                    BaseLhs, BaseRhs, 
                    SomeLhs, SomeRhs,
                    ResultType,
                    typename ApplyInnerType2<CastingPolicy, SomeLhs,BaseLhs>::type, 
                    typename ApplyInnerType2<CastingPolicy,SomeRhs,BaseRhs>::type, 
                    callback> Local;

            Add(&Local::Trampoline, (SomeLhs*)0, (SomeRhs*)0);
            if (Symmetric)
            {
                Add(&Local::Trampoline, (SomeLhs*)0, (SomeRhs*)0);
            }
        }
        
        template <class SomeLhs, class SomeRhs>
        void Remove(SomeLhs* pDummy1, SomeRhs* pDummy2)
        {
            backEnd_.Remove(pDummy1, pDummy2);
        }

        ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
        {
            return backEnd_.Go(lhs, rhs);
        }
    };

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcherAdaptor
// permits use of FunctorDispatcher under gcc.2.95.2/3
///////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
    template <class BaseLhs, class BaseRhs,
          class SomeLhs, class SomeRhs,
          typename ResultType,
          class CastLhs, class CastRhs,
          class Fun, bool SwapArgs>
        class FunctorDispatcherHelper 
        {
            Fun fun_;
            ResultType Fire(BaseLhs& lhs, BaseRhs& rhs,Int2Type<false>)
            {
                return fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
            ResultType Fire(BaseLhs& rhs, BaseRhs& lhs,Int2Type<true>)
            {
                return fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
        public:
            FunctorDispatcherHelper(const Fun& fun) : fun_(fun) {}

            ResultType operator()(BaseLhs& lhs, BaseRhs& rhs)
            {
                return Fire(lhs,rhs,Int2Type<SwapArgs>());
            }
        };
    }

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcher
// Implements a logarithmic double dispatcher for functors
// Features automated casting
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
              typename ResultType = int/*void*/,
              class CastingPolicy = DynamicCasterWrapper, 
              class DispatcherBackend = BasicDispatcherWrapper>
    class FunctorDispatcher
    {
        typedef TYPELIST_2(BaseLhs&, BaseRhs&) ArgsList;
        typedef Functor<ResultType, ArgsList, DEFAULT_THREADING> FunctorType;

        ApplyInnerType4<DispatcherBackend,BaseLhs, BaseRhs, ResultType, FunctorType>::type backEnd_;

    public:
        template <class SomeLhs, class SomeRhs, class Fun>
        void Add(const Fun& fun, SomeLhs* pDummy1, SomeRhs* pDummy2)
        {
            typedef typename ApplyInnerType2<CastingPolicy,SomeLhs, BaseLhs>::type CastOne;
			typedef typename ApplyInnerType2<CastingPolicy,SomeRhs, BaseRhs>::type CastTwo;
			typedef Private::FunctorDispatcherHelper<
                    BaseLhs, BaseRhs,
                    SomeLhs, SomeRhs,
                    ResultType,
                    CastOne,
					CastTwo,
                    Fun, false> Adapter;

            
			
			backEnd_.Add(FunctorType(Adapter(fun), (int*) 0), pDummy1, pDummy2);
    }
        template <class SomeLhs, class SomeRhs, class Fun>
        void Add(const Fun& fun, SomeLhs* pDummy1, SomeRhs* pDummy2, bool symmetric)
        {
        Add(fun, pDummy1, pDummy2);

        if (symmetric)
        {
        // Note: symmetry only makes sense where BaseLhs==BaseRhs
                typedef Private::FunctorDispatcherHelper<
                    BaseLhs, BaseLhs,
                    SomeLhs, SomeRhs,
                    ResultType,
                    ApplyInnerType2<CastingPolicy,SomeLhs, BaseLhs>::type,
                    ApplyInnerType2<CastingPolicy,SomeRhs, BaseLhs>::type,
                    Fun, true> AdapterR;

                backEnd_.Add(FunctorType(Adapter(fun)), pDummy1, pDummy2);
        }
        }
        
        template <class SomeLhs, class SomeRhs>
        void Remove(SomeLhs* pDummy1, SomeRhs* pDummy2)
        {
            backEnd_.Remove(pDummy1, pDummy2);
        }

        ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
        {
            return backEnd_.Go(lhs, rhs);
        }
    };
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Oct  28, 2002: ported by Benjamin Kaufmann
////////////////////////////////////////////////////////////////////////////////

#endif
