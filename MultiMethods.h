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

// Last update: February 19, 2001

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
        template <bool swapArgs, class SomeLhs, class SomeRhs, class Executor,
            typename ResultType>
        struct InvocationTraits
        {
            static ResultType DoDispatch(SomeLhs& lhs, SomeRhs& rhs,
                Executor& exec)
            {
                return exec.Fire(lhs, rhs);
            }
        };
        
        template <class SomeLhs, class SomeRhs, class Executor, 
            typename ResultType>
        struct InvocationTraits<true, SomeLhs, SomeRhs, Executor, ResultType>
        {
            static ResultType DoDispatch(SomeLhs& lhs, SomeRhs& rhs,
                Executor& exec)
            {
                return exec.Fire(rhs, lhs); // swap arguments
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
        bool symmetric,
        class BaseLhs, 
        class TypesLhs,
        class BaseRhs = BaseLhs,
        class TypesRhs = TypesLhs,
        typename ResultType = void
    >
    class StaticDispatcher
    {
        template <class SomeLhs>
        static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
            Executor exec, NullType)
        { return exec.OnError(lhs, rhs); }
        
        template <class TList, class SomeLhs>
        static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
            Executor exec, TList)
        {
            typedef typename TList::Head Head;
            typedef typename TList::Tail Tail;
            
            if (Head* p2 = dynamic_cast<Head*>(&rhs))
            {
                enum { swapArgs = symmetric &&
                    TL::IndexOf<TypesRhs, Head>::value <
                    TL::IndexOf<TypesLhs, SomeLhs>::value };
                typedef Private::InvocationTraits<swapArgs, 
                        SomeLhs, Head, Executor, ResultType>
                    CallTraits;
                    
                return CallTraits::DoDispatch(lhs, *p2, exec);
            }
            return DispatchRhs(lhs, rhs, exec, Tail());
        }
        
        static ResultType DispatchLhs(BaseLhs& lhs, BaseRhs& rhs,
            Executor exec, NullType)
        { return exec.OnError(lhs, rhs); }
        
        template <class TList>
        static ResultType DispatchLhs(BaseLhs& lhs, BaseRhs& rhs,
            Executor exec, TList)
        {
            typedef typename TList::Head Head;
            typedef typename TList::Tail Tail;
            
            if (Head* p1 = dynamic_cast<Head*>(&lhs))
            {
                return DispatchRhs(*p1, rhs, exec, TypesRhs());
            }
            return DispatchLhs(lhs, rhs, exec, Tail());
        }

    public:
        static ResultType Go(BaseLhs& lhs, BaseRhs& rhs,
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
        typename ResultType = void,
        typename CallbackType = ResultType (*)(BaseLhs&, BaseRhs&)
    >
    class BasicDispatcher
    {
        typedef std::pair<TypeInfo, TypeInfo> KeyType;
        typedef CallbackType MappedType;
        typedef AssocVector<KeyType, MappedType> MapType;
        MapType callbackMap_;
        
    public:
        void DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun)
        {
            callbackMap_[KeyType(lhs, rhs)] = fun;
        }
        
        bool DoRemove(TypeInfo lhs, TypeInfo rhs)
        {
            return callbackMap_.erase(KeyType(lhs, rhs)) == 1;
        }
        
        template <class SomeLhs, class SomeRhs>
        void Add(CallbackType fun)
        {
            DoAdd(typeid(SomeLhs), typeid(SomeRhs), fun);
        }
        
        template <class SomeLhs, class SomeRhs>
        bool Remove()
        {
            return DoRemove(typeid(SomeLhs), typeid(SomeRhs));
        }
        
        ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
        {
            MapType::iterator i = callbackMap_.find(
                KeyType(typeid(lhs), typeid(rhs)));
            if (i == callbackMap_.end())
            {
                throw std::runtime_error("Function not found");
            }
            return (i->second)(lhs, rhs);
        }
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

////////////////////////////////////////////////////////////////////////////////
// class template FnDispatcher
// Implements an automatic logarithmic double dispatcher for functions
// Features automated conversions
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
        typename ResultType = void,
        template <class, class> class CastingPolicy = DynamicCaster,
        template <class, class, class, class>
            class DispatcherBackend = BasicDispatcher>
    class FnDispatcher
    {
        DispatcherBackend<BaseLhs, BaseRhs, ResultType, 
            ResultType (*)(BaseLhs&, BaseRhs&)> backEnd_;
        
    public:
        template <class ConcreteLhs, class ConcreteRhs>
        void Add(ResultType (*pFun)(BaseLhs&, BaseRhs&))
        {
            return backEnd_.Add<ConcreteLhs, ConcreteRhs>(pFun);
        }        
        
        template <class ConcreteLhs, class ConcreteRhs,
            ResultType (*callback)(ConcreteLhs&, ConcreteRhs&),
            bool symmetric>
        void Add()
        {
            struct Local
            {
                static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs)
                {
                    return callback(
                        CastingPolicy<ConcreteLhs, BaseLhs>::Cast(lhs),
                        CastingPolicy<ConcreteRhs, BaseRhs>::Cast(rhs));
                }
                static ResultType TrampolineR(BaseRhs& rhs, BaseLhs& lhs)
                {
                    return Trampoline(lhs, rhs);
                }
            };
            Add<ConcreteLhs, ConcreteRhs>(&Local::Trampoline);
            if (symmetric)
            {
                Add<ConcreteRhs, ConcreteLhs>(&Local::TrampolineR);
            }
        }
        
        ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
        {
            return backEnd_.Go(lhs, rhs);
        }
    };

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcher
// Implements a logarithmic double dispatcher for functors
// Features automated casting
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
        typename ResultType = void,
        template <class, class> class CastingPolicy = DynamicCaster,
        template <class, class, class, class>
            class DispatcherBackend = BasicDispatcher>
    class FunctorDispatcher
    {
        typedef TYPELIST_2(BaseLhs&, BaseRhs&) ArgsList;
        typedef Functor<ResultType, ArgsList, DEFAULT_THREADING> FunctorType;
            
        typedef DispatcherBackend<BaseLhs, BaseRhs, ResultType, FunctorType>
            BackEndType;
        BackEndType backEnd_;

    public:
        template <class SomeLhs, class SomeRhs, class Fun>
        void Add(const Fun& fun)
        {
            typedef FunctorImpl<ResultType, TYPELIST_2(BaseLhs, BaseRhs)>
                FunctorImplType;
            class Adapter : public FunctorImplType
            {
                Fun fun_;
                virtual ResultType operator()(BaseLhs& lhs, BaseRhs& rhs)
                {
                    return fun_(
                        CastingPolicy<ConcreteLhs, BaseLhs>::Cast(lhs),
                        CastingPolicy<ConcreteRhs, BaseRhs>::Cast(rhs));
                }
                virtual FunctorImplType* DoClone() const
                { return new Adapter(*this); }
            public:
                Adapter(const Fun& fun) : fun_(fun) {}
            };
            backEnd_.Add<SomeLhs, SomeRhs>(
                Functor<ResultType, ArgsList, DEFAULT_THREADING>(new Adapter(fun)));
        }
        
        ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
        {
            return backEnd_.Go(lhs, rhs);
        }
    };
} // namespace Loki


#endif