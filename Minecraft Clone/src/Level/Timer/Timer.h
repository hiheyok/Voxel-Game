#pragma once
#include <chrono>
#include <string>

class Timer {
public:
    
    Timer() {
        first_ = std::chrono::high_resolution_clock::now();
    }

    double GetTimePassed_ns() const;
    double GetTimePassed_μs() const;
    double GetTimePassed_ms() const;
    double GetTimePassed_s() const;

    std::string StrGetTimePassed_ns() const;
    std::string StrGetTimePassed_μs() const;
    std::string StrGetTimePassed_ms() const;
    std::string StrGetTimePassed_s() const;

    void Set();

private:
    std::chrono::high_resolution_clock::time_point first_;

};
