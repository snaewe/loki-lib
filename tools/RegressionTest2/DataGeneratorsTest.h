//DataGeneratorsTest.h


#ifndef DATAGENERATORSTEST_H
#define DATAGENERATORSTEST_H

#include <typeinfo>
#include <Loki/DataGenerators.h>
#include "UnitTest.h"

template<typename T>
T& remove_const(const T& t)
	{
	return const_cast<T&>(t);
	}


struct DataGeneratorsTest : public Test
	{
	DataGeneratorsTest() : Test("DataGeneratorsTest.h")
		{}
	virtual void execute(TestResult& result)
		{
		this->printName(result);
		
		bool b;
		typedef Loki::TL::MakeTypelist<>::Result null_tl;
		typedef Loki::TL::MakeTypelist<char,
		                               unsigned char,
		                               signed char,
		                               wchar_t>::Result char_types;
		int n = Loki::TL::Length<char_types>::value;
		
		std::vector<const char*> names;
		names.reserve(n);
		Loki::TL::IterateTypes<char_types, Loki::TL::name_from_type> makenames;
		//Some gcc fascist decided to make all temporaries /const/!
		makenames(remove_const(std::back_inserter(names)));
		b = names.size() == n;
		testAssert("iterate_types - Check Length", b, result);
		
		std::vector<size_t> sizes;
		sizes.reserve(n);
		typedef Loki::TL::MakeTypelist<char,
		                               short,
		                               int,
		                               double>::Result some_types;
		Loki::TL::iterate_types<some_types, Loki::TL::sizeof_type>(remove_const(std::back_inserter(sizes)));
		size_t apriori_size[] = {sizeof(char), sizeof(short), sizeof(int), sizeof(double)};
		b = true;
		for(int i=0; i<n; ++i)
			b &= sizes[i] == apriori_size[i];
		testAssert("iterate_types - Check Elements", b, result);
		
		sizes.resize(0);
		Loki::TL::iterate_types<null_tl, Loki::TL::sizeof_type>(sizes);
		b = sizes.size() == 0;
		testAssert("iterate_types - Degenerate Case 1 - Null List", b, result);

		sizes.resize(0);
		Loki::TL::iterate_types<Loki::NullType, Loki::TL::sizeof_type>(sizes);
		b = sizes.size() == 0;
		testAssert("iterate_types - Degenerate Case 2 - NullType", b, result);
		}
	};

#endif //DATAGENERATORSTEST_H
