
namespace Loki
	{
	namespace Test
		{
		struct CTBaseType
			{
			};
		struct CTDerivedType : CTBaseType
			{
			};
		struct RTBaseType
			{
			virtual ~RTBaseType(){}
			virtual const char*const name(){return "BaseType";}
			};
		struct RTDerivedType : RTBaseType
			{
			virtual const char*const name(){return "DerivedType";}
			};
		}//ns Test
	}//ns Loki	

//TODO Test the other operators and before
#define LOKI_TEST_TYPEINFO\
	{\
	cout<<"Testing TypeInfo"<<endl;\
	Loki::TypeInfo type_CTDerived(typeid(Loki::Test::CTDerivedType));\
	\
	Loki::Test::CTDerivedType* pDerived=0; pDerived;\
	if(typeid(*pDerived) == type_CTDerived)\
		cout<<"Compile-time TypeInfo works"<<endl;\
	else\
		cout<<"Compile-time TypeInfo is ***BROKEN***"<<endl;\
	\
	try\
		{\
		Loki::TypeInfo type_RTDerived(typeid(Loki::Test::RTDerivedType));\
		Loki::Test::RTBaseType* pBase = new Loki::Test::RTDerivedType;\
		pBase;\
		if(typeid(*pBase) == type_RTDerived)\
			cout<<"Run-time TypeInfo works"<<endl;\
		else\
			{\
			cout<<"Run-time TypeInfo is ***BROKEN***"<<endl;\
			cout<<"Is RTTI enabled?"<<endl;\
			cout<<"Compiler thinks pBase points to a ";\
			cout<<typeid(*pBase).name()<<endl;\
			}\
		delete pBase;\
		}\
	catch(...)\
		{\
		cout<<"Run-time TypeInfo is ***BROKEN***"<<endl;\
		cout<<"Run-time TypeInfo check threw - Is RTTI enabled?"<<endl;\
		}\
	cout<<endl<<endl;\
	}