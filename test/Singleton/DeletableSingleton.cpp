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
//    LogClass::LogClass()
//    LogClass singleton instantiated
//    Going to manually delete LogBook.
//    LogClass::~LogClass()
//    LogClass::LogClass()
//    LogClass reinstantiated.
//    Going to terminate program now.
//    LogClass::~LogClass()
//

#include <iostream>
#include "loki/Singleton.h"   // for Loki::SingletonHolder

using namespace std;   // okay for small programs
using namespace Loki;  // okay for small programs

// A singleton LogClass object derived from the Example class.
// Its longevity is set by the user on the command line.
//
class LogClass
{
public:
    LogClass()
    {
        print("LogClass::LogClass()");
    };
    ~LogClass()
    {
        print("LogClass::~LogClass()");
    }
    void print(const char *s)
    {
        cout << s << endl;
    }
};

typedef SingletonHolder<LogClass, CreateUsingNew, DeletableSingleton> LogBook;

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
    LogBook::Instance().print("LogClass singleton instantiated");
    LogBook::Instance().print("Going to manually delete LogBook.");

    DeletableSingleton<LogClass>::GracefulDelete();

    LogBook::Instance().print("LogClass reinstantiated.");
    LogBook::Instance().print("Going to terminate program now.");

#if defined(__BORLANDC__) || defined(__GNUC__) || defined(_MSC_VER)
    system("pause"); // Stop console window from closing if run from IDE.
#endif

    return 0;
}

