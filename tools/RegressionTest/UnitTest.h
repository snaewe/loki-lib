///////////////////////////////////////////////////////////////////////////////
// Unit Test framework
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

#ifndef UNITTEST_H
#define UNITTEST_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// SameType
///////////////////////////////////////////////////////////////////////////////

#if _MSC_VER && !__INTEL_COMPILER && !__MWERKS__

// Rani Sharoni's SameType

template<class T1,class T2>
struct SameType
{
private:
	template<class>
	struct In 
	{ enum { value = false }; };

	template<>
	struct In<T1>
	{ enum { value = true };	};

public:
	enum { value = In<T2>::value };
};

#else

template<class T1,class T2>
struct SameType
{
		static const bool value=false;
};

template<class T>
struct SameType<T,T>
{
		static const bool value=true;
};

#endif

///////////////////////////////////////////////////////////////////////////////
// TestResult
///////////////////////////////////////////////////////////////////////////////

class TestResult
{
public:
	TestResult() : pos(0),passed(0),failed(0),notSupported(0) {}

	int pos;
	int passed;
	int failed;
	int notSupported;
};

///////////////////////////////////////////////////////////////////////////////
// Test
///////////////////////////////////////////////////////////////////////////////

class Test
{
public:
	explicit Test(const std::string &n) : name(n) {}

	virtual void execute(TestResult &) =0;

protected:
	~Test() {}

	void printName(const TestResult &result) const
		{
		if(name.length()!=0)
			std::cout << std::string(result.pos,' ') << name << '\n';
		}

	void testAssert(const std::string &s,bool assertion,TestResult &result,bool supported =true,
									const std::string &failStr =emptyStr())
		{
		std::string str=std::string(result.pos+2,' ')+s;

		str+=std::string(offset-str.length(),' ');

		if(supported)
			{
			if(assertion)
				{
				std::cout << str << "Passed\n";

				++result.passed;
				}
			else
				{
				std::cout << str << (failStr==emptyStr() ? std::string("Failed") : "Failed - "+failStr) << '\n';

				++result.failed;
				}
			}
		else
			{
			std::cout << str << "Not Supported\n";

			++result.notSupported;
			}
		}

	static std::string emptyStr()
		{
		return std::string();
		}

public:
	enum { offset=63 };

protected:
	const std::string name;
};

///////////////////////////////////////////////////////////////////////////////
// TestSuite
///////////////////////////////////////////////////////////////////////////////

class TestSuite : public Test
{
private:
	typedef std::vector<Test *> TestList;

public:
	explicit TestSuite(const std::string &name =emptyStr()) : Test(name) {}

	void add(Test &test)
		{
		tests.push_back(&test);
		}

	virtual void execute(TestResult &result)
		{
		printName(result);

		if(name.length()!=0)
			result.pos+=2;

		for(TestList::iterator i=tests.begin();i!=tests.end();++i)
			(*i)->execute(result);

		if(name.length()!=0)
			result.pos-=2;
		}

private:
	TestList tests;
};

///////////////////////////////////////////////////////////////////////////////
// UnitTest
///////////////////////////////////////////////////////////////////////////////

class UnitTest
{
public:
	int run(const std::string &title,TestSuite &tests) const
		{
		std::cout << title << std::string(Test::offset-title.length(),' ') << "Result\n";
		std::cout << std::string(76,'-') << '\n';

		TestResult testResult;

		tests.execute(testResult);

		std::cout << std::string(76,'-') << '\n';

		const int total=testResult.passed+testResult.failed;
		const int totalAll=total+testResult.notSupported;

		if(total!=0)
			std::cout << "Total - " << testResult.passed << '/' << total << (total==1 ? " test, " : " tests, ")
								<< testResult.passed*100/total << "% Passed\n";

		if(testResult.notSupported!=0)
			std::cout << "Not Supported - " << testResult.notSupported << '/' << totalAll << ", "
								<< testResult.notSupported*100/totalAll << "%\n";

		return testResult.failed;
		}
};

#endif
