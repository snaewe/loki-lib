//#include <windows.h>
//#include "Threads.h"
#include "MultiMethods.h"
#include "Visitor.h"
#include "AbstractFactory.h"
#include "SmartPtr.h"
#include "static_check.h"
#include "TypeManip.h"
#include "TypeInfo.h"
#include "TypeTraits.h"
#include "HierarchyGenerators.h"
#include "SmallObj.h"
#include "Functor.h"
#include "Singleton.h"
#include "Factory.h"
#include "SmallObj.h"
#include "Factory.h"

#include <cassert>

using namespace Loki;

class Base
{
public:
    int Fun(const char*)
    {
        return 0;
    }
};
class Derived : public Base {};

class Base1
{
public:
    int Fun(const char*)
    {
        return 0;
    }
};
class Derived1 : public Base1 {};

unsigned int GetLongevity(Base*) { return 1; }

template <class T>
struct Holder
{
    T value_;
}; 

template <class T, class Base>
struct Holder2 : public Base
{
};

int Fun(double)
{
    return 0;
}

template class SmartPtr< Base >;

template class Factory< Base, int, Base* (*)(), DefaultFactoryError >;

template class CloneFactory< Base, Base* (*)(const Base*), DefaultFactoryError>;

template class AbstractFactory<TYPELIST_2(Base, Derived)>;

class DocElement;
class Paragraph;
class RasterBitmap;

typedef CyclicVisitor
<
    void, // return type
    TYPELIST_3(DocElement, Paragraph, RasterBitmap)
>
MyVisitor;

class DocElement
{
public:
    virtual void Accept(MyVisitor&) = 0;
};

class Paragraph : public DocElement
{
public:
    DEFINE_CYCLIC_VISITABLE(MyVisitor)
};

class MyConcreteVisitor : public MyVisitor
{
    void Visit(DocElement&)
    {}
    
    void Visit(Paragraph&)
    {}
    
    void Visit(RasterBitmap&)
    {}
};

extern void TestMultiMethods();
extern void TestThreads();
extern void TestAbstractFactory();
extern void TestFactory();
extern void TestFunctor();
extern void TestAssocVector();

int main()
{
    TestAbstractFactory();
    TestAssocVector();
    TestFactory();
    TestFunctor();
    TestMultiMethods();
    TestThreads();
    
    {
        MyConcreteVisitor vis;
        Paragraph par;
        DocElement& docElem = par;
        
        docElem.Accept(vis);
    }

    {
        typedef AbstractFactory<TYPELIST_2(Base, Base1)> Fact;
        typedef ConcreteFactory<Fact, OpNewFactoryUnit, 
            TYPELIST_2(Derived, Derived1)> CFact;
            
        CFact factory;
        factory.Create<Base>();
    }

    STATIC_CHECK(sizeof(int) == 4, Int_Too_Small);
	
	{
	    Int2Type<3> i2t;
	    i2t;
	}
	
	{
	    Type2Type<int> t2t;
	    t2t;
	}
	
	{
	    Select<true, int, char*>::Result test = 9;
	}

	{
	    Select<false, int, char*>::Result test = "9";
	}

    {
        //STATIC_CHECK(X<int>::a, zzz);
        //STATIC_CHECK((Conversion<Derived, Base>::exists == 4), X);
        /*STATIC_CHECK((Conversion<int, char>::exists2Way), X);
        STATIC_CHECK((!Conversion<int, char>::sameType), X);
        STATIC_CHECK(!Conversion<int, char*>::exists, X);
        STATIC_CHECK(Conversion<int, char*>::exists2Way;
        Conversion<int, char*>::sameType;
        Conversion<char*, char*>::exists;
        Conversion<char*, char*>::exists2Way;
        Conversion<char*, char*>::sameType;
        
        SUPERSUBCLASS(int, char*);*/
    }
    
    {   
        TypeInfo ti1;
        TypeInfo ti2(typeid(int));
        TypeInfo ti3(ti1);
        ti2 = ti1;
        
        ti1.name();
        ti1 == ti2;
        ti1 != ti2;
        ti1 < ti2;
        ti1 > ti2;
        ti1.before(ti2);
        
    }
    
    {
        STATIC_CHECK(TypeTraits<char*>::isPointer, TypeTraits_Broken);
        STATIC_CHECK(!TypeTraits<char>::isPointer, TypeTraits_Broken);

        STATIC_CHECK(TypeTraits<char&>::isReference, TypeTraits_Broken);
        STATIC_CHECK(!TypeTraits<char*>::isReference, TypeTraits_Broken);
        
        STATIC_CHECK(TypeTraits<int>::isStdSignedInt &&
            !TypeTraits<int>::isStdUnsignedInt &&
            TypeTraits<int>::isStdIntegral &&
            !TypeTraits<int>::isStdFloat &&
            TypeTraits<int>::isStdArith &&
            TypeTraits<int>::isStdFundamental, TypeTraits_Broken);
    }
 
    {
        typedef TYPELIST_5(int, long int, char*, char, short int)
            TList;
        STATIC_CHECK(TL::Length<TList>::value == 5, Typelist_Broken);
        TL::TypeAt<TList, 2>::Result x = "oo";
        
        typedef TL::Append<TList, int>::Result TList2;
        STATIC_CHECK(TL::Length<TList2>::value == 6, Typelist_Broken);
        
        typedef TL::Erase<TList2, int>::Result TList3;
        STATIC_CHECK(TL::Length<TList3>::value == 5, Typelist_Broken);
        TL::TypeAt<TList3, 1>::Result y = "d";        
        
        typedef TL::EraseAll<TList2, int>::Result TList4;
        STATIC_CHECK(TL::Length<TList4>::value == 4, Typelist_Broken);
        TL::TypeAt<TList3, 1>::Result z = "f";        
        
        typedef TL::Append<TList2, long int>::Result TList5;
        STATIC_CHECK(TL::Length<TList5>::value == 7, Typelist_Broken);
        typedef TL::NoDuplicates<TList5>::Result TList6;
        STATIC_CHECK(TL::Length<TList6>::value == 5, Typelist_Broken);
        TL::TypeAt<TList6, 2>::Result c = "d";        
        
        typedef TL::Replace<TList, char*, int>::Result TList7;
        TL::TypeAt<TList7, 2>::Result d = 44;
        
        typedef TL::ReplaceAll<TList2, int, char*>::Result TList8;
        TL::TypeAt<TList8, 5>::Result e = "44";
    }
    
    {
        //GenScatterHierarchy<TYPELIST_2(char*, int), Holder> obj1;
        //obj1;
        GenLinearHierarchy<TYPELIST_2(char*, int), Holder2> obj2;
        obj2;
        //Field<char*>(obj1).value_ = "hello";
        //Field<int>(obj1).value_ = 3;

        //Field<0>(obj1).value_ = "hello";
        //Field<1>(obj1).value_ = 3;

        //const GenScatterHierarchy<TYPELIST_2(char*, int), Holder> obj3;
        //obj3;
        //char*& p1 = Field<0>(obj3).value_;
        //char*& p2 = Field<0>(obj1).value_;
    }
    
    {
        Functor<int, TYPELIST_1(double)> fn(Fun);
        fn(4.5);
        Base b;
        Functor<int, TYPELIST_1(char*)> fn2(&b, &Base::Fun);
        fn2("a");
        
        char* ddd = "aaa";
        Functor<int> fn3 = BindFirst(fn2, "ddd");
        fn3();
        
        Functor<int> fn4 = Chain(fn3, fn3);
        fn4();
    }
    
    {
        typedef SingletonHolder<Base> TheBase;
        TheBase::Instance();
    }

    {
        typedef SingletonHolder<Base, CreateUsingNew, SingletonWithLongevity> TheBase;
        TheBase::Instance();
    }
    
    {
        SmartPtr<int> sp1, sp2;
        GetImpl(sp1);
        sp1 = sp2;
     
        SmartPtr<Base, RefLinked> sp3;
        SmartPtr<Derived, RefLinked> sp4(new Derived);
        
        sp3 = sp4;
        
        SmartPtr<Derived, RefCounted, AllowConversion> sp5;
        
        if (!!sp5)
        {}
        
        //Derived* p = sp5;
        
        //delete sp5;
    }
    
    {
        class MyClass : public SmallObject<> {};
        MyClass* p1 = new MyClass;
        MyClass* p2 = new MyClass;
        delete p1;
        delete p2;
    }
}        
