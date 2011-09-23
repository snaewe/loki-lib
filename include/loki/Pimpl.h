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
#ifndef LOKI_PIMPL_INC_
#define LOKI_PIMPL_INC_

// $Id$


///  \defgroup PimplGroup Pimpl

#ifndef LOKI_INHERITED_PIMPL_NAME
#define LOKI_INHERITED_PIMPL_NAME d
#endif

#ifndef LOKI_INHERITED_RIMPL_NAME
#define LOKI_INHERITED_RIMPL_NAME d
#endif

namespace Loki
{

    //////////////////////////////////////////
    ///  \class ConstPropPtr
    ///
    ///  \ingroup PimplGroup
    ///   Simple const propagating smart pointer
    ///   Is the default smart pointer of Pimpl.
    //////////////////////////////////////////

    template<class T>
    struct ConstPropPtr
    {
        explicit ConstPropPtr(T* p) : ptr_(p) {}
        ~ConstPropPtr() { delete  ptr_; ptr_ = 0; }
        T* operator->()    { return  ptr_; }
        T& operator*()    { return *ptr_; }
        const T* operator->() const    { return  ptr_; }
        const T& operator*()  const    { return *ptr_; }

    private:
        ConstPropPtr();
        ConstPropPtr(const ConstPropPtr&);
        ConstPropPtr& operator=(const ConstPropPtr&);
        T* ptr_;
    };


    ////////////////////////////////////////////////////////////////////////////////
    ///  \class Pimpl
    ///
    ///  \ingroup PimplGroup
    ///
    ///  Implements the Pimpl idiom. It's a wrapper for a smart pointer which
    ///  automatically creates and deletes the implementation object and adds
    ///  const propagation to the smart pointer.
    ///
    ///  \par Usage
    ///  see test/Pimpl
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        typename Pointer = ConstPropPtr<T>
    >
    class Pimpl
    {
    public:

        typedef T Impl;

        Pimpl() : ptr_(new T)
        {}

        ~Pimpl()
        {
            // Don't compile with incomplete type
            //
            // If compilation breaks here make sure
            // the compiler does not auto-generate the
            // destructor of the class hosting the pimpl:
            // - implement the destructor of the class
            // - don't inline the destructor
            typedef char T_must_be_defined[sizeof(T) ? 1 : -1 ];
        }


        T* operator->()
        {
            return ptr_.operator->();
        }

        T& operator*()
        {
            return ptr_.operator*();
        }

        const T* operator->() const
        {
            return ptr_.operator->();
        }

        const T& operator*() const
        {
            return ptr_.operator*();
        }

        Pointer& wrapped()
        {
            return ptr_;
        }

        const Pointer& wrapped() const
        {
            return ptr_;
        }


    private:
        Pimpl(const Pimpl&);
        Pimpl& operator=(const Pimpl&);

        Pointer ptr_;
    };


    template<class T, typename Pointer = ConstPropPtr<T> >
    struct PimplOwner
    {
        Pimpl<T,Pointer> LOKI_INHERITED_PIMPL_NAME;
    };


    //////////////////////////////////////////
    /// \class  ImplOf
    ///
    /// \ingroup PimplGroup
    /// Convenience template for the
    /// implementations which Pimpl points to.
    //////////////////////////////////////////

    template<class T>
    struct ImplOf;


    //////////////////////////////////////////
    /// \class  PImplOf
    ///
    /// \ingroup PimplGroup
    /// Convenience template which uses ImplOf
    /// as implementation structure
    //////////////////////////////////////////


    template<class T, template<class> class Ptr = ConstPropPtr>
    struct PimplOf
    {
        typedef T Impl;

        // declare pimpl
        typedef Pimpl<ImplOf<T>, Ptr<ImplOf<T> > > Type;

        // inherit pimpl
        typedef PimplOwner<ImplOf<T>, Ptr<ImplOf<T> > > Owner;
    };


    template<class T, class UsedPimpl = typename PimplOf<T>::Type >
    struct RimplOf
    {
        typedef typename UsedPimpl::Impl & Type;

        class Owner
        {
            UsedPimpl pimpl;

        public:
            Owner() : LOKI_INHERITED_RIMPL_NAME(*pimpl)
            {}

            Type LOKI_INHERITED_RIMPL_NAME;
        };

    };

}

#endif // end file guardian

