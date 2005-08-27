////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 by Andrei Alexandrescu
// Permission to use, copy, modify, distribute, and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this software 
//     for any purpose. It is provided "as is" without express or implied 
//     warranty.
////////////////////////////////////////////////////////////////////////////////

#include "SafeFormat.h"
#include <iostream>
#include <cassert>
#include <utility>

using namespace std;

template <class Integral1, class Integral2>
Integral2 RandomInt(Integral1 low, Integral2 up)
{
    // From ``Accelerated C++'', page 135:
    // random integer in the range [0, n)
    // We adjust to generate in the range [0, n]
    const Integral2 
        low2 = low,
        n = up - low;
    
    assert(n > 0);

    const unsigned int bucket_size = RAND_MAX / n;
    assert(bucket_size > 0);
   
    Integral2 r;
    do r = Integral2(rand() / bucket_size);
    while (r > n);

    r += low2;   
    assert(r >= low2 && r <= up);
    return r;
}

string RandomString(unsigned int maxSize)
{
    string result(RandomInt(0, maxSize), '\0');
    unsigned int i = 0;
    for (; i != result.size(); ++i) {
        result[i] = RandomInt('a', 'z');
    }
    return result;
}

template <class T>
void TestCase(const string& fmt, T value) {
    char buf[4096];
    std::string s;
    const int i1 = SPrintf(s, fmt.c_str())(value); 
    const int i2 = _snprintf(buf, sizeof(buf), fmt.c_str(), value); 
    if (i1 != i2 || s != buf) {
        cout << 
            "\nReference: " << i2 << 
            "; Actual: " << i1 << "\nV: [" << value << "]\nF: [" << 
            fmt << "]\n" <<
            "R: [" << buf << "]\nA: [" << s.c_str() << "]\n";
        assert(false); 
    }
}

template <class T, class U>
void TestCase2(const string& fmt, T value, U value2) {
    char buf[4096];
    std::string s;
    const int i1 = SPrintf(s, fmt.c_str())(value)(value2); 
    const int i2 = snprintf(buf, sizeof(buf), fmt.c_str(), value, value2); 
    assert(i1 == i2); 
    assert(s == buf); 
}

int main(int argc, char** argv) {
    if (argc == 3) {
        // test speed
        int i = atoi(argv[2]);
        switch (argv[1][0]) {
            case 'p':
                for (; i > 0; --i) {
                    printf("Hey, %u frobnicators and %u twiddlicators\n",
                        i, i);
                }
                break;
            case 's':
                for (; i > 0; --i) {
                    cout << "Hey, " << i << " frobnicators and " << i << 
                        " twiddlicators\n";
                }
                break;
            case 'n':
                for (; i > 0; --i) {
                    Printf("Hey, %u frobnicators and %u twiddlicators\n")
                        (i)(i);
                }
                break;
        }
        return 0;
    }
    
    //srand(time(0));
    srand(0);
    for (unsigned i = 0; ; ++i) {
        printf("%u\r", i);
        
        // Generate a random string for the head
        string lead = RandomString(100);
        // This string will hold a random format specification
        string formatSpec(lead + "|%");
        // Generate a random set of flags
        static const string flags("-+0 #");
        unsigned int maxFlags = RandomInt(0u, flags.length() - 1);
        for (unsigned int i = 0; i != maxFlags; ++i) {
            formatSpec += flags[RandomInt(0u, flags.length() - 1)];
        }
        // Generate an optional random width
        if (RandomInt(0, 1)) {
            const unsigned int width = RandomInt(0, 100);
            char buf[4];
            sprintf(buf, "%u", width);
            formatSpec += buf;
        }
        // Generate an optional random precision
        if (RandomInt(0, 1)) {
            const unsigned int prec = RandomInt(0, 100);
            char buf[4];
            sprintf(buf, "%u", prec);
            formatSpec += '.';
            formatSpec += buf;
        }
        // Generate a random type character
        static const string type("cdeEfgGinopsuxX");
        const char typeSpec = type[RandomInt(0, type.size() - 1)];
        // Generate an optional type prefix
        static const string prefix("hl");
        if (typeSpec != 's' && RandomInt(0, 1)) {
            formatSpec += prefix[RandomInt(0, prefix.size() - 1)];
        }
        formatSpec += typeSpec;
        formatSpec += '|';
        formatSpec += RandomString(100);
        
        switch (typeSpec) {
        case 'c': 
            TestCase(formatSpec, RandomInt(1, 127));
            break;
        case 'd': case 'i': case 'o': case 'u': case 'x': case 'X': 
            TestCase(formatSpec, RandomInt(-10000, 10000));
            break;
        case 'e': case 'E': case 'f': case 'g': case 'G': 
            TestCase(formatSpec, 
                RandomInt(-10000, 10000) / double(RandomInt(1, 100)));
            break;
        case 'n': 
            break;
        case 'p': 
            {
                void * p = malloc(RandomInt(1, 1000));
                TestCase(formatSpec, p);
                free(p);
            }
            break;
        case 's': 
            TestCase(formatSpec, RandomString(100).c_str());
            break;
        default:
            assert(false);
            break;            
        }
    }
}
