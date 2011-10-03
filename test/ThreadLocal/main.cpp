////////////////////////////////////////////////////////////////////////////////
//
// ThreadLocal test program for The Loki Library
// Copyright (c) 2009 by Richard Sposato
// The copyright on this file is protected under the terms of the MIT license.
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
//
////////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------

#include <cstdlib>

#include <iostream>

using namespace ::std;

extern bool TestThreadLocalClassStaticValue( void );

extern bool TestThreadLocalFunctionStaticValue( void );

extern bool TestThreadLocalStaticValue( void );


// ----------------------------------------------------------------------------

int main( int argc, const char * const argv[] )
{
    (void)argc;
    (void)argv;

    bool okay = true;
    cout << "Starting ThreadLocal tests." << endl;
    cout << "If any tests fail, or any assertions fail," << endl
         << "then your compiler does not implement thread_local storage correctly." << endl;

    cout << endl << "Testing static thread_local storage inside classes." << endl;
    okay = TestThreadLocalClassStaticValue();
    if ( okay )
        cout << "Your compiler correctly implements thread_local storage for class static values." << endl;
    else
        cout << "Your compiler does not properly implement thread_local storage for class static values." << endl;

    cout << endl << "Testing static thread_local storage inside functions." << endl;
    okay = TestThreadLocalFunctionStaticValue();
    if ( okay )
        cout << "Your compiler correctly implements thread_local storage for function static values." << endl;
    else
        cout << "Your compiler does not properly implement thread_local storage for function static values." << endl;

    cout << endl << "Testing standalone static thread_local storage." << endl;
    okay = TestThreadLocalStaticValue();
    if ( okay )
        cout << "Your compiler correctly implements thread_local storage for standalone static values." << endl;
    else
        cout << "Your compiler does not properly implement thread_local storage for standalone static values." << endl;

    ::std::cout << "Please press enter key to continue." << ::std::endl;
    ::std::cin.get();

    return 0;
}

// ----------------------------------------------------------------------------
