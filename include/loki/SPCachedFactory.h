////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 by Guillaume Chatelet
//
// Code covered by the MIT License
//
// Permission to use, copy, modify, distribute and sell this software for any 
// purpose is hereby granted without fee, provided that the above copyright 
// notice appear in all copies and that both that copyright notice and this 
// permission notice appear in supporting documentation.
//
// The authors make no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
//
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SPCACHEDFACTORY_H_
#define SPCACHEDFACTORY_H_

#include <loki/Functor.h>
#include <loki/SmartPtr.h>
#include <loki/CachedFactory.h>

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
///  \class FunctionStorage
///
///  \ingroup  SmartPointerStorageGroup 
///  Implementation of the StoragePolicy used by SmartPtr
////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class FunctionStorage
    {
    public:
        typedef T* StoredType;    // the type of the pointee_ object
        typedef T* InitPointerType; /// type used to declare OwnershipPolicy type.
        typedef T* PointerType;   // type returned by operator->
        typedef T& ReferenceType; // type returned by operator*
        typedef Functor< void , Seq< void* > > FunctorType; // type of the Functor to set

        FunctionStorage() : pointee_(Default()), functor_()
        {}

        // The storage policy doesn't initialize the stored pointer 
        //     which will be initialized by the OwnershipPolicy's Clone fn
        FunctionStorage(const FunctionStorage& rsh) : pointee_(0), functor_(rsh.functor_)
        {}

        template <class U>
        FunctionStorage(const FunctionStorage<U>& rsh) : pointee_(0), functor_(rsh.functor_)
        {}
        
        FunctionStorage(const StoredType& p) : pointee_(p), functor_() {}
        
        PointerType operator->() const { return pointee_; }
        
        ReferenceType operator*() const { return *pointee_; }
        
        void Swap(FunctionStorage& rhs)
        { 
        	std::swap(pointee_, rhs.pointee_);
        	std::swap(functor_, rhs.functor_);
        }
        
        void SetCallBackFunction(const FunctorType &functor)
        {
        	functor_ = functor;
        }
    
        // Accessors
        template <class F>
        friend typename FunctionStorage<F>::PointerType GetImpl(const FunctionStorage<F>& sp);

        template <class F>
        friend const typename FunctionStorage<F>::StoredType& GetImplRef(const FunctionStorage<F>& sp);

        template <class F>
        friend typename FunctionStorage<F>::StoredType& GetImplRef(FunctionStorage<F>& sp);

    protected:
        // Destroys the data stored
        // (Destruction might be taken over by the OwnershipPolicy)
        void Destroy()
        {
            functor_(this);
        }

        // Default value to initialize the pointer
        static StoredType Default()
        { return 0; }
    
    private:
        // Data
        StoredType pointee_;
        FunctorType functor_;
    };

    template <class T>
    inline typename FunctionStorage<T>::PointerType GetImpl(const FunctionStorage<T>& sp)
    { return sp.pointee_; }

    template <class T>
    inline const typename FunctionStorage<T>::StoredType& GetImplRef(const FunctionStorage<T>& sp)
    { return sp.pointee_; }

    template <class T>
    inline typename FunctionStorage<T>::StoredType& GetImplRef(FunctionStorage<T>& sp)
    { return sp.pointee_; }

    /**
	 * \class	SmartPointer
	 * \ingroup	EncapsulationPolicyCachedFactoryGroup
	 * \brief	Encapsulate the object in a SmartPtr with FunctionStorage policy
	 * 
	 * Encapsulate the object in a SmartPtr with FunctionStorage policy.
	 * The object will come back to the Cache as soon as the smartPtr leaves the scope.
	 */
     template
     <
     	class AbstractProduct,
     	template <class> class OwnershipPolicy = RefCounted,
        class ConversionPolicy = DisallowConversion,
        template <class> class CheckingPolicy = AssertCheck,
        template<class> class ConstnessPolicy = LOKI_DEFAULT_CONSTNESS 
     >     
     class SmartPointer
     {
     private:
     	   typedef SmartPtr< AbstractProduct,OwnershipPolicy,
     	   	ConversionPolicy, CheckingPolicy,
     	   	FunctionStorage, ConstnessPolicy > CallBackSP;
     protected:           
           typedef CallBackSP ProductReturn;
           SmartPointer() : fun(this, &SmartPointer::smartPointerCallbackFunction) {}
           virtual ~SmartPointer(){};
           
           ProductReturn encapsulate(AbstractProduct* pProduct)
           {
           		CallBackSP SP(pProduct);
           		SP.SetCallBackFunction(fun);
                return SP;
           }
           
           AbstractProduct* release(ProductReturn &pProduct)
           {
                return GetImpl(pProduct);
           }
           
           const char* name(){return "smart pointer";}

     private:
     	   void smartPointerCallbackFunction(void* pSP)
     	   {
     	   		CallBackSP &SP(*reinterpret_cast<CallBackSP*>(pSP));
     	   		ReleaseObject(SP);
     	   }
           virtual void ReleaseObject(ProductReturn &object)=0;
           const typename CallBackSP::FunctorType fun;
     };

} // namespace Loki

#endif /*SPCACHEDFACTORY_H_*/
