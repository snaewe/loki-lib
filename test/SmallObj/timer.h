////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The authors make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$

#ifndef LOKI_TEST_TIMER_H
#define LOKI_TEST_TIMER_H


#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <cmath>

class Timer 
{
public:

    Timer()
    {
        t100 = 0;
    };

    void start()
    {
        t0 = clock();
    }

    void stop()
    {
        t1 = clock();
    }
    
    int t()
    {
        return t1-t0;
    }

    double sec(int t)
    { 
        return floor(100.0*double(t)/1000.0 )/100.0; 
    }
    
    int rel(int t)
    {
        return ( t100==0 ? 100 : static_cast<int>(floor(100.0*t/t100+0.5)) ); 
    }
    
    double speedup(int t)
    {
        double tup=t;
        return (tup!=0 ? floor(100.0*(t100!=0?t100:tup)/tup+0.5)/100 : 1);
    }

    double  t100;

    void print(int t, const char* s)
    {
        std::cout << s << "\tseconds: " << sec(t) << "\trelative time: " << rel(t) << "%\tspeed-up factor: " << speedup(t) << "" << std::endl;
    }
private:
    int t0;
    int t1;
};

#endif

// $Log$
// Revision 1.6  2006/01/19 23:11:57  lfittl
// - Disabled -Weffc++ flag, fixing these warnings produces too much useless code
// - Enabled -pedantic, -Wold-style-cast and -Wundef for src/ and test/
//
// Revision 1.5  2005/10/30 14:03:23  syntheticpp
// replace tabs space
//
// Revision 1.4  2005/10/26 00:38:49  rich_sposato
// Added CVS keywords and header lines.
//
