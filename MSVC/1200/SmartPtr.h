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

// Last update: Oct 26, 2002
// replaced all template template parameters with 'normal' parameters
// For each Policy there is now a wrapper-class (non template class) 
// containing a nested template class called In which
// provides a typedef (type) to the real Policy-class
// 
// VC special: The MSVC 6.0 has problems with template ctors resp. template
// assignemt operators. If there exists a template copy ctor (a template op=) the
// compiler will complain about a non-template version. On the other hand, 
// if one does not provide a non-template version and one tries to
// initialize a new object with one having the same type the compiler will synthesize
// a default version (the same for assignment).
// Because the MSVC allows explicit specialization in class scope i used 
// this as a workaround.
// Instead of:
//	template <class T>
//	struct Foo
//	{
//		Foo(const Foo&)
//		{}
//		template <class U>
//		Foo(const Foo<U>& r)
//		{}
//	};
//	
//	this port uses:
//
//	template <class T>
//	struct Foo
//	{
//		template <class U>
//		Foo(const Foo<U>& r)
//		{}
//		template <>
//		Foo(const Foo& r)
//		{}
//	};

#ifndef SMARTPTR_INC_
#define SMARTPTR_INC_

////////////////////////////////////////////////////////////////////////////////
// IMPORTANT NOTE
// Due to threading issues, the OwnershipPolicy has been changed as follows:
//     Release() returns a boolean saying if that was the last release
//        so the pointer can be deleted by the StoragePolicy
//     IsUnique() was removed
////////////////////////////////////////////////////////////////////////////////


#include "SmallObj.h"
#include "TypeManip.h"
#include "MSVC6Helpers.h"
#include "static_check.h"
#include <functional>
#include <stdexcept>
#include <cassert>

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template DefaultSPStorage
// Implementation of the StoragePolicy used by SmartPtr
////////////////////////////////////////////////////////////////////////////////
	
    template <class T>
    class DefaultSPStorage
    {
    public:
        typedef T* StoredType;    // the type of the pointee_ object
        typedef T* PointerType;   // type returned by operator->
        typedef T& ReferenceType; // type returned by operator*
        
    public:
        DefaultSPStorage() : pointee_(Default()) 
        {}

        // The storage policy doesn't initialize the stored pointer 
        //     which will be initialized by the OwnershipPolicy's Clone fn
#if !defined(_MSC_VER)        
		DefaultSPStorage(const DefaultSPStorage&)
        {}
#endif
        template <class U>
        DefaultSPStorage(const DefaultSPStorage<U>&) 
        {}
#if _MSC_VER <= 1200
		template <>
        DefaultSPStorage(const DefaultSPStorage&) 
        {}
#endif
        DefaultSPStorage(const StoredType& p) : pointee_(p) {}
        
        PointerType operator->() const { return pointee_; }
        
        ReferenceType operator*() const { return *pointee_; }
        
        void Swap(DefaultSPStorage& rhs)
        { std::swap(pointee_, rhs.pointee_); }
    
        // Accessors
        friend inline PointerType GetImpl(const DefaultSPStorage& sp)
        { return sp.pointee_; }
        
    	friend inline const StoredType& GetImplRef(const DefaultSPStorage& sp)
    	{ return sp.pointee_; }

    	friend inline StoredType& GetImplRef(DefaultSPStorage& sp)
    	{ return sp.pointee_; }

    protected:
        // Destroys the data stored
        // (Destruction might be taken over by the OwnershipPolicy)
        void Destroy()
        { delete pointee_; }
        
        // Default value to initialize the pointer
        static StoredType Default()
        { return 0; }
    
    private:
        // Data
        StoredType pointee_;
    };
	
	struct DefaultSPStorageWrapper
	{
		template <class T>
		struct In
		{
			typedef DefaultSPStorage<T> type;
		};
	};
    
////////////////////////////////////////////////////////////////////////////////
// class template RefCounted
// Implementation of the OwnershipPolicy used by SmartPtr
// Provides a classic external reference counting implementation
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    class RefCounted
    {
    public:
        RefCounted() 
        {
            pCount_ = static_cast<unsigned int*>(
                SmallObject<>::operator new(sizeof(unsigned int)));
            assert(pCount_);
            *pCount_ = 1;
        }
        
#if !defined(_MSC_VER)        
		RefCounted(const RefCounted& rhs) 
        : pCount_(rhs.pCount_)
        {}
#endif
        
        // MWCW lacks template friends, hence the following kludge
        template <typename P1>
        RefCounted(const RefCounted<P1>& rhs) 
        : pCount_(reinterpret_cast<const RefCounted&>(rhs).pCount_)
        {}
        
#if _MSC_VER <= 1200
		template<>
		RefCounted(const RefCounted& rhs) 
        : pCount_(rhs.pCount_)
        {}
#endif
        P Clone(const P& val)
        {
            ++*pCount_;
            return val;
        }
        
        bool Release(const P&)
        {
            if (!--*pCount_)
            {
                SmallObject<>::operator delete(pCount_, sizeof(unsigned int));
                return true;
            }
            return false;
        }
        
        void Swap(RefCounted& rhs)
        { std::swap(pCount_, rhs.pCount_); }
    
        enum { destructiveCopy = false };
		
    private:
        // Data
        unsigned int* pCount_;
    };
    

	struct RefCountedWrapper
	{
		template<class T>
		struct In
		{
			typedef RefCounted<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template RefCountedMT
// Implementation of the OwnershipPolicy used by SmartPtr
// Implements external reference counting for multithreaded programs
////////////////////////////////////////////////////////////////////////////////
// Note: I could not figure out how this class is supposed to work. Therefore
// i could not port it.
// pCount has type volatile unsigned int but the different Thread-Policies 
// expect volatile int& resp. volatile long&
// :-(

	/*
	template <class P,
        template <class> class ThreadingModel>
    class RefCountedMT : public ThreadingModel< RefCountedMT<P, ThreadingModel> >
    {
    public:
        RefCountedMT() 
        {
            pCount_ = static_cast<unsigned int*>(
                SmallObject<ThreadingModel>::operator new(
                    sizeof(unsigned int)));
            assert(pCount_);
            *pCount_ = 1;
        }
        
        RefCountedMT(const RefCountedMT& rhs) 
        : pCount_(rhs.pCount_)
        {}
        
        // MWCW lacks template friends, hence the following kludge
        template <typename P1>
        RefCountedMT(const RefCountedMT<P1, ThreadingModel>& rhs) 
        : pCount_(reinterpret_cast<const RefCounted<P>&>(rhs).pCount_)
        {}
        
        P Clone(const P& val)
        {
            ThreadingModel<RefCountedMT>::AtomicIncrement(*pCount_);
            return val;
        }
        
        bool Release(const P&)
        {
            if (!ThreadingModel<RefCountedMT>::AtomicDecrement(*pCount_))
            {
                SmallObject<ThreadingModel>::operator delete(pCount_, 
                    sizeof(unsigned int));
                return true;
            }
            return false;
        }
        
        void Swap(RefCountedMT& rhs)
        { std::swap(pCount_, rhs.pCount_); }
    
        enum { destructiveCopy = false };

    private:
        // Data
        volatile unsigned int* pCount_;
    };*/
////////////////////////////////////////////////////////////////////////////////
// class template COMRefCounted
// Implementation of the OwnershipPolicy used by SmartPtr
// Adapts COM intrusive reference counting to OwnershipPolicy-specific syntax
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    class COMRefCounted
    {
    public:
        COMRefCounted()
        {}
        
        template <class U>
        COMRefCounted(const COMRefCounted<U>&)
        {}
        
        static P Clone(const P& val)
        {
            val->AddRef();
            return val;
        }
        
        static bool Release(const P& val)
        { val->Release(); return false; }
        
        enum { destructiveCopy = false };
        
        static void Swap(COMRefCounted&)
        {}
    };
	
	struct COMRefCountedWrapper
	{
		template <class T>
		struct In
		{
			typedef COMRefCounted<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template DeepCopy
// Implementation of the OwnershipPolicy used by SmartPtr
// Implements deep copy semantics, assumes existence of a Clone() member 
//     function of the pointee type
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    struct DeepCopy
    {
        DeepCopy()
        {}
        
        template <class P1>
        DeepCopy(const DeepCopy<P1>&)
        {}
        
        static P Clone(const P& val)
        { return val->Clone(); }
        
        static bool Release(const P& val)
        { return true; }
        
        static void Swap(DeepCopy&)
        {}
        
        enum { destructiveCopy = false };
    };

	struct DeepCopyWrapper
	{
		template <class T>
		struct In
		{
			typedef DeepCopy<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template RefLinked
// Implementation of the OwnershipPolicy used by SmartPtr
// Implements reference linking
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
        class RefLinkedBase
        {
        public:
            RefLinkedBase() 
            { prev_ = next_ = this; }
            
            RefLinkedBase(const RefLinkedBase& rhs) 
            {
                prev_ = &rhs;
                next_ = rhs.next_;
                prev_->next_ = this;
                next_->prev_ = this;
            }
            
            bool Release()
            {
                if (next_ == this)
                {   
                    assert(prev_ == this);
                    return true;
                }
                prev_->next_ = next_;
                next_->prev_ = prev_;
                return false;
            }
            
            void Swap(RefLinkedBase& rhs)
            {
                if (next_ == this)
                {
                    assert(prev_ == this);
                    if (rhs.next_ == &rhs)
                    {
                        assert(rhs.prev_ == &rhs);
                        // both lists are empty, nothing 2 do
                        return;
                    }
                    prev_ = rhs.prev_;
                    next_ = rhs.next_;
                    prev_->next_ = next_->prev_ = this;
                    rhs.next_ = rhs.prev_ = &rhs;
                    return;
                }
                if (rhs.next_ == &rhs)
                {
                    rhs.Swap(*this);
                    return;
                }
                std::swap(prev_, rhs.prev_);
                std::swap(next_, rhs.next_);
                std::swap(prev_->next_, rhs.prev_->next_);
                std::swap(next_->prev_, rhs.next_->prev_);
            }
                
            enum { destructiveCopy = false };

        private:
            mutable const RefLinkedBase* prev_;
            mutable const RefLinkedBase* next_;
        };
    }
    
    
	template <class P>
    class RefLinked : public Private::RefLinkedBase
    {
    public:
        RefLinked()
        {}
        
        template <class P1>
        RefLinked(const RefLinked<P1>& rhs) 
        : Private::RefLinkedBase(rhs)
        {}

        static P Clone(const P& val)
        { return val; }

        bool Release(const P&)
        {	typedef Private::RefLinkedBase MyBase;
			return this->MyBase::Release(); 
		}
    };
	
	struct RefLinkedWrapper
	{
		template <class T>
		struct In
		{
			typedef RefLinked<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template DestructiveCopy
// Implementation of the OwnershipPolicy used by SmartPtr
// Implements destructive copy semantics (a la std::auto_ptr)
////////////////////////////////////////////////////////////////////////////////

    template <class P>
    class DestructiveCopy
    {
    public:
        DestructiveCopy()
        {}
        
        template <class P1>
        DestructiveCopy(const DestructiveCopy<P1>&)
        {}
        
        template <class P1>
        static P Clone(P1& val)
        {
            P result(val);
            val = P1();
            return result;
        }
        
        static bool Release(const P&)
        { return true; }
        
        static void Swap(DestructiveCopy&)
        {}
        
        enum { destructiveCopy = true };
    };
    struct DestructiveCopyWrapper
	{
		template <class T>
		struct In
		{
			typedef DestructiveCopy<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template NoCopy
// Implementation of the OwnershipPolicy used by SmartPtr
// Implements a policy that doesn't allow copying objects
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    class NoCopy
    {
    public:
        NoCopy()
        {}
        
        template <class P1>
        NoCopy(const NoCopy<P1>&)
        {}
        
        static P Clone(const P&)
        {
            CT_ASSERT(false, This_Policy_Disallows_Value_Copying);
        }
        
        static bool Release(const P&)
        { return true; }
        
        static void Swap(NoCopy&)
        {}
        
        enum { destructiveCopy = false };
    };
    
	struct NoCopyWrapper
	{
		template <class T>
		struct In
		{
			typedef NoCopy<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template AllowConversion
// Implementation of the ConversionPolicy used by SmartPtr
// Allows implicit conversion from SmartPtr to the pointee type
////////////////////////////////////////////////////////////////////////////////

    struct AllowConversion
    {
        enum { allow = true };

        void Swap(AllowConversion&)
        {}
    };

////////////////////////////////////////////////////////////////////////////////
// class template DisallowConversion
// Implementation of the ConversionPolicy used by SmartPtr
// Does not allow implicit conversion from SmartPtr to the pointee type
// You can initialize a DisallowConversion with an AllowConversion
////////////////////////////////////////////////////////////////////////////////

    struct DisallowConversion
    {
        DisallowConversion()
        {}
        
        DisallowConversion(const AllowConversion&)
        {}
        
        enum { allow = false };

        void Swap(DisallowConversion&)
        {}
    };

////////////////////////////////////////////////////////////////////////////////
// class template NoCheck
// Implementation of the CheckingPolicy used by SmartPtr
// Well, it's clear what it does :o)
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    struct NoCheck
    {
        NoCheck()
        {}
        
        template <class P1>
        NoCheck(const NoCheck<P1>&)
        {}
        
        static void OnDefault(const P&)
        {}

        static void OnInit(const P&)
        {}

        static void OnDereference(const P&)
        {}

        static void Swap(NoCheck&)
        {}
    };
	
	struct NoCheckWrapper
	{
		template <class T>
		struct In
		{
			typedef NoCheck<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template AssertCheck
// Implementation of the CheckingPolicy used by SmartPtr
// Checks the pointer before dereference
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    struct AssertCheck
    {
        AssertCheck()
        {}
        
        template <class P1>
        AssertCheck(const AssertCheck<P1>&)
        {}
        
        template <class P1>
        AssertCheck(const NoCheck<P1>&)
        {}
        
        static void OnDefault(const P&)
        {}

        static void OnInit(const P&)
        {}

        static void OnDereference(P val)
        { assert(val); }

        static void Swap(AssertCheck&)
        {}
    };
	
	struct AssertCheckWrapper
	{
		template <class T>
		struct In {typedef AssertCheck<T> type;};
	};
////////////////////////////////////////////////////////////////////////////////
// class template AssertCheckStrict
// Implementation of the CheckingPolicy used by SmartPtr
// Checks the pointer against zero upon initialization and before dereference
// You can initialize an AssertCheckStrict with an AssertCheck 
////////////////////////////////////////////////////////////////////////////////

    template <class P>
    struct AssertCheckStrict
    {
        AssertCheckStrict()
        {}
        
        template <class U>
        AssertCheckStrict(const AssertCheckStrict<U>&)
        {}
        
        template <class U>
        AssertCheckStrict(const AssertCheck<U>&)
        {}
        
        template <class P1>
        AssertCheckStrict(const NoCheck<P1>&)
        {}
        
        static void OnDefault(P val)
        { assert(val); }
        
        static void OnInit(P val)
        { assert(val); }
        
        static void OnDereference(P val)
        { assert(val); }
        
        static void Swap(AssertCheckStrict&)
        {}
    };

	struct AssertCheckStrictWrapper
	{
		template <class T>
		struct In
		{
			typedef AssertCheckStrict<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class NullPointerException
// Used by some implementations of the CheckingPolicy used by SmartPtr
////////////////////////////////////////////////////////////////////////////////

    struct NullPointerException : public std::runtime_error
    {
        NullPointerException() : std::runtime_error("")
        { }
        const char* what() const throw()
        { return "Null Pointer Exception"; }
    };
        
////////////////////////////////////////////////////////////////////////////////
// class template RejectNullStatic
// Implementation of the CheckingPolicy used by SmartPtr
// Checks the pointer upon initialization and before dereference
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    struct RejectNullStatic
    {
        RejectNullStatic()
        {}
        
        template <class P1>
        RejectNullStatic(const RejectNullStatic<P1>&)
        {}
        
        template <class P1>
        RejectNullStatic(const NoCheck<P1>&)
        {}
        
        template <class P1>
        RejectNullStatic(const AssertCheck<P1>&)
        {}
        
        template <class P1>
        RejectNullStatic(const AssertCheckStrict<P1>&)
        {}
        
        static void OnDefault(const P&)
        {
            CompileTimeError<false>
                ERROR_This_Policy_Does_Not_Allow_Default_Initialization;
        }
        
        static void OnInit(const P& val)
        { if (!val) throw NullPointerException(); }
        
        static void OnDereference(const P& val)
        { if (!val) throw NullPointerException(); }
        
        static void Swap(RejectNullStatic&)
        {}
    };
	
	struct RejectNullStaticWrapper
	{
		template <class T>
		struct In
		{
			typedef RejectNullStatic<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template RejectNull
// Implementation of the CheckingPolicy used by SmartPtr
// Checks the pointer before dereference
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    struct RejectNull
    {
        RejectNull()
        {}
        
        template <class P1>
        RejectNull(const RejectNull<P1>&)
        {}
        
        static void OnInit(P val)
        { if (!val) throw NullPointerException(); }

        static void OnDefault(P val)
        { OnInit(val); }
        
        void OnDereference(P val)
        { OnInit(val); }
        
        void Swap(RejectNull&)
        {}        
    };
	
	struct RejectNullWrapper
	{
		template <class T>
		struct In
		{
			typedef RejectNull<T> type;
		};
	};

////////////////////////////////////////////////////////////////////////////////
// class template RejectNullStrict
// Implementation of the CheckingPolicy used by SmartPtr
// Checks the pointer upon initialization and before dereference
////////////////////////////////////////////////////////////////////////////////
	
    template <class P>
    struct RejectNullStrict
    {
        RejectNullStrict()
        {}
        
        template <class P1>
        RejectNullStrict(const RejectNullStrict<P1>&)
        {}
        
        template <class P1>
        RejectNullStrict(const RejectNull<P1>&)
        {}
        
        static void OnInit(P val)
        { if (!val) throw NullPointerException(); }

        void OnDereference(P val)
        { OnInit(val); }
        
        void Swap(RejectNullStrict&)
        {}        
    };
	
	struct RejectNullStrictWrapper
	{
		template <class T>
		struct In
		{
			typedef RejectNullStrict<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template ByRef
// Transports a reference as a value
// Serves to implement the Colvin/Gibbons trick for SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class ByRef
    {
    public:
        ByRef(T& v) : value_(v) {}
        operator T&() { return value_; }
        // gcc doesn't like this:
        // operator const T&() const { return value_; }
    private:
        T& value_;
    };

////////////////////////////////////////////////////////////////////////////////
// class template SmartPtr (declaration)
// The reason for all the fuss above
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OwnershipPolicy = RefCountedWrapper,
        class ConversionPolicy = DisallowConversion,
        class CheckingPolicy = AssertCheckWrapper,
        class StoragePolicy = DefaultSPStorageWrapper
    >
    class SmartPtr;


////////////////////////////////////////////////////////////////////////////////
// class template SmartPtr (definition)
////////////////////////////////////////////////////////////////////////////////

namespace Private
{
	template <class T, class Ow, class Con, class Check, class Stor>
	struct SmartPtrImpl
	{
		struct Dummy{};
		typedef typename ApplyInnerType<Stor, T>::type TempType;
		typedef VC_Base_Workaround<TempType, Dummy> sttype;
		// VC 6.0 will emit an "Error C2516. : is not a legal base class"
		// if one tries to use TempType as base class for SmartPtr.
		// Don't know why the compiler is happy with this workaround
		typedef sttype::LeftBase Storage; 
		typedef Storage::PointerType PointerType;
		typedef Storage::StoredType StoredType;
		typedef Storage::ReferenceType ReferenceType;

		typedef typename ApplyInnerType<Ow, PointerType>::type Temp2Type;
		typedef typename ApplyInnerType<Check, StoredType>::type Temp3Type;

		typedef VC_Base_Workaround<Temp2Type, Dummy> owtype;
		typedef owtype::LeftBase Owner;

		typedef VC_Base_Workaround<Temp3Type, Dummy> chtype;
		typedef chtype::LeftBase Checking;

		typedef Con Conversion;
	};
}
    template
    <
        typename T,
        class OwnershipPolicy,
        class ConversionPolicy,
        class CheckingPolicy,
        class StoragePolicy
    >
    class SmartPtr
		: public Private::SmartPtrImpl<T, OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy>::Storage
        , public Private::SmartPtrImpl<T, OwnershipPolicy, ConversionPolicy,CheckingPolicy, StoragePolicy>::Owner
        , public Private::SmartPtrImpl<T, OwnershipPolicy, ConversionPolicy,CheckingPolicy, StoragePolicy>::Checking
        , public Private::SmartPtrImpl<T, OwnershipPolicy, ConversionPolicy,CheckingPolicy, StoragePolicy>::Conversion
    {
    public:
        typedef typename Private::SmartPtrImpl
		<	
			T, OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy
		>::Storage SP;
		typedef SP::PointerType PointerType;
        typedef SP::StoredType StoredType;
        typedef SP::ReferenceType ReferenceType;
        
		typedef typename Private::SmartPtrImpl
		<	
			T, OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy
		>::Owner OP;
        typedef typename Private::SmartPtrImpl
		<	
			T, OwnershipPolicy, ConversionPolicy, CheckingPolicy, StoragePolicy
		>::Checking KP;
		
		typedef ConversionPolicy CP;
        typedef typename Select
		<
			OP::destructiveCopy, 
			SmartPtr, const SmartPtr
		>::Result CopyArg;
    
        // i think the following two ctors have an exception-safety problem
		// in the original version. If KP throws one can't release the resources
		// which were possibly allocated by SP and/or OP.
		// don't know if my solution works.
		SmartPtr()
        { 
			try
			{
				KP::OnDefault(GetImpl(*this));
			}
			catch(...)
			{
				if (OP::Release(GetImpl(*static_cast<SP*>(this))))
    			{
    				SP::Destroy();
    			}		
				throw;
			}
		}
    	
        SmartPtr(const StoredType& p) : SP(p)
        { 
			try
			{
				KP::OnInit(GetImpl(*this)); 
			}
			catch(...)
			{
				if (OP::Release(GetImpl(*static_cast<SP*>(this))))
    			{
    				SP::Destroy();
    			}		
				throw;
			}
		
		}

#if !defined(_MSC_VER)
		SmartPtr(CopyArg& rhs)
        : SP(rhs), OP(rhs), KP(rhs), CP(rhs)
#endif
		template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
    	SmartPtr(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	: SP(rhs), OP(rhs), KP(rhs), CP(rhs)
    	{ GetImplRef(*this) = OP::Clone(GetImplRef(rhs)); }

        template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
    	SmartPtr(SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	: SP(rhs), OP(rhs), KP(rhs), CP(rhs)
    	{ GetImplRef(*this) = OP::Clone(GetImplRef(rhs)); }

#if _MSC_VER <= 1200
		
		template <>
		SmartPtr(CopyArg& rhs)
        : SP(rhs), OP(rhs), KP(rhs), CP(rhs)
#endif
        
		{ GetImplRef(*this) = OP::Clone(GetImplRef(rhs)); }
        SmartPtr(ByRef<SmartPtr> rhs)
    	: SP(rhs), OP(rhs), KP(rhs), CP(rhs)
        {}
        
        operator ByRef<SmartPtr>()
        { return ByRef<SmartPtr>(*this); }
#if !defined(_MSC_VER)
    	SmartPtr& operator=(CopyArg& rhs)
    	{
    	    SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}
#endif
        template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
    	SmartPtr& operator=(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	{
    	    SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}
    	
        template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
    	SmartPtr& operator=(SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs)
    	{
    		
			SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}
#if _MSC_VER <= 1200    	
		template<>
		SmartPtr& operator=(CopyArg& rhs)
    	{
    	    SmartPtr temp(rhs);
    	    temp.Swap(*this);
    	    return *this;
    	}
#endif
    	void Swap(SmartPtr& rhs)
    	{
    	    OP::Swap(rhs);
    	    CP::Swap(rhs);
    	    KP::Swap(rhs);
    	    SP::Swap(rhs);
    	}
    	
    	~SmartPtr()
    	{
    	    if (OP::Release(GetImpl(*static_cast<SP*>(this))))
    	    {
    	        SP::Destroy();
    	    }
    	}
    	
    	friend inline void Release(SmartPtr& sp, StoredType& p)
    	{
    	    p = GetImplRef(sp);
    	    GetImplRef(sp) = SP::Default();
    	}
    	
    	friend inline void Reset(SmartPtr& sp, StoredType p)
    	{ SmartPtr(p).Swap(sp); }

        PointerType operator->()
        {
            KP::OnDereference(GetImplRef(*this));
            return SP::operator->();
        }

        PointerType operator->() const
        {
            KP::OnDereference(GetImplRef(*this));
            return SP::operator->();
        }

        ReferenceType operator*()
        {
            KP::OnDereference(GetImplRef(*this));
            return SP::operator*();
        }
    	
        ReferenceType operator*() const
        {
            KP::OnDereference(GetImplRef(*this));
            return SP::operator*();
        }
    	
        bool operator!() const // Enables "if (!sp) ..."
        { return GetImpl(*this) == 0; }
        
        inline friend bool operator==(const SmartPtr& lhs,
            const T* rhs)
        { return GetImpl(lhs) == rhs; }
        
        inline friend bool operator==(const T* lhs,
            const SmartPtr& rhs)
        { return rhs == lhs; }
        
        inline friend bool operator!=(const SmartPtr& lhs,
            const T* rhs)
        { return !(lhs == rhs); }
        
        inline friend bool operator!=(const T* lhs,
            const SmartPtr& rhs)
        { return rhs != lhs; }

        // Ambiguity buster
        template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
        bool operator==(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs) const
        { return *this == GetImpl(rhs); }

        // Ambiguity buster
        template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
        bool operator!=(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs) const
        { return !(*this == rhs); }

        // Ambiguity buster
        template
        <
            typename T1,
            class OP1,
            class CP1,
            class KP1,
            class SP1
        >
        bool operator<(const SmartPtr<T1, OP1, CP1, KP1, SP1>& rhs) const
        { return *this < GetImpl(rhs); }

    private:
        // Helper for enabling 'if (sp)'
        struct Tester
        {
            Tester() {}
        private:
            void operator delete(void*);
        };
        
    public:
        // enable 'if (sp)'
        operator Tester*() const
        {
            if (!*this) return 0;
            static Tester t;
            return &t;
        }

    private:
        // Helper for disallowing automatic conversion
        struct Insipid
        {
            Insipid(PointerType) {}
        };
        
        typedef typename Select<CP::allow, PointerType, Insipid>::Result
            AutomaticConversionResult;
    
    public:        
        operator AutomaticConversionResult() const
        { return GetImpl(*this); }
    };

////////////////////////////////////////////////////////////////////////////////
// free comparison operators for class template SmartPtr
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// operator== for lhs = SmartPtr, rhs = raw pointer
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator==(const SmartPtr<T, OP, CP, KP, SP>& lhs,
        const U* rhs)
    { return GetImpl(lhs) == rhs; }
    
////////////////////////////////////////////////////////////////////////////////
// operator== for lhs = raw pointer, rhs = SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator==(const U* lhs,
        const SmartPtr<T, OP, CP, KP, SP>& rhs)
    { return rhs == lhs; }

////////////////////////////////////////////////////////////////////////////////
// operator!= for lhs = SmartPtr, rhs = raw pointer
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator!=(const SmartPtr<T, OP, CP, KP, SP>& lhs,
        const U* rhs)
    { return !(lhs == rhs); }
    
////////////////////////////////////////////////////////////////////////////////
// operator!= for lhs = raw pointer, rhs = SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator!=(const U* lhs,
        const SmartPtr<T, OP, CP, KP, SP>& rhs)
    { return rhs != lhs; }

////////////////////////////////////////////////////////////////////////////////
// operator< for lhs = SmartPtr, rhs = raw pointer -- NOT DEFINED
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator<(const SmartPtr<T, OP, CP, KP, SP>& lhs,
        const U* rhs);
        
////////////////////////////////////////////////////////////////////////////////
// operator< for lhs = raw pointer, rhs = SmartPtr -- NOT DEFINED
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator<(const U* lhs,
        const SmartPtr<T, OP, CP, KP, SP>& rhs);
        
////////////////////////////////////////////////////////////////////////////////
// operator> for lhs = SmartPtr, rhs = raw pointer -- NOT DEFINED
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator>(const SmartPtr<T, OP, CP, KP, SP>& lhs,
        const U* rhs)
    { return rhs < lhs; }
        
////////////////////////////////////////////////////////////////////////////////
// operator> for lhs = raw pointer, rhs = SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator>(const U* lhs,
        const SmartPtr<T, OP, CP, KP, SP>& rhs)
    { return rhs < lhs; }
  
////////////////////////////////////////////////////////////////////////////////
// operator<= for lhs = SmartPtr, rhs = raw pointer
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator<=(const SmartPtr<T, OP, CP, KP, SP>& lhs,
        const U* rhs)
    { return !(rhs < lhs); }
        
////////////////////////////////////////////////////////////////////////////////
// operator<= for lhs = raw pointer, rhs = SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator<=(const U* lhs,
        const SmartPtr<T, OP, CP, KP, SP>& rhs)
    { return !(rhs < lhs); }

////////////////////////////////////////////////////////////////////////////////
// operator>= for lhs = SmartPtr, rhs = raw pointer
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator>=(const SmartPtr<T, OP, CP, KP, SP>& lhs,
        const U* rhs)
    { return !(lhs < rhs); }
        
////////////////////////////////////////////////////////////////////////////////
// operator>= for lhs = raw pointer, rhs = SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP,
        typename U
    >
    inline bool operator>=(const U* lhs,
        const SmartPtr<T, OP, CP, KP, SP>& rhs)
    { return !(lhs < rhs); }

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// specialization of std::less for SmartPtr
////////////////////////////////////////////////////////////////////////////////

// MSVC 6.0 does not support partial template specialization :-(
#if !defined(_MSC_VER)
namespace std
{
    template
    <
        typename T,
        class OP,
        class CP,
        class KP,
        class SP
    >
    struct less< Loki::SmartPtr<T, OP, CP, KP, SP> >
        : public binary_function<Loki::SmartPtr<T, OP, CP, KP, SP>,
            Loki::SmartPtr<T, OP, CP, KP, SP>, bool>
    {
        bool operator()(const Loki::SmartPtr<T, OP, CP, KP, SP>& lhs,
            const Loki::SmartPtr<T, OP, CP, KP, SP>& rhs) const
        { return less<T*>()(GetImpl(lhs), GetImpl(rhs)); }
    };
}
#endif
////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// December 09, 2001: Included <cassert>
// Oct  26, 2002: ported by Benjamin Kaufmann to MSVC 6.0
////////////////////////////////////////////////////////////////////////////////

#endif // SMARTPTR_INC_
