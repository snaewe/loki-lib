///////////////////////////////////////////////////////////////////////////////
// Unit Test for Loki
//
// Copyright Terje Slettebø and Pavel Vozenilek 2002.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.
//
// Last update: September 16, 2002
///////////////////////////////////////////////////////////////////////////////

#ifndef LOKITEST_H
#define LOKITEST_H

#include "TypelistTest.h"
#include "TypeManipTest.h"
#include "TypeTraitsTest.h"
//#include "SmallObjectTest.h"
//#include "SingletonTest.h"
//#include "SmartPtrTest.h"
//#include "FactoryTest.h"
#include "AbstractFactoryTest.h"
//#include "AssocVectorTest.h"
//#include "FunctorTest.h"

///////////////////////////////////////////////////////////////////////////////
// LokiTest
///////////////////////////////////////////////////////////////////////////////

class LokiTest
{
public:
	LokiTest()
		{
		addTests();
		}

	int result()
		{
		return unitTest.run("Unit Test",tests);
		}

private:
	void addTests()
		{
		tests.add(typelistTest);
		tests.add(typeManipTest);
		tests.add(typeTraitsTest);
//		tests.add(smallObjectTest);
//		tests.add(singletonTest);
//		tests.add(smartPtrTest);
//		tests.add(factoryTest);
		tests.add(abstractFactoryTest);
//		tests.add(assocVectorTest);
//		tests.add(functorTest);
		}

private:
	UnitTest unitTest;
	TestSuite tests;

	TypelistTest typelistTest;
	TypeManipTest typeManipTest;
	TypeTraitsTest typeTraitsTest;
//	SmallObjectTest smallObjectTest;
//	SingletonTest singletonTest;
//	SmartPtrTest smartPtrTest;
//	FactoryTest factoryTest;
	AbstractFactoryTest abstractFactoryTest;
//	AssocVectorTest assocVectorTest;
//	FunctorTest functorTest;
};

#endif
