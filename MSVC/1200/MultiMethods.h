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

// Last update: Mar 06, 2003
// All dispatchers now have void as default value for return types.
// All dispatchers now support void as return type. 
// 
//
// The port now uses Tye2Type-parameters instead of plain pointers as
// a workaround of VC's explicit template argument specification bug.
// 
// For example:
// The original declaration of BasicDispatcher::Add looks like this:
//
//	template <class SomeLhs, class SomeRhs>
//	void Add(CallbackType fun);
// 
// and you call it like this:
//	obj.Add<Type1, Type2>(yourFun);
//
// This port uses:
//
//	template <class SomeLhs, class SomeRhs>
//	void Add(CallbackType fun, Type2Type<SomeLhs>, Type2Type<SomeRhs>);
//
// and you call it like this:
//	obj.Add(yourFun, Type2Type<Type1>(), Type2Type<Type2>());


#ifndef MULTIMETHODS_INC_
#define MULTIMETHODS_INC_

#include "Typelist.h"
#include "LokiTypeInfo.h"
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
// Implementation helpers for StaticDispatcher
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
	
	template <class SomeLhs, class SomeRhs, class Executor>
    struct InvocationTraitsVoid
    {
        typedef void ResultType;
		static ResultType 
		DoDispatch(SomeLhs& lhs, SomeRhs& rhs, Executor& exec, Int2Type<false>)
        {
            exec.Fire(lhs, rhs);
        }
        static ResultType DoDispatch(	SomeLhs& lhs, SomeRhs& rhs, 
										Executor& exec, Int2Type<true>)
        {
            exec.Fire(rhs, lhs);
        }
    };
	
	// Implementation for StaticDispatcher with result type != void
	template<class Executor, class BaseLhs, class TypesLhs, bool symmetric,
		class BaseRhs, class TypesRhs, typename ResultType>
	class StaticDispatcherBase
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

	protected:
		~StaticDispatcherBase() {}
	public:
		static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
								Executor exec)
		{ return DispatchLhs(lhs, rhs, exec, TypesLhs()); }
	};

	// Implementation for StaticDispatcher with result type = void
	template<class Executor, class BaseLhs, class TypesLhs, bool symmetric,
		class BaseRhs, class TypesRhs>
	class StaticDispatcherVoidBase
	{
		typedef void ResultType;
		template <class SomeLhs>
		static ResultType DispatchRhs(	SomeLhs& lhs, BaseRhs& rhs,
										Executor exec, NullType)
		{ exec.OnError(lhs, rhs); }
    
		template <class Head, class Tail, class SomeLhs>
		static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
										Executor exec, Typelist<Head, Tail>)
		{            
			if (Head* p2 = dynamic_cast<Head*>(&rhs))
			{
				Int2Type<(symmetric &&
						  int(TL::IndexOf<TypesRhs, Head>::value) <
						  int(TL::IndexOf<TypesLhs, SomeLhs>::value))> i2t;

				typedef Private::InvocationTraitsVoid< 
						SomeLhs, Head, Executor> CallTraits;
                
				CallTraits::DoDispatch(lhs, *p2, exec, i2t);
				return;
			}
			DispatchRhs(lhs, rhs, exec, Tail());
		}

		static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
										Executor exec, NullType)
		{ exec.OnError(lhs, rhs); }
    
		template <class Head, class Tail>
		static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
										Executor exec, Typelist<Head, Tail>)
		{            
			if (Head* p1 = dynamic_cast<Head*>(&lhs))
			{
				DispatchRhs(*p1, rhs, exec, TypesRhs());
				return;
			}
			DispatchLhs(lhs, rhs, exec, Tail());
		}

	protected:
		~StaticDispatcherVoidBase() {}
	public:
		static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
								Executor exec)
		{ DispatchLhs(lhs, rhs, exec, TypesLhs()); }
	};

}	// namespace Private

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
        typename ResultType = Loki::Private::VoidWrap::type
    >
    class StaticDispatcher : public ::Loki::Select
								<
								::Loki::Private::IsVoid<ResultType>::value,
								::Loki::Private::StaticDispatcherVoidBase<Executor,
									BaseLhs, TypesLhs, symmetric, BaseRhs, 
									TypesRhs>,
								::Loki::Private::StaticDispatcherBase<Executor,
									BaseLhs, TypesLhs, symmetric, BaseRhs, 
									TypesRhs, ResultType>
								>::Result
    {
        
    public:
        // member functions moved to base class
		// static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
		// 							Executor exec)
    };    

////////////////////////////////////////////////////////////////////////////////
// Implementation helpers for BasicDispatcher
////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	// Implementation for result types != void
	template <class BaseLhs, class BaseRhs, typename ResultType,
		typename CallbackType> struct BasicDispatcherBase;

	// Implementation for result type = 0
	template <class BaseLhs, class BaseRhs,
		typename CallbackType> struct BasicDispatcherVoidBase;

	// Common (independent of the result type) code for BasicDispatcher
	template<typename CallbackType>
	class BasicDispatcherCommonBase
	{
	private:
		void DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun);
		bool DoRemove(TypeInfo lhs, TypeInfo rhs);
	protected:
		typedef std::pair<TypeInfo,TypeInfo> KeyType;
		typedef CallbackType MappedType;
		typedef AssocVector<KeyType, MappedType> MapType;
		MapType callbackMap_;
		~BasicDispatcherCommonBase() {}
	public:
		template <class SomeLhs, class SomeRhs>
		void Add(CallbackType fun, ::Loki::Type2Type<SomeLhs>,
										::Loki::Type2Type<SomeRhs>)
		{
			DoAdd(typeid(SomeLhs), typeid(SomeRhs), fun);
		}


		template <class SomeLhs, class SomeRhs>
		bool Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
		{
			return DoRemove(typeid(SomeLhs), typeid(SomeRhs));
		}
		
	};

	// Non-inline to reduce compile time overhead...
	template <typename CallbackType>
	void BasicDispatcherCommonBase<CallbackType>
		 ::DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun)
	{
		callbackMap_[KeyType(lhs, rhs)] = fun;
	}

	template <typename CallbackType>
	bool BasicDispatcherCommonBase<CallbackType>
		 ::DoRemove(TypeInfo lhs, TypeInfo rhs)
	{
		return callbackMap_.erase(KeyType(lhs, rhs)) == 1;
	}

	// Implementation for result types != void
	template <class BaseLhs, class BaseRhs, typename ResultType,
		typename CallbackType>
	struct BasicDispatcherBase : public BasicDispatcherCommonBase<CallbackType>
	{
		typedef BasicDispatcherCommonBase<CallbackType> Base;

		ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
	protected:
		~BasicDispatcherBase() {}
	};
	template <class BaseLhs, class BaseRhs,
    typename ResultType, typename CallbackType>
	ResultType
	BasicDispatcherBase<BaseLhs,BaseRhs,ResultType,CallbackType>
			   ::Go(BaseLhs& lhs, BaseRhs& rhs)
	{

		typename MapType::key_type k(typeid(lhs),typeid(rhs));
		typename MapType::iterator i = Base::callbackMap_.find(k);
		if (i == Base::callbackMap_.end())
		{
				throw std::runtime_error("Function not found");
		}
		return (i->second)(lhs, rhs);
	}

	// Implementation for result types = void
	template <class BaseLhs, class BaseRhs, typename CallbackType>
	struct BasicDispatcherVoidBase : public BasicDispatcherCommonBase<CallbackType>
	{
		typedef void ResultType;
		typedef BasicDispatcherCommonBase<CallbackType> Base;

		ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
	protected:
		~BasicDispatcherVoidBase() {}
	};
	template <class BaseLhs, class BaseRhs, typename CallbackType>
	typename BasicDispatcherVoidBase<BaseLhs,BaseRhs, CallbackType>::ResultType
	BasicDispatcherVoidBase<BaseLhs,BaseRhs, CallbackType>
			   ::Go(BaseLhs& lhs, BaseRhs& rhs)
	{

		typename MapType::key_type k(typeid(lhs),typeid(rhs));
		typename MapType::iterator i = Base::callbackMap_.find(k);
		if (i == Base::callbackMap_.end())
		{
				throw std::runtime_error("Function not found");
		}
		(i->second)(lhs, rhs);
	}
} // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template BasicDispatcher
// Implements a logarithmic double dispatcher for functors (or functions)
// Doesn't offer automated casts or symmetry
////////////////////////////////////////////////////////////////////////////////
	template
    <
        class BaseLhs,
        class BaseRhs = BaseLhs,
        typename ResultType = Loki::Private::VoidWrap::type,
        typename CallbackType = ResultType (*)(BaseLhs&, BaseRhs&)
    >
    class BasicDispatcher : public ::Loki::Select
							<
								::Loki::Private::IsVoid<ResultType>::value,
								::Loki::Private::BasicDispatcherVoidBase<
									BaseLhs, BaseRhs, CallbackType>,
								::Loki::Private::BasicDispatcherBase<
									BaseLhs, BaseRhs, ResultType, CallbackType>
							>::Result
    {
        public:
			// member functions moved to base class
			// template <class SomeLhs, class SomeRhs>
			// void Add(CallbackType fun,	::Loki::Type2Type<SomeLhs>,
			//								::Loki::Type2Type<SomeRhs>)
			//
			// template <class SomeLhs, class SomeRhs>
			// bool Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
			//
			// ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
    };


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
		template <typename ResultType, class BaseLhs, class BaseRhs,
			class DispatcherBackend>
		struct FnDispatcherBase
		{
			ApplyInnerType4<DispatcherBackend, BaseLhs, BaseRhs, ResultType,
				ResultType (*)(BaseLhs&, BaseRhs&)>::type backEnd_;

			ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
			{
				return backEnd_.Go(lhs, rhs);
			}
		protected:
			~FnDispatcherBase() {}
		};

		template <class BaseLhs, class BaseRhs, class DispatcherBackend>
		struct FnDispatcherVoidBase
		{
			typedef void ResultType;
			ApplyInnerType4<DispatcherBackend, BaseLhs, BaseRhs, ResultType,
				ResultType (*)(BaseLhs&, BaseRhs&)>::type backEnd_;

			ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
			{
				backEnd_.Go(lhs, rhs);
			}
		protected:
			~FnDispatcherVoidBase() {}
		};


		template< class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
				class CastLhs, class CastRhs,
				void (*Callback)(SomeLhs&, SomeRhs&)>
        struct FnDispatcherHelperVoidBase
        {
            typedef void ResultType;
			static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs)
            {
                Callback(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
            static ResultType TrampolineR(BaseRhs& rhs, BaseLhs& lhs)
            {
                Trampoline(lhs, rhs);
            }
		protected:
			~FnDispatcherHelperVoidBase() {}
        };

		template<typename ResultType, class BaseLhs, class BaseRhs,
			class SomeLhs, class SomeRhs, class CastLhs, class CastRhs,
			ResultType (*Callback)(SomeLhs&, SomeRhs&)>
        struct FnDispatcherHelperBase
        {
            typedef void ResultType;
			static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs)
            {
                Callback(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
            static ResultType TrampolineR(BaseRhs& rhs, BaseLhs& lhs)
            {
                Trampoline(lhs, rhs);
            }
		protected:
			~FnDispatcherHelperBase() {}
        };

		template
		<
			class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			typename ResultType, class CastLhs, class CastRhs,
			ResultType (*Callback)(SomeLhs&, SomeRhs&)
		>
        struct FnDispatcherHelper : public ::Loki::Select
									<
									::Loki::Private::IsVoid<ResultType>::value,
									FnDispatcherHelperVoidBase<BaseLhs, BaseRhs,
										SomeLhs, SomeRhs, CastLhs, CastRhs,
										Callback>,
									FnDispatcherHelperBase<ResultType, BaseLhs, BaseRhs,
										SomeLhs, SomeRhs, CastLhs, CastRhs,
										Callback>
									>::Result
        {};
    }

////////////////////////////////////////////////////////////////////////////////
// class template FnDispatcher
// Implements an automatic logarithmic double dispatcher for functions
// Features automated conversions
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
              typename ResultType = Loki::Private::VoidWrap::type,
              class CastingPolicy = DynamicCasterWrapper,
              class DispatcherBackend = BasicDispatcherWrapper>
	class FnDispatcher : public ::Loki::Select
							<
							::Loki::Private::IsVoid<ResultType>::value,
							::Loki::Private::FnDispatcherVoidBase<BaseLhs,
								BaseRhs, DispatcherBackend>,
							::Loki::Private::FnDispatcherBase<ResultType, BaseLhs,
								BaseRhs, DispatcherBackend>
							>::Result
    {
        typedef typename ::Loki::Select
			<::Loki::Private::IsVoid<ResultType>::value,
			::Loki::Private::FnDispatcherVoidBase<BaseLhs,
				BaseRhs, DispatcherBackend>,
			::Loki::Private::FnDispatcherBase<ResultType, BaseLhs,
				BaseRhs, DispatcherBackend>
			>::Result Base;
        
    public:
        template <class SomeLhs, class SomeRhs>
        void Add(ResultType (*pFun)(BaseLhs&, BaseRhs&), 
				::Loki::Type2Type<SomeLhs>, 
				::Loki::Type2Type<SomeRhs>)
        {
			Base::backEnd_.Add(pFun, ::Loki::Type2Type<SomeLhs>(), 
				::Loki::Type2Type<SomeRhs>());
        }        
        
        template <class SomeLhs, class SomeRhs,
            ResultType (*callback)(SomeLhs&, SomeRhs&)>
        void Add(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
        {
        typedef Private::FnDispatcherHelper<
                    BaseLhs, BaseRhs, 
                    SomeLhs, SomeRhs,
                    ResultType,
                    typename ApplyInnerType2<CastingPolicy, SomeLhs,BaseLhs>::type, 
                    typename ApplyInnerType2<CastingPolicy,SomeRhs,BaseRhs>::type, 
                    callback> Local;

            Add(&Local::Trampoline, ::Loki::Type2Type<SomeLhs>(), 
					::Loki::Type2Type<SomeRhs>());
        }
        
        template <class SomeLhs, class SomeRhs,
            ResultType (*callback)(SomeLhs&, SomeRhs&)>
        void Add(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>, 
					bool Symmetric)
        {
        typedef Private::FnDispatcherHelper<
                    BaseLhs, BaseRhs, 
                    SomeLhs, SomeRhs,
                    ResultType,
                    typename ApplyInnerType2<CastingPolicy, SomeLhs,BaseLhs>::type, 
                    typename ApplyInnerType2<CastingPolicy,SomeRhs,BaseRhs>::type, 
                    callback> Local;

            Add(&Local::Trampoline, ::Loki::Type2Type<SomeLhs>(), 
					::Loki::Type2Type<SomeRhs>());
            if (Symmetric)
            {
                Add(&Local::Trampoline, ::Loki::Type2Type<SomeLhs>(), 
					::Loki::Type2Type<SomeRhs>());
            }
        }
        
        template <class SomeLhs, class SomeRhs>
        void Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
        {
			Base::backEnd_.Remove(::Loki::Type2Type<SomeLhs>(), 
				::Loki::Type2Type<SomeRhs>());
        }

        
		// moved to base class
		// ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
    };

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcherAdaptor
// permits use of FunctorDispatcher under gcc.2.95.2/3
///////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
		template <class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			typename ResultType, class CastLhs, class CastRhs, class Fun, bool SwapArgs>
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

		template <class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			class CastLhs, class CastRhs, class Fun, bool SwapArgs>
        class FunctorDispatcherHelperVoid
        {
            Fun fun_;
            typedef void ResultType;
			ResultType Fire(BaseLhs& lhs, BaseRhs& rhs,Int2Type<false>)
            {
                fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
            ResultType Fire(BaseLhs& rhs, BaseRhs& lhs,Int2Type<true>)
            {
                fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
            }
        public:
            FunctorDispatcherHelperVoid(const Fun& fun) : fun_(fun) {}

            ResultType operator()(BaseLhs& lhs, BaseRhs& rhs)
            {
                Fire(lhs,rhs,Int2Type<SwapArgs>());
            }
        };

		template <class BaseLhs, class BaseRhs, typename ResultType, 
			class DispatcherBackend> 
		class FunctorDispatcherCommonBase
		{
			protected:
			typedef TYPELIST_2(BaseLhs&, BaseRhs&) ArgsList;
			typedef Functor<ResultType, ArgsList, DEFAULT_THREADING> FunctorType;

			ApplyInnerType4<DispatcherBackend,BaseLhs, BaseRhs, ResultType, 
				FunctorType>::type backEnd_;
			~FunctorDispatcherCommonBase() {}
		};
		
		template <typename ResultType, class BaseLhs, class BaseRhs, 
			class DispatcherBackend>
		class FunctorDispatcherBase : public FunctorDispatcherCommonBase<BaseLhs, 
												BaseRhs, ResultType, DispatcherBackend>
		{
			typedef FunctorDispatcherCommonBase<BaseLhs, BaseRhs, ResultType, 
				DispatcherBackend> Base;
		public:
			typedef typename Base::ArgsList ArgsList;
			typedef typename Base::FunctorType FunctorType;

			ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
			{
				return Base::backEnd_.Go(lhs, rhs);
			}
		protected:
			~FunctorDispatcherBase() {}
		};
		
		template <class BaseLhs, class BaseRhs, class DispatcherBackend>
		class FunctorDispatcherVoidBase : public FunctorDispatcherCommonBase<BaseLhs, 
												BaseRhs, void, DispatcherBackend>
		{
			typedef void ResultType;
			typedef FunctorDispatcherCommonBase<BaseLhs, BaseRhs, ResultType, 
				DispatcherBackend> Base;
		public:
			typedef typename Base::ArgsList ArgsList;
			typedef typename Base::FunctorType FunctorType;

			ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
			{
				Base::backEnd_.Go(lhs, rhs);
			}
		protected:
			~FunctorDispatcherVoidBase() {}
		};
    }

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcher
// Implements a logarithmic double dispatcher for functors
// Features automated casting
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
              typename ResultType = Loki::Private::VoidWrap::type,
              class CastingPolicy = DynamicCasterWrapper, 
              class DispatcherBackend = BasicDispatcherWrapper>
	class FunctorDispatcher : public ::Loki::Select
								< 
								::Loki::Private::IsVoid<ResultType>::value,
								::Loki::Private::FunctorDispatcherVoidBase<BaseLhs,
									BaseRhs, DispatcherBackend>,
								::Loki::Private::FunctorDispatcherBase<ResultType,
									BaseLhs, BaseRhs, DispatcherBackend>
								>::Result
    {
        typedef typename ::Loki::Select
		< 
			::Loki::Private::IsVoid<ResultType>::value,
			::Loki::Private::FunctorDispatcherVoidBase<BaseLhs, BaseRhs, 
				DispatcherBackend>,
			::Loki::Private::FunctorDispatcherBase<ResultType, BaseLhs, BaseRhs, 
				DispatcherBackend>
		>::Result Base;
    public:
        typedef Base::ArgsList ArgsList;
        typedef Base::FunctorType FunctorType;
		template <class SomeLhs, class SomeRhs, class Fun>
		void Add(const Fun& fun, ::Loki::Type2Type<SomeLhs>, 
								::Loki::Type2Type<SomeRhs>)
        {
            typedef typename ApplyInnerType2<CastingPolicy,SomeLhs, BaseLhs>::type CastOne;
			typedef typename ApplyInnerType2<CastingPolicy,SomeRhs, BaseRhs>::type CastTwo;
			typedef typename Select<
				Private::IsVoid<ResultType>::value,	
				Private::FunctorDispatcherHelperVoid<BaseLhs, BaseRhs, SomeLhs, 
							SomeRhs, CastOne,CastTwo,Fun, false>,
				Private::FunctorDispatcherHelper<BaseLhs, BaseRhs, SomeLhs, SomeRhs,
							ResultType,CastOne,CastTwo,Fun, false>
							>::Result Adapter;
			Base::backEnd_.Add(FunctorType(Adapter(fun), Loki::Disambiguate()), 
				Type2Type<SomeLhs>(), Type2Type<SomeRhs>());
		}
        template <class SomeLhs, class SomeRhs, class Fun>
		void Add(const Fun& fun, ::Loki::Type2Type<SomeLhs>, 
								::Loki::Type2Type<SomeRhs>, bool symmetric)
        {
			Add(fun, Type2Type<SomeLhs>(), Type2Type<SomeRhs>());

			if (symmetric)
			{
			// Note: symmetry only makes sense where BaseLhs==BaseRhs
				typedef typename ApplyInnerType2<CastingPolicy,SomeLhs, BaseLhs>::type CastOne;
				typedef typename ApplyInnerType2<CastingPolicy,SomeRhs, BaseLhs>::type CastTwo;
				typedef typename Select<Private::IsVoid<ResultType>::value,
						Private::FunctorDispatcherHelperVoid<BaseLhs, BaseLhs,
							SomeLhs, SomeRhs, CastOne,CastTwo, Fun, true>,
						Private::FunctorDispatcherHelper<BaseLhs, BaseLhs,
							ResultType, SomeLhs, SomeRhs,CastOne, CastTwo,Fun, true>
							>::Result AdapterR;

				Base::backEnd_.Add(FunctorType(Adapter(fun)), 
						Type2Type<SomeLhs>(), Type2Type<SomeRhs>());
			}
        }
        
        template <class SomeLhs, class SomeRhs>
		void Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
        {
			Base::backEnd_.Remove(Type2Type<SomeLhs>(), Type2Type<SomeRhs>());
        }

        // moved to base class
		// ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
		
    };
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Oct  28, 2002: ported by Benjamin Kaufmann to MSVC 6
// Feb	19, 2003: replaced pointer-Dummies with Type2Type-Parameters and added 
//					support for return type void. B.K.
// Mar	06, 2003: Changed default values for return types to void.
//				  Added protected destructors to private implementation classes B.K.
////////////////////////////////////////////////////////////////////////////////

#endif
