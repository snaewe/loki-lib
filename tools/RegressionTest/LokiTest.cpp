// LokiTest.cpp : Defines the entry point for the console application.
//

#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

//#define LOKI_USE_REFERENCE

#include <Loki\static_check.h>
#include <Loki\NullType.h>
#include <Loki\TypeInfo.h>
#include <Loki\TypeManip.h>
#include <Loki\TypeList.h>
#include <Loki\TypeTraits.h>

//The test use macros so that you can comment out any test
// that result in compilation errors and continue testing
// the remaining components
//TODO break into seperate files, use boost methods to test
// successful & unsuccessful compilations

#include "LokiTestTypes.h"
#include "LokiTest_TypeManip.h"
#include "LokiTest_TypeList.h"
#include "LokiTest_TypeInfo.h"
#include "LokiTest_TypeTraits.h"

//LOKI_TEST_TYPE2TYPE_HEADER
LOKI_TEST_TYPE2TYPE_HEADER_BROKEN_PATTERN_MATCHING

int main(int argc, char* argv[])
	{
	argc;
	argv;
	//Compile-Time Assertions
	STATIC_CHECK(true, ThisShouldCompile);
	//STATIC_CHECK(false, ThisShouldNotCompile);
	
	LOKI_TEST_NULLTYPE;
	
	//Test TypeManip
	LOKI_TEST_INT2TYPE;
	//LOKI_TEST_TYPE2TYPE;
	LOKI_TEST_TYPE2TYPE_BROKEN_PATTERN_MATCHING;
	LOKI_TEST_SELECT;
	#pragma warning(push)
	#pragma warning(disable:4127) //constant conditionals
	LOKI_TEST_CONVERSION;
	#pragma warning(pop)
	
	//TypeInfo
	LOKI_TEST_TYPEINFO;
	
	//TypeTraits
	#pragma warning(push)
	#pragma warning(disable:4127) //constant conditionals
	{
	//TODO figure out correct results
	cout<<"Testing TypeTraits"<<endl;
	DUMP_TRAITS(int);
	DUMP_TRAITS(float);
	DUMP_TRAITS(volatile unsigned int);
	DUMP_TRAITS(const float);
	cout<<"Are the const & volatile qualifers handled correctly?"<<endl;
	cout<<endl<<endl;
	}
	#pragma warning(pop)
	
	//Test TypeList
	#pragma warning(push)
	#pragma warning(disable:4127) //constant conditionals
	LOKI_TEST_LENGTH;
	#pragma warning(pop)
	
	getch();
	return 0;
	}
//*/