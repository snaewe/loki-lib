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

#ifndef FUNCTORTEST_H
#define FUNCTORTEST_H

#include <loki/Functor.h>

///////////////////////////////////////////////////////////////////////////////
// FunctorTest
///////////////////////////////////////////////////////////////////////////////

class FunctorTest : public Test
{
public:
  FunctorTest() : Test("Functor.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    bool r;

    TestFunctor testFunctor;
    TestClass testClass;


#ifndef LOKI_DISABLE_TYPELIST_MACROS
    Functor<void,LOKI_TYPELIST_1(bool &)> function(testFunction);
    Functor<void,LOKI_TYPELIST_1(bool &)> functor(testFunctor);
    Functor<void,LOKI_TYPELIST_1(bool &)> classFunctor(&testClass,&TestClass::member);
    Functor<void,LOKI_TYPELIST_1(bool &)> functorCopy(function);
    Functor<void,NullType> bindFunctor(BindFirst(function,testResult));
    Functor<void> chainFunctor(Chain(bindFunctor,bindFunctor));
#else
    Functor<void,Seq<bool &> > function(testFunction);
    Functor<void,Seq<bool &> > functor(testFunctor);
    Functor<void,Seq<bool &> > classFunctor(&testClass,&TestClass::member);
    Functor<void,Seq<bool &> > functorCopy(function);
    //TODO:
    //Functor<void,NullType> bindFunctor(BindFirst(function,testResult));
    //Functor<void> chainFunctor(Chain(bindFunctor,bindFunctor));
#endif

    testResult=false;
    function(testResult);
    bool functionResult=testResult;

    testResult=false;
    functor(testResult);
    bool functorResult=testResult;

    testResult=false;
    classFunctor(testResult);
    bool classFunctorResult=testResult;

    testResult=false;
    functorCopy(testResult);
    bool functorCopyResult=testResult;

#ifndef LOKI_DISABLE_TYPELIST_MACROS
    testResult=false;
    bindFunctor();
    bool bindFunctorResult=testResult;

    testResult=false;
    chainFunctor();
    bool chainFunctorResult=testResult;

    r=functionResult && functorResult && classFunctorResult && functorCopyResult && bindFunctorResult &&
      chainFunctorResult;
#else
    //TODO!
     r=functionResult && functorResult && classFunctorResult && functorCopyResult;
#endif

    testAssert("Functor",r,result);

    std::cout << '\n';
    }

private:
  static bool testResult;

  static void testFunction(bool &result)
    {
    result=true;
    }

  class TestFunctor
  {
  public:
    void operator()(bool &result)
      {
      result=true;
      }
  };

  class TestClass
  {
  public:
    void member(bool &result)
      {
      result=true;
      }
  };
} functorTest;

bool FunctorTest::testResult;

#ifndef SMALLOBJ_CPP
# define SMALLOBJ_CPP
# include "../../SmallObj.cpp"
#endif
#endif
