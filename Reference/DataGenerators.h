////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Data Generator by Shannon Barber
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Code covered by the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "TypeList.h"

/************************************************************************************
// class template GenData
// Iteratates a TypeList, and invokes the functor GenFunc<T>
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

Loki::GenData<parameter_tl, ExtractDataType> gen;
std::vector<user_defined_type> stuff;
gen(std::back_inserter(stuff));
*******************************************************************************/
namespace Loki
{
	namespace TL
		{
		template<typename T>
		struct name_from_type
			{
			const char* operator()()
				{
				return typeid(T).name();
				}
			};

    template <class TList, template <class> class GenFunc>
    struct IterateTypes;
     
    template <class T1, class T2, template <class> class GenFunc>
    struct IterateTypes<Typelist<T1, T2>, GenFunc>
        : public IterateTypes<T1, GenFunc>
        , public IterateTypes<T2, GenFunc>
    {
    template<class II>
    void operator()(II& ii)
        {
        typedef IterateTypes<T1, GenFunc> head_t;
        typedef IterateTypes<T2, GenFunc> tail_t;
        head_t::operator()(ii);
		tail_t::operator()(ii);
		}
    };
     
    template <class AtomicType, template <class> class GenFunc>
    struct IterateTypes
    {
    template<class II>
    void operator()(II& ii)
        {
        typedef GenFunc<AtomicType> genfunc_t;
        *ii = genfunc_t()();
        ++ii;
        }
    };
    
    template <template <class> class GenFunc>
    struct IterateTypes<NullType, GenFunc>
    {
    template<class II>
    void operator()(II& ii)
        {}
    };
	}//ns TL
}//ns Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// 9/20/02 Named changed from GenData to IterateTypes
////////////////////////////////////////////////////////////////////////////////
