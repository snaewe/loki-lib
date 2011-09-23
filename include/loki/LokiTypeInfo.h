////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
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
#ifndef LOKI_LOKITYPEINFO_INC_
#define LOKI_LOKITYPEINFO_INC_

// $Id$


#include <typeinfo>
#include <cassert>
#include <loki/Typelist.h>

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class TypeInfo
// Purpose: offer a first-class, comparable wrapper over std::type_info
////////////////////////////////////////////////////////////////////////////////

    class TypeInfo
    {
    public:
        // Constructors
        TypeInfo(); // needed for containers
        TypeInfo(const std::type_info&); // non-explicit

        // Access for the wrapped std::type_info
        const std::type_info& Get() const;
        // Compatibility functions
        bool before(const TypeInfo& rhs) const;
        const char* name() const;

    private:
        const std::type_info* pInfo_;
    };

// Implementation

    inline TypeInfo::TypeInfo()
    {
        class Nil {};
        pInfo_ = &typeid(Nil);
        assert(pInfo_);
    }

    inline TypeInfo::TypeInfo(const std::type_info& ti)
    : pInfo_(&ti)
    { assert(pInfo_); }

    inline bool TypeInfo::before(const TypeInfo& rhs) const
    {
        assert(pInfo_);
        // type_info::before return type is int in some VC libraries
        return pInfo_->before(*rhs.pInfo_) != 0;
    }

    inline const std::type_info& TypeInfo::Get() const
    {
        assert(pInfo_);
        return *pInfo_;
    }

    inline const char* TypeInfo::name() const
    {
        assert(pInfo_);
        return pInfo_->name();
    }

// Comparison operators

    inline bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
    // type_info::operator== return type is int in some VC libraries
    { return (lhs.Get() == rhs.Get()) != 0; }

    inline bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
    { return lhs.before(rhs); }

    inline bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
    { return !(lhs == rhs); }

    inline bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
    { return rhs < lhs; }

    inline bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
    { return !(lhs > rhs); }

    inline bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
    { return !(lhs < rhs); }
}

#endif // end file guardian
