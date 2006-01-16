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
#ifndef LOKI_PIMPLDEF_H
#define LOKI_PIMPLDEF_H

#include <loki/Pimpl.h>

namespace Loki
{
    template<class T>
    void AutoDeletePimpl<T>::Destroy(T ptr)
    { 
#ifndef _MSC_VER // msvc bug

        typedef char T_must_be_defined[
            sizeof(typename TypeTraits<T>::PointeeType) ? 1 : -1 ];
        delete ptr; 
        ptr = 0;

#else

#pragma warning(push)
#pragma warning(disable: 4150)

        delete ptr; 
        ptr = 0;

#pragma warning(pop)
#endif
    }


    template
    <
        class Impl,
        class Ptr,
        template<class> class Del
    >
    inline 
    PimplLife<Impl,Ptr,Del>::PimplLife()    //: ptr(Ptr()) this owerwrites the pointer to PtrImpl
    {}                                      // when using DeclaredRimpl!!

    template
    <
        class Impl,
        class Ptr,
        template<class> class Del
    >
    inline 
    PimplLife<Impl,Ptr,Del>::~PimplLife()
    {
        Del<Ptr>::Destroy( ptr );
    }

    template
    <
        class Impl,
        class Ptr,
        template<class> class Del
    >
    template<class T>
    inline 
    PimplLife<Impl,Ptr,Del>::operator T&()
    {
        ptr = Ptr( new Impl );
        return *ptr;
    }

    template
    <
        class Impl,
        class Ptr,
        template<class> class Del
    >
    inline
    Ptr PimplLife<Impl,Ptr,Del>::Create()
    {
        ptr = Ptr( new Impl );
        return ptr;
    }
}

#endif
