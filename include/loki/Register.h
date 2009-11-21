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
#ifndef LOKI_REGISTER_INC_
#define LOKI_REGISTER_INC_

// $Id$


#include "TypeManip.h"
#include "HierarchyGenerators.h"
#include "ForEachType.h"


///  \defgroup RegisterGroup Register 

namespace Loki
{

    ////////////////////////////////////////////////////////////////////////////////
    //
    //  Helper classes/functions for RegisterByCreateSet
    //
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    ///  \ingroup RegisterGroup
    ///  Must be specialized be the user
    ////////////////////////////////////////////////////////////////////////////////
    template<class T> 
    bool RegisterFunction();

    ////////////////////////////////////////////////////////////////////////////////
    ///  \ingroup RegisterGroup
    ///  Must be specialized be the user
    ////////////////////////////////////////////////////////////////////////////////
    template<class T> 
    bool UnRegisterFunction();

    namespace Private
    {
        struct RegisterOnCreate
        {
            template< int Index, typename T >
            void operator()()
            {
                RegisterFunction<T>(); 
            }
        };

        struct UnRegisterOnDelete
        {
            template< int Index, typename T >
            void operator()()
            {
                UnRegisterFunction<T>();
            }
        };

    }


    ////////////////////////////////////////////////////////////////////////////////
    ///  \class RegisterOnCreateSet
    ///
    ///  \ingroup RegisterGroup
    ///  Implements a generic register class which registers classes of a typelist
    ///
    ///  \par Usage
    ///  see test/Register
    ////////////////////////////////////////////////////////////////////////////////

    template<typename ElementList>
    struct RegisterOnCreateSet
    {
      RegisterOnCreateSet()
      {
        Private::RegisterOnCreate d;
        ForEachType< ElementList, Private::RegisterOnCreate > dummy(d);
      }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ///  \class UnRegisterOnDeleteSet
    ///
    ///  \ingroup RegisterGroup
    ///  Implements a generic register class which unregisters classes of a typelist
    ///
    ///  \par Usage
    ///  see test/Register
    ////////////////////////////////////////////////////////////////////////////////
    template<typename ElementList>
    struct UnRegisterOnDeleteSet
    {
        ~UnRegisterOnDeleteSet()
        {
            Private::UnRegisterOnDelete d;
            ForEachType< ElementList, Private::UnRegisterOnDelete > dummy(d);
        }
    };


    ////////////////////////////////////////////////////////////////////////////////
    ///  \def  LOKI_CHECK_CLASS_IN_LIST( CLASS , LIST )
    ///
    ///  \ingroup RegisterGroup
    ///  Check if CLASS is in the typelist LIST.
    ///
    ///  \par Usage
    ///  see test/Register
    ////////////////////////////////////////////////////////////////////////////////

    
#define LOKI_CONCATE(a,b,c,d) a ## b ## c ## d 
#define LOKI_CONCAT(a,b,c,d) LOKI_CONCATE(a,b,c,d)

#define LOKI_CHECK_CLASS_IN_LIST( CLASS , LIST )                                \
                                                                                \
    struct LOKI_CONCAT(check_,CLASS,_isInList_,LIST)                            \
    {                                                                           \
        typedef int LOKI_CONCAT(ERROR_class_,CLASS,_isNotInList_,LIST);         \
    };                                                                          \
    typedef Loki::Select<Loki::TL::IndexOf<LIST, CLASS>::value == -1,           \
                        CLASS,                                                  \
                        LOKI_CONCAT(check_,CLASS,_isInList_,LIST)>              \
                        ::Result LOKI_CONCAT(CLASS,isInList,LIST,result);       \
    typedef LOKI_CONCAT(CLASS,isInList,LIST,result)::                           \
                        LOKI_CONCAT(ERROR_class_,CLASS,_isNotInList_,LIST)      \
                        LOKI_CONCAT(ERROR_class_,CLASS,_isNotInList__,LIST);


} // namespace Loki


#endif // end file guardian

