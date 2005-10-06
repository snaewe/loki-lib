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
		return ( t100==0 ? 100 : (int) floor(100.0*t/t100+0.5) ); 
	}
	
	double speedup(int t)
	{
		double tup=t;
		return (tup!=0 ? floor(100.0*(t100!=0?t100:tup)/tup+0.5)/100 : 1);
	}

	double  t100;

	void print(int t, const char* s)
	{
		std::cout << s <<"\trelative time: " << rel(t) << "%\tspeed-up factor: " << speedup(t) << "" << std::endl;
	}
private:
    int t0;
	int t1;
};
