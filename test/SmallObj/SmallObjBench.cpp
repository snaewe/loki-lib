////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter Kümmel
// Copyright (c) 2005 Richard Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The authors make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$

//#define LOKI_CLASS_LEVEL_THREADING
//#define LOKI_OBJECT_LEVEL_THREADING

// Uncomment this to test new [] and delete [].
#define LOKI_SMALL_OBJECT_USE_NEW_ARRAY

#include "../../include/loki/SmallObj.h"
#include "timer.h"

#include <iostream>
#include <string>

//#define COMPARE_BOOST_POOL 
#ifdef COMPARE_BOOST_POOL
    #include <boost\pool\object_pool.hpp>
#endif

using namespace std;


template<unsigned int N>
class ThisIsASmallObject
{
    char data[N];
};

template<unsigned int N, class T>        
struct Base : public ThisIsASmallObject<N>, public T 
{};

template<unsigned int N>        
struct Base<N, void> : public ThisIsASmallObject<N> 
{};


#ifdef COMPARE_BOOST_POOL

template<unsigned int N>
class BoostPoolNew : public Base<N,void>
{
private:
    static boost::object_pool< BoostPoolNew<N> > BoostPool;

public:
    /// Throwing single-object new throws bad_alloc when allocation fails.
#ifdef _MSC_VER
    /// @note MSVC complains about non-empty exception specification lists.
    static void * operator new ( std::size_t )
#else
    static void * operator new ( std::size_t ) throw ( std::bad_alloc )
#endif
    {
        return BoostPool.malloc();
    }

    /// Non-throwing single-object new returns NULL if allocation fails.
    static void * operator new ( std::size_t, const std::nothrow_t & ) throw ()
    {
        return BoostPool.malloc();
    }

    /// Placement single-object new merely calls global placement new.
    inline static void * operator new ( std::size_t size, void * place )
    {
        return ::operator new( size, place );
    }

    /// Single-object delete.
    static void operator delete ( void * p ) throw ()
    {
        BoostPool.free( reinterpret_cast< BoostPoolNew * >( p ) );
    }

    /** Non-throwing single-object delete is only called when nothrow
        new operator is used, and the constructor throws an exception.
        */
    static void operator delete ( void * p, const std::nothrow_t & ) throw()
    {
        BoostPool.free( reinterpret_cast< BoostPoolNew * >( p ) );
    }

    /// Placement single-object delete merely calls global placement delete.
    inline static void operator delete ( void * p, void * place )
    {
        ::operator delete ( p, place );
    }

};

template<unsigned int N>
boost::object_pool< BoostPoolNew<N> > BoostPoolNew<N>::BoostPool;

#endif


int array_test_nr = 0;
double t100_new = 0;
double t100_delete = 0;

#define LOKI_SMALLOBJ_BENCH(FUNC, CODE_LOOP)                                 \
template<class T, int TN>                                                    \
int FUNC(void**, const int N, int loop, Timer& t, const char* s)             \
{                                                                            \
    t.start();                                                               \
    /****************************************************************/       \
    for (int i=0; i<loop; ++i)                                               \
    {                                                                        \
        CODE_LOOP                                                            \
    }                                                                        \
    /****************************************************************/       \
    t.stop();                                                                \
    if(array_test_nr==0)                                                     \
        t.t100 = t.t();                                                      \
    array_test_nr++;                                                         \
    t.print(t.t(),s);                                                        \
    return t.t();                                                            \
}

#define LOKI_SMALLOBJ_BENCH_ARRAY(FUNC, CODE_DECL, CODE_NEW, CODE_DELETE)    \
template<class T, int TN>                                                    \
int FUNC(void** arrv, const int N, int loop, Timer& t, const char* s)        \
{                                                                            \
                                                                             \
    CODE_DECL;                                                               \
    T** arr = reinterpret_cast<T**>(arrv);                                   \
    t.start();                                                               \
    /****************************************************************/       \
    for (int i=0; i<loop; ++i)                                               \
    {                                                                        \
        CODE_NEW                                                             \
    }                                                                        \
    /****************************************************************/       \
    t.stop();                                                                \
    cout << "1. ";                                                           \
    if(array_test_nr==0)                                                     \
    {                                                                        \
        t.t100 = t.t();                                                      \
        t100_new = t.t100;                                                   \
    }                                                                        \
    else                                                                     \
        t.t100 = t100_new;                                                   \
    t.print(t.t(),s);                                                        \
                                                                             \
    if(array_test_nr==3)                                                     \
    {                                                                        \
        cout <<                                                              \
        "2. boost    :\tboost::object_pool is not tested because it's too slow"\
        << endl << endl;                                                     \
        array_test_nr++;                                                     \
        return t.t();                                                        \
    }                                                                        \
    t.start();                                                               \
    /****************************************************************/       \
    for (int i=0; i<loop; ++i)                                               \
    {                                                                        \
        CODE_DELETE                                                          \
    }                                                                        \
    /****************************************************************/       \
    t.stop();                                                                \
    cout << "2. ";                                                           \
    if(array_test_nr==0)                                                     \
    {                                                                        \
        t.t100 = t.t();                                                      \
        t100_delete = t.t100;                                                \
    }                                                                        \
    else                                                                     \
        t.t100 = t100_delete;                                                \
    t.print(t.t(),s);                                                        \
    array_test_nr++;                                                         \
    cout << endl;                                                            \
    return t.t();                                                            \
}


LOKI_SMALLOBJ_BENCH(delete_new        ,delete new T;)
LOKI_SMALLOBJ_BENCH(delete_new_mal    ,std::free(std::malloc(sizeof(T))););
LOKI_SMALLOBJ_BENCH(delete_new_all    ,std::allocator<T> st;st.deallocate(st.allocate(1), 1);)

LOKI_SMALLOBJ_BENCH(delete_new_array    ,delete[] new T[N];)
LOKI_SMALLOBJ_BENCH(delete_new_array_mal,std::free(std::malloc(sizeof(T[TN]))););
LOKI_SMALLOBJ_BENCH(delete_new_array_all,std::allocator<T[TN]> st;st.deallocate(st.allocate(1), 1);)

LOKI_SMALLOBJ_BENCH_ARRAY(new_del_on_arr    , , arr[i] = new T; , 
                                                delete arr[i];)
LOKI_SMALLOBJ_BENCH_ARRAY(new_del_on_arr_mal, , arr[i] = static_cast<T*>(std::malloc(sizeof(T))); , 
                                                std::free(arr[i]);)
LOKI_SMALLOBJ_BENCH_ARRAY(new_del_on_arr_all,    std::allocator<T> st , 
                                                arr[i]=st.allocate(1); , 
                                                st.deallocate(arr[i], 1);)

LOKI_SMALLOBJ_BENCH_ARRAY(new_del_a_on_a    , , arr[i] = new T[TN]; , 
                                                delete[] arr[i];)
LOKI_SMALLOBJ_BENCH_ARRAY(new_del_a_on_a_mal, , arr[i] = static_cast<T*>(std::malloc(sizeof(T[TN]))); , 
                                                std::free(arr[i]);)
LOKI_SMALLOBJ_BENCH_ARRAY(new_del_a_on_a_all,std::allocator<T[TN]> st , 
                                                arr[i]=reinterpret_cast<T*>(st.allocate(1)); , 
                                                st.deallocate(reinterpret_cast<T(*)[TN]>(arr[i]), 1);)



#ifndef COMPARE_BOOST_POOL
#define LOKI_SMALLOBJBECH_ABCD(FUNC,N,LOOP,TIMER,MESSAGE)              \
    array_test_nr = 0;                                                 \
    cout << MESSAGE << endl;                                           \
    FUNC<A,N>(a,N,LOOP,TIMER,"new      :");                            \
    FUNC<B,N>(a,N,LOOP,TIMER,"SmallObj :");                            \
    FUNC<C,N>(a,N,LOOP,TIMER,"ValueObj :");                            \
    FUNC##_all<A,N>(a,N,LOOP,TIMER,"allocator:");                      \
    FUNC##_mal<A,N>(a,N,LOOP,TIMER,"malloc   :");                      \
    cout << endl << endl;    
#else
#define LOKI_SMALLOBJBECH_ABCD(FUNC,N,LOOP,TIMER,MESSAGE)              \
    array_test_nr = 0;                                                 \
    cout << MESSAGE << endl;                                           \
    FUNC<A,N>(a,N,LOOP,TIMER,"new      :");                            \
    FUNC<B,N>(a,N,LOOP,TIMER,"SmallObj :");                            \
    FUNC<C,N>(a,N,LOOP,TIMER,"ValueObj :");                            \
    FUNC<D,N>(a,N,LOOP,TIMER,"boost    :");                            \
    FUNC##_all<A,N>(a,N,LOOP,TIMER,"allocator:");                      \
    FUNC##_mal<A,N>(a,N,LOOP,TIMER,"malloc   :");                      \
    cout << endl << endl;
#endif

template<unsigned int Size, int loop>
void testSize()
{
    typedef Base<Size, void> A;
    typedef Base<Size, Loki::SmallObject< Loki::SingleThreaded > > B;
    typedef Base<Size, Loki::SmallValueObject< Loki::SingleThreaded > > C;
#ifdef COMPARE_BOOST_POOL
    typedef BoostPoolNew<Size> D;
#endif

    cout << endl << endl;
    cout << "Allocator Benchmark Tests with " << Size << " bytes big objects " << endl;
    cout << endl;
    cout << "new      = global new/delete     \tsizeof(A) = " << sizeof(A) << endl;
    cout << "SmallObj = Loki::SmallObject     \tsizeof(B) = " << sizeof(B) << endl;
    cout << "ValueObj = Loki::SmallValueObject\tsizeof(C) = " << sizeof(C) << endl;
#ifdef COMPARE_BOOST_POOL
    cout << "boost    = boost::object_pool    \tsizeof(D) = " << sizeof(D) << endl;
#endif
    cout << "allocator= std::allocator        \tsizeof(A) = " << sizeof(A) << endl;
    cout << "malloc   = std::malloc/free      \tsizeof(A) = " << sizeof(A) << endl;
    cout << endl << endl;

    Timer t;

    const int N = 3;    
    int Narr = 1000*1000;

    void** a= new void*[Narr];

    cout << loop  << " times ";
    LOKI_SMALLOBJBECH_ABCD(delete_new        ,0,loop,t,"'delete new T'");
    
    cout << "N=" << N <<" :  " << loop  << " times ";
    LOKI_SMALLOBJBECH_ABCD(delete_new_array    ,N,loop,t,"'delete[] new T[N]'");

    cout << "i=0..." << Narr << " :  ";
    LOKI_SMALLOBJBECH_ABCD(new_del_on_arr    ,0,Narr,t,"1. 'arr[i] = new T'   2. 'delete arr[i]'");
    
    cout << "i=0..." << Narr << ",  N=" << N <<" :  ";
    LOKI_SMALLOBJBECH_ABCD(new_del_a_on_a    ,N,Narr,t,"1. 'arr[i] = new T[N]'   2. 'delete[] arr[i]'");


    delete [] a;
    
    cout << "_________________________________________________________________" << endl;
    Loki::AllocatorSingleton<>::ClearExtraMemory();
}


int main()
{
    const int loop = 1000*1000;

    cout << endl;

    testSize< 2,loop>();
    testSize< 3,loop>();
    testSize< 8,loop>();
    testSize< 9,loop>();
    testSize<16,loop>();
    testSize<17,loop>();


#if defined(__BORLANDC__) || defined(__GNUC__) || defined(_MSC_VER)
    // Stop console window from closing if run from IDE.
    system("pause"); 
#endif

    return 0;
}

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.13  2005/10/30 14:03:23  syntheticpp
// replace tabs space
//
// Revision 1.12  2005/10/29 12:38:22  syntheticpp
// replace with new implementation
//
// Revision 1.11  2005/10/29 10:21:46  syntheticpp
// find loki include files without a correct sreach pathand some small fixes
//
// Revision 1.10  2005/10/27 19:10:32  syntheticpp
// gcc fix
//
// Revision 1.9  2005/10/26 23:30:06  syntheticpp
// make object size more flexible
//
// Revision 1.8  2005/10/26 00:41:00  rich_sposato
// Added comparison to boost::pool memory allocator.
//
// Revision 1.7  2005/10/14 18:35:06  rich_sposato
// Added cvs keywords.
//


