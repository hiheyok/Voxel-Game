#pragma once
#include <chrono>
#include <string>

class Timer {
public:
	
	Timer() {
		First = std::chrono::high_resolution_clock::now();
	}

	double GetTimePassed_ns();
	double GetTimePassed_μs();
	double GetTimePassed_ms();
	double GetTimePassed_s();

	std::string StrGetTimePassed_ns();
	std::string StrGetTimePassed_μs();
	std::string StrGetTimePassed_ms();
	std::string StrGetTimePassed_s();

	void Set();

private:
	std::chrono::steady_clock::time_point First;

};

void PrecisionSleep();