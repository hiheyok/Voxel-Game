#include "Utils/Clock.h"

#include <chrono>
#include <thread>

void timerSleep(double seconds) {
    using namespace std::chrono;

    if (seconds <= 0.0) return;

    constexpr double sleep_threshold = 10e-3;  // 10 milliseconds
    constexpr double yield_threshold = 1e-3;
    auto start_time = high_resolution_clock::now();

    if (seconds > sleep_threshold) {
        auto sleep_duration = duration<double>(seconds - sleep_threshold);
        std::this_thread::sleep_for(sleep_duration);
    }

    auto curr_time = high_resolution_clock::now();

    // Spin-wait for the remaining time
    while (duration<double>(high_resolution_clock::now() - start_time).count() <
           seconds) {
        std::this_thread::yield();
    }
}