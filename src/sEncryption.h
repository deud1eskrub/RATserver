#pragma once
#include <cstdlib>
#include <iostream>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
//--winapi
#pragma comment(lib, "Advapi32.lib")
#include <windows.h>

//--C std library ported to c++
#include <cstdlib>

//--c++ std library
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <thread>
#include <bit>
#include <mutex>
#include <cmath>
#include <chrono>


using namespace std;
void __cdecl zeroBuffer(char*, int);

typedef double(*f)(double);

double derivative(f fn, double x, double dx=0.00000001)
{
	return (fn(x + dx) - fn(x)) / dx;
}
double integral(f fn, double from, double to, double dx = 0.000001)
{
	double sum = 0.0f;
	for (double i = from; i <= to; i+=dx)
	{
		sum += fn(i)*dx;
	}
	return sum;
}


double eff(double x)
{
	return 1/tan(x);
}

int main(int argc, char**argv)
{

	
	//printf("%f", derivative(eff, 0.3));
	std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
	printf("%f\n", integral(eff, 0.1, 1));
	std::chrono::system_clock::time_point time1 = std::chrono::system_clock::now();
	std::cout << ((std::chrono::duration<double>)(time1 - time)).count() << std::endl;
	return 0;
}

	
	return 0;
}
