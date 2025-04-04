#include "Utils/Timer/Timer.h"

void Timer::Set() {
    first_ = std::chrono::high_resolution_clock::now();
}

double Timer::GetTimePassed_s() const {
    return (double)(std::chrono::high_resolution_clock::now() - first_).count() / 1000000000.0;
}

double Timer::GetTimePassed_ms() const {
    return (double)(std::chrono::high_resolution_clock::now() - first_).count() / 1000000.0;
}

double Timer::GetTimePassed_μs()const {
    return (double)(std::chrono::high_resolution_clock::now() - first_).count() / 1000.0;
}

double Timer::GetTimePassed_ns()const {
    return (double)(std::chrono::high_resolution_clock::now() - first_).count();
}

std::string Timer::StrGetTimePassed_s() const {
    return std::to_string(GetTimePassed_s());
}

std::string Timer::StrGetTimePassed_ms()const {
    return std::to_string(GetTimePassed_ms());
}

std::string Timer::StrGetTimePassed_μs() const {
    return std::to_string(GetTimePassed_μs());
}

std::string Timer::StrGetTimePassed_ns() const {
    return std::to_string(GetTimePassed_ns());
}