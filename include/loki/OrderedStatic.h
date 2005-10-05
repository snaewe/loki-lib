////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header:

#ifndef LOKI_ORDEREDSTATIC_H_
#define LOKI_ORDEREDSTATIC_H_

#include <vector>
#include <iostream>

#include "loki/Singleton.h"
#include "loki/TypeList.h"


namespace Loki
{
	namespace Private
	{
		////////////////////////////////////////////////////////////////////////////////
		// polymorph base class for OrderedStatic template,
		// necessary because of creator
		////////////////////////////////////////////////////////////////////////////////
		class OrderedStaticCreatorFunc
		{
		public:
			virtual void createObject() = 0;
		
		protected:
			OrderedStaticCreatorFunc();
			virtual ~OrderedStaticCreatorFunc();
		
		private:
			OrderedStaticCreatorFunc(const OrderedStaticCreatorFunc&);
		};

		////////////////////////////////////////////////////////////////////////////////
		// template base clase for OrderedStatic template, 
		// common for all specializations
		////////////////////////////////////////////////////////////////////////////////
		template<class T>
		class OrderedStaticBase : public OrderedStaticCreatorFunc
		{
		public:
			T& operator*()
			{
				return *val_;
			}

		protected:
			OrderedStaticBase(unsigned int longevity) :  val_(0), longevity_(longevity)
			{}
			virtual ~OrderedStaticBase()
			{}

			T* val_;
			unsigned int longevity_;
			
		private:
			OrderedStaticBase();
			OrderedStaticBase(const OrderedStaticBase&);
		};

		////////////////////////////////////////////////////////////////////////////////
		// OrderedStaticManagerClass implements details 
		// OrderedStaticManager is then defined as a Singleton
		////////////////////////////////////////////////////////////////////////////////
		class OrderedStaticManagerClass
		{
		public:
			OrderedStaticManagerClass();
			virtual ~OrderedStaticManagerClass();

			typedef void (OrderedStaticCreatorFunc::*Creator)();

			void createObjects();
			void registerObject(OrderedStaticCreatorFunc*,Creator, unsigned int longevity);

		private:
			OrderedStaticManagerClass(const OrderedStaticManagerClass&);
			OrderedStaticManagerClass& operator=(const OrderedStaticManagerClass&);
			
			struct Data
			{
				Data(OrderedStaticCreatorFunc*, Creator, unsigned int);
				OrderedStaticCreatorFunc* object;
				Creator creator;
				unsigned int longevity;
			};

			std::vector<Data> staticObjects_;
			unsigned int max_longevity_;
			unsigned int min_longevity_;
		};

	}// namespace Private

	////////////////////////////////////////////////////////////////////////////////
	// OrderedStaticManager is only a a Singleton typedef
	////////////////////////////////////////////////////////////////////////////////

	typedef Loki::SingletonHolder
	<
		Loki::Private::OrderedStaticManagerClass, 
		Loki::CreateUsingNew,
		Loki::NoDestroy,
		Loki::SingleThreaded
	>
	OrderedStaticManager;

	////////////////////////////////////////////////////////////////////////////////
	// template OrderedSatic template: 
	// L		: longevity
	// T		: object type
	// TList	: creator parameters
	////////////////////////////////////////////////////////////////////////////////

	template<int L, class T, class TList = Loki::NullType>
	class OrderedStatic;


	////////////////////////////////////////////////////////////////////////////////
	// OrderedSatic specializations
	////////////////////////////////////////////////////////////////////////////////

	template<int L, class T>
	class OrderedStatic<L, T, Loki::NullType> : public Private::OrderedStaticBase<T>
	{
	public:
		
		OrderedStatic() : Private::OrderedStaticBase<T>(L)
		{
			OrderedStaticManager::Instance().registerObject
				(this,&Private::OrderedStaticCreatorFunc::createObject, L);
		}

		void createObject()
		{
			Private::OrderedStaticBase<T>::val_ = new T;
			Loki::SetLongevity( Private::OrderedStaticBase<T>::val_, Private::OrderedStaticBase<T>::longevity_);
		}
	};

	template<int L, class T, typename P1>
	class OrderedStatic<L, T, LOKI_TYPELIST_1(P1)> : public Private::OrderedStaticBase<T>
	{
	public:
		OrderedStatic(P1 p) : Private::OrderedStaticBase<T>(L)
		{
			OrderedStaticManager::Instance().registerObject
							(this,&Private::OrderedStaticCreatorFunc::createObject, L);
			para_ = p;
		}

		void createObject()
		{
			Private::OrderedStaticBase<T>::val_ = new T(para_);
			Loki::SetLongevity(Private::OrderedStaticBase<T>::val_, Private::OrderedStaticBase<T>::longevity_);
		}

		T& operator*()
		{
			return *Private::OrderedStaticBase<T>::val_;
		}

	private:
		OrderedStatic();
		P1 para_;
	};

	template<int L, class T, typename P1>
	class OrderedStatic<L, T,  P1(*)() > : public Private::OrderedStaticBase<T>
	{
	public:

		typedef P1(*Func)();

		OrderedStatic() : Private::OrderedStaticBase<T>(L)
		{
			OrderedStaticManager::Instance().registerObject
							(this,&Private::OrderedStaticCreatorFunc::createObject, L);
		}

		OrderedStatic(Func p) : Private::OrderedStaticBase<T>(L)
		{
			OrderedStaticManager::Instance().registerObject
								(this,&Private::OrderedStaticCreatorFunc::createObject, L);
			para_ = p;
		}

		void createObject()
		{
			Private::OrderedStaticBase<T>::val_ = new T(para_());
			Loki::SetLongevity(Private::OrderedStaticBase<T>::val_, Private::OrderedStaticBase<T>::longevity_);
		}

		T& operator*()
		{
			return *Private::OrderedStaticBase<T>::val_;
		}

	private:
		Func para_;
	};

}// namespace Loki


#endif
