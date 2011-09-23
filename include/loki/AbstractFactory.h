////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
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
#ifndef LOKI_ABSTRACTFACTORY_INC_
#define LOKI_ABSTRACTFACTORY_INC_

// $Id$


#include <loki/Typelist.h>
#include <loki/Sequence.h>
#include <loki/TypeManip.h>
#include <loki/HierarchyGenerators.h>

#include <cassert>

/**
 * \defgroup	FactoriesGroup Factories
 * \defgroup	AbstractFactoryGroup Abstract Factory
 * \ingroup		FactoriesGroup
 * \brief		Implements an abstract object factory.
 */

/**
 * \class		AbstractFactory
 * \ingroup		AbstractFactoryGroup
 * \brief		Implements an abstract object factory.
 */

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template AbstractFactoryUnit
// The building block of an Abstract Factory
////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class AbstractFactoryUnit
    {
    public:
        virtual T* DoCreate(Type2Type<T>) = 0;
        virtual ~AbstractFactoryUnit() {}
    };

////////////////////////////////////////////////////////////////////////////////
// class template AbstractFactory
// Defines an Abstract Factory interface starting from a typelist
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class TList,
        template <class> class Unit = AbstractFactoryUnit
    >
    class AbstractFactory : public GenScatterHierarchy<TList, Unit>
    {
    public:
        typedef TList ProductList;

        template <class T> T* Create()
        {
            Unit<T>& unit = *this;
            return unit.DoCreate(Type2Type<T>());
        }
    };

////////////////////////////////////////////////////////////////////////////////
// class template OpNewFactoryUnit
// Creates an object by invoking the new operator
////////////////////////////////////////////////////////////////////////////////

    template <class ConcreteProduct, class Base>
    class OpNewFactoryUnit : public Base
    {
        typedef typename Base::ProductList BaseProductList;

    protected:
        typedef typename BaseProductList::Tail ProductList;

    public:
        typedef typename BaseProductList::Head AbstractProduct;
        ConcreteProduct* DoCreate(Type2Type<AbstractProduct>)
        {
            return new ConcreteProduct;
        }
    };

////////////////////////////////////////////////////////////////////////////////
// class template PrototypeFactoryUnit
// Creates an object by cloning a prototype
// There is a difference between the implementation herein and the one described
//     in the book: GetPrototype and SetPrototype use the helper friend
//     functions DoGetPrototype and DoSetPrototype. The friend functions avoid
//     name hiding issues. Plus, GetPrototype takes a reference to pointer
//     instead of returning the pointer by value.
////////////////////////////////////////////////////////////////////////////////

    template <class ConcreteProduct, class Base>
    class PrototypeFactoryUnit : public Base
    {
        typedef typename Base::ProductList BaseProductList;

    protected:
        typedef typename BaseProductList::Tail ProductList;

    public:
        typedef typename BaseProductList::Head AbstractProduct;

        PrototypeFactoryUnit(AbstractProduct* p = 0)
            : pPrototype_(p)
        {}

        template <class CP, class Base1>
        friend void DoGetPrototype(const PrototypeFactoryUnit<CP, Base1>& me,
                                   typename Base1::ProductList::Head*& pPrototype);

        template <class CP, class Base1>
        friend void DoSetPrototype(PrototypeFactoryUnit<CP, Base1>& me,
                                   typename Base1::ProductList::Head* pObj);

        template <class U>
        void GetPrototype(U*& p)
        { return DoGetPrototype(*this, p); }

        template <class U>
        void SetPrototype(U* pObj)
        { DoSetPrototype(*this, pObj); }

        AbstractProduct* DoCreate(Type2Type<AbstractProduct>)
        {
            assert(pPrototype_);
            return pPrototype_->Clone();
        }

    private:
        AbstractProduct* pPrototype_;
    };

    template <class CP, class Base>
    inline void DoGetPrototype(const PrototypeFactoryUnit<CP, Base>& me,
                               typename Base::ProductList::Head*& pPrototype)
    { pPrototype = me.pPrototype_; }

    template <class CP, class Base>
    inline void DoSetPrototype(PrototypeFactoryUnit<CP, Base>& me,
                               typename Base::ProductList::Head* pObj)
    { me.pPrototype_ = pObj; }

////////////////////////////////////////////////////////////////////////////////
// class template ConcreteFactory
// Implements an AbstractFactory interface
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractFact,
        template <class, class> class Creator = OpNewFactoryUnit,
        class TList = typename AbstractFact::ProductList
    >
    class ConcreteFactory
        : public GenLinearHierarchy<
            typename TL::Reverse<TList>::Result, Creator, AbstractFact>
    {
    public:
        typedef typename AbstractFact::ProductList ProductList;
        typedef TList ConcreteProductList;
    };

} // namespace Loki


#endif // end file guardian

