////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 by Andrei Alexandrescu
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

// $Id$


#include <loki/SafeFormat.h>


namespace Loki
{

    // Crude writing method: writes straight to the file, unbuffered
    // Must be combined with a buffer to work properly (and efficiently)

    void write(std::FILE* f, const char* from, const char* to) {
        assert(from <= to);
        ::std::fwrite(from, 1, to - from, f);
    }

    // Write to a string

    void write(std::string& s, const char* from, const char* to) {
        assert(from <= to);
        const size_t addCount = to - from;
        if ( s.capacity() <= s.size() + addCount )
        {
            s.reserve( 2 * s.size() + addCount );
        }
        s.append(from, to);
    }

    // Write to a stream

    void write(std::ostream& f, const char* from, const char* to) {
        assert(from <= to);
        f.write(from, std::streamsize(to - from));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // PrintfState class template
    // Holds the formatting state, and implements operator() to format stuff
    // Todo: make sure errors are handled properly
    ////////////////////////////////////////////////////////////////////////////////


    PrintfState<std::FILE*, char> Printf(const char* format) {
        return PrintfState<std::FILE*, char>(stdout, format);
    }

    PrintfState<std::FILE*, char> Printf(const std::string& format) {
        return PrintfState<std::FILE*, char>(stdout, format.c_str());
    }

    PrintfState<std::FILE*, char> FPrintf(std::FILE* f, const char* format) {
        return PrintfState<std::FILE*, char>(f, format);
    }

    PrintfState<std::FILE*, char> FPrintf(std::FILE* f, const std::string& format) {
        return PrintfState<std::FILE*, char>(f, format.c_str());
    }

    PrintfState<std::ostream&, char> FPrintf(std::ostream& f, const char* format) {
        return PrintfState<std::ostream&, char>(f, format);
    }

    PrintfState<std::ostream&, char> FPrintf(std::ostream& f, const std::string& format) {
        return PrintfState<std::ostream&, char>(f, format.c_str());
    }

    PrintfState<std::string&, char> SPrintf(std::string& s, const char* format) {
        const size_t estimate = ::strlen( format ) + 128;
        s.reserve( estimate );
        return PrintfState<std::string&, char>(s, format);
    }

    PrintfState<std::string&, char> SPrintf(std::string& s, const std::string& format) {
        const size_t estimate = format.size() + 128;
        s.reserve( estimate );
        return PrintfState<std::string&, char>(s, format.c_str());
    }


} // end namespace Loki

