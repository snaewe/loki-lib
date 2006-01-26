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
#include <loki/SmartPtr.h>
#include <exception>
#include <memory>


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
        // does not work with std::auto_ptr
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

            // defined in #include<loki/PimplDef.h>
            ~AutoPtrHolder(); 
            
           
            Ptr Create()
            {
                ptr = Ptr( new Impl );
                return ptr;
            }

            Ptr ptr;
        };

        // don't compile with std::auto_ptr
        template<class Impl,template<class> class Del>
        struct AutoPtrHolder<Impl,std::auto_ptr<Impl>,Del>{};


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
                AutoPtrHolder<Impl,Ptr,Del>::Create();
                return *AutoPtrHolder<Impl,Ptr,Del>::ptr;
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
        Ptr LOKI_INHERITED_PIMPL_NAME;

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

#if defined(LOKI_DEFAULT_CONSTNESS) 
		typedef typename LOKI_DEFAULT_CONSTNESS<Ptr>::Type ConstPtr;
		typedef typename LOKI_DEFAULT_CONSTNESS<Impl>::Type ConstImpl;
#else // default: enable 
		typedef const Ptr ConstPtr;
		typedef const Impl ConstImpl;
#endif

    public:

		Ptr operator->()
        {
            return ptr;
        }

        Impl& operator*()
        {
            return *ptr;
        }

        ConstPtr operator->() const
        {
            return ptr;
        }

        ConstImpl& operator*() const
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
        Ptr;

        // init declared rimpl
        typedef Private::AutoPtrHolderChecked
        <
            Ptr,
            Ptr*,
            AutoDeletePimpl
        >
        Init;

        typedef Ptr & Type;


        // inherit rimpl
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
// Revision 1.14  2006/01/26 14:28:59  syntheticpp
// remove wrong 'typename'
//
// Revision 1.13  2006/01/23 17:22:49  syntheticpp
// add support of deep constness, only supported by (future) Loki::SmartPtr, not supported by boost::shared_ptr and plain pointer. Maybe deep constness forces a redesign of Pimpl. Is there a way to support deep constness by a rimpl?
//
// Revision 1.12  2006/01/19 18:16:39  syntheticpp
// disable usage with auto_ptr: don't compile with std::auto_ptr
//
// Revision 1.11  2006/01/18 19:03:06  syntheticpp
// make rimpl type really a reference
//
// Revision 1.10  2006/01/18 16:49:06  syntheticpp
// move definition of the destructor to extra file, because msvc tries to use the incomplete type; not inlining does not help; maybe this is a compiler bug.
//
// Revision 1.9  2006/01/17 12:03:36  syntheticpp
// add comment about auto_ptr
//
// Revision 1.8  2006/01/17 11:07:34  syntheticpp
// AutoPtrHolderChecked inherits from AutoPtrHolder
//
// Revision 1.7  2006/01/16 19:48:23  syntheticpp
// add error policy
//
// Revision 1.6  2006/01/16 19:05:09  rich_sposato
// Added cvs keywords.
//
