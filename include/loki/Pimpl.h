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
#define LOKI_INHERITED_PIMPL_NAME d
#endif

#ifndef LOKI_INHERITED_RIMPL_NAME
#define LOKI_INHERITED_RIMPL_NAME d
#endif

namespace Loki
{
    /////////////////////
    //  template for the implementations
    /////////////////////
    template<class T>
    struct Impl;




    /////////////////////
    // creation policies
    /////////////////////
    // hard coded creation preferred


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
    // error handling
    /////////////////////
    template<class T>
    struct ExceptionOnPimplError
    {
        struct Exception : public std::exception
        {
            const char* what() const throw() { return "error in loki/Pimpl.h"; }
        };
        
        static void PimplError()
        {
            throw Exception();
        }
        
    };

    template<class T>
    struct IgnorPimplError
    {
        static void PimplError()
        {}
        
    };

    /////////////////////
    // Helper class AutoPtrHolder to manage pimpl lifetime
    /////////////////////

    namespace Private
    {
        template
        <
            class Impl,
            class Ptr,
            template<class> class Del
        >
        struct AutoPtrHolder
        {
            AutoPtrHolder() : ptr(Ptr())
            {}              

            ~AutoPtrHolder()
            {
                // delete automatically by the delete policy
                Del<Ptr>::Destroy( ptr );
            }

            Ptr Create()
            {
                ptr = Ptr( new Impl );
                return ptr;
            }

            Ptr ptr;
        };

        template
        <
            class Impl,
            class Ptr,
            template<class> class Del,
            template<class> class ErrorPolicy = ExceptionOnPimplError
        >
        struct AutoPtrHolderChecked : AutoPtrHolder<Impl,Ptr,Del>
        {
            static bool init_;

            AutoPtrHolderChecked()
            {
                init_ = true;
            }              

            template<class T>
            operator T&()
            {
                // if this throws change the declaration order
                // of the DeclaredRimpl construct
                if(!init_)
                    ErrorPolicy<T>::PimplError();
                Create();
                return *ptr;
            }
        };

        template
        <
            class Impl,
            class Ptr,
            template<class> class Del,
            template<class> class Err
        >
        bool AutoPtrHolderChecked<Impl,Ptr,Del,Err>::init_ = false;


        template<class T> 
        struct HavePtrHolder
        {
            T ptr;
        };
    }



    /////////////////////
    // Pimpl usage policies
    /////////////////////

    template<class Impl, typename Ptr, template<class> class Del>
    class InheritedPimpl : private 
        Private::HavePtrHolder<Private::AutoPtrHolder<Impl,Ptr,Del> >
    {    
        typedef Private::HavePtrHolder<Private::AutoPtrHolder<Impl,Ptr,Del> > PtrHolder;

    public:
        const Ptr LOKI_INHERITED_PIMPL_NAME;

    protected:
        InheritedPimpl() : LOKI_INHERITED_PIMPL_NAME(PtrHolder::ptr.Create())
        {}

    private:
        InheritedPimpl& operator=(const InheritedPimpl&);
    };

    template<class Impl, typename Ptr, template<class> class Del>
    class DeclaredPimpl : private 
        Private::HavePtrHolder<Private::AutoPtrHolder<Impl,Ptr,Del> >
    {
        typedef Private::HavePtrHolder<Private::AutoPtrHolder<Impl,Ptr,Del> > PtrHolder;

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
        DeclaredPimpl() : ptr(PtrHolder::ptr.Create())
        {}

    private:
        DeclaredPimpl& operator=(const DeclaredPimpl&);
        const Ptr ptr;
    };

    /////////////////////
    // Rimpl usage policies
    /////////////////////


    template<class Impl, typename Ptr, template<class> class Del>
    class InheritedRimpl : private 
        Private::HavePtrHolder<Private::AutoPtrHolder<Impl,Ptr,Del> >
    {
        typedef Private::HavePtrHolder<Private::AutoPtrHolder<Impl,Ptr,Del> > PtrHolder;

    public:
        Impl& LOKI_INHERITED_RIMPL_NAME;

    protected:
        InheritedRimpl() :
            LOKI_INHERITED_RIMPL_NAME(*PtrHolder::ptr.Create())
        {}

    private:
        InheritedRimpl& operator=(const InheritedRimpl&);
    };

    template<class Impl, typename PointerPolicy, template<class> class DeletePolicy>
    class DeclaredRimpl : public Impl
    {
    protected:
        DeclaredRimpl()
        {}
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
    public:

        typedef Impl ImplType;
        typedef PointerPolicy PointerType;
    
        PtrImpl() : UsagePolicy<Impl,PointerPolicy,DeletePolicy>()
        {}

    private:
        PtrImpl& operator=(const PtrImpl&);

    };

    
    /////////////////////
    // Predefined convenience "templated typedef"
    /////////////////////


    template<class T>
    struct Pimpl
    {
        // declare pimpl
        typedef PtrImpl
        <
            Impl<T>,
            Impl<T>*,
            AutoDeletePimpl,
            DeclaredPimpl
        >
        Type;

        // inherit pimpl
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

        // declare rimpl
        typedef PtrImpl
        <
            Impl<T>,
            Impl<T>*,
            DontDeletePimpl,
            DeclaredRimpl
        >
        Type;

        // init declared rimpl
        typedef Private::AutoPtrHolderChecked
        <
            Type,
            Type*,
            AutoDeletePimpl
        >
        Init;


        // ingerit rimpl
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

// $Log$
// Revision 1.8  2006/01/17 11:07:34  syntheticpp
// AutoPtrHolderChecked inherits from AutoPtrHolder
//
// Revision 1.7  2006/01/16 19:48:23  syntheticpp
// add error policy
//
// Revision 1.6  2006/01/16 19:05:09  rich_sposato
// Added cvs keywords.
//
