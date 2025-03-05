#include "Timer.h"

using namespace std;
using namespace chrono;

void Timer::Set() {
    first_ = high_resolution_clock::now();
}

double Timer::GetTimePassed_s() {
    return (double)(high_resolution_clock::now() - first_).count() / 1000000000.0;
}

double Timer::GetTimePassed_ms() {
    return (double)(high_resolution_clock::now() - first_).count() / 1000000.0;
}

double Timer::GetTimePassed_μs() {
    return (double)(high_resolution_clock::now() - first_).count() / 1000.0;
}

double Timer::GetTimePassed_ns() {
    return (double)(high_resolution_clock::now() - first_).count();
}

std::string Timer::StrGetTimePassed_s() {
    return to_string(GetTimePassed_s());
}

std::string Timer::StrGetTimePassed_ms() {
    return to_string(GetTimePassed_ms());
}

std::string Timer::StrGetTimePassed_μs() {
    return to_string(GetTimePassed_μs());
}

std::string Timer::StrGetTimePassed_ns() {
    return to_string(GetTimePassed_ns());
}