////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// Copyright (c) 2005 by Peter Kuemmel
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Code covered by the MIT License
// The authors make no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$

#ifndef FACTORYPARM_INC_
#define FACTORYPARM_INC_

#include "LokiTypeInfo.h"
#include "Functor.h"
#include "AssocVector.h"
#include "SmallObj.h"


namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template DefaultFactoryError
// Manages the "Unknown Type" error in an object factory
////////////////////////////////////////////////////////////////////////////////

    template <typename IdentifierType, class AbstractProduct>
    struct DefaultFactoryError
    {
        struct Exception : public std::exception
        {
            const char* what() const throw() { return "Unknown Type"; }
        };
        
        static AbstractProduct* OnUnknownType(IdentifierType)
        {
            throw Exception();
        }
    };
    
    
//#define ENABLE_FACTORY_BOOK_VERSION_ONLY
#ifndef ENABLE_FACTORY_BOOK_VERSION_ONLY

////////////////////////////////////////////////////////////////////////////////
/*
    class template Factory
    Implements a generic object factory     
    cretae functions with up to 15 parameters
    
    
    template
    <
        class AbstractProduct,
        typename IdentifierType,
        typename CreatorParmTList = NullType,
        template<typename, class> class FactoryErrorPolicy = ExceptionFactoryError
    >
    class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct>
    {

    public:
        typedef Functor<AbstractProduct*, CreatorParmTList> ProductCreator;
        typedef FactoryImpl< AbstractProduct, IdentifierType, CreatorParmTList > Impl;

        typedef typename Impl::Parm1Parm1;
        // ... up to 15 Parameters
        
        typedef typename IdToProductMap::iterator iterator;
        iterator begin();
        iterator end();

        bool Register(const IdentifierType& id, ProductCreator creator);
        bool Unregister(const IdentifierType& id);

        template <class PtrObj, typename CreaFn>
        bool Register(const IdentifierType& id, const PtrObj& p, CreaFn fn);

        AbstractProduct* CreateObject(const IdentifierType& id);
        AbstractProduct* CreateObject(const IdentifierType& id,    Parm1 p1);
        // ... up to 15 Parameters
    };
*/
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl
////////////////////////////////////////////////////////////////////////////////

    struct FactoryImplBase 
    {
        typedef EmptyType Parm1;
        typedef EmptyType Parm2;
        typedef EmptyType Parm3;
        typedef EmptyType Parm4;
        typedef EmptyType Parm5;
        typedef EmptyType Parm6;
        typedef EmptyType Parm7;
        typedef EmptyType Parm8;
        typedef EmptyType Parm9;
        typedef EmptyType Parm10;
        typedef EmptyType Parm11;
        typedef EmptyType Parm12;
        typedef EmptyType Parm13;
        typedef EmptyType Parm14;
        typedef EmptyType Parm15;
    };

    template <typename AP, typename Id, typename TList >
    struct FactoryImpl;

    template<typename AP, typename Id>
    struct FactoryImpl<AP, Id, NullType>
                : public FactoryImplBase
    {
        virtual AP* CreateObject(const Id & id ) = 0;
    };

    template <typename AP, typename Id, typename P1 >
    struct FactoryImpl<AP,Id, TYPELIST_1( P1 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        virtual AP* CreateObject(const Id& id,Parm1 ) = 0;
    };

    template<typename AP, typename Id, typename P1,typename P2 >
    struct FactoryImpl<AP, Id, TYPELIST_2( P1, P2 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2 ) = 0;
    };

    template<typename AP, typename Id, typename P1,typename P2,typename P3 >
    struct FactoryImpl<AP, Id, TYPELIST_3( P1, P2, P3 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3 ) = 0;
    };

    template<typename AP, typename Id, typename P1,typename P2,typename P3,typename P4 >
    struct FactoryImpl<AP, Id, TYPELIST_4( P1, P2, P3, P4 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4 ) = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5 >
    struct FactoryImpl<AP, Id, TYPELIST_5( P1, P2, P3, P4, P5 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5 ) = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6>
    struct FactoryImpl<AP, Id, TYPELIST_6( P1, P2, P3, P4, P5, P6 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6 )
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7>
    struct FactoryImpl<AP, Id, TYPELIST_7( P1, P2, P3, P4, P5, P6, P7 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7 )
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8>
    struct FactoryImpl<AP, Id, TYPELIST_8( P1, P2, P3, P4, P5, P6, P7, P8 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9>
    struct FactoryImpl<AP, Id, TYPELIST_9( P1, P2, P3, P4, P5, P6, P7, P8, P9 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm9)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9,typename P10>
    struct FactoryImpl<AP, Id, TYPELIST_10( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        typedef typename TypeTraits<P10>::ParameterType Parm10;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm9,Parm10)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9,typename P10,
    typename P11>
    struct FactoryImpl<AP, Id, TYPELIST_11( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        typedef typename TypeTraits<P10>::ParameterType Parm10;
        typedef typename TypeTraits<P11>::ParameterType Parm11;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm9,Parm10,
                                Parm11)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9,typename P10,
    typename P11,typename P12>
    struct FactoryImpl<AP, Id, TYPELIST_12( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        typedef typename TypeTraits<P10>::ParameterType Parm10;
        typedef typename TypeTraits<P11>::ParameterType Parm11;
        typedef typename TypeTraits<P12>::ParameterType Parm12;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm9,Parm10,
                                Parm11,Parm12)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9,typename P10,
    typename P11,typename P12,typename P13>
    struct FactoryImpl<AP, Id, TYPELIST_13( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        typedef typename TypeTraits<P10>::ParameterType Parm10;
        typedef typename TypeTraits<P11>::ParameterType Parm11;
        typedef typename TypeTraits<P12>::ParameterType Parm12;
        typedef typename TypeTraits<P13>::ParameterType Parm13;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm9,Parm10,
                                Parm11,Parm12,Parm13)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9,typename P10,
    typename P11,typename P12,typename P13,typename P14>
    struct FactoryImpl<AP, Id, TYPELIST_14( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        typedef typename TypeTraits<P10>::ParameterType Parm10;
        typedef typename TypeTraits<P11>::ParameterType Parm11;
        typedef typename TypeTraits<P12>::ParameterType Parm12;
        typedef typename TypeTraits<P13>::ParameterType Parm13;
        typedef typename TypeTraits<P14>::ParameterType Parm14;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm8,Parm10,
                                Parm11,Parm12,Parm13,Parm14)
        = 0;
    };

    template<typename AP, typename Id,
    typename P1,typename P2,typename P3,typename P4,typename P5,
    typename P6,typename P7,typename P8,typename P9,typename P10,
    typename P11,typename P12,typename P13,typename P14,typename P15 >
    struct FactoryImpl<AP, Id, TYPELIST_15( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 )>
                : public FactoryImplBase
    {
        typedef typename TypeTraits<P1>::ParameterType Parm1;
        typedef typename TypeTraits<P2>::ParameterType Parm2;
        typedef typename TypeTraits<P3>::ParameterType Parm3;
        typedef typename TypeTraits<P4>::ParameterType Parm4;
        typedef typename TypeTraits<P5>::ParameterType Parm5;
        typedef typename TypeTraits<P6>::ParameterType Parm6;
        typedef typename TypeTraits<P7>::ParameterType Parm7;
        typedef typename TypeTraits<P8>::ParameterType Parm8;
        typedef typename TypeTraits<P9>::ParameterType Parm9;
        typedef typename TypeTraits<P10>::ParameterType Parm10;
        typedef typename TypeTraits<P11>::ParameterType Parm11;
        typedef typename TypeTraits<P12>::ParameterType Parm12;
        typedef typename TypeTraits<P13>::ParameterType Parm13;
        typedef typename TypeTraits<P14>::ParameterType Parm14;
        typedef typename TypeTraits<P15>::ParameterType Parm15;
        virtual AP* CreateObject(const Id& id,Parm1, Parm2, Parm3, Parm4, Parm5,
                                Parm6, Parm7, Parm8, Parm9,Parm10,
                                Parm11,Parm12,Parm13,Parm14,Parm15 )
        = 0;
    };



////////////////////////////////////////////////////////////////////////////////
// class template Factory
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractProduct,
        typename IdentifierType,
        typename CreatorParmTList = NullType,
        template<typename, class> class FactoryErrorPolicy = DefaultFactoryError
    >
    class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct>
    {


    public:
        typedef Functor<AbstractProduct*, CreatorParmTList> ProductCreator;
        typedef FactoryImpl< AbstractProduct, IdentifierType, CreatorParmTList > Impl;

    private:
        typedef AssocVector<IdentifierType, Functor<AbstractProduct*, CreatorParmTList> > IdToProductMap;

    public:
        typedef typename Impl::Parm1 Parm1;
        typedef typename Impl::Parm2 Parm2;
        typedef typename Impl::Parm3 Parm3;
        typedef typename Impl::Parm4 Parm4;
        typedef typename Impl::Parm5 Parm5;
        typedef typename Impl::Parm6 Parm6;
        typedef typename Impl::Parm7 Parm7;
        typedef typename Impl::Parm8 Parm8;
        typedef typename Impl::Parm9 Parm9;
        typedef typename Impl::Parm10 Parm10;
        typedef typename Impl::Parm11 Parm11;
        typedef typename Impl::Parm12 Parm12;
        typedef typename Impl::Parm13 Parm13;
        typedef typename Impl::Parm14 Parm14;
        typedef typename Impl::Parm15 Parm15;

        Factory()
        {
        }

        ~Factory()
        {
            associations_.erase(associations_.begin(), associations_.end());
        }

        typedef typename IdToProductMap::iterator iterator;
        
        iterator begin()
        {
            return associations_.begin();
        }
        iterator end()
        {
            return associations_.end();
        }

        bool Register(const IdentifierType& id, ProductCreator creator)
        {
            return associations_.insert(
                         typename IdToProductMap::value_type(id, creator)).second != 0;
        }

        template <class PtrObj, typename CreaFn>
        bool Register(const IdentifierType& id, const PtrObj& p, CreaFn fn)
        {
            ProductCreator creator( p, fn );
            return associations_.insert(
                typename IdToProductMap::value_type(id, creator)).second != 0;

        }

        bool Unregister(const IdentifierType& id)
        {
            return associations_.erase(id) != 0;
        }

        AbstractProduct* CreateObject(const IdentifierType& id)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
                                            Parm6 p6)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
                                            Parm6 p6, Parm7 p7 )
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
                                            Parm6 p6, Parm7 p7, Parm8 p8)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
                                            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9 );
            return this->OnUnknownType(id);
        }
        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
                                            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9,Parm10 p10)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1  p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5  p5,
                                            Parm6  p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10,
                                            Parm11 p11)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1  p1,  Parm2  p2, Parm3 p3, Parm4 p4, Parm5  p5,
                                            Parm6  p6,  Parm7  p7, Parm8 p8, Parm9 p9, Parm10 p10,
                                            Parm11 p11, Parm12 p12)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1  p1,  Parm2  p2,  Parm3  p3, Parm4 p4, Parm5  p5,
                                            Parm6  p6,  Parm7  p7,  Parm8  p8, Parm9 p9, Parm10 p10,
                                            Parm11 p11, Parm12 p12, Parm13 p13)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1  p1,  Parm2  p2,  Parm3  p3,  Parm4  p4, Parm5  p5,
                                            Parm6  p6,  Parm7  p7,  Parm8  p8,  Parm9  p9, Parm10 p10,
                                            Parm11 p11, Parm12 p12, Parm13 p13, Parm14 p14)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14 );
            return this->OnUnknownType(id);
        }

        AbstractProduct* CreateObject(const IdentifierType& id,
                                            Parm1  p1,  Parm2  p2,  Parm3  p3,  Parm4  p4,  Parm5  p5,
                                            Parm6  p6,  Parm7  p7,  Parm8  p8,  Parm9  p9,  Parm10 p10,
                                            Parm11 p11, Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
                return (i->second)( p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15 );
            return this->OnUnknownType(id);
        }

    private:
        IdToProductMap associations_;
    };


#else

    template
    <
        class AbstractProduct, 
        typename IdentifierType,
        typename ProductCreator = AbstractProduct* (*)(),
        template<typename, class>
            class FactoryErrorPolicy = DefaultFactoryError
    >
    class Factory 
        : public FactoryErrorPolicy<IdentifierType, AbstractProduct>
    {
    public:
        bool Register(const IdentifierType& id, ProductCreator creator)
        {
            return associations_.insert(
                typename IdToProductMap::value_type(id, creator)).second;
        }
        
        bool Unregister(const IdentifierType& id)
        {
            return associations_.erase(id) == 1;
        }
        
        AbstractProduct* CreateObject(const IdentifierType& id)
        {
            typename IdToProductMap::iterator i = associations_.find(id);
            if (i != associations_.end())
            {
                return (i->second)();
            }
            return this->OnUnknownType(id);
        }
        
    private:
        typedef AssocVector<IdentifierType, ProductCreator> IdToProductMap;
        IdToProductMap associations_;
    };


#endif //#define ENABLE_FACTORY_BOOK_VERSION_ONLY

////////////////////////////////////////////////////////////////////////////////
// class template CloneFactory
// Implements a generic cloning factory
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractProduct, 
        class ProductCreator = 
            AbstractProduct* (*)(const AbstractProduct*),
        template<typename, class>
            class FactoryErrorPolicy = DefaultFactoryError
    >
    class CloneFactory
        : public FactoryErrorPolicy<TypeInfo, AbstractProduct>
    {
    public:
        bool Register(const TypeInfo& ti, ProductCreator creator)
        {
            return associations_.insert(
                typename IdToProductMap::value_type(ti, creator)).second;
        }
        
        bool Unregister(const TypeInfo& id)
        {
            return associations_.erase(id) == 1;
        }
        
        AbstractProduct* CreateObject(const AbstractProduct* model)
        {
            if (model == 0) return 0;
            
            typename IdToProductMap::iterator i = 
                associations_.find(typeid(*model));
            if (i != associations_.end())
            {
                return (i->second)(model);
            }
            return this->OnUnknownType(typeid(*model));
        }
        
    private:
        typedef AssocVector<TypeInfo, ProductCreator> IdToProductMap;
        IdToProductMap associations_;
    };
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20,    2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May 08,     2002: replaced const_iterator with iterator so that self-modifying
//                   ProductCreators are supported. Also, added a throw()
//                   spec to what(). Credit due to Jason Fischl.
// February 2, 2003: fixed dependent names - credit due to Rani Sharoni
// March 4,    2003: fixed dependent names - credit due to Ruslan Zasukhin and CW 8.3 
// July 26,    2005: parameter support by Peter Kümmel 
////////////////////////////////////////////////////////////////////////////////

#endif // FACTORY_INC_

// $Log$
// Revision 1.4  2005/07/28 14:26:09  syntheticpp
// add cvs Header/Log
//

