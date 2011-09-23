////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Data Generator by Shannon Barber
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Code covered by the MIT License:
// Copyright(c) 2010 Shannon Barber
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_DATAGENERATORS_INC_
#define LOKI_DATAGENERATORS_INC_

// $Id$


#include <loki/Typelist.h>

//Reference version

/************************************************************************************
// class template GenData
// Iteratates a Typelist, and invokes the functor GenFunc<T>
// for each type in the list, passing a functor along the way.
// The functor is designed to be an insertion iterator which GenFunc<T>
// can use to output information about the types in the list.
//

Example Use

template<typename T>
struct ExtractDataType
    {
    some_type operator()()
        {
        return create_value_from_type<T>;
        }
    };

Loki::IterateTypes<parameter_tl, ExtractDataType> gendata;
std::vector<some_type> stuff;
gendata(std::back_inserter(stuff));
*******************************************************************************/
namespace Loki
{
    namespace TL
        {
        template<typename T>
        struct nameof_type
            {
            const char* operator()()
                {
                return typeid(T).name();
                }
            };
        template<typename T>
        struct sizeof_type
            {
            size_t operator()()
                {
                return sizeof(T);
                }
            };
    template <class TList, template <class> class GenFunc>
    struct IterateTypes;

    template <class T1, class T2, template <class> class GenFunc>
    struct IterateTypes<Typelist<T1, T2>, GenFunc>
    {
    typedef IterateTypes<T1, GenFunc> head_t;
    head_t head;
    typedef IterateTypes<T2, GenFunc> tail_t;
    tail_t tail;
    template<class II>
    void operator()(II ii)
        {
        head.operator()(ii);
        tail.operator()(ii);
        }
    };

    template <class AtomicType, template <class> class GenFunc>
    struct IterateTypes
    {
    template<class II>
    void operator()(II ii)
        {
        GenFunc<AtomicType> genfunc;
        *ii = genfunc();
        ++ii; //Is this even needed?
        }
    };

    template <template <class> class GenFunc>
    struct IterateTypes<NullType, GenFunc>
    {
    template<class II>
    void operator()(II ii)
        {}
    };

    template<typename Types, template <class> class UnitFunc, typename II>
    void iterate_types(II ii)
        {
        Loki::TL::IterateTypes<Types, UnitFunc> it;
        it(ii);
        }
    }//ns TL
}//ns Loki

#endif // end file guardian

