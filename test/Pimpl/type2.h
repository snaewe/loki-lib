////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header:


#include <loki/Pimpl.h>

/////////////////////////////////////////
// class A2 declaration
/////////////////////////////////////////

class A2
{
public:
    A2();
    void foo();

private:
    PimplT<A2> d;
};


/////////////////////////////////////////
// class B2 declaration
/////////////////////////////////////////

class B2 : private PimplT<B2>::Owner
{
public:
    B2();
    void foo();
};



/////////////////////////////////////////
// class C2 declaration
/////////////////////////////////////////

class C2
{
public:
    C2();
    void foo();

private:
    PimplT<C2>::Type rint; 
    RimplT<C2>::Type d; 
};


/////////////////////////////////////////
// class D2 declaration
/////////////////////////////////////////

class D2 : private RimplT<D2>::Owner
{
public:
    D2();
    void foo();
};
