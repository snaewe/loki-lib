////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Oct 05, 2002
// Most of the code is taken from Rani Sharoni's Loki VC 7 port
// Reference, pointer and array detection is based on boost's type traits
// AdjReference has moved to namespace-scope. Explicit specialization is
// only allowed there.

#ifndef TYPETRAITS_INC_
#define TYPETRAITS_INC_

#include "Typelist.h"

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template IsCustomUnsignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits 
//     class template defined below.
// Invocation: IsCustomUnsignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in unsigned
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomUnsignedInt
    {
        enum { value = 0 };
    };        

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomSignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits 
//     class template defined below.
// Invocation: IsCustomSignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in signed
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomSignedInt
    {
        enum { value = 0 };
    };        

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomFloat
// Offers a means to integrate nonstandard floating point types with the
//     TypeTraits class template defined below.
// Invocation: IsCustomFloat<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in
//     floating point type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct IsCustomFloat
    {
        enum { value = 0 };
    };        

////////////////////////////////////////////////////////////////////////////////
// Helper types for class template TypeTraits defined below
////////////////////////////////////////////////////////////////////////////////
	namespace Private
    {
        typedef TYPELIST_4(unsigned char, unsigned short int,
           unsigned int, unsigned long int) StdUnsignedInts;
        typedef TYPELIST_4(signed char, short int,
           int, long int) StdSignedInts;
        typedef TYPELIST_3(bool, char, wchar_t) StdOtherInts;
        typedef TYPELIST_3(float, double, long double) StdFloats;
    }
        
////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
// Figures out various properties of any given type
// Invocations (T is a type):
// a) TypeTraits<T>::isPointer
// returns (at compile time) true if T is a pointer type
// b) TypeTraits<T>::PointeeType
// returns the type to which T points is T is a pointer type, NullType otherwise
// a) TypeTraits<T>::isReference
// returns (at compile time) true if T is a reference type
// b) TypeTraits<T>::ReferredType
// returns the type to which T refers is T is a reference type, NullType
// otherwise
// c) TypeTraits<T>::isMemberPointer
// returns (at compile time) true if T is a pointer to member type
// d) TypeTraits<T>::isStdUnsignedInt
// returns (at compile time) true if T is a standard unsigned integral type
// e) TypeTraits<T>::isStdSignedInt
// returns (at compile time) true if T is a standard signed integral type
// f) TypeTraits<T>::isStdIntegral
// returns (at compile time) true if T is a standard integral type
// g) TypeTraits<T>::isStdFloat
// returns (at compile time) true if T is a standard floating-point type
// h) TypeTraits<T>::isStdArith
// returns (at compile time) true if T is a standard arithmetic type
// i) TypeTraits<T>::isStdFundamental
// returns (at compile time) true if T is a standard fundamental type
// j) TypeTraits<T>::isUnsignedInt
// returns (at compile time) true if T is a unsigned integral type
// k) TypeTraits<T>::isSignedInt
// returns (at compile time) true if T is a signed integral type
// l) TypeTraits<T>::isIntegral
// returns (at compile time) true if T is a integral type
// m) TypeTraits<T>::isFloat
// returns (at compile time) true if T is a floating-point type
// n) TypeTraits<T>::isArith
// returns (at compile time) true if T is a arithmetic type
// o) TypeTraits<T>::isFundamental
// returns (at compile time) true if T is a fundamental type
// p) TypeTraits<T>::ParameterType
// returns the optimal type to be used as a parameter for functions that take Ts
// q) TypeTraits<T>::isConst
// returns (at compile time) true if T is a const-qualified type
// r) TypeTraits<T>::NonConstType
// removes the 'const' qualifier from T, if any
// s) TypeTraits<T>::isVolatile
// returns (at compile time) true if T is a volatile-qualified type
// t) TypeTraits<T>::NonVolatileType
// removes the 'volatile' qualifier from T, if any
// u) TypeTraits<T>::UnqualifiedType
// removes both the 'const' and 'volatile' qualifiers from T, if any
////////////////////////////////////////////////////////////////////////////////
	namespace Private
	{
		struct pointer_helper
		{
			pointer_helper(const volatile void*);
		};
		
		struct EnumFalse{ enum {value = 0};};
		struct EnumTrue{ enum {value = 1};};
		template<bool IsRef>
        struct AdjReference
        {
            template<typename U>
            struct In { typedef U const & Result; };
        };

        template<>
        struct AdjReference<true>
        {
            template<typename U>
            struct In { typedef U Result; };
        };

	}
  
	template <typename T>
    class TypeTraits
    {
    private:	
//		static T MakeT(...);
		template <class U> struct Wrap{};
		struct pointer_helper
		{
			pointer_helper(const volatile void*);
		};
		static Private::YES is_pointer(pointer_helper);
		static Private::NO  is_pointer(...);
		
		// With the VC 6. Rani Sharoni's approach to detect references unfortunately
		// results in an error C1001: INTERNER COMPILER- FEHLER
		// 
		// is_reference_helper1 is a function taking a Wrap<T> returning
		// a pointer to a function taking a Wrap<T> returning a T&.
		// This function can only be used if T is not a reference-Type.
		// If T is a reference Type the return type would be
		// a function taking a Wrap<T> returning a reference to a T-reference.
		// That is illegal, therefore is_reference_helper1(...) is used for
		// references.
		// In order to detect a reference, use the return-type of is_reference_helper1
		// with is_reference_helper2.
		// 
		// this reference-detection approach is based on boost's
		// Type-Traits. See: boost::composite_traits.h
		template <class T> 
		static T&(* is_reference_helper1(Wrap<T>) )(Wrap<T>);
		static Private::NO	is_reference_helper1(...);

		template <class T> 
		static Private::NO	is_reference_helper2(T&(*)(Wrap<T>));
		static Private::YES is_reference_helper2(...);
	
		template <class T, class U> 
		static Private::YES is_pointer2member(U T::*);
		static Private::NO	is_pointer2member(...);

		// This function can only be used for non-array-types, because
		// functions can't return arrays.
		// this array-detection approach is based on boost's
		// Type-Traits. See: boost::array_traits.h
		template <class T> 
		static T(* is_array_helper1(Wrap<T>) )(Wrap<T>);
		static Private::NO is_array_helper1(...);

		template <class T> 
		static Private::NO is_array_helper2(T(*)(Wrap<T>));
		static Private::YES is_array_helper2(...);

		template<typename U>
		static Private::YES is_const(Wrap<const U>);
        static Private::NO  is_const(...);

        template<typename U>
		static Private::YES is_volatile(Wrap<volatile U>);
        static Private::NO  is_volatile(...);
	public:
		enum {isReference = sizeof(
            is_reference_helper2(
            is_reference_helper1(Wrap<T>()))) == sizeof(Private::YES)};
//		enum {isPointer = sizeof(is_pointer(MakeT())) == sizeof(Private::YES)};
//		enum {isMemberPointer = sizeof(is_pointer2member(MakeT())) == sizeof(Private::YES)};
		enum {isArray = sizeof(
				is_array_helper1(
				is_array_helper2(Wrap<T>()))) == sizeof(Private::YES)};
		enum {isVoid = Private::IsVoid<T>::value};

		enum { isStdUnsignedInt = 
            TL::IndexOf<Private::StdUnsignedInts, T>::value >= 0 };
        enum { isStdSignedInt = 
            TL::IndexOf<Private::StdSignedInts, T>::value >= 0 };
        enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
            TL::IndexOf<Private::StdOtherInts, T>::value >= 0 };
        enum { isStdFloat = TL::IndexOf<Private::StdFloats, T>::value >= 0 };
        enum { isStdArith = isStdIntegral || isStdFloat };
        enum { isStdFundamental = isStdArith || isStdFloat || isVoid };
            
        enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<T>::value };
        enum { isSignedInt = isStdSignedInt || IsCustomSignedInt<T>::value };
        enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };
        enum { isFloat = isStdFloat || IsCustomFloat<T>::value };
        enum { isArith = isIntegral || isFloat };
        enum { isFundamental = isStdFundamental || isArith || isFloat };
		enum { 
            isConst = 
                sizeof(is_const(Wrap<T>())) == sizeof(Private::YES)
        };

        enum { 
            isVolatile = 
                sizeof(is_volatile(Wrap<T>())) == sizeof(Private::YES)
        };
		enum { isScalar = isStdArith /* || isPointer */ /* ||  isMemberPointer */ };
	private:
        typedef typename Private::AdjReference<isReference || isVoid>::
                template In<T>::Result AdjType;

    public:        


        typedef typename Select
        <
            isScalar || isArray, T, AdjType
        >
        ::Result ParameterType;
        
        //
        // We get is_class for free
        // BUG - fails with functions types (ICE) and unknown size array
        // (but works for other incomplete types)
        // (the boost one (Paul Mensonides) is better)
        //
        enum { isClass = 
                !isScalar    && 
                !isArray     && 
                !isReference &&
                !isVoid 
        };
    };
}
////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Oct	05, 2002: ported by Benjamin Kaufmann to MSVC 6 
////////////////////////////////////////////////////////////////////////////////

#endif // TYPETRAITS_INC_