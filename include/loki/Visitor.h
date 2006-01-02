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

// $Header:

#ifndef LOKI_VISITOR_INC_
#define LOKI_VISITOR_INC_

#include "Typelist.h"
#include "HierarchyGenerators.h"

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitor
// The base class of any Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////

    class BaseVisitor
    {
    public:
        virtual ~BaseVisitor() {}
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template Visitor
// The building block of Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////

	template <class T, typename R = void, bool ConstVisit = false>
    class Visitor;

    template <class T, typename R>
    class Visitor<T, R, false>
    {
    public:
        typedef R ReturnType;
		typedef T ParamType;
        virtual ReturnType Visit(ParamType&) = 0;
    };

    template <class T, typename R>
    class Visitor<T, R, true>
    {
    public:
        typedef R ReturnType;
		typedef const T ParamType;
        virtual ReturnType Visit(ParamType&) = 0;
    };

////////////////////////////////////////////////////////////////////////////////
// class template Visitor (specialization)
// This specialization is not present in the book. It makes it easier to define
// Visitors for multiple types in a shot by using a typelist. Example:
//
// class SomeVisitor : 
//     public BaseVisitor // required
//     public Visitor<LOKI_TYPELIST_2(RasterBitmap, Paragraph)>
// {
// public:
//     void Visit(RasterBitmap&); // visit a RasterBitmap
//     void Visit(Paragraph &);   // visit a Paragraph
// };
////////////////////////////////////////////////////////////////////////////////

    template <class Head, class Tail, typename R>
    class Visitor<Typelist<Head, Tail>, R, false>
        : public Visitor<Head, R, false>, public Visitor<Tail, R, false>
    {
    public:
        typedef R ReturnType;
       // using Visitor<Head, R>::Visit;
       // using Visitor<Tail, R>::Visit;
    };
    
    template <class Head, typename R>
    class Visitor<Typelist<Head, NullType>, R, false> : public Visitor<Head, R, false>
    {
    public:
        typedef R ReturnType;
        using Visitor<Head, R, false>::Visit;
    };

	template <class Head, class Tail, typename R>
    class Visitor<Typelist<Head, Tail>, R, true>
        : public Visitor<Head, R, true>, public Visitor<Tail, R, true>
    {
    public:
        typedef R ReturnType;
       // using Visitor<Head, R>::Visit;
       // using Visitor<Tail, R>::Visit;
    };
    
    template <class Head, typename R>
    class Visitor<Typelist<Head, NullType>, R, true> : public Visitor<Head, R, true>
    {
    public:
        typedef R ReturnType;
        using Visitor<Head, R, true>::Visit;
    };


////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitorImpl
// Implements non-strict visitation (you can implement only part of the Visit
//     functions)
////////////////////////////////////////////////////////////////////////////////

    template <class TList, typename R = void> class BaseVisitorImpl;

    template <class Head, class Tail, typename R>
    class BaseVisitorImpl<Typelist<Head, Tail>, R>
        : public Visitor<Head, R>
        , public BaseVisitorImpl<Tail, R>
    {
    public:
       // using BaseVisitorImpl<Tail, R>::Visit;

        virtual R Visit(Head&)
        { return R(); }
    };
    
    template <class Head, typename R>
    class BaseVisitorImpl<Typelist<Head, NullType>, R>
        : public Visitor<Head, R>
    {
    public:
        virtual R Visit(Head&)
        { return R(); }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

template <typename R, typename Visited>
struct DefaultCatchAll
{
    static R OnUnknownVisitor(Visited&, BaseVisitor&)
    { return R(); }
};

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

    template 
    <
        typename R = void, 
        template <typename, class> class CatchAll = DefaultCatchAll,
		bool ConstVisitable = false
    >
    class BaseVisitable;

    template<typename R,template <typename, class> class CatchAll>
    class BaseVisitable<R, CatchAll, false>
    {
    public:
        typedef R ReturnType;
        virtual ~BaseVisitable() {}
        virtual ReturnType Accept(BaseVisitor&) = 0;
        
    protected: // give access only to the hierarchy
        template <class T>
        static ReturnType AcceptImpl(T& visited, BaseVisitor& guest)
        {
            // Apply the Acyclic Visitor
            if (Visitor<T,R>* p = dynamic_cast<Visitor<T,R>*>(&guest))
            {
                return p->Visit(visited);
            }
            return CatchAll<R, T>::OnUnknownVisitor(visited, guest);
        }
    };

    template<typename R,template <typename, class> class CatchAll>
    class BaseVisitable<R, CatchAll, true>
    {
    public:
        typedef R ReturnType;
        virtual ~BaseVisitable() {}
        virtual ReturnType Accept(BaseVisitor&) const = 0;
        
    protected: // give access only to the hierarchy
        template <class T>
        static ReturnType AcceptImpl(const T& visited, BaseVisitor& guest)
        {
            // Apply the Acyclic Visitor
            if (Visitor<T,R,true>* p = dynamic_cast<Visitor<T,R,true>*>(&guest))
            {
                return p->Visit(visited);
            }
            return CatchAll<R, T>::OnUnknownVisitor(const_cast<T&>(visited), guest);
        }
    };

////////////////////////////////////////////////////////////////////////////////
// macro DEFINE_VISITABLE
// Put it in every class that you want to make visitable (in addition to 
//     deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////

#define LOKI_DEFINE_VISITABLE() \
    virtual ReturnType Accept(::Loki::BaseVisitor& guest) \
    { return AcceptImpl(*this, guest); }

#define LOKI_DEFINE_CONST_VISITABLE() \
    virtual ReturnType Accept(::Loki::BaseVisitor& guest) const \
    { return AcceptImpl(*this, guest); }

////////////////////////////////////////////////////////////////////////////////
// class template CyclicVisitor
// Put it in every class that you want to make visitable (in addition to 
//     deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////

    template <typename R, class TList>
    class CyclicVisitor : public Visitor<TList, R>
    {
    public:
        typedef R ReturnType;
        // using Visitor<TList, R>::Visit;
        
        template <class Visited>
        ReturnType GenericVisit(Visited& host)
        {
            Visitor<Visited, ReturnType>& subObj = *this;
            return subObj.Visit(host);
        }
    };
    
////////////////////////////////////////////////////////////////////////////////
// macro LOKI_DEFINE_CYCLIC_VISITABLE
// Put it in every class that you want to make visitable by a cyclic visitor
////////////////////////////////////////////////////////////////////////////////

#define LOKI_DEFINE_CYCLIC_VISITABLE(SomeVisitor) \
    virtual SomeVisitor::ReturnType Accept(SomeVisitor& guest) \
    { return guest.GenericVisit(*this); }

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// March     20, ????: add default argument DefaultCatchAll to BaseVisitable
// June      20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// September 28, 2004: replaced Loki:: with ::Loki:: in DEFINE_VISITABLE
// January    2, 2006: add support for visiting constant member functions, Peter Kümmel
////////////////////////////////////////////////////////////////////////////////

#endif // VISITOR_INC_

