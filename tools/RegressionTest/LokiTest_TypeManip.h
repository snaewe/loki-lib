
#include "LokiTestTypes.h"
using std::endl;
using std::cout;

//You're not using a C++ compiler if this fails.
//Requires #include <Loki\NullType.h>
#define LOKI_TEST_NULLTYPE\
	{\
	Loki::NullType nullTest;\
	cout<<"Testing NullType"<<endl;\
	cout<<"Sizeof nullType: "<<sizeof(nullTest)<<endl;\
	cout<<endl<<endl;\
	}

#define LOKI_TEST_SELECT\
	{\
	cout<<"Testing Select"<<endl;\
	typedef Loki::Select<true, void, Loki::NullType>::Result TrueType;\
	typedef Loki::Select<false, int, double>::Result FalseType;\
	cout<<"True  case: "<<typeid(TrueType).name()<<endl;\
	cout<<"False case: "<<typeid(FalseType).name()<<endl;\
	cout<<endl<<endl;\
	}


//TODO Need more test cases for _STRICT varaints
//TODO What is the proper conversion results to/from void?
#define LOKI_TEST_CONVERSION\
	{\
	cout<<"Testing Conversion"<<endl;\
	typedef Loki::Conversion<int, int> tySame;\
	typedef Loki::Conversion<int, Loki::RegressionTest::Complex> tyNoCon;\
	typedef Loki::Conversion<int, Loki::RegressionTest::Complex_Convertable> tyCon;\
	typedef Loki::Conversion<int, Loki::RegressionTest::TwoWay> ty2Way;\
	typedef Loki::Conversion<void, int> tyVoid;\
	cout<< tySame::sameType<<" <int, int>::sameType" <<endl;\
	cout<< tySame::exists<<" <int, int>::exists" <<endl;\
	cout<< tySame::exists2Way<<" <int, int>::exists2Way" <<endl;\
	if (tySame::sameType && tySame::exists && tySame::exists2Way)\
		cout<<"Works for same type"<<endl;\
	else\
		cout<<"Same type is ***BROKEN***"<<endl;\
	cout<<endl;\
	\
	cout<< tyNoCon::sameType<<" <int, Complex>::sameType" <<endl;\
	cout<< tyNoCon::exists<<" <int, Complex>::exists" <<endl;\
	cout<< tyNoCon::exists2Way<<" <int, Complex>::exists2Way" <<endl;\
	if (!tyNoCon::sameType && !tyNoCon::exists && !tyNoCon::exists2Way)\
		cout<<"Works for non-convertibles"<<endl;\
	else\
		cout<<"Non-convertibles is ***BROKEN***"<<endl;\
	cout<<endl;\
	\
	cout<< tyCon::sameType<<" <int, Complex_Convertable>::sameType" <<endl;\
	cout<< tyCon::exists<<" <int, Complex_Convertable>::exists" <<endl;\
	cout<< tyCon::exists2Way<<" <int, Complex_Convertable>::exists2Way" <<endl;\
	if (!tyCon::sameType && tyCon::exists && !tyCon::exists2Way)\
		cout<<"Works for one-way convertible"<<endl;\
	else\
		cout<<"One-way convertible is ***BROKEN***"<<endl;\
	cout<<endl;\
	\
	cout<< ty2Way::sameType<<" <int, TwoWay>::sameType" <<endl;\
	cout<< ty2Way::exists<<" <int, TwoWay>::exists" <<endl;\
	cout<< ty2Way::exists2Way<<" <int, TwoWay>::exists2Way" <<endl;\
	if (!ty2Way::sameType && ty2Way::exists && ty2Way::exists2Way)\
		cout<<"Works for two-way convertibles"<<endl;\
	else\
		cout<<"Two-way convertibles is ***BROKEN***"<<endl;\
	cout<<endl;\
	\
	cout<< tyVoid::sameType<<" <void, int>::sameType" <<endl;\
	cout<< tyVoid::exists<<" <void, int>::exists" <<endl;\
	cout<< tyVoid::exists2Way<<" <void, int>::exists2Way" <<endl;\
	if (!tyVoid::sameType && !tyVoid::exists && !tyVoid::exists2Way)\
		cout<<"Works for void"<<endl;\
	else\
		cout<<"Void conversion is ***BROKEN***"<<endl;\
	cout<<"What is the correct behavior here?"<<endl;\
	cout<<endl;\
	\
	cout<< SUPERSUBCLASS(Loki::RegressionTest::SuperClass, Loki::RegressionTest::SubClass)<<" SuperSubClass Super&Sub"<<endl;\
	cout<< SUPERSUBCLASS_STRICT(Loki::RegressionTest::SuperClass, Loki::RegressionTest::SubClass)<<" SuperSubClass_Strict Super&Sub"<<endl;\
	cout<< SUPERSUBCLASS(void, Loki::RegressionTest::SubClass)<<" to void*"<<endl;\
	cout<< SUPERSUBCLASS_STRICT(void, Loki::RegressionTest::SubClass)<<" _Strict to void*"<<endl;\
	cout<< SUPERSUBCLASS(Loki::RegressionTest::SubClass, Loki::RegressionTest::SuperClass)<<" SubSuperClass Sub&Super"<<endl;\
	cout<<endl<<endl;\
	}
	
#define LOKI_TEST_INT2TYPE\
	{\
	cout<<"Testing Int2Type"<<endl;\
	typedef Loki::Int2Type<0> tyZero;\
	typedef Loki::Int2Type<1> tyOne;\
	typedef Loki::Int2Type<-1> tyNegOne;\
	cout<<"   Zero: "<<typeid(tyZero).name()<<endl;\
	cout<<"    One: "<<typeid(tyOne).name()<<endl;\
	cout<<"Neg One: "<<typeid(tyNegOne).name()<<endl;\
	cout<<endl<<endl;\
	}
    
//I think this example is an undesirable way to solve this
// problem, but we're just exercising Type2Type
//MSVC7 has a template pattern matching problem that prevents this
// example solution from compiling correctly.
// Type2Type does work, but the solution it was created for doesn't
#define LOKI_TEST_TYPE2TYPE_HEADER\
	struct Widget\
		{\
		Widget(int x, int reserved)\
			{\
			assert(reserved==-1);\
			}\
		};\
	struct NewWidget\
		{\
		NewWidget(int x)\
			{}\
		};\
	template<class T, class U>\
	T* Create(const U& arg, Loki::Type2Type<T>)\
		{\
		return new T(arg);\
		}\
	template<class U>\
	Widget* Create(const U& arg, Loki::Type2Type<Widget>)\
		{\
		return new Widget(arg, -1);\
		}
#define LOKI_TEST_TYPE2TYPE\
    {\
    cout<<"Testing Type2Type"<<endl;\
    NewWidget* pNewWidget = Create(4, Loki::Type2Type<NewWidget>() );\
    delete pNewWidget;\
    Widget* pWidget = Create(100, Loki::Type2Type<Widget>() );\
    delete pWidget;\
    cout<<"Used Type2Type"<<endl;\
    }

#define LOKI_TEST_TYPE2TYPE_HEADER_BROKEN_PATTERN_MATCHING\
	struct Widget\
		{\
		Widget(int x, int reserved)\
			{\
			cc=x;\
			assert(reserved==-1);\
			}\
		int cc;\
		};\
	template<class U>\
	Widget* Create(const U& arg, Loki::Type2Type<Widget>)\
		{\
		return new Widget(arg, -1);\
		}
#define LOKI_TEST_TYPE2TYPE_BROKEN_PATTERN_MATCHING\
    {\
    cout<<"Testing Type2Type"<<endl;\
    Widget* pWidget = Create(100, Loki::Type2Type<Widget>() );\
    delete pWidget;\
    cout<<"Used Type2Type"<<endl;\
    cout<<endl<<endl;\
    }

