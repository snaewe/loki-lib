////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 by Curtis Krauskopf
// Copyright (c) 2005 by Peter Kuemmel
//
// Code covered by the MIT License
// The authors make no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// A singleton Log object that resurrects itself after
// it has been automatically destroyed during program
// termination.  When a dead-reference to the Log
// singleton is detected, it is resurrected automatically.
//
// Expected output:
//   Example c'tor
//   Log c'tor
//   Log now instantiated.
//   Log d'tor
//   Example d'tor starting
//   Log c'tor
//   Log: inside Example d'tor
//   Example d'tor finished
//
// The last line of the output only appears when this
// program is compiled with the ATEXIT_FIXED symbol
// defined (see the Loki library and the CUJ article).
//

#include <iostream>
#include "loki/Singleton.h"   // for Loki::SingletonHolder

using namespace std;   // okay for small programs
using namespace Loki;  // okay for small programs

class Log
{
public:
    Log()
    {
        echo("Log c'tor");
    }
    ~Log()
    {
        echo("Log d'tor");
    }
    void echo(const char *s)
    {
        cout << s << endl;
    }
};

typedef SingletonHolder<Log, CreateUsingNew, PhoenixSingleton> log;


// A generic example class that stores and echoes a const char.
//
class Example
{
public:
    Example()
    {
        echo("Example c'tor");
    }
    ~Example()
    {
        echo("Example d'tor starting");
        log::Instance().echo("Log: inside Example d'tor");
        echo("Example d'tor finished");
    }
    void echo(const char *s)
    {
        cout << s << endl;
    }
};


int main(int argc, char* argv[])
{
    Example *example = new Example();
     SetLongevity<Example, void (*)(Example*)>(example, 1, &Loki::Private::Deleter<Example>::Delete);
    log::Instance().echo("Log now instantiated.");
    
#if defined(__BORLANDC__) || defined(__GNUC__) || defined(_MSC_VER)
    system("pause"); // Stop console window from closing if run from IDE.
#endif

    return 0;
}

