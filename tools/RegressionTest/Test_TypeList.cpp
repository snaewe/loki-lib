#include <cstdio>
#include <typeinfo>
#include <Loki\Typelist.h>
#include <iostream>
using std::cout;
using std::endl;

using namespace ::Loki;

namespace
{
	typedef struct IncompType *IncompPtr;

	struct A1 {};
	struct B1 : A1 {};

	struct A2 {};
	struct B2 : A2 {};

	struct C1 : B1 {};
	struct C2 : B2 {};
	struct D1 : C1, C2 {};

	template<typename TList>
	struct PrintTypelist
	{
		PrintTypelist()
		{
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;

			printf("%s ,", typeid(Head).name());

			PrintTypelist<Tail>();
		}
	};

	template<>
	struct PrintTypelist<NullType>
	{
		PrintTypelist()
		{
			printf("\n");
		}
	};


	void Test_Length()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, int, B2*, B2 const *, B1>::Result typelist_t;

		printf("%u\n", TL::Length<typelist_t>::value);
	}


	void Test_TypeAt()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, int>::Result typelist_t;

		printf("%s, ", typeid(TL::TypeAt<typelist_t, 0>::Result).name());
		printf("%s, ", typeid(TL::TypeAt<typelist_t, 1>::Result).name());
		printf("%s, ", typeid(TL::TypeAt<typelist_t, 2>::Result).name());
		printf("%s, ", typeid(TL::TypeAt<typelist_t, 3>::Result).name());
		printf("%s, ", typeid(TL::TypeAt<typelist_t, 4>::Result).name());
	    
		printf("\n");
	}

	void Test_IndexOf()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, int, B2*, B2 const *, B1>::Result typelist_t;

		printf("%u, ", TL::IndexOf<typelist_t, IncompPtr>::value);
		printf("%u, ", TL::IndexOf<typelist_t, B1>::value);
		printf("%u, ", TL::IndexOf<typelist_t, B2*>::value);
		printf("%u, ", TL::IndexOf<typelist_t, A2**>::value);
		printf("%u, ", TL::IndexOf<typelist_t, int>::value);
		printf("%u, ", TL::IndexOf<typelist_t, B2 const *>::value);
		printf("%u, ", TL::IndexOf<typelist_t, const B1>::value);
		printf("%u, ", TL::IndexOf<typelist_t, B1&>::value);
		printf("%u, ", TL::IndexOf<typelist_t, int const &>::value);

		printf("\n");
	}

	void Test_TypeAtNonStrict()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, int>::Result typelist_t;

		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 0>::Result).name());
		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 1>::Result).name());
		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 2, double>::Result).name());
		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 3>::Result).name());
		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 4>::Result).name());
		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 5>::Result).name());
		printf("%s, ", typeid(TL::TypeAtNonStrict<typelist_t, 9, void>::Result).name());
	    
		printf("\n");
	}

	void Test_Append()
	{
		typedef TL::MakeTypeList<int, A1, A2>::Result typelist1_t;
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**>::Result typelist2_t;

		PrintTypelist<TL::Append<NullType, NullType>::Result>();
		PrintTypelist<TL::Append<NullType, typelist2_t>::Result>();
		PrintTypelist<TL::Append<typelist1_t, NullType>::Result>();
		PrintTypelist<TL::Append<NullType, int>::Result>();
		PrintTypelist<TL::Append<typelist1_t, int[10]>::Result>();
		PrintTypelist<TL::Append<typelist1_t, typelist2_t>::Result>();
	}

	void Test_Erase()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B1>::Result typelist_t;

		PrintTypelist<TL::Erase<typelist_t, int>::Result>();
		PrintTypelist<TL::Erase<typelist_t, B1>::Result>();
		PrintTypelist<TL::Erase<typelist_t, B2>::Result>();
		PrintTypelist<TL::Erase<typelist_t, const B1>::Result>();
		PrintTypelist<TL::Erase<typelist_t, B2 const *>::Result>();
		PrintTypelist<TL::Erase<typelist_t, A2**>::Result>();
		PrintTypelist<TL::Erase<typelist_t, NullType>::Result>();
	}

	void Test_EraseAll()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B1, B2*, B2 const *, B1>::Result typelist_t;

		PrintTypelist<TL::EraseAll<typelist_t, int>::Result>();
		PrintTypelist<TL::EraseAll<typelist_t, B1>::Result>();
		PrintTypelist<TL::EraseAll<typelist_t, B2*>::Result>();
		PrintTypelist<TL::EraseAll<typelist_t, B2 const *>::Result>();
		PrintTypelist<TL::EraseAll<typelist_t, NullType>::Result>();
	}

	void Test_NoDuplicates()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B1, B2*, B2 const *, B1>::Result typelist1_t;
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B2 const *>::Result              typelist2_t;

		PrintTypelist<TL::NoDuplicates<typelist1_t>::Result>();
		PrintTypelist<TL::NoDuplicates<typelist2_t>::Result>();
	}

	void Test_Replace()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B1, B2*, B2 const *, B1>::Result typelist1_t;

		PrintTypelist<TL::Replace<typelist1_t, int, A1>::Result>();
		PrintTypelist<TL::Replace<typelist1_t, B1, int>::Result>();
		PrintTypelist<TL::Replace<typelist1_t, B2*, B2[1]>::Result>();
	}

	void Test_ReplaceAll()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B1, B2*, B2 const *, B1>::Result typelist1_t;

		PrintTypelist<TL::ReplaceAll<typelist1_t, int, A1>::Result>();
		PrintTypelist<TL::ReplaceAll<typelist1_t, B1, int>::Result>();
		PrintTypelist<TL::ReplaceAll<typelist1_t, B2*, B2[1]>::Result>();
	}

	void Test_Reverse()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**, B1, B2*, B2 const *, B1>::Result typelist1_t;

		PrintTypelist<TL::Reverse<typelist1_t>::Result>();
	}

	void Test_MostDerived()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2, C1, B1, B2*, B2 const *, B1>::Result typelist1_t;

		printf("%s, ", typeid(TL::MostDerived<typelist1_t, A2>::Result).name());
		printf("%s, ", typeid(TL::MostDerived<typelist1_t, A1>::Result).name());
		printf("%s, ", typeid(TL::MostDerived<typelist1_t, B1>::Result).name());
	    
		printf("\n");
	}

	void Test_DerivedToFront()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2, A2, C1, B1, C2, D1, B2, A1, B1, A1>::Result typelist1_t;
		typedef TL::MakeTypeList<B1, A1, C1>::Result typelist2_t;

		PrintTypelist<TL::DerivedToFront<typelist1_t>::Result>();
		PrintTypelist<TL::DerivedToFront<typelist2_t>::Result>();
	}

	void Test_DerivedToFrontAll()
	{
		typedef TL::MakeTypeList<IncompPtr, B1, B2, A2, C1, B1, C2, D1, B2, A1, B1, A1>::Result typelist1_t;
		typedef TL::MakeTypeList<B1, A1, C1>::Result typelist2_t;

		PrintTypelist<TL::DerivedToFrontAll<typelist1_t>::Result>();
		PrintTypelist<TL::DerivedToFrontAll<typelist2_t>::Result>();
	}

}// anon namespace

extern void Test_TypeList()
{
	cout << "Testing Loki::TypeList" << endl;
	Test_Length();
	Test_TypeAt();
	Test_TypeAtNonStrict();
	Test_IndexOf();
	Test_Append();    
	Test_Erase();
	Test_EraseAll();
	Test_NoDuplicates();
	Test_Replace();
	Test_ReplaceAll();
	Test_Reverse();
	Test_MostDerived();
	Test_DerivedToFront();
	Test_DerivedToFrontAll();
	cout << endl << endl;
}


