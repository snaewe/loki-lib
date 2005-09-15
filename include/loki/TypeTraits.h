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
        typedef TYPELIST_4(unsigned char, unsigned short int,unsigned int, unsigned long int) 
            StdUnsignedInts;
        typedef TYPELIST_4(signed char, short int,int, long int) 
            StdSignedInts;
        typedef TYPELIST_3(bool, char, wchar_t) 
            StdOtherInts;
        typedef TYPELIST_3(float, double, long double) 
            StdFloats;

        template <class U> struct AddReference
        {
            typedef U & Result;
        };

        template <class U> struct AddReference<U &>
        {
            typedef U & Result;
        };

        template <> struct AddReference<void>
        {
            typedef NullType Result;
        };

        template <class U> struct AddConstReference
        {
            typedef const U & Result;
        };

        template <class U> struct AddConstReference<U &>
        {
            typedef const U & Result;
        };

        template <> struct AddConstReference<void>
        {
            typedef NullType Result;
        };
                
    }
        
////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
//
// Figures out at compile time various properties of any given type
// Invocations (T is a type, TypeTraits<T>::Propertie):
//
// - isPointer       : returns true if T is a pointer type
// - PointeeType     : returns the type to which T points if T is a pointer 
//                     type, NullType otherwise
// - isReference     : returns true if T is a reference type
// - ReferredType    : returns the type to which T refers if T is a reference 
//                     type, NullType otherwise
// - isMemberPointer : returns true if T is a pointer to member type
// - isStdUnsignedInt: returns true if T is a standard unsigned integral type
// - isStdSignedInt  : returns true if T is a standard signed integral type
// - isStdIntegral   : returns true if T is a standard integral type
// - isStdFloat      : returns true if T is a standard floating-point type
// - isStdArith      : returns true if T is a standard arithmetic type
// - isStdFundamental: returns true if T is a standard fundamental type
// - isUnsignedInt   : returns true if T is a unsigned integral type
// - isSignedInt     : returns true if T is a signed integral type
// - isIntegral      : returns true if T is a integral type
// - isFloat         : returns true if T is a floating-point type
// - isArith         : returns true if T is a arithmetic type
// - isFundamental   : returns true if T is a fundamental type
// - ParameterType   : returns the optimal type to be used as a parameter for 
//                     functions that take Ts
// - isConst         : returns true if T is a const-qualified type
// - NonConstType    : Type with removed 'const' qualifier from T, if any
// - isVolatile      : returns true if T is a volatile-qualified type
// - NonVolatileType : Type with removed 'volatile' qualifier from T, if any
// - UnqualifiedType : Type with removed 'const' and 'volatile' qualifiers from 
//                     T, if any
// - ConstParameterType: returns the optimal type to be used as a parameter 
//                       for functions that take 'const T's
//
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    class TypeTraits
    {
    private:
        template <class U> struct PointerTraits
        {
            enum { result = false };
            typedef NullType PointeeType;
        };
        
        template <class U> struct PointerTraits<U*>
        {
            enum { result = true };
            typedef U PointeeType;
        };
        
        template <class U> struct PointerTraits<U*&>
        {
            enum { result = true };
            typedef U PointeeType;
        };
        
        template <class U> struct ReferenceTraits
        {
            enum { result = false };
            typedef U ReferredType;
        };
        
        template <class U> struct ReferenceTraits<U&>
        {
            enum { result = true };
            typedef U ReferredType;
        };
        
        template <class U> struct PToMTraits
        {
            enum { result = false };
        };
        
        template <class U, class V> struct PToMTraits<U V::*>
        {
            enum { result = true };
        };
        
        template <class U, class V> struct PToMTraits<U V::*&>
        {
            enum { result = true };
        };
        
        template <class U> struct UnConst
        {
            typedef U Result;
            enum { isConst = 0 };
        };
        
        template <class U> struct UnConst<const U>
        {
            typedef U Result;
            enum { isConst = 1 };
        };

        template <class U> struct UnConst<const U&>
        {
            typedef U& Result;
            enum { isConst = 1 };
        };
  
        template <class U> struct UnVolatile
        {
            typedef U Result;
            enum { isVolatile = 0 };
        };
       
        template <class U> struct UnVolatile<volatile U>
        {
            typedef U Result;
            enum { isVolatile = 1 };
        };

        template <class U> struct UnVolatile<volatile U&>
        {
            typedef U& Result;
            enum { isVolatile = 1 };
        };

        
    public:
        typedef typename UnConst<T>::Result 
            NonConstType;
        typedef typename UnVolatile<T>::Result 
            NonVolatileType;
        typedef typename UnVolatile<typename UnConst<T>::Result>::Result 
            UnqualifiedType;
        typedef typename PointerTraits<UnqualifiedType>::PointeeType 
            PointeeType;
        typedef typename ReferenceTraits<T>::ReferredType 
            ReferredType;

        enum { isConst          = UnConst<T>::isConst };
        enum { isVolatile       = UnVolatile<T>::isVolatile };
        enum { isPointer        = PointerTraits<UnqualifiedType>::result };
        enum { isReference      = ReferenceTraits<UnqualifiedType>::result };
        enum { isMemberPointer  = PToMTraits<UnqualifiedType>::result };

        enum { isStdUnsignedInt = TL::IndexOf<Private::StdUnsignedInts, UnqualifiedType>::value >= 0 ||
                                  TL::IndexOf<Private::StdUnsignedInts, 
                                          typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
        enum { isStdSignedInt   = TL::IndexOf<Private::StdSignedInts, UnqualifiedType>::value >= 0 ||
                                  TL::IndexOf<Private::StdSignedInts, 
                                          typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
        enum { isStdIntegral    = isStdUnsignedInt || isStdSignedInt ||
                                  TL::IndexOf<Private::StdOtherInts, UnqualifiedType>::value >= 0 ||
                                  TL::IndexOf<Private::StdOtherInts, 
                                          typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
        enum { isStdFloat       = TL::IndexOf<Private::StdFloats, UnqualifiedType>::value >= 0 ||
                                  TL::IndexOf<Private::StdFloats, 
                                          typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
        enum { isStdArith       = isStdIntegral || isStdFloat };
        enum { isStdFundamental = isStdArith || isStdFloat || Conversion<T, void>::sameType };
            
        enum { isUnsignedInt    = isStdUnsignedInt || IsCustomUnsignedInt<UnqualifiedType>::value };
        enum { isSignedInt      = isStdSignedInt || IsCustomSignedInt<UnqualifiedType>::value };
        enum { isIntegral       = isStdIntegral || isUnsignedInt || isSignedInt };
        enum { isFloat          = isStdFloat || IsCustomFloat<UnqualifiedType>::value };
        enum { isArith          = isIntegral || isFloat };
        enum { isFundamental    = isStdFundamental || isArith || isFloat };
        
        typedef typename Select<isStdArith || isPointer || isMemberPointer, T, 
                                            typename Private::AddReference<T>::Result>::Result 
            ParameterType;

        typedef typename Select<isStdArith || isPointer || isMemberPointer, T, 
                                            typename Private::AddConstReference<T>::Result>::Result 
            ConstParameterType;
        
    };
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20,      2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// September 16, 2002: ParameterType fixed, as TypeTraits<void> made 
//                     ParameterType give error about reference to void. T.S.
// August 26,    2005: better support of types with const/volatile qualifiers,
//                     thanks to Kalle Rutanen
////////////////////////////////////////////////////////////////////////////////

#endif // TYPETRAITS_INC_
