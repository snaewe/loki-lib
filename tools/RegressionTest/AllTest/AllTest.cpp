
extern void Test_TypeList();
extern void Test_SmallObj();
extern void Test_Functor();
extern void Test_Singleton();
extern void Test_SmartPtr();
extern void Test_ObjectFactory();
extern void Test_AbstractFactory();
extern void Test_Vistor();
extern void Test_MultiMethods();
extern void Test_Threads();

#include <stdlib.h>

int main()
	{
	Test_TypeList();
	Test_SmartPtr();
	Test_Singleton();
	Test_Threads();
	/*Test_SmallObj();
	Test_Functor();
	*/
	
	/*Test_ObjectFactory();
	Test_AbstractFactory();
	Test_Vistor();
	Test_MultiMethods();
	*/
	system("pause");
	};