//
// in this source code I will demonstrate via example the "canonical" and compliant way of
// simulating class template partial specialization and function templates partial ordering
// both are not implemented in VC7 and from the simulation you will see that they are closely related
// in the example I will simulate the Append class template which have 4 specializations
// Part of the sample was also tested with Comeau C/C++ 4.3.0 online
// although the code is perfectly compliant many compilers (like bcc 5.6 and gcc 3.03 which support partial stuff) can't
// handle it (maybe some workaround can help).
//

#include <stdlib.h>
#include <cstdio>
#include <typeinfo>
#include <Loki\Typelist.h>
#include <Loki\static_check.h>

using namespace ::Loki;

//
// Simulating function templates partial ordering (14.5.5.2):
// The following simple example demonstrate the whole idea
// template<typename T> void f(T);
// template<typename T> void f(T*);
// the functions are partial ordered and T* is more specialized then T
// compiler which is not supporting partial ordering will decide that f((int*)0) 
// is ambiguous. We must resolve the ambiguity our self.
// There is more then one way of doing it but I like the following:
// struct A{}; 
// struct B : A{};
// template<typename T> void f(T, A);
// template<typename T> void f(T*, B);
// template<typename T> void f(T x) { f(x, B()); }
// for pointers f(T*, B) is better then f(T, A) when the call is f(ptr, B());
//

//
// This template will help us to "order" the function templates.
//
template<unsigned I>
struct Order : Order<I - 1> {};

template<>
struct Order<0> {};

template<unsigned I>
struct TypeTag
{
    typedef char (&X)[I];
};

//
// This template class simulate the detection of the best specialization
//
template<class TList, class T>
class AppendPattern
{
    // you can call me insane but VC7 fails without the following typedefs
    typedef TypeTag<2>::X X2;
    typedef TypeTag<3>::X X3;
    typedef TypeTag<4>::X X4;

    // Match Append<NullType, NullType>
    static TypeTag<1>::X Match(Type2Type<NullType>, Type2Type<NullType>, Order<1>);
    
    // Match Append<NullType, T>
    template<typename T1>
    static TypeTag<2>::X Match(Type2Type<NullType>, Type2Type<T1>, Order<0>);

    // Match Append<NullType, Typelist<Head, Tail>>
    template <class Head, class Tail>
    static TypeTag<3>::X Match(Type2Type<NullType>, Type2Type<Typelist<Head, Tail> >, Order<1>);

    // Match Append<Typelist<Head, Tail>, T>
    template <class Head, class Tail, typename T1>
    static TypeTag<4>::X Match(Type2Type<Typelist<Head, Tail> >, Type2Type<T1>, Order<0>);
    
    static Type2Type<TList> Get1();
    static Type2Type<T>     Get2();
    
    // VC will fail if the sizeof is written directly to the TypeTag
    enum { value = sizeof(Match( Get1(), Get2(), Order<1>() )) };

public:
    typedef TypeTag<value> Result;    
};

template <class TList, class T> class  Append2;

//
// This template class simulate the detection of the partial specialization
// The append helper simulate the partial specialized class templates with 
// explicit specialization that represent the selected specialization and 
// nested class template which implement the specialization 
//
template<typename Tag> struct AppendHelper;

template<> 
struct AppendHelper< TypeTag<1> >
{ // Append<NullType, NullType>
    template <class, class> 
    struct In
    {
        typedef NullType Result;
    };
};

template<>
struct AppendHelper< TypeTag<2> >
{ // Append<NullType, T>
    template <class, class T> 
    struct In 
    {
        typedef TYPELIST_1(T) Result;
    };
};

template<>
struct AppendHelper< TypeTag<3> >
{ // Append<NullType, Typelist<Head, Tail> >
    template <class, class TList> 
    struct In 
    {  
        typedef typename TList::Head Head;
        typedef typename TList::Tail Tail;

        typedef Typelist<Head, Tail> Result;
        //typedef TList Result;
    };
};

template<>
struct AppendHelper< TypeTag<4> >
{ // Append<Typelist<Head, Tail>, T>
    template <class TList, class T> 
    struct In 
    {
        typedef typename TList::Head Head;
        typedef typename TList::Tail Tail;

        typedef Typelist<Head, typename Append2<Tail, T>::Result> Result;
    };
};


template <class TList, class T> 
class Append2
{
    // First we detect the best specialization
    typedef typename AppendPattern<TList, T>::Result Pattern;

public:

    // Insatiate the best specialization
    typedef typename AppendHelper<Pattern>::
        template In<TList, T>::Result 
        Result;
};

//
// Lets toy with the simulated Append
//

typedef struct IncompType *IncompPtr;

struct A1 {};
struct B1 : A1 {};

struct A2 {};
struct B2 : A2 {};

struct C1 : B1 {};
struct C2 : B2 {};
struct D1 : C1, C2 {};

namespace 
{

template<typename TList>
struct PrintTypelist
{
    PrintTypelist()
    {
        typedef typename TList::Head Head;
        typedef typename TList::Tail Tail;

        printf("%s ,", typeid(Head).name());

        PrintTypelist<Tail>();
    }
};

template<>
struct PrintTypelist<NullType>
{
    PrintTypelist()
    {
        printf("\n");
    }
};

template<class TList, typename T>
void Test_Append2()
{
    typedef typename TL::Append<TList, T>::Result typelist1_t;
    typedef typename Append2<TList, T>::Result    typelist2_t;

    // Assert (compile time) that typelist1_t and typelist2_t are the same type
    STATIC_CHECK((Conversion<typelist1_t, typelist2_t>::sameType), Append2_Failed);

    PrintTypelist<typelist1_t>();
    PrintTypelist<typelist2_t>();
}

void Test_Append()
{
    typedef TL::MakeTypeList<int, A1, A2>::Result typelist1_t;
    typedef TL::MakeTypeList<IncompPtr, B1, B2*, A2**>::Result typelist2_t;

    Test_Append2<NullType, NullType>();
    Test_Append2<NullType, typelist2_t>();
    Test_Append2<typelist1_t, NullType>();
    Test_Append2<NullType, int>();
    Test_Append2<typelist1_t, int[10]>();
    Test_Append2<typelist1_t, typelist2_t>();
}

} // anonymous namespace


int main()
{
    Test_Append();
    system("pause");
}

