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
// Last update: October 10, 2002
///////////////////////////////////////////////////////////////////////////////

#ifdef __INTEL_COMPILER
# pragma warning(disable: 111 193 304 383 444 488 981 1418)
#endif

//Some platforms might have difficulty with this
//Need to ifdef around those cases.
//TODOSGB

#include "UnitTest.h"

//static variable defintion, do not remove
Test::tests_type Test::tests;


//Merely comment out any of the following headers to
// prevent thier execution during the test.
//A pluggable-factory-like method is used to 
// auto-register the test, so all that is needed
// is the header inclusion to execute the correspond
// unit test.

#include "TypelistTest.h"
#include "TypeManipTest.h"
#include "TypeTraitsTest.h"
#include "SmallObjectTest.h"
#include "SingletonTest.h"
#include "SmartPtrTest.h"
#include "FactoryTest.h"
#include "AbstractFactoryTest.h"
#include "AssocVectorTest.h"
#include "FunctorTest.h"
#include "DataGeneratorsTest.h"


/*
 * AP - All Pass
 * FC - Fails to Compile
 * ?  - Unknown/Not Tested/Not Recorded
 *
 *               TypelistTest    TypeManipTest  TypeTraitsTest  SmallObjectTest  SingletonTest
 * gcc   2.95.3  ?               ?              ?               ?                ?            
 * gcc   3.2     AP              AP             AP              AP               P #ifdef?    
 * MSVC  6       ?               ?              ?               ?                ?            
 * MSVC  7       AP              Conversion     FC              AP               P #ifdef?    
 * Intel ?       ?               ?              ?               ?                ?            
 * BCB   5.5?    ?               ?              ?               ?                ?            
 * CW    6.0     ?               ?              ?               ?                ?            
 *
 *               SmartPtrTest  FactoryTest  AbstractFactoryTest  AssocVectorTest  FunctorTest
 * gcc   2.95.3  ?             ?            ?                    ?                ?          
 * gcc   3.2     FC            AP           AP                   FC               AP         
 * MSVC  6       ?             ?            ?                    ?                ?          
 * MSVC  7       FC            AP           AP                   FC               AP         
 * Intel ?       ?             ?            ?                    ?                ?          
 * BCB   5.5?    ?             ?            ?                    ?                ?          
 * CW    6.0     ?             ?            ?                    ?                ?          
 *
 *               DataGeneratorsTest
 * gcc   2.95.3  ?                 
 * gcc   3.2     AP                
 * MSVC  6       ?                 
 * MSVC  7       AP                
 * Intel ?       ?                 
 * BCB   5.5?    ?                 
 * CW    6.0     ?                 

 */ 

int main()
{

int result = Test::run("Loki Unit Test");

#if __BORLANDC__
	while(true); // Stop console window from closing if run from IDE.
#elif _MSC_VER || __GNUC__
	system("pause");
#endif

return result;
}
