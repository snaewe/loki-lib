////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 by Curtis Krauskopf
// Copyright (c) 2005 by Peter Kuemmel
//
// Code covered by the MIT License
// The authors make no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Show an example of a Loki policy that uses DeletableSingleton.
//
// Expected output:
//
//    Log::Log()
//    Log singleton instantiated
//    Going to manually delete log.
//    Log::~Log()
//    Log::Log()
//    Log reinstantiated.
//    Going to terminate program now.
//    Log::~Log()
//

#include <iostream>
#include "loki/Singleton.h"   // for Loki::SingletonHolder

using namespace std;   // okay for small programs
using namespace Loki;  // okay for small programs

// A singleton Log object derived from the Example class.
// Its longevity is set by the user on the command line.
//
class Log
{
public:
    Log()
    {
        print("Log::Log()");
    };
    ~Log()
    {
        print("Log::~Log()");
    }
    void print(const char *s)
    {
        cout << s << endl;
    }
};

typedef SingletonHolder<Log, CreateUsingNew, DeletableSingleton> log;

class Example
{
public:
    void method()
    {
        cout << "test\n";
    }
};


int main(int argc, char *argv[])
{
    // Instantiate both singletons by calling them...
    log::Instance().print("Log singleton instantiated");
    log::Instance().print("Going to manually delete log.");

    DeletableSingleton<Log>::GracefulDelete();

    log::Instance().print("Log reinstantiated.");
    log::Instance().print("Going to terminate program now.");

#if defined(__BORLANDC__) || defined(__GNUC__) || defined(_MSC_VER)
    system("pause"); // Stop console window from closing if run from IDE.
#endif

    return 0;
}

