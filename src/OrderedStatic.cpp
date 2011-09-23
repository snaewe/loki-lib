////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter Kümmel
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

// $Id$


#include <loki/OrderedStatic.h>
#include <limits>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace Loki
{
    namespace Private
    {

        OrderedStaticCreatorFunc::OrderedStaticCreatorFunc()
        {
        }

        OrderedStaticCreatorFunc::~OrderedStaticCreatorFunc()
        {
        }


        OrderedStaticManagerClass::OrderedStaticManagerClass() :
                    staticObjects_(),
                    max_longevity_(std::numeric_limits<unsigned int>::min()),
                    min_longevity_(std::numeric_limits<unsigned int>::max())
        {
        }

        OrderedStaticManagerClass::~OrderedStaticManagerClass()
        {
        }

        void OrderedStaticManagerClass::createObjects()
        {
            for(unsigned int longevity=max_longevity_; longevity>=min_longevity_; longevity--)
            {
                for(unsigned int i=0; i<staticObjects_.size(); i++)
                {
                    Data cur = staticObjects_.at(i);
                    if(cur.longevity==longevity)
                        ( (*cur.object).*cur.creator )();
                }
            }
        }

        void OrderedStaticManagerClass::registerObject(unsigned int l, OrderedStaticCreatorFunc* o,Creator f)
        {
            staticObjects_.push_back(Data(l,o,f));

            if(l>max_longevity_) max_longevity_=l;
            if(l<min_longevity_) min_longevity_=l;
        }

        OrderedStaticManagerClass::Data::Data(unsigned int l, OrderedStaticCreatorFunc* o, Creator f)
            : longevity(l), object(o), creator(f)
        {
        }

    }//namespace Private

} // end namespace Loki

