////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006,2009 Peter Kümmel
// Copyright (C) 2009 Andy Balaam
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$


#include <iostream>

#include <loki/Factory.h>
#include <loki/Singleton.h>
#include <loki/Register.h>
#include "loki/ForEachType.h"

#include "classlist.h"




struct NumMeth
{
    typedef std::pair<int, std::string> IdxType;

    template< int Index, typename Type >
    void operator()()
    {
        Type tmp;
        (void) tmp;
        called_for_.push_back( IdxType( Index, typeid( tmp ).name() ) );
    }

    std::vector< IdxType > called_for_;
};


void test_typelist_foreach_forward()
{
    typedef LOKI_TYPELIST_4( int, int, double, unsigned int ) MyTypeList;

    NumMeth num_meth;
    Loki::ForEachType< MyTypeList, NumMeth > dummy( num_meth );

    const std::vector<NumMeth::IdxType>& called_for = num_meth.called_for_;

    std::string int_typename = typeid( static_cast<int>(0)          ).name();
    std::string dou_typename = typeid( static_cast<double>(0)       ).name();
    std::string uin_typename = typeid( static_cast<unsigned int>(0) ).name();

    assert( called_for.size() == 4 );
    assert( called_for[0] == NumMeth::IdxType( 0, int_typename ) );
    assert( called_for[1] == NumMeth::IdxType( 1, int_typename ) );
    assert( called_for[2] == NumMeth::IdxType( 2, dou_typename ) );
    assert( called_for[3] == NumMeth::IdxType( 3, uin_typename ) );
}


void test_typelist_foreach_backward()
{
    typedef LOKI_TYPELIST_3( double, std::string, int ) MyTypeList;

    NumMeth num_meth;
    Loki::ForEachType< MyTypeList, NumMeth, Loki::OrderPolicyBackward > dummy( num_meth );

    const std::vector<NumMeth::IdxType>& called_for = num_meth.called_for_;

    std::string dou_typename = typeid( static_cast<double>(0) ).name();
    std::string str_typename = typeid( dou_typename           ).name();
    std::string int_typename = typeid( static_cast<int>(0)    ).name();

    assert( called_for.size() == 3 );
    assert( called_for[0] == NumMeth::IdxType( 0, int_typename ) );
    assert( called_for[1] == NumMeth::IdxType( 1, str_typename ) );
    assert( called_for[2] == NumMeth::IdxType( 2, dou_typename ) );
}





typedef Loki::SingletonHolder
<
    Loki::Factory<Base, std::string>
>
BaseFactory;


bool registerClass(std::string key, Base*(*creator)() )
{
    return BaseFactory::Instance().Register(key,creator);
}






int main()
{
    // register test
    {
        Loki::RegisterOnCreateSet<ClassList> registerAllClasses;
        Loki::UnRegisterOnDeleteSet<ClassList> unregisterAllClasses;

        Base* foo = BaseFactory::Instance().CreateObject("Foo");
        Base* boo = BaseFactory::Instance().CreateObject("Boo");
        
        foo->foo();
        boo->foo();

        delete foo;
        delete boo;

    }

    // typelist tests

    test_typelist_foreach_forward();
    test_typelist_foreach_backward();

#if defined(__BORLANDC__) || defined(_MSC_VER)
    system("PAUSE");
#endif



    return 0;
}

