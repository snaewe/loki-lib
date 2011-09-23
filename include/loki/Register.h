////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Code covered by the MIT License
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
#ifndef LOKI_REGISTER_INC_
#define LOKI_REGISTER_INC_

// $Id$


#include <loki/TypeManip.h>
#include <loki/HierarchyGenerators.h>
#include <loki/ForEachType.h>


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

