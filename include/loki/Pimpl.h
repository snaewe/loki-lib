////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_PIMPL_H
#define LOKI_PIMPL_H

#include <loki/TypeTraits.h>


#ifndef LOKI_INHERITED_PIMPL_NAME
#define LOKI_INHERITED_PIMPL_NAME pimpl
#endif

#ifndef LOKI_INHERITED_RIMPL_NAME
#define LOKI_INHERITED_RIMPL_NAME rimpl
#endif

namespace Loki
{
    template<class T>
    struct Impl;


    /////////////////////
    // creation policies
    /////////////////////
    // hard coded creation prefered


    /////////////////////
    // destroy policies
    /////////////////////
    template<class T>
    struct AutoDeletePimpl
    {
        static void Destroy(T ptr)
        { 
            typedef char T_must_be_defined[ 
                    sizeof(typename TypeTraits<T>::PointeeType) ? 1 : -1 ];
            delete ptr; 
            ptr = 0;
        }
    };

    template<class T>
    struct DontDeletePimpl
    {
        static void Destroy(T)
        {}
    };


    /////////////////////
    // Lifetime manager
    /////////////////////

    template
    <
        class Impl,
        class Ptr,
        template<class> class Del
    >
    struct PimplLife
    {
        PimplLife();
        ~PimplLife();

        template<class T>
        operator T&();

        Ptr Create();

        Ptr ptr;
    };


    namespace Private
    {
        template<class T> 
        struct HaveAPimplLife
        {
            T life;
        };
    }



    /////////////////////
    // Pimpl usage policies
    /////////////////////

    template<class Impl, typename Ptr, template<class> class Del>
    class DeclaredPimpl : private Private::HaveAPimplLife<PimplLife<Impl,Ptr,Del> >
    {
        typedef Private::HaveAPimplLife<PimplLife<Impl,Ptr,Del> > Life;

    public:
        Ptr operator->() const
        {
            return ptr;
        }

        Impl& operator*() const
        {
            return *ptr;
        }
    
    protected:
        DeclaredPimpl() : ptr(Life::life.Create())
        {}

    private:
        const Ptr ptr;
    };

    template<class Impl, typename Ptr, template<class> class Del>
    class InheritedPimpl : private Private::HaveAPimplLife<PimplLife<Impl,Ptr,Del> >
    {    
        typedef Private::HaveAPimplLife<PimplLife<Impl,Ptr,Del> > Life;

    public:
        const Ptr LOKI_INHERITED_PIMPL_NAME;

    protected:
        InheritedPimpl() : LOKI_INHERITED_PIMPL_NAME(Life::life.Create())
        {}
    };

    /////////////////////
    // Rimpl usage policies
    /////////////////////

    template<class Impl, typename PointerPolicy, template<class> class DeletePolicy>
    class DeclaredRimpl : public Impl
    {
    protected:
        DeclaredRimpl()
        {}
    };


    template<class Impl, typename Ptr, template<class> class Del>
    class InheritedRimpl : private Private::HaveAPimplLife<PimplLife<Impl,Ptr,Del> >
    {
        typedef Private::HaveAPimplLife<PimplLife<Impl,Ptr,Del> > Life;

    public:
        Impl& LOKI_INHERITED_RIMPL_NAME;

    protected:
        InheritedRimpl() :
            LOKI_INHERITED_RIMPL_NAME(*Life::life.Create())
        {}

    private:
        InheritedRimpl& operator=(const InheritedRimpl&);
    };




    /////////////////////
    // Wrapper for "pointer to implementation" alias pimpl.
    // Impl: implementation class
    // PointerTypePolicy: arbitrary pointer implementation
    // DeletePolicy: delete implementation object on destruction of PtrImpl
    // UsagePolicy: how to access the stored pointer, as pointer, checked, unchecked
    /////////////////////


    template
    <
        class Impl,
        class PointerPolicy = Impl*,
        template<class> class DeletePolicy = AutoDeletePimpl,
        template<class,class,template<class> class> class UsagePolicy = InheritedPimpl
    >
    class PtrImpl : public UsagePolicy<Impl,PointerPolicy,DeletePolicy>
    {
        typedef PimplLife<Impl,PointerPolicy,DeletePolicy> Life;

    public:

        typedef Impl ImplType;
        typedef PointerPolicy PointerType;
    
        PtrImpl() : UsagePolicy<Impl,PointerPolicy,DeletePolicy>()//Private::HaveALife<Life>::life.Create())
        {}

        PtrImpl(PointerPolicy pimpl) : UsagePolicy<Impl,PointerPolicy,DeletePolicy>(pimpl)
        {}

        virtual ~PtrImpl()
        {}
    };

    
    /////////////////////
    // Predefined convenience "templated typedef"
    /////////////////////


    template<class T>
    struct Pimpl
    {
        typedef PtrImpl
        <
            Impl<T>,
            Impl<T>*,
            AutoDeletePimpl,
            DeclaredPimpl
        >
        Type;

        typedef PtrImpl
        <
            Impl<T>,
            Impl<T>*,
            AutoDeletePimpl,
            InheritedPimpl
        >
        Owner;
    };


    template<class T>
    struct Rimpl
    {

        typedef PtrImpl
        <
            Impl<T>,
            Impl<T>*,
            DontDeletePimpl,
            DeclaredRimpl
        >
        Type;


        typedef PimplLife
        <
            Type,
            Type*,
            AutoDeletePimpl
        >
        Life;



        typedef PtrImpl
        <
            Impl<T>,
            Impl<T>*,
            AutoDeletePimpl,
            InheritedRimpl
        >
        Owner;

    };
    

}

#endif
